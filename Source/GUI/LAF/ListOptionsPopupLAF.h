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
        highlightColourId  = 0x2a10004
    };

    ListOptionsPopupLAF()
    {
        setColour (backgroundColourId, GTColors::presetBox.darker(0.1f));
        setColour (outlineColourId, GTColors::dividerLinesStart);
        setColour (dividerColourId, GTColors::dividerLinesStart);
        setColour (highlightColourId, GTColors::presetPopupHighlight);
        setColour (juce::TextButton::textColourOffId, GTColors::sectionTitleText);
        setColour (juce::TextButton::textColourOnId, GTColors::activeNeonBlue);
    }

    void drawListOptionsBackground (juce::Graphics& g, juce::Rectangle<float> bounds, const int numRows, const bool drawRowDividers)
    {
        const auto rowHeight = bounds.getHeight() / static_cast<float>(juce::jmax(1, numRows));
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
            g.fillRoundedRectangle (boundsF.reduced(outlineThickness, outlineThickness * 0.5f), cornerSize);
        }

        g.setColour (shouldDrawButtonAsHighlighted ? textColour.withBrightness (2.f) : textColour);
        if (!button.isEnabled())
            g.setColour (textColour.withAlpha (0.3f));
        g.setFont (AAFonts::montserratMedium (boundsF.getHeight() * textPOH));
        g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
    }

    // float cornerSizePOH {0.05f};
    float cornerSizePOH {0.0f};
    float outlineThicknessPOH {0.07f};
    float textPOH {0.65f};
    float highlightCornerSizePOH {0.22f};

private:
    float outlineThickness {0.f};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListOptionsPopupLAF)
};
