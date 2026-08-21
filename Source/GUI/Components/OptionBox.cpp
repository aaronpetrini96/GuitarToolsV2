/*
  ==============================================================================

    OptionBox.cpp
    Created: 21 Apr 2025 12:06:19am
    Author:  Aaron Petrini

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OptionBox.h"
#include "../LAF/LookAndFeel.h"

OptionBox::OptionBox(const std::vector<juce::String>& options, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID)
: apvtsRef(apvts), paramID(parameterID), optionStrings(options)
{
    
//    Fill combo box
    for (int i = 0; i < options.size(); ++i)
        comboBox.addItem(options[i], i+1);
    
    addAndMakeVisible(comboBox);
    comboBox.setSelectedId(1);
//    comboBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.1f));
    comboBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
    comboBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    comboBox.setBounds(0, 0, 80, 20);
    setSize(80, 20);
    setLookAndFeel(ComboBoxLookAndFeel::get());
    
    //    Attach to APVTS
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvtsRef, paramID.getParamID(), comboBox);
    
    restoreSavedValue();
        
}

OptionBox::~OptionBox() {}

void OptionBox::resized(){}

void OptionBox::restoreSavedValue()
{
    if (auto* param = apvtsRef.getParameter(paramID.getParamID()))
    {
        // Get the normalized 0.0–1.0 parameter value
        float normalized = param->getValue();

        // Map to 0-based index, clamp to valid range
        int index = static_cast<int>(normalized * (optionStrings.size() - 1) + 0.5f); // +0.5f for rounding
        index = juce::jlimit(0, static_cast<int>(optionStrings.size() - 1), index);

        // ComboBox IDs start at 1, so add 1
        int selectedId = index + 1;

        comboBox.setSelectedId(selectedId, juce::dontSendNotification);
    }
}
