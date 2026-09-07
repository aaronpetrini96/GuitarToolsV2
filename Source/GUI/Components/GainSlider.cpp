/*
  ==============================================================================

    GainSlider.cpp
    Created: 7 May 2025 9:03:09pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#include "GainSlider.h"
//==============================================================================
GainSlider::GainSlider(const juce::String& text,
                                   juce::AudioProcessorValueTreeState& apvts,
                                   const juce::ParameterID& parameterID,
                                   const juce::String& suffix,
                                   const juce::Slider::TextEntryBoxPosition textBoxPosition)
: attachment(apvts, parameterID.getParamID(), slider)

{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxIsEditable(true);
    slider.setTextBoxStyle(textBoxPosition, false, 90, 20);
    slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId,juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::grey.withAlpha(0.3f));
    slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::red.withAlpha(0.8f));
    if (suffix != "") {
        slider.setTextValueSuffix(suffix);
    }
    setSize(200, 20);
    slider.setBounds(0, 0, 200, 20);
    addAndMakeVisible(slider);

    setLookAndFeel(RotaryKnobLookAndFeel::get());
    
    slider.setColour (juce::Slider::ColourIds::textBoxTextColourId, juce::Colours::white);
}

GainSlider::~GainSlider()
{
}

void GainSlider::paint (juce::Graphics& g)
{
    // Call the base class paint method to ensure the slider looks normal
    slider.paint(g);
}

void GainSlider::resized()
{
    slider.setBounds(getLocalBounds());
}

