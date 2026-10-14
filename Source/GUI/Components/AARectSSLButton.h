//
// Created by Aaron Petrini on 13/10/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "AALook.h"

class AARectSSLButton : public juce::Component
{
public:
    explicit AARectSSLButton (juce::RangedAudioParameter& parameterToUse, juce::String newNameText)
        : parameter (parameterToUse), nameText (std::move (newNameText))
    {
        setComponentID (parameter.getParameterID());
        button.setImages (imgOFF.get(), nullptr, nullptr, nullptr, imgON.get());
        button.setClickingTogglesState (true);
        button.setColour (juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);
        button.setColour (juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible (button);
        addAndMakeVisible (printedName);
        button.onStateChange = [this] { resized(); };

        buttonAttachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, button, nullptr);
        buttonAttachment->sendInitialUpdate();
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto imageBounds = juce::RectanglePlacement (juce::RectanglePlacement::centred).appliedTo (imgOFF->getDrawableBounds(), boundsF);
        button.setBounds (imageBounds.toNearestInt());

        const auto imageBoundsHeight = imageBounds.getHeight();
        const auto faceCentreYPOH = button.getToggleState() ? 0.5f : 0.42f;
        const auto textBounds = imageBounds.withSizeKeepingCentre (imageBounds.getWidth() * 0.8f, imageBoundsHeight * 0.4f)
                                           .withCentre ({imageBounds.getCentreX(), imageBounds.getY() + imageBoundsHeight * faceCentreYPOH});
        printedName.setFont (AAFonts::interSemiBold (textBounds.getHeight()));
        printedName.setBounds (textBounds.toNearestInt());
    }

    void setUndoManager (juce::UndoManager& undoManager)
    {
        buttonAttachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, button, &undoManager);
        buttonAttachment->sendInitialUpdate();
    }

    void setTooltip (const juce::String& newTooltip)
    {
        button.setTooltip (newTooltip);
    }

private:
    std::unique_ptr<juce::Drawable> imgON {juce::Drawable::createFromImageData(BinaryData::button2ONBetter_png, BinaryData::button2ONBetter_pngSize)};
    std::unique_ptr<juce::Drawable> imgOFF {juce::Drawable::createFromImageData(BinaryData::button2OFFBetter_png, BinaryData::button2OFFBetter_pngSize)};
    juce::DrawableButton button{{}, juce::DrawableButton::ImageFitted};
    juce::RangedAudioParameter& parameter;
    juce::String nameText{};
    AAUITools::PrintedText printedName{nameText, juce::Colours::black};
    std::unique_ptr<juce::ButtonParameterAttachment> buttonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AARectSSLButton)
};