/*
  ==============================================================================

    ImgButton.cpp
    Created: 30 Apr 2025 3:18:17am
    Author:  Aaron Petrini

  ==============================================================================
*/

#include "ImgButton.h"
#include "GTLook.h"
#include "BinaryData.h"



//==============================================================================
ImgButton::ImgButton(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID parameterID)
: attachment(apvts, parameterID.getParamID(), button)
{
    
    auto bypassIcon = juce::ImageCache::getFromMemory(BinaryData::Bypass_png, BinaryData::Bypass_pngSize);
    button.setClickingTogglesState(true);
    button.setBounds(0, 0, 20, 20);
    button.setImages(false, true, true,
                           bypassIcon, 1.f, GTColors::accentActive,
                           bypassIcon, 1.f, GTColors::accentActive,
                           bypassIcon, 1.f, juce::Colours::red,
                           0.f);
    button.onClick = [this]
    {
        
    };
    addAndMakeVisible(button);
    setSize(25, 25);
}

ImgButton::~ImgButton()
{
}

void ImgButton::paint (juce::Graphics& g)
{
}

void ImgButton::resized()
{
}
