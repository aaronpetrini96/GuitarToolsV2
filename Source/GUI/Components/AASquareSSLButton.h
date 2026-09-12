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

class AASquareSSLButton : public juce::Component
{
public:
    AASquareSSLButton (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, int numberOfButtons, bool isRadioButton = true, juce::UndoManager* undoManager = nullptr)
    {
        for (auto index = 0; index < numberOfButtons; ++index)
        {
            auto* button = buttons.add (new juce::DrawableButton ({}, juce::DrawableButton::ImageFitted));

            button->setImages (buttonOff.get(), nullptr, nullptr, nullptr, buttonOn.get());
            button->setClickingTogglesState (true);
            button->setColour (juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);

            if (isRadioButton)
                button->setRadioGroupId (radioGroupId);

            addAndMakeVisible (button);
        }

        attachment = std::make_unique<juce::ParameterAttachment> (
            *apvts.getParameter (parameterID.getParamID()),
            [this] (float newValue)
            {
                if (buttons.size() == 1)
                    buttons[0]->setToggleState (newValue > 0.5f, juce::dontSendNotification);
                else
                    buttons[juce::jlimit (0, buttons.size() - 1, juce::roundToInt (newValue))]
                        ->setToggleState (true, juce::dontSendNotification);

                if (onStateChange != nullptr)
                    onStateChange();
            },
            undoManager);

        for (auto index = 0; index < buttons.size(); ++index)
        {
            buttons[index]->onClick = [this, index]
            {
                const auto newValue = buttons.size() == 1
                                          ? (buttons[0]->getToggleState() ? 1.0f : 0.0f)
                                          : (float) index;
                attachment->setValueAsCompleteGesture (newValue);

                if (onStateChange != nullptr)
                    onStateChange();
            };
        }

        attachment->sendInitialUpdate();
    }



    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto cellWidth = boundsFWidth / (float) buttons.size();
        const auto gapFactor = buttons.size() > 1 ? 1.0f + gapToButtonSizeRatio : 1.0f;
        const auto buttonSize = juce::jmin (cellWidth / gapFactor, boundsFHeight);

        for (auto index = 0; index < buttons.size(); ++index)
            buttons[index]->setBounds (boundsF.withWidth (cellWidth) .withX (cellWidth * (float) index).withSizeKeepingCentre (buttonSize, buttonSize).toNearestInt());
    }

    bool getToggleState() const
    {
        jassert (buttons.size() == 1);
        return buttons[0]->getToggleState();
    }

    void setOnStateChange (std::function<void()> callback)
    {
        onStateChange = std::move (callback);
        onStateChange();
    }

    // 0.22 leaves the same spacing as the magic buttons
    float gapToButtonSizeRatio { 0.22f };

    // 0 turns them back into independent toggles
    void setRadioGroupId (const int newRadioGroupId)
    {
        radioGroupId = newRadioGroupId;

        for (auto* button : buttons)
            button->setRadioGroupId (radioGroupId);
    }

private:
    std::unique_ptr<juce::Drawable> buttonOff = juce::Drawable::createFromImageData (BinaryData::radioButtonOff_png, BinaryData::radioButtonOff_pngSize);
    std::unique_ptr<juce::Drawable> buttonOn = juce::Drawable::createFromImageData (BinaryData::radioButtonOn_png, BinaryData::radioButtonOn_pngSize);
    juce::OwnedArray<juce::DrawableButton> buttons;
    std::unique_ptr<juce::ParameterAttachment> attachment;
    std::function<void()> onStateChange;
    int radioGroupId {188888};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AASquareSSLButton)
};
