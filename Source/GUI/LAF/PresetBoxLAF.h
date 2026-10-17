//
// Created by Aaron Petrini on 19/09/2026.
//

#pragma once
#include "../../GTLook.h"

class PresetBoxLAF : public juce::LookAndFeel_V4
{
public:
    PresetBoxLAF() = default;

    void drawBoxBackground (juce::Graphics& g, const juce::Rectangle<float> bounds)
    {
        const auto outlineThickness = bounds.getHeight() * GTOutlineThicknessPOH;
        const auto cornerSize = bounds.getHeight() * GTCornerPOH;
        AAUITools::drawOutlinedRoundedRectangle (g, bounds, cornerSize, outlineThickness, GTColors::presetBoxBackground, GTColors::presetBoxOutlineColor);
        // AAUITools::drawOutlinedRoundedRectangle (g, bounds, 0.1F, 0.f, GTColors::almostWhite.withAlpha(0.175f), GTColors::transparent);
    }

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& , bool , bool ) override
    {
    }

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
