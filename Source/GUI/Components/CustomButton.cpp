/*
  ==============================================================================

    Button.cpp
    Created: 21 Apr 2025 10:26:58pm
    Author:  Aaron Petrini

  ==============================================================================p
*/

#include <JuceHeader.h>
#include "CustomButton.h"


//==============================================================================
CustomButton::CustomButton(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID parameterID)
: attachment(apvts, parameterID.getParamID(), button)
{
    button.setButtonText(text);
//    button.setToggleable(true);
    button.setClickingTogglesState(true);
    
//    button.setColour(juce::ToggleButton::ColourIds::textColourId, juce::Colours::white);
    button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(100, 100, 110).darker(0.5f));
    button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(0xFF40C4FF).darker(1.f));
    button.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    button.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
    
    addAndMakeVisible(button);
    button.setBounds(0, 0, 120, 50);
    
    setSize(130, 60);
//    setLookAndFeel(ButtonLookAndFeel::get());
    
    // Forward the internal button's click to this class's onClick
    button.onClick = [this]()
    {
        if (onClick)
            onClick();  // Call external onClick if set
    };
}

CustomButton::~CustomButton()
{
}

void CustomButton::paint (juce::Graphics& g)
{
    
    auto bounds = button.getLocalBounds().toFloat();
    auto cornersize = bounds.getHeight() * 0.25f;

    currentBackground = button.getToggleState() ? backgroundColourWhenOn : backgroundColourWhenOff;

    auto baseColour = currentBackground.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                      .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

    if (button.isDown() || button.isMouseOverOrDragging())
        baseColour = baseColour.contrasting (button.isDown() ? 0.2f : 0.05f);

    g.setColour (baseColour);

    auto flatOnLeft   = button.isConnectedOnLeft();
    auto flatOnRight  = button.isConnectedOnRight();
    auto flatOnTop    = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
    {
        juce::Path path;
        path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                  bounds.getWidth(), bounds.getHeight(),
                                  cornersize, cornersize,
                                  ! (flatOnLeft  || flatOnTop),
                                  ! (flatOnRight || flatOnTop),
                                  ! (flatOnLeft  || flatOnBottom),
                                  ! (flatOnRight || flatOnBottom));

        g.fillPath (path);

        g.setColour (button.findColour (juce::ComboBox::outlineColourId));
        g.strokePath (path, juce::PathStrokeType (1.0f));
    }
    else
    {
        g.fillRoundedRectangle (bounds, cornersize);
        g.setColour (juce::Colours::transparentBlack);
        g.drawRoundedRectangle (bounds, cornersize, 1.0f);
    }
    
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText(getName(), getLocalBounds(), juce::Justification::centred);
 
}

void CustomButton::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


bool CustomButton::getToggleState()
{
    return button.getToggleState();
}
