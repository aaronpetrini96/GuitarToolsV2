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
#include "../../GTLook.h"

class AASquareSSLButton : public juce::Button
{
public:
    explicit AASquareSSLButton (juce::RangedAudioParameter& parameterToUse, juce::String  newNameText)
        : juce::Button ({}), parameter (parameterToUse), nameText (std::move(newNameText))
    {
        setComponentID (parameter.getParameterID());
        setClickingTogglesState (true);
        addAndMakeVisible (printedName);
        attachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, *this, nullptr);
        attachment->sendInitialUpdate();
    }

    void paintButton (juce::Graphics& g, bool, bool shouldDrawButtonAsDown) override
    {
        const auto& image = getToggleState() ? buttonOn : buttonOff;
        const auto boundsF = getLocalBounds().toFloat();
        image->drawWithin (g, boundsF.reduced (boundsF.getHeight() * imgReducePOH), juce::RectanglePlacement::centred, 1.0f);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto textBounds = boundsF.withSizeKeepingCentre (boundsF.getWidth() * 0.65f, boundsF.getHeight() * 0.7f);
        printedName.setFont(GTFonts::interBold(textBounds.getHeight() * 0.295f));
        printedName.setBounds (textBounds.toNearestInt());
    }


    float imgReducePOH {0.04f};

private:
    std::unique_ptr<juce::Drawable> buttonOn = juce::Drawable::createFromImageData (BinaryData::defON22Neutral_png, BinaryData::defON22Neutral_pngSize);
    std::unique_ptr<juce::Drawable> buttonOff = juce::Drawable::createFromImageData (BinaryData::defOff22Neutral_png, BinaryData::defOff22Neutral_pngSize);
    juce::RangedAudioParameter& parameter;
    juce::String nameText;
    AAUITools::PrintedText printedName {nameText, juce::Colours::black};
    std::unique_ptr<juce::ButtonParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AASquareSSLButton)
};
