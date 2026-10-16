//
// Created by Aaron Petrini on 19/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "BinaryData.h"
#include "../../GTLook.h"
#include "../../Utils/PresetManager.h"

class PresetBoxLAF : public juce::LookAndFeel_V4
{
public:
    PresetBoxLAF() = default;

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& , bool , bool ) override
    {
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool highlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();

        g.setColour (highlighted ? GTColors::sectionTitleText.withMultipliedBrightness(2.f) : GTColors::sectionTitleText);
        g.setFont (GTFonts::montserratMedium (boundsF.getHeight() * AAUITools::fontHeightPOH * 0.95f));
        g.drawFittedText (button.getButtonText(), boundsF.toNearestInt(), juce::Justification::centred, 1);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBoxLAF)
};

class AAPresetBox : public juce::Component
{
public:
    explicit AAPresetBox (PresetManager& presetManagerToUse) : presetManager (presetManagerToUse)
    {
        nameButton.setLookAndFeel (&presetBoxLAF);
        nameButton.setColour(juce::TextButton::ColourIds::buttonColourId, GTColors::transparent);
        nameButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, GTColors::transparent);
        nameButton.onClick = [this] { if (onNameClicked != nullptr) onNameClicked(); };
        addAndMakeVisible (nameButton);

        AAUITools::setIconButtonImages (leftArrowButton, BinaryData::leftChevronArrow_svg, BinaryData::leftChevronArrow_svgSize, GTColors::iconSVGColor, GTColors::iconNormalColor, GTColors::iconOverColor, GTColors::activeNeonBlue);
        AAUITools::setIconButtonImages (rightArrowButton, BinaryData::rightChevronArrow_svg, BinaryData::rightChevronArrow_svgSize, GTColors::iconSVGColor, GTColors::iconNormalColor, GTColors::iconOverColor, GTColors::activeNeonBlue);
        leftArrowButton.onClick = [this] { presetManager.loadAdjacentPreset (-1); };
        rightArrowButton.onClick = [this] { presetManager.loadAdjacentPreset (1); };
        addAndMakeVisible (leftArrowButton);
        addAndMakeVisible (rightArrowButton);
    }

    ~AAPresetBox() override
    {
        nameButton.setLookAndFeel (nullptr);
    }

    void setPresetName (const juce::String& presetName)
    {
        nameButton.setButtonText (presetName);
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto outlineThickness = boundsF.getHeight() * AAUITools::outlineThicknessPOH * 0.75f;
        const auto corner = boundsF.getHeight() * AAUITools::cornerPOH;
        g.setColour(GTColors::presetBoxBackground);
        g.fillRoundedRectangle(boxBounds, corner);
        g.setColour (GTColors::almostBlack);
        g.drawRoundedRectangle (boxBounds.reduced (outlineThickness * 0.5f), corner, outlineThickness);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        boxBounds = boundsF.withSizeKeepingCentre (boundsFWidth * 0.7f, boundsFHeight);
        const auto nameButtonBounds = boxBounds.reduced (boundsFHeight, 0.0f);
        nameButton.setBounds (nameButtonBounds.toNearestInt());

        const auto arrowSize = boundsFHeight * 0.6f;
        leftArrowButton.setBounds (boxBounds.withRight (nameButtonBounds.getX()).withSizeKeepingCentre (arrowSize, arrowSize).toNearestInt());
        rightArrowButton.setBounds (boxBounds.withLeft (nameButtonBounds.getRight()).withSizeKeepingCentre (arrowSize, arrowSize).toNearestInt());
    }

    std::function<void()> onNameClicked;

private:
    PresetManager& presetManager;
    PresetBoxLAF presetBoxLAF;
    juce::TextButton nameButton;
    juce::DrawableButton leftArrowButton {"Previous preset", juce::DrawableButton::ImageFitted};
    juce::DrawableButton rightArrowButton {"Next preset", juce::DrawableButton::ImageFitted};
    juce::Rectangle<float> boxBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPresetBox)
};
