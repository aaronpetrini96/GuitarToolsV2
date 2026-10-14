//
// Created by Aaron Petrini on 19/09/2026.
//

#pragma once
#include "../../AALook.h"

class ListOptionsPopupLAF : public juce::LookAndFeel_V4
{
public:
    enum ColourIds
    {
        backgroundColourId = 0x2a10001,
        outlineColourId    = 0x2a10002,
        dividerColourId    = 0x2a10003,
        highlightColourId  = 0x2a10004,
        titleBackgroundColourId = 0x2a10005,
        titleTextColourId  = 0x2a10006
    };

    ListOptionsPopupLAF()
    {
        setColour (backgroundColourId, GTColors::popupDarkBackground);
        setColour (outlineColourId, GTColors::dividerLinesStart);
        setColour (dividerColourId, GTColors::dividerLinesStart);
        setColour (highlightColourId, GTColors::presetPopupHighlight);
        setColour (titleBackgroundColourId, GTColors::tabButtonSelected);
        setColour (titleTextColourId, GTColors::sectionTitleText);
        setColour (juce::TextButton::textColourOffId, GTColors::sectionTitleText);
        setColour (juce::TextButton::textColourOnId, GTColors::activeNeonBlue);
    }

    void drawListOptionsBackground (juce::Graphics& g, juce::Rectangle<float> bounds, const float numRows, const bool drawRowDividers)
    {
        const auto rowHeight = bounds.getHeight() / numRows;
        const auto cornerSize = bounds.getHeight() * cornerSizePOH;
        outlineThickness = rowHeight * outlineThicknessPOH;

        g.setColour (findColour (backgroundColourId));
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour (findColour (outlineColourId));
        g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);

        if (drawRowDividers)
        {
            const auto dividerBounds = bounds.withSizeKeepingCentre (bounds.getWidth() * 0.9f, rowHeight * 0.06f);
            g.setColour (findColour (outlineColourId));

            for (int rowIndex = 1; rowIndex < (int) numRows; ++rowIndex)
                g.fillRoundedRectangle (dividerBounds.withCentre ({ bounds.getCentreX(), bounds.getBottom() - rowHeight * (float) rowIndex }), dividerBounds.getHeight() * 0.5f);
        }
    }

    void drawListOptionsTitle (juce::Graphics& g, juce::Rectangle<float> titleBounds, const juce::String& title)
    {
        const auto titleDividerThickness = outlineThickness * 0.5f;

        g.setColour (findColour (titleBackgroundColourId));
        g.fillRect (titleBounds.reduced (outlineThickness, 0.0f).withTrimmedTop (outlineThickness));

        g.setColour (findColour (outlineColourId));
        g.fillRect (titleBounds.withTop (titleBounds.getBottom() - titleDividerThickness));

        g.setColour (findColour (titleTextColourId));
        g.setFont (AAFonts::montserratMedium (titleBounds.getHeight() * titleTextPOH));
        g.drawText (title, titleBounds, juce::Justification::centred, true);
    }

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override {}

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, const bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * 0.15f;
        const auto textColour = findColour (button.getToggleState() ? juce::TextButton::textColourOnId : juce::TextButton::textColourOffId);

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (findColour (highlightColourId));
            g.fillRoundedRectangle (boundsF.reduced(outlineThickness), cornerSize);
        }

        g.setColour (shouldDrawButtonAsHighlighted ? textColour.withBrightness (2.f) : textColour);
        if (!button.isEnabled())
            g.setColour (textColour.withAlpha (0.3f));
        g.setFont (AAFonts::montserratMedium (boundsF.getHeight() * textPOH));
        const auto textBounds = leftAlignText ? boundsF.withLeft (boundsF.getHeight() * labelLeftPOH) : boundsF;
        g.drawText (button.getButtonText(), textBounds, leftAlignText ? juce::Justification::centredLeft : juce::Justification::centred, true);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto textColour = button.findColour(juce::TextButton::textColourOffId);

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (findColour (highlightColourId));
            g.fillRoundedRectangle (boundsF.reduced (boundsFHeight * outlineThicknessPOH), boundsFHeight * 0.15f);
        }

        const auto tickBoxSize = boundsFHeight * 0.5f;
        const auto tickBoxBounds = boundsF.withSizeKeepingCentre (tickBoxSize, tickBoxSize).withRightX (boundsF.getRight() - boundsFHeight * 0.45f);
        const auto tickBoxLineThickness = tickBoxSize * 0.08f;
        g.setColour (textColour);
        g.drawRoundedRectangle (tickBoxBounds, tickBoxSize * 0.2f, tickBoxLineThickness);

        if (button.getToggleState())
        {
            juce::Path tick;
            tick.startNewSubPath (tickBoxBounds.getRelativePoint (0.25f, 0.52f));
            tick.lineTo (tickBoxBounds.getRelativePoint (0.43f, 0.7f));
            tick.lineTo (tickBoxBounds.getRelativePoint (0.75f, 0.32f));
            g.setColour (findColour (juce::TextButton::textColourOnId));
            g.strokePath (tick, juce::PathStrokeType (tickBoxLineThickness * 1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        g.setColour (shouldDrawButtonAsHighlighted ? textColour.withBrightness (2.f) : textColour);
        g.setFont (AAFonts::montserratMedium (boundsFHeight * textPOH));
        g.drawText (button.getButtonText(), boundsF.withLeft (boundsFHeight * 0.45f).withRight (tickBoxBounds.getX()), juce::Justification::centredLeft, true);
    }

    float cornerSizePOH {AAUITools::cornerPOH};
    float outlineThicknessPOH {AAUITools::outlineThicknessPOH};
    float textPOH {AAUITools::fontHeightPOH};
    float titleTextPOH {0.6f};
    bool leftAlignText {false};
private:
    float outlineThickness {0.f};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListOptionsPopupLAF)
};
