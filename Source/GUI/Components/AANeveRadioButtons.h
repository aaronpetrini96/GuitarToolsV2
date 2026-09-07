//
// Created by Aaron Petrini on 27/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <memory>

#include "BinaryData.h"
#include "GTLook.h"

class AANeveStyleRadioButtons : public juce::Component
{
public:
    AANeveStyleRadioButtons (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID)
    {
        for (auto index = 0; index < 4; ++index)   // the housing image has four recesses
        {
            auto* button = buttons.add (new juce::DrawableButton ({}, juce::DrawableButton::ImageFitted));

            button->setImages (capOff.get(), nullptr, nullptr, nullptr, capOn.get());
            button->setClickingTogglesState (true);
            button->setEdgeIndent (0);
            button->setColour (juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);
            button->setRadioGroupId (radioGroupId);

            addAndMakeVisible (button);
        }

        attachment = std::make_unique<juce::ParameterAttachment> (
            *apvts.getParameter (parameterID.getParamID()),
            [this] (float newValue)
            {
                buttons[juce::jlimit (0, buttons.size() - 1, juce::roundToInt (newValue))]
                    ->setToggleState (true, juce::dontSendNotification);
            },
            nullptr);

        for (auto index = 0; index < buttons.size(); ++index)
            buttons[index]->onClick = [this, index] { attachment->setValueAsCompleteGesture ((float) index); };

        attachment->sendInitialUpdate();
    }

    void paint (juce::Graphics& g) override
    {
        housing->drawWithin (g, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit, 1.0f);
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
    std::unique_ptr<juce::Drawable> housing = juce::Drawable::createFromImageData (BinaryData::neveRadioStrip_bg_png, BinaryData::neveRadioStrip_bg_pngSize);
    std::unique_ptr<juce::Drawable> capOff = juce::Drawable::createFromImageData (BinaryData::neveRadioCap_off_v2_png, BinaryData::neveRadioCap_off_v2_pngSize);
    std::unique_ptr<juce::Drawable> capOn = juce::Drawable::createFromImageData (BinaryData::neveRadioCap_on_v2_png, BinaryData::neveRadioCap_on_v2_pngSize);
    juce::OwnedArray<juce::DrawableButton> buttons;
    std::unique_ptr<juce::ParameterAttachment> attachment;
    int radioGroupId {188889};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AANeveStyleRadioButtons)
};
