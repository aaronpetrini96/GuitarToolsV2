//
// Created by Aaron Petrini on 23/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>
#include <memory>
#include <utility>

#include "BinaryData.h"
#include "../../AALook.h"

class AASquareSSLButton : public juce::Component
{
public:
    explicit AASquareSSLButton (juce::RangedAudioParameter& parameterToUse, juce::String  newNameText)
        : parameter (parameterToUse), nameText (std::move(newNameText))
    {
        setComponentID (parameter.getParameterID());
        button.setImages (buttonOff.get(), nullptr, buttonDownOff.get(), nullptr, buttonOn.get(), nullptr, buttonDown.get());
        button.setClickingTogglesState (true);
        button.setColour (juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);
        button.setColour (juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        button.onClick = [this]
        {
            if (onClick != nullptr)
                onClick();
        };
        addAndMakeVisible (button);
        addAndMakeVisible (printedName);
        button.onStateChange = [this] { resized(); };

        attachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, button, nullptr);
        attachment->sendInitialUpdate();
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto buttonSize = juce::jmin (boundsF.getWidth(), boundsF.getHeight());
        button.setBounds (boundsF.withSizeKeepingCentre (buttonSize, buttonSize).toNearestInt());

        const auto textBounds = boundsF.withSizeKeepingCentre (boundsF.getWidth() * 0.65f, boundsF.getHeight() * 0.7f);
        const auto fontHeight = textBounds.getHeight() * 0.285f;
        printedName.setFont (button.getToggleState() ? AAFonts::interBold (fontHeight) : AAFonts::interSemiBold (fontHeight));
        printedName.setBounds (textBounds.toNearestInt());
    }

    bool getToggleState() const
    {
        return button.getToggleState();
    }

    void setUndoManager (juce::UndoManager& undoManager)
    {
        attachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, button, &undoManager);
        attachment->sendInitialUpdate();
    }

    void setTooltip (const juce::String& newTooltip)
    {
        button.setTooltip (newTooltip);
    }

    std::function<void()> onClick;

private:
    std::unique_ptr<juce::Drawable> buttonOn = juce::Drawable::createFromImageData (BinaryData::defON22Neutral_png, BinaryData::defON22Neutral_pngSize);
    std::unique_ptr<juce::Drawable> buttonOff = juce::Drawable::createFromImageData (BinaryData::defOff22Neutral_png, BinaryData::defOff22Neutral_pngSize);
    std::unique_ptr<juce::Drawable> buttonDown = juce::Drawable::createFromImageData (BinaryData::defDown22Neutral_png, BinaryData::defDown22Neutral_pngSize);
    std::unique_ptr<juce::Drawable> buttonDownOff = juce::Drawable::createFromImageData (BinaryData::defDownOff22Neutral_png, BinaryData::defDownOff22Neutral_pngSize);
    juce::DrawableButton button {{}, juce::DrawableButton::ImageFitted};
    juce::RangedAudioParameter& parameter;
    juce::String nameText;
    AAUITools::PrintedText printedName {nameText, juce::Colours::black};
    std::unique_ptr<juce::ButtonParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AASquareSSLButton)
};
