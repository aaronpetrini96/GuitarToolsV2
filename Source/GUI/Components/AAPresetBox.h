//
// Created by Aaron Petrini on 19/09/2026.
//

#pragma once

#include <functional>

#include "../../GTLook.h"
#include "../../Utils/PresetManager.h"
#include "../../Utils/AAParamUtils.h"

class PresetBoxLAF : public juce::LookAndFeel_V4
{
public:
    PresetBoxLAF() = default;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& , bool , bool ) override {}


    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool highlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        g.setColour (highlighted ? GTColors::sectionTitleText.withMultipliedBrightness (0.6f) : GTColors::sectionTitleText);
        g.setFont (GTFonts::montserratSemiBold (boundsF.getHeight() * GTFontHeightPOH));
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
        nameButton.onClick = [this] { if (onNameClicked != nullptr) onNameClicked(); };
        nameButton.setTooltip (ParamTooltips::getPresetBoxTooltip());
        addAndMakeVisible (nameButton);

        AAUITools::setIconButtonImages (leftArrowButton, BinaryData::leftChevronArrow_svg, BinaryData::leftChevronArrow_svgSize, GTColors::iconSVGColor, GTColors::iconNormalColor, GTColors::iconOverColor, GTColors::activeNeonBlue);
        AAUITools::setIconButtonImages (rightArrowButton, BinaryData::rightChevronArrow_svg, BinaryData::rightChevronArrow_svgSize, GTColors::iconSVGColor, GTColors::iconNormalColor, GTColors::iconOverColor, GTColors::activeNeonBlue);
        leftArrowButton.onClick = [this] { presetManager.loadAdjacentPreset (-1); };
        rightArrowButton.onClick = [this] { presetManager.loadAdjacentPreset (1); };
        leftArrowButton.setTooltip (ParamTooltips::getPreviousPresetTooltip());
        rightArrowButton.setTooltip (ParamTooltips::getNextPresetTooltip());
        addAndMakeVisible (leftArrowButton);
        addAndMakeVisible (rightArrowButton);
    }

    ~AAPresetBox() override
    {
        nameButton.setLookAndFeel (nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        AAUITools::drawOutlinedRoundedRectangle (g, boundsF, boundsFHeight * GTCornerPOH, boundsFHeight * GTOutlineThicknessPOH, GTColors::presetBoxBackground, GTColors::presetBoxOutlineColor);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();

        const auto nameButtonBounds = boundsF.reduced (boundsFHeight, 0.0f);
        nameButton.setBounds (nameButtonBounds.toNearestInt());

        const auto arrowSize = boundsFHeight * 0.6f;
        leftArrowButton.setBounds (boundsF.withRight (nameButtonBounds.getX()).withSizeKeepingCentre (arrowSize, arrowSize).toNearestInt());
        rightArrowButton.setBounds (boundsF.withLeft (nameButtonBounds.getRight()).withSizeKeepingCentre (arrowSize, arrowSize).toNearestInt());
    }

    void setPresetName (const juce::String& presetName)
    {
        nameButton.setButtonText (presetName);
    }

    std::function<void()> onNameClicked;

private:
    PresetManager& presetManager;
    PresetBoxLAF presetBoxLAF;
    juce::TextButton nameButton;
    juce::DrawableButton leftArrowButton {"Previous preset", juce::DrawableButton::ImageFitted};
    juce::DrawableButton rightArrowButton {"Next preset", juce::DrawableButton::ImageFitted};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPresetBox)
};
