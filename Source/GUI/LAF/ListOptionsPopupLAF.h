//
// Created by Aaron Petrini on 19/09/2026.
//

#pragma once
#include "../../GTLook.h"

class ListOptionsPopupLAF : public juce::LookAndFeel_V4
{
public:
    ListOptionsPopupLAF () = default;

    void drawListOptionsBackground (juce::Graphics& g, juce::Rectangle<float> bounds, const float numRows, const bool drawRowDividers)
    {
        const auto rowHeight = bounds.getHeight() / numRows;
        listCornerSize = bounds.getHeight() * GTCornerPOH;
        outlineThickness = rowHeight * GTOutlineThicknessPOH;

        g.setColour (GTPopupBackgroundColor);
        g.fillRoundedRectangle (bounds, listCornerSize);

        if (drawRowDividers)
        {
            const auto dividerBounds = bounds.withSizeKeepingCentre (bounds.getWidth() * 0.9f, rowHeight * 0.06f);
            g.setColour (GTPopupOutlineColor);

            for (int rowIndex = 1; rowIndex < (int) numRows; ++rowIndex)
                g.fillRoundedRectangle (dividerBounds.withCentre ({ bounds.getCentreX(), bounds.getBottom() - rowHeight * (float) rowIndex }), dividerBounds.getHeight() * 0.5f);
        }
    }

    void drawListOptionsOutline (juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        g.setColour (GTPopupOutlineColor);
        g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), listCornerSize, outlineThickness);
    }

    void drawListOptionsTitle (juce::Graphics& g, juce::Rectangle<float> titleBounds, const juce::String& title)
    {
        const auto titleDividerThickness = outlineThickness * 0.5f;
        juce::Path titleShape;
        titleShape.addRoundedRectangle (titleBounds.getX(), titleBounds.getY(), titleBounds.getWidth(), titleBounds.getHeight(), listCornerSize, listCornerSize,
                                        true, true, false, false);
        g.setColour (GTColors::presetBoxBackground);
        g.fillPath (titleShape);

        g.setColour (GTPopupOutlineColor);
        g.fillRect (titleBounds.withTop (titleBounds.getBottom() - titleDividerThickness));

        g.setColour (GTPopupTextColor.withBrightness (2.f));
        g.setFont (GTFonts::montserratSemiBold (titleBounds.getHeight() * GTFontHeightPOH));
        g.drawText (title, titleBounds, juce::Justification::centred, true);
    }

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override {}

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, const bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * 0.15f;
        const auto textColour = button.getToggleState() ? GTPopupSelectedTextColor : GTPopupTextColor;

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (GTColors::presetBoxBackground);
            g.fillRoundedRectangle (boundsF.reduced (outlineThickness, 0.f), cornerSize);
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
        g.setColour (button.getToggleState() ? GTPopupSelectedTextColor : GTPopupTextColor);
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
    float outlineThickness {0.f};
    float listCornerSize {0.f};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListOptionsPopupLAF)
};
