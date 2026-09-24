//
// Created by Aaron Petrini on 19/09/2026.
//

#pragma once
#include "../../Utils/AALook.h"

class ListOptionsPopupLAF : public juce::LookAndFeel_V4
{
public:
    enum ColourIds
    {
        backgroundColourId = 0x2a10001,
        outlineColourId    = 0x2a10002,
        dividerColourId    = 0x2a10003,
        highlightColourId  = 0x2a10004
    };

    ListOptionsPopupLAF()
    {
        setColour (backgroundColourId, GTColors::presetBox.darker(0.1f));
        setColour (outlineColourId, GTColors::headerPanelEnd);
        setColour (dividerColourId, GTColors::headerPanelStart.withAlpha (0.9f));
        setColour (highlightColourId, GTColors::presetPopupHighlight);
        setColour (juce::TextButton::textColourOffId, GTColors::iconNormalColor);
        setColour (juce::TextButton::textColourOnId, GTColors::activeNeonBlue);
    }

    void drawListOptionsBackground (juce::Graphics& g, juce::Rectangle<float> bounds, const int numRows, const bool drawRowDividers) const
    {
        const auto rowHeight = bounds.getHeight() / (float) juce::jmax (1, numRows);
        const auto cornerSize = bounds.getHeight() * 0.05f;
        const auto outlineThickness = bounds.getHeight() * outlineThicknessPOH;

        g.setColour (findColour (backgroundColourId));
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour (findColour (outlineColourId));
        g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);

        if (drawRowDividers)
        {
            const auto dividerBounds = bounds.withSizeKeepingCentre (bounds.getWidth() * 0.9f, rowHeight * 0.06f);
            g.setColour (findColour (outlineColourId));

            for (int rowIndex = 1; rowIndex < numRows; ++rowIndex)
                g.fillRoundedRectangle (dividerBounds.withCentre ({ bounds.getCentreX(), rowHeight * (float) rowIndex }), dividerBounds.getHeight() * 0.5f);
        }
    }

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override {}

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, const bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * highlightCornerSizePOH;
        const auto textColour = findColour (button.getToggleState() ? juce::TextButton::textColourOnId : juce::TextButton::textColourOffId);

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (findColour (highlightColourId));
            g.fillRoundedRectangle (boundsF, cornerSize);
        }

        g.setColour ((shouldDrawButtonAsHighlighted ? textColour.withBrightness (2.f) : textColour).withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.4f));
        g.setFont (AAFonts::ibmPlexSansMedium ((float) button.getHeight() * textPOH));
        g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
    }

    float outlineThicknessPOH {0.012f};
    float textPOH {0.75f};
    float highlightCornerSizePOH {0.22f};
};
