//
// Created by Aaron Petrini on 21/08/2026.
//

#pragma once

#include <JuceHeader.h>

class ImgKnob : public juce::Slider
{
public:
    ImgKnob (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const juce::String& suffix = {});

    void paint (juce::Graphics& g) override;

private:
    juce::Image knobImage = juce::ImageCache::getFromMemory (BinaryData::knob_png, BinaryData::knob_pngSize);
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    juce::String unit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImgKnob)
};
