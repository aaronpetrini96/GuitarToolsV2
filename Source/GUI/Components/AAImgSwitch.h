//
// Created by Aaron Petrini on 13/09/2026.
//

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../AALook.h"


class AAImgSwitch : public juce::Component
{
public:
    explicit AAImgSwitch (juce::RangedAudioParameter& parameterToUse)
        : parameter (parameterToUse)
    {
        setComponentID (parameter.getParameterID());

        if (imgPosition1 == nullptr && imgPosition2 == nullptr)
            return;
        const auto imgBounds = imgPosition1->getDrawableBounds();
        imgAspectRatio = imgBounds.getWidth() / imgBounds.getHeight();

        toggleSwitch.setImages (imgPosition1.get(), nullptr, imgPosition2.get(), nullptr, imgPosition2.get());
        toggleSwitch.setClickingTogglesState (true);
        toggleSwitch.setColour (juce::DrawableButton::backgroundColourId, GTColors::transparent);
        toggleSwitch.setColour (juce::DrawableButton::backgroundOnColourId, GTColors::transparent);
        addAndMakeVisible (toggleSwitch);

        label.setJustificationType (juce::Justification::centredTop);
        label.setColour (juce::Label::textColourId, GTColors::almostWhite.withAlpha (0.9f));
        label.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (label);

        createAttachment (nullptr);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto switchHeight = boundsF.getHeight() * 0.6f;
        const auto switchWidth = switchHeight * imgAspectRatio;
        const auto labelBounds = boundsF.withTop (switchHeight);

        toggleSwitch.setBounds (boundsF.withSizeKeepingCentre (switchWidth, switchHeight).withY (0.f).toNearestInt());
        label.setFont (AAFonts::ibmPlexSansMedium (labelBounds.getHeight() * 0.575f));
        label.setBounds (labelBounds.toNearestInt());
    }

    void setUndoManager (juce::UndoManager& undoManager)
    {
        createAttachment (&undoManager);
    }

    void setTooltip (const juce::String& newTooltip)
    {
        toggleSwitch.setTooltip (newTooltip);
    }

    void setLabelText (const juce::String& newText)
    {
        label.setText (newText, juce::dontSendNotification);
    }

    float imgAspectRatio {1.0f};

private:
    void createAttachment (juce::UndoManager* undoManager)
    {
        attachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, toggleSwitch, undoManager);
        attachment->sendInitialUpdate();
    }


    std::unique_ptr<juce::Drawable> imgPosition1 {juce::Drawable::createFromImageData(BinaryData::toggleSwitchLeft_png, BinaryData::toggleSwitchLeft_pngSize)};
    std::unique_ptr<juce::Drawable> imgPosition2 {juce::Drawable::createFromImageData(BinaryData::toggleSwitchRight_png, BinaryData::toggleSwitchRight_pngSize)};
    juce::DrawableButton toggleSwitch {"Switch", juce::DrawableButton::ImageFitted};
    juce::Label label;
    juce::RangedAudioParameter& parameter;
    std::unique_ptr<juce::ButtonParameterAttachment> attachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AAImgSwitch)
};
