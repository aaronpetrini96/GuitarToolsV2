//
// Created by Aaron Petrini on 13/10/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "GTLook.h"

class AARectSSLButton : public juce::Button
{
public:
    explicit AARectSSLButton (juce::RangedAudioParameter& parameterToUse, juce::String newNameText)
        : juce::Button ({}), parameter (parameterToUse), nameText (std::move (newNameText))
    {
        setComponentID (parameter.getParameterID());
        setClickingTogglesState (true);
        addAndMakeVisible (printedName);

        buttonAttachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, *this, nullptr);
        buttonAttachment->sendInitialUpdate();
    }

    void paintButton (juce::Graphics& g, bool, bool) override
    {
        const auto& image = getToggleState() ? imgON : imgOFF;
        image->drawWithin (g, imageBounds, juce::RectanglePlacement::centred, 1.0f);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        imageBounds = juce::RectanglePlacement (juce::RectanglePlacement::centred).appliedTo (imgOFF->getDrawableBounds(), boundsF);
        updateLabel();
    }

    void buttonStateChanged() override
    {
        updateLabel();
    }

    void setUndoManager (juce::UndoManager& undoManager)
    {
        buttonAttachment = std::make_unique<juce::ButtonParameterAttachment> (parameter, *this, &undoManager);
        buttonAttachment->sendInitialUpdate();
    }

private:
    void updateLabel()
    {
        const auto imageBoundsHeight = imageBounds.getHeight();
        const auto faceCentreYPOH = getToggleState() ? 0.5f : 0.42f;
        const auto textBounds = imageBounds.withSizeKeepingCentre (imageBounds.getWidth(), imageBoundsHeight * 0.5f)
                                           .withCentre ({imageBounds.getCentreX(), imageBounds.getY() + imageBoundsHeight * faceCentreYPOH});
        printedName.setFont (GTFonts::interBold (textBounds.getHeight() * 0.875f));
        printedName.setBounds (textBounds.toNearestInt());
    }

    juce::Rectangle<float> imageBounds;
    std::unique_ptr<juce::Drawable> imgON {juce::Drawable::createFromImageData(BinaryData::button2ONBetter_png, BinaryData::button2ONBetter_pngSize)};
    std::unique_ptr<juce::Drawable> imgOFF {juce::Drawable::createFromImageData(BinaryData::button2OFFBetter_png, BinaryData::button2OFFBetter_pngSize)};
    juce::RangedAudioParameter& parameter;
    juce::String nameText{};
    AAUITools::PrintedText printedName{nameText, juce::Colours::black};
    std::unique_ptr<juce::ButtonParameterAttachment> buttonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AARectSSLButton)
};
