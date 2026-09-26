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
        tabTextHoverColourId    = 0x2a20007
    };

    PresetBrowserLAF()
    {
        setColour (backgroundColourId, GTColors::presetBox.darker(0.1f));
        setColour (dividerColourId, GTColors::headerPanelStart);
        setColour (highlightColourId, GTColors::presetPopupHighlight);
        setColour (activeColourId, GTColors::activeNeonBlue);
        setColour (textColourId, GTColors::sectionTitleText);
        setColour (tabTextColourId, GTColors::sectionTitleText);
        setColour (tabTextHoverColourId, GTColors::almostWhite);
        setColour (juce::ScrollBar::thumbColourId, GTColors::iconOverColor);
    }

    static PresetBrowserLAF* get()
    {
        static PresetBrowserLAF instance;
        return &instance;
    }

    void drawMenuListBackground (juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        //bounds
        const auto cornerSize = bounds.getWidth() * menuCornerSizePOW;
        // const auto outlineThickness = bounds.getWidth() * outlineThicknessPOW;

        //Colors
        const auto backgroundColour = findColour (backgroundColourId);
        // const auto outlineColour = findColour (dividerColourId);

        g.setColour (backgroundColour);
        g.fillRoundedRectangle (bounds, cornerSize);

        // g.setColour (outlineColour);
        // g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);
    }

    void drawMenuListHeader (juce::Graphics& g, juce::Rectangle<float> bounds, const juce::String& text)
    {
        //bounds
        const auto fontHeight = bounds.getHeight() * 0.75f;
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
        // const auto dividerThickness = boundsF.getHeight() * dividerThicknessPOH;
        const auto underlineThickness = boundsF.getHeight() * underlineThicknessPOH;

        //bool checkers
        const auto isTabButton = button.getConnectedEdgeFlags() != 0;
        // const auto isRightTabButton = (button.getConnectedEdgeFlags() & juce::Button::ConnectedOnLeft) != 0;
        // const auto isFirstRow = button.getY() == 0;
        const auto isSelected = button.getToggleState();

        //Colors
        const auto dividerColour = findColour (dividerColourId);
        const auto highlightColour = findColour (highlightColourId);
        const auto activeColour = findColour (activeColourId);

        g.setColour (dividerColour);

        if (! isTabButton)
        {
            // if (! isFirstRow)
            //     g.fillRect (boundsF.withHeight (dividerThickness));
            return;
        }

        // if (isRightTabButton)
        //     g.fillRect (boundsF.withWidth (dividerThickness));

        if (isHighlighted)
        {
            g.setColour (highlightColour);
            g.fillRect (boundsF);
        }

        g.setColour (isSelected ? activeColour : dividerColour);
        g.fillRect (boundsF.withTop (boundsF.getBottom() - underlineThickness));
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isHighlighted, bool) override
    {
        //bounds
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * highlightCornerSizePOH;

        //bool checkers
        const auto isTabButton = button.getConnectedEdgeFlags() != 0;
        const auto isSelected = button.getToggleState();
        const auto isEnabled = button.isEnabled();

        //Colors
        const auto highlightColour = findColour (highlightColourId);
        const auto tabButtonTextColour = findColour (isSelected ? textColourId : (isHighlighted ? tabTextHoverColourId : tabTextColourId)).withMultipliedAlpha (isEnabled ? 1.0f : 0.4f);
        const auto presetTextColour = findColour (isSelected ? activeColourId : textColourId);
        const auto buttonTextColour = isTabButton ? tabButtonTextColour : (isHighlighted ? presetTextColour.withBrightness (2.f) : presetTextColour);
        const auto fontHeight = boundsF.getHeight() * (isTabButton ? tabTextFontPOH : presetFontPOH);

        if (! isTabButton && isHighlighted)
        {
            g.setColour (highlightColour);
            g.fillRoundedRectangle (boundsF, cornerSize);
        }

        g.setColour (buttonTextColour);
        g.setFont (AAFonts::interMedium (fontHeight));
        g.drawFittedText (button.getButtonText(), boundsF.toNearestInt(), juce::Justification::centred, 1);
    }

    float presetFontPOH {0.5f};
    float tabTextFontPOH {0.5f};
    float menuCornerSizePOW {0.03f};
    float outlineThicknessPOW {0.012f};
    float highlightCornerSizePOH {0.22f};
    float dividerThicknessPOH {0.065f};
    float underlineThicknessPOH {0.075f};

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserLAF)
};
