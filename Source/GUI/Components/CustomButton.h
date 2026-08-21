/*
  ==============================================================================

    Button.h
    Created: 21 Apr 2025 10:26:58pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../LAF/LookAndFeel.h"
//==============================================================================
/*
*/
class CustomButton  : public juce::Component
{
public:
    CustomButton(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID parameterID);
    ~CustomButton() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool getToggleState();
    std::function<void()> onClick;
    
    

private:
//    juce::ToggleButton button;
    juce::TextButton button;
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment;
    juce::Label label;
    
    const juce::Colour backgroundColourWhenOff = juce::Colour(100, 100, 110).darker(1.2f);
    const juce::Colour backgroundColourWhenOn = juce::Colour(0xFF40C4FF).darker(1.5f);
    juce::Colour currentBackground {backgroundColourWhenOff};

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomButton)
};


