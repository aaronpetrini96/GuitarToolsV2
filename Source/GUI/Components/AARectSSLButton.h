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
        paintLight (g);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto lightArea = boundsF.withTop (boundsF.getBottom() - boundsF.getHeight() * 0.32f);
        imageBounds = juce::RectanglePlacement (juce::RectanglePlacement::centred).appliedTo (imgOFF->getDrawableBounds(), boundsF.withBottom (lightArea.getY()));
        const auto lightSize = lightArea.getHeight() * 0.9f;
        lightBounds = lightArea.withSizeKeepingCentre (lightSize, lightSize);
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
    void paintLight (juce::Graphics& g)
    {
        const auto ledColour = GTColors::activeNeonBlue;
        const auto litAmount = getToggleState() ? 1.0f : 0.0f;
        const auto ledCentre = lightBounds.getCentre();
        const auto ledDiameter = lightBounds.getWidth() * 0.8f;
        const auto ledBounds = lightBounds.withSizeKeepingCentre (ledDiameter, ledDiameter);

        if (litAmount > 0.0f)
        {
            const auto glowRadius = ledDiameter * 0.9f;
            g.setGradientFill (juce::ColourGradient (ledColour.withAlpha (0.2f * litAmount), ledCentre, ledColour.withAlpha (0.0f), ledCentre.translated (glowRadius, 0.0f), true));
            g.fillEllipse (ledBounds.withSizeKeepingCentre (glowRadius * 2.0f, glowRadius * 2.0f));
        }

        g.setColour (GTColors::almostBlack);
        g.fillEllipse (lightBounds);

        const auto centreColour = ledColour.withMultipliedBrightness (0.4f).interpolatedWith (ledColour.interpolatedWith (juce::Colours::white, 0.5f), litAmount);
        const auto edgeColour = ledColour.withMultipliedBrightness (0.15f).interpolatedWith (ledColour, litAmount);
        g.setGradientFill (juce::ColourGradient (centreColour, ledCentre.translated (0.0f, ledDiameter * 0.3f), edgeColour, { ledCentre.x, ledBounds.getY() }, true));
        g.fillEllipse (ledBounds);

        g.setColour (juce::Colours::white.withAlpha (0.55f));
        g.fillEllipse (ledBounds.withSizeKeepingCentre (ledDiameter * 0.15f, ledDiameter * 0.12f).translated (0.0f, -ledDiameter * 0.22f));
    }

    void updateLabel()
    {
        const auto imageBoundsHeight = imageBounds.getHeight();
        const auto faceCentreYPOH = getToggleState() ? 0.5f : 0.42f;
        const auto textBounds = imageBounds.withSizeKeepingCentre (imageBounds.getWidth(), imageBoundsHeight * 0.5f)
                                           .withCentre ({imageBounds.getCentreX(), imageBounds.getY() + imageBoundsHeight * faceCentreYPOH});
        printedName.setFont (GTFonts::interBold (textBounds.getHeight() * 0.95f));
        printedName.setBounds (textBounds.toNearestInt());
    }

    juce::Rectangle<float> imageBounds, lightBounds;
    std::unique_ptr<juce::Drawable> imgON {juce::Drawable::createFromImageData(BinaryData::button2ONBetter_png, BinaryData::button2ONBetter_pngSize)};
    std::unique_ptr<juce::Drawable> imgOFF {juce::Drawable::createFromImageData(BinaryData::button2OFFBetter_png, BinaryData::button2OFFBetter_pngSize)};
    juce::RangedAudioParameter& parameter;
    juce::String nameText{};
    AAUITools::PrintedText printedName{nameText, juce::Colours::black};
    std::unique_ptr<juce::ButtonParameterAttachment> buttonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AARectSSLButton)
};
