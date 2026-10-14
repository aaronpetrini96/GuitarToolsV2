//
// Created by Aaron Petrini on 03/09/2026.
//

#pragma once
#include "../../AALook.h"

class PresetBrowserLAF : public juce::LookAndFeel_V4
{
public:
    enum ColourIds
    {
        backgroundColourId      = 0x2a20001,
        dividerColourId         = 0x2a20002,
        highlightColourId       = 0x2a20003,
        activeColourId          = 0x2a20004,
        textColourId            = 0x2a20005,
        tabTextColourId         = 0x2a20006,
        tabTextHoverColourId    = 0x2a20007,
        outlineColourId         = 0x2a20008
    };

    PresetBrowserLAF()
    {
        setColour (backgroundColourId, GTColors::popupDarkBackground);
        setColour (dividerColourId,GTColors::dividerLinesStart);
        setColour (highlightColourId, GTColors::presetPopupHighlight);
        setColour (activeColourId, GTColors::activeNeonBlue);
        setColour (textColourId, GTColors::sectionTitleText);
        setColour (tabTextColourId, GTColors::sectionTitleText);
        setColour (tabTextHoverColourId, GTColors::sectionTitleText.brighter(2.f));
        setColour (outlineColourId, GTColors::dividerLinesStart);
    }

    void drawMenuListBackground (juce::Graphics& g, juce::Rectangle<float> bounds, const float numRows)
    {
        //bounds
        const auto rowHeight = bounds.getHeight() / numRows;
        const auto cornerSize = bounds.getHeight() * menuCornerSizePOH;
        outlineThickness = rowHeight * outlineThicknessPOH;

        //Colors
        const auto backgroundColour = findColour (backgroundColourId);
        const auto outlineColour = findColour (outlineColourId);

        g.setColour (backgroundColour);
        g.fillRoundedRectangle (bounds, cornerSize);

        g.setColour (outlineColour);
        g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);
    }

    void drawMenuListHeader (juce::Graphics& g, juce::Rectangle<float> bounds, const juce::String& text)
    {
        //bounds
        const auto fontHeight = bounds.getHeight() * tabTextFontPOH;
        //Colors
        const auto textColour = findColour (textColourId);
        g.setColour (textColour);
        g.setFont (AAFonts::interMedium (fontHeight));
        g.drawFittedText (text, bounds.toNearestInt(), juce::Justification::centred, 1);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& , bool isHighlighted, bool) override
    {
        //bounds
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * highlightCornerSizePOH;

        //bool checkers
        const auto isTabButton = button.getConnectedEdgeFlags() != 0;
        const auto isSelected = button.getToggleState();

        if (isTabButton)
        {
            g.setColour (isSelected ? GTColors::tabButtonSelected : GTColors::tabButtonUnselected);
            g.fillRect(boundsF);
            g.setColour (findColour(outlineColourId));
            const auto tabOutlineThickness = outlineThickness * 0.5f;
            const auto tabOutlineBounds = boundsF.withTrimmedLeft (button.isConnectedOnLeft() ? -tabOutlineThickness * 0.5f : 0.0f)
                                                 .withTrimmedRight (button.isConnectedOnRight() ? -tabOutlineThickness * 0.5f : 0.0f);
            g.drawRect (tabOutlineBounds, tabOutlineThickness);
        }

        if (!isTabButton && isHighlighted)
        {
            g.setColour (findColour(highlightColourId));
            g.fillRoundedRectangle (boundsF.reduced(outlineThickness), cornerSize);
        }
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isHighlighted, bool) override
    {
        //bounds
        const auto boundsF = button.getLocalBounds().toFloat();
        //bool checkers
        const auto isTabButton = button.getConnectedEdgeFlags() != 0;
        const auto isSelected = button.getToggleState();
        const auto isEnabled = button.isEnabled();

        //Colors
        const auto tabButtonTextColour = findColour (tabTextColourId).withMultipliedAlpha (isEnabled ? 1.f : 0.4f);
        const auto presetTextColour = findColour (isSelected ? activeColourId : textColourId);
        const auto buttonTextColour = isTabButton ? tabButtonTextColour : presetTextColour;

        g.setColour (isHighlighted ? buttonTextColour.withBrightness(2.f) : buttonTextColour);
        g.setFont (AAFonts::montserratMedium (boundsF.getHeight() * (isTabButton ? tabTextFontPOH : presetFontPOH)));
        g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
    }

    float presetFontPOH {AAUITools::fontHeightPOH};
    float tabTextFontPOH {0.6f};
    float menuCornerSizePOH {AAUITools::cornerPOH};
    float outlineThicknessPOH {AAUITools::outlineThicknessPOH};
    float highlightCornerSizePOH {AAUITools::cornerPOH};

private:
    float outlineThickness {0.0f};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserLAF)
};
