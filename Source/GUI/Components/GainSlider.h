#pragma once

#include <JuceHeader.h>
#include "../LAF/LookAndFeel.h"

//==============================================================================
/*
*/
class GainSlider  : public juce::Component, public juce::LookAndFeel_V4
{
public:
    GainSlider(const juce::String& text,
                     juce::AudioProcessorValueTreeState& apvts,
                     const juce::ParameterID& parameterID,
                     const juce::String& suffix,
                     const juce::Slider::TextEntryBoxPosition textBoxPosition);
    ~GainSlider() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setClippingStatus(bool);
        
    juce::Slider slider;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

private:
//    const float pi = juce::MathConstants<float>::pi;
    bool isClipping = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainSlider)
};
