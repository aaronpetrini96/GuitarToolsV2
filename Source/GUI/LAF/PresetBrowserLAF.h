//
// Created by Aaron Petrini on 03/09/2026.
//

#pragma once
#include "../../GTLook.h"

class PresetBrowserLAF : public juce::LookAndFeel_V4
{
public:
    PresetBrowserLAF()
    {
        setColour (juce::PopupMenu::textColourId, GTPopupTextColor);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, GTColors::presetBoxBackground);
        setColour (juce::PopupMenu::highlightedTextColourId, GTPopupTextColor);
    }

    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override
    {
        g.fillAll (GTPopupBackgroundColor);
        g.setColour (GTPopupOutlineColor);
        g.drawRect (juce::Rectangle<int> (width, height).toFloat(), 1.0f);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& , bool isHighlighted, bool) override
    {
        //bounds
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * GTCornerPOH;
        const auto outlineThickness = boundsF.getHeight() * GTOutlineThicknessPOH;

        //bool checkers
        const auto isTabButton = button.getConnectedEdgeFlags() != 0;
        const auto isSelected = button.getToggleState();

        if (isTabButton)
        {
            const auto tabLineThickness = boundsF.getHeight() / GTPopupTitleRowRatio * GTOutlineThicknessPOH * 0.5f;
            juce::Path tabShape;
            tabShape.addRoundedRectangle (boundsF.getX(), boundsF.getY(), boundsF.getWidth(), boundsF.getHeight(), cornerSize, cornerSize,
                                          button.isConnectedOnRight(), button.isConnectedOnLeft(), false, false);

            g.setColour (isSelected ? GTColors::presetBoxBackground : GTPopupBackgroundColor);
            g.fillPath (tabShape);

            g.setColour (GTPopupOutlineColor);
            g.fillRect (boundsF.withTop (boundsF.getBottom() - tabLineThickness));

            if (button.isConnectedOnRight())
                g.fillRect (boundsF.withLeft (boundsF.getRight() - tabLineThickness * 0.5f));

            if (button.isConnectedOnLeft())
                g.fillRect (boundsF.withRight (boundsF.getX() + tabLineThickness * 0.5f));
        }

        if (!isTabButton && isHighlighted)
        {
            g.setColour (GTColors::presetBoxBackground);
            g.fillRoundedRectangle (boundsF.reduced (outlineThickness, 0.f), cornerSize);
        }
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isHighlighted, bool) override
    {
        //bounds
        const auto boundsF = button.getLocalBounds().toFloat();
        //bool checkers
        const auto isTabButton = button.getConnectedEdgeFlags() != 0;
        const auto isSelected = button.getToggleState();

        //Colors
        const auto tabButtonTextColour = isSelected || isHighlighted ? GTPopupTextColor.withBrightness (2.f) : GTPopupTextColor.withMultipliedAlpha (0.4f);
        const auto presetTextColour = isSelected ? GTPopupSelectedTextColor : GTPopupTextColor;
        const auto buttonTextColour = isTabButton ? tabButtonTextColour : presetTextColour;

        g.setColour (buttonTextColour);
        g.setFont (GTFonts::montserratMedium (boundsF.getHeight() * GTFontHeightPOH));
        g.drawText (isTabButton ? button.getButtonText().toUpperCase() : button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserLAF)
};
