//
// Created by Aaron Petrini on 30/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "../LAF/AAFaderLAF.h"

class AAFaderSSL : public  juce::Slider
{
public:
    AAFaderSSL (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID,
                const juce::String& suffix = {}, const int numberOfDecimalPlaces = 0)
        : attachment (apvts, parameterID.getParamID(), *this)
    {
        setSliderStyle (juce::Slider::LinearVertical);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setNumDecimalPlacesToDisplay (numberOfDecimalPlaces);
        setTextValueSuffix (suffix);
        setLookAndFeel (&laf);
    }

    ~AAFaderSSL() override
    {
        setLookAndFeel (nullptr);
    }

    juce::String getTextFromValue (double value) override
    {
        if (getNumDecimalPlacesToDisplay() == 0)
            return juce::String (juce::roundToInt (value)) + getTextValueSuffix();

        return juce::String (value, getNumDecimalPlacesToDisplay()) + getTextValueSuffix();
    }

private:
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    AAFaderLAF laf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AAFaderSSL)
};
