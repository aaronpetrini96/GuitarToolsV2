//
// Created by Aaron Petrini on 27/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <memory>

#include "BinaryData.h"
#include "../../AALook.h"

class AANeveRadioButtons : public juce::Component
{
public:
    AANeveRadioButtons (juce::RangedAudioParameter& parameterToUse, const juce::StringArray& newNameText = {})
        : parameter (parameterToUse), nameText (newNameText)
    {
        setComponentID (parameter.getParameterID());

        for (auto index = 0; index < 4; ++index)
        {
            auto* button = buttons.add (new juce::DrawableButton ({}, juce::DrawableButton::ImageFitted));
            button->setImages (capOff.get(), nullptr, nullptr, nullptr, capOn.get());
            button->setClickingTogglesState (true);
            button->setEdgeIndent (0);
            button->setColour (juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);
            button->setRadioGroupId (radioGroupId);
            addAndMakeVisible (button);
        }

        for (auto index = 0; index < buttons.size(); ++index)
            buttons[index]->onClick = [this, index]
            {
                attachment->setValueAsCompleteGesture ((float) index);
                updateNames();
            };

        for (auto index = 0; index < juce::jmin (buttons.size(), nameText.size()); ++index)
            addAndMakeVisible (printedNames.add (new AAUITools::PrintedText (nameText[index], juce::Colours::black)));

        createAttachment (nullptr);
    }

    void setUndoManager (juce::UndoManager& undoManager)
    {
        createAttachment (&undoManager);
    }

    void setTooltip (const juce::String& newTooltip)
    {
        for (auto* button : buttons)
            button->setTooltip (newTooltip);
    }

    void paint (juce::Graphics& g) override
    {
        housing->drawWithin (g, housingBounds, juce::RectanglePlacement::centred, 1.f);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        housingBounds = juce::RectanglePlacement (juce::RectanglePlacement::centred).appliedTo (housing->getDrawableBounds(), boundsF);

        const auto capsArea = housingBounds.withSizeKeepingCentre (housingBounds.getWidth() * capsToWidthRatio, housingBounds.getHeight() * capsToHeightRatio);
        const auto cellWidth = capsArea.getWidth() / (float) buttons.size();

        for (auto index = 0; index < buttons.size(); ++index)
            buttons[index]->setBounds (capsArea.withWidth (cellWidth).withX (capsArea.getX() + cellWidth * (float) index).toNearestIntEdges());

        updateNames();
    }

private:
    void updateNames()
    {
        for (auto index = 0; index < printedNames.size(); ++index)
        {
            const auto textBounds = buttons[index]->getBounds().toFloat();
            const auto isSelected = buttons[index]->getToggleState();
            const auto fontHeight = textBounds.getHeight() * 0.5f;
            printedNames[index]->setFont (isSelected ? AAFonts::interBold (fontHeight) : AAFonts::interSemiBold (fontHeight));
            printedNames[index]->setAlpha (isSelected ? 1.0f : 0.9f);
            printedNames[index]->setBounds (textBounds.toNearestInt());
        }
    }

    // the share of the housing image the caps sit on, the rest being the frame and shadow around them
    float capsToWidthRatio {0.9095f}, capsToHeightRatio {0.6401f};
    juce::Rectangle<float> housingBounds;

    void createAttachment (juce::UndoManager* undoManager)
    {
        attachment = std::make_unique<juce::ParameterAttachment> (
            parameter,
            [this] (float newValue)
            {
                buttons[juce::jlimit (0, buttons.size() - 1, juce::roundToInt (newValue))]
                    ->setToggleState (true, juce::dontSendNotification);

                updateNames();
            },
            undoManager);

        attachment->sendInitialUpdate();
    }

    std::unique_ptr<juce::Drawable> housing = juce::Drawable::createFromImageData (BinaryData::neveRadioStrip_bg_png, BinaryData::neveRadioStrip_bg_pngSize);
    std::unique_ptr<juce::Drawable> capOff = juce::Drawable::createFromImageData (BinaryData::neveRadioCap_off_v2_png, BinaryData::neveRadioCap_off_v2_pngSize);
    std::unique_ptr<juce::Drawable> capOn = juce::Drawable::createFromImageData (BinaryData::neveRadioCap_on_v3_png, BinaryData::neveRadioCap_on_v3_pngSize);
    juce::RangedAudioParameter& parameter;
    juce::StringArray nameText;
    juce::OwnedArray<juce::DrawableButton> buttons;
    juce::OwnedArray<AAUITools::PrintedText> printedNames;
    std::unique_ptr<juce::ParameterAttachment> attachment;
    int radioGroupId {188889};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AANeveRadioButtons)
};
