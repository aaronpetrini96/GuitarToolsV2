//
// Created by Aaron Petrini on 17/09/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <utility>

#include "../../AALook.h"

class AAIconAndTextButton : public juce::Button
{
public:
    AAIconAndTextButton (juce::RangedAudioParameter& parameterToUse, const void* iconData, const int iconDataSize, juce::String newOffText, juce::String newOnText)
        : juce::Button ({}), parameter (parameterToUse), offText (std::move (newOffText)), onText (std::move (newOnText))
    {
        setComponentID (parameter.getParameterID());
        setClickingTogglesState (true);

        if (const auto icon = juce::Drawable::createFromImageData (iconData, (size_t) iconDataSize))
        {
            iconOff = icon->createCopy();
            iconOver = icon->createCopy();
            iconOn = icon->createCopy();
            iconOff->replaceColour (GTColors::activeNeonBlue, GTColors::iconNormalColor);
            iconOver->replaceColour (GTColors::activeNeonBlue, GTColors::iconOverColor);
        }

        textLabel.setJustificationType (juce::Justification::centredTop);
        textLabel.setInterceptsMouseClicks (false, false);
        textLabel.setBorderSize(juce::BorderSize<int> (0));
        addAndMakeVisible (textLabel);

        attachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, *this, nullptr);
        attachment->sendInitialUpdate();
        updateLabel();
    }

    void setUndoManager (juce::UndoManager& undoManager)
    {
        attachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, *this, &undoManager);
        attachment->sendInitialUpdate();
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto iconSize = boundsF.getHeight() * 0.6f;
        const auto textBounds = boundsF.withTop (iconSize);

        iconBounds = boundsF.withSizeKeepingCentre (iconSize, iconSize).withY (0.f);
        textLabel.setFont (AAFonts::ibmPlexSansMedium (textBounds.getHeight() * 0.5f));
        textLabel.setBounds (textBounds.toNearestInt());
    }

    void paintButton (juce::Graphics& g, const bool shouldDrawButtonAsHighlighted, const bool shouldDrawButtonAsDown) override
    {
        const auto isOn = getToggleState() || shouldDrawButtonAsDown;
        auto* icon = isOn ? iconOn.get() : (shouldDrawButtonAsHighlighted ? iconOver.get() : iconOff.get());

        if (icon != nullptr)
            icon->drawWithin (g, iconBounds, juce::RectanglePlacement::centred, 0.85f);
    }

private:
    void buttonStateChanged() override
    {
        updateLabel();
    }

    void updateLabel()
    {
        const auto isOn = getToggleState() || isDown();
        const auto textColour = isOn ? GTColors::activeNeonBlue : (isOver() ? GTColors::iconOverColor : GTColors::almostWhite.withAlpha (0.9f));


        textLabel.setText (getToggleState() ? onText : offText, juce::dontSendNotification);
        textLabel.setColour (juce::Label::textColourId, textColour);
    }

    juce::RangedAudioParameter& parameter;
    juce::String offText, onText;
    std::unique_ptr<juce::Drawable> iconOff, iconOver, iconOn;
    juce::Rectangle<float> iconBounds;
    juce::Label textLabel;
    std::unique_ptr<juce::ButtonParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAIconAndTextButton)
};
