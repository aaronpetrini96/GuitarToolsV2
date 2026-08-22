//
// Created by Aaron Petrini on 21/08/2026.
//

#pragma once

#include <JuceHeader.h>

class ImgKnob : public juce::Slider
{
public:
    ImgKnob (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const juce::String& suffix = {}, bool drawFromMiddle = false);

    void paint (juce::Graphics& g) override;

private:
    void paintFillStroke (juce::Graphics& g, juce::Point<float> centre, float r, const juce::PathStrokeType& stroke, float angle);
    void paintLightDots (juce::Graphics& g, juce::Point<float> centre, float r, float knobRadius, float angle);


    juce::Image knobImage = juce::ImageCache::getFromMemory (BinaryData::knob_png, BinaryData::knob_pngSize);
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    juce::String unit;
    bool drawFromMiddle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImgKnob)
};
