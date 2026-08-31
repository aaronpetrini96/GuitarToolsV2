//
// Created by Aaron Petrini on 21/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "BinaryData.h"

class AAImgKnob : public juce::Slider
{
public:
    AAImgKnob (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const juce::String& name, const juce::String& suffix = {}, bool drawFromMiddle = false, bool fillFromEnd = false, bool useFillStroke = false);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void paintFillStroke (juce::Graphics& g, juce::Point<float> centre, float r, const juce::PathStrokeType& stroke, float angle);
    void paintLightDots (juce::Graphics& g, juce::Point<float> centre, float r, float knobRadius, float angle);
    void updateValueLabelText();

    juce::Image knobImage = juce::ImageCache::getFromMemory (BinaryData::knob_png, BinaryData::knob_pngSize);
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    juce::String unit;
    bool drawFromMiddle;
    bool fillFromEnd;
    bool useFillStroke;
    juce::Label nameLabel;
    juce::Label valueLabel;

    const float startAngle { 1.25f * juce::MathConstants<float>::pi };
    const float endAngle { 2.75f * juce::MathConstants<float>::pi };
    const int numberOfDots { 11 };

    // shrinks the artwork inside the bounds, leaving a transparent margin for the value label
    const float imageScale { 0.85f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAImgKnob)
};
