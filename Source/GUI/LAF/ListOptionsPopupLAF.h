//
// Created by Aaron Petrini on 19/09/2026.
//

#pragma once
#include "../../GTLook.h"

class ListOptionsPopupLAF : public juce::LookAndFeel_V4
{
public:
    ListOptionsPopupLAF () = default;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override {}

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, const bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * 0.15f;
        const auto textColour = button.getToggleState() ? GTPopupSelectedTextColor : GTPopupTextColor;

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (GTColors::presetBoxBackground);
            g.fillRoundedRectangle (boundsF.reduced (boundsF.getHeight() * GTOutlineThicknessPOH, 0.f), cornerSize);
        }

        g.setColour (textColour);
        if (!button.isEnabled())
            g.setColour (textColour.withAlpha (0.3f));
        g.setFont (GTFonts::montserratMedium (boundsF.getHeight() * GTFontHeightPOH));
        const auto textBounds = leftAlignText ? boundsF.withLeft (boundsF.getHeight() * 0.45f) : boundsF;
        g.drawText (button.getButtonText(), textBounds, leftAlignText ? juce::Justification::centredLeft : juce::Justification::centred, true);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (GTColors::presetBoxBackground);
            g.fillRoundedRectangle (boundsF.reduced (boundsFHeight * GTOutlineThicknessPOH, 0.f), boundsFHeight * 0.15f);
        }

        const auto tickBoxSize = boundsFHeight * 0.55f;
        const auto tickBoxBounds = boundsF.withSizeKeepingCentre (tickBoxSize, tickBoxSize).withRightX (boundsF.getRight() - boundsFHeight * 0.45f);
        const auto tickBoxLineThickness = tickBoxSize * 0.08f;
        g.setColour (button.getToggleState() ? GTPopupSelectedTextColor : (shouldDrawButtonAsHighlighted ? GTColors::iconOverColor : GTPopupTextColor));
        g.drawRoundedRectangle (tickBoxBounds, tickBoxSize * 0.2f, tickBoxLineThickness);

        if (button.getToggleState())
        {
            juce::Path tick;
            tick.startNewSubPath (tickBoxBounds.getRelativePoint (0.25f, 0.52f));
            tick.lineTo (tickBoxBounds.getRelativePoint (0.43f, 0.7f));
            tick.lineTo (tickBoxBounds.getRelativePoint (0.75f, 0.32f));
            g.setColour (GTPopupSelectedTextColor);
            g.strokePath (tick, juce::PathStrokeType (tickBoxLineThickness * 1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        g.setColour (GTPopupTextColor);
        g.setFont (GTFonts::montserratMedium (boundsFHeight * GTFontHeightPOH));
        g.drawText (button.getButtonText(), boundsF.withLeft (boundsFHeight * 0.45f).withRight (tickBoxBounds.getX()), juce::Justification::centredLeft, true);
    }

    bool leftAlignText {false};
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListOptionsPopupLAF)
};
