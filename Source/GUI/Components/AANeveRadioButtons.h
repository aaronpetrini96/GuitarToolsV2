//
// Created by Aaron Petrini on 27/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <memory>

#include "BinaryData.h"
#include "../../Utils/GTLook.h"

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
                repaint();
            };

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
        housing->drawWithin (g, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit, 1.f);
    }

    void paintOverChildren (juce::Graphics& g) override
    {
        const auto textCount = juce::jmin (buttons.size(), nameText.size());
        for (auto index = 0; index < textCount; ++index)
        {
            const auto textBounds = buttons[index]->getBounds().toFloat();
            const auto isSelected = buttons[index]->getToggleState();
            const auto selectedFont = GTFonts::interBold (textBounds.getHeight() * 0.475f).withExtraKerningFactor(0.01f);
            const auto unselectedFont = GTFonts::interMedium (textBounds.getHeight() * 0.475f).withExtraKerningFactor(0.01f);
            g.setColour (isSelected ? juce::Colours::black : juce::Colours::black.withAlpha (0.9f));
            g.setFont (isSelected ? selectedFont : unselectedFont);
            g.drawFittedText (nameText[index], textBounds.toNearestInt(), juce::Justification::centred, 1);
            const auto delta = textBounds.getWidth() * 0.01f;
            g.setColour (juce::Colours::black.withAlpha (0.1f));
            g.drawFittedText (nameText[index], textBounds.translated(delta, delta).toNearestInt(), juce::Justification::centred, 1);
        }
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto capsArea = boundsF.withSizeKeepingCentre (boundsFWidth * capsToWidthRatio, boundsFHeight * capsToHeightRatio);
        const auto cellWidth = capsArea.getWidth() / (float) buttons.size();

        for (auto index = 0; index < buttons.size(); ++index)
            buttons[index]->setBounds (capsArea.withWidth (cellWidth).withX (capsArea.getX() + cellWidth * (float) index).toNearestIntEdges());
    }

    float getImageAspectRatio() const
    {
        const auto imageBounds = housing->getDrawableBounds();
        return imageBounds.getWidth() / imageBounds.getHeight();
    }

    // the share of the housing image the caps sit on, the rest being the frame around them
    float capsToWidthRatio {0.9354f}, capsToHeightRatio {0.8296f};

private:
    void createAttachment (juce::UndoManager* undoManager)
    {
        attachment = std::make_unique<juce::ParameterAttachment> (
            parameter,
            [this] (float newValue)
            {
                buttons[juce::jlimit (0, buttons.size() - 1, juce::roundToInt (newValue))]
                    ->setToggleState (true, juce::dontSendNotification);

                repaint();
            },
            undoManager);

        attachment->sendInitialUpdate();
    }

    std::unique_ptr<juce::Drawable> housing = juce::Drawable::createFromImageData (BinaryData::neveRadioStrip_bg_png, BinaryData::neveRadioStrip_bg_pngSize);
    std::unique_ptr<juce::Drawable> capOff = juce::Drawable::createFromImageData (BinaryData::neveRadioCap_off_v2_png, BinaryData::neveRadioCap_off_v2_pngSize);
    std::unique_ptr<juce::Drawable> capOn = juce::Drawable::createFromImageData (BinaryData::neveRadioCap_on_v2_png, BinaryData::neveRadioCap_on_v2_pngSize);
    juce::RangedAudioParameter& parameter;
    juce::StringArray nameText;
    juce::OwnedArray<juce::DrawableButton> buttons;
    std::unique_ptr<juce::ParameterAttachment> attachment;
    int radioGroupId {188889};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AANeveRadioButtons)
};
