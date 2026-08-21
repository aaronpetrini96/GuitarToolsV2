#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OptionBox  : public juce::Component
{
public:
    OptionBox(const std::vector <juce::String>& options, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID);
    ~OptionBox() override;

//    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::ComboBox comboBox;
//    juce::AudioProcessorValueTreeState::ComboBoxAttachment attachment;
    juce::Label label;

private:
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::ParameterID paramID;
    std::vector<juce::String> optionStrings;
    
    void restoreSavedValue();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionBox)
};
