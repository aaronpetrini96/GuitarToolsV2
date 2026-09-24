//
// Created by Aaron Petrini on 18/09/2026.
//

#pragma once
#include "../../Utils/AALook.h"

class PresetPopupLAF : public juce::LookAndFeel_V4
{
public:
    enum ColourIds
    {
        backgroundColourId = 0x2a30001,
        textColourId       = 0x2a30002,
        outlineColourId    = 0x2a30003,
        highlightColourId  = 0x2a30004
    };

    PresetPopupLAF()
    {
        setColour (backgroundColourId, GTColors::presetBox.darker(0.1f));
        setColour (textColourId, GTColors::sectionTitleText);
        setColour (outlineColourId, GTColors::headerPanelEnd);
        setColour (highlightColourId, GTColors::presetPopupHighlight);
    }

    static PresetPopupLAF* get()
    {
        static PresetPopupLAF instance;
        return &instance;
    }

    void drawPopupBackground (juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        const auto cornerSize = bounds.getWidth() * 0.03f;
        // const auto outlineThickness = bounds.getWidth() * outlineThicknessPOW;
        const auto backgroundColour = findColour (backgroundColourId);
        // const auto outlineColour = findColour (outlineColourId);
        g.setColour (backgroundColour);
        g.fillRoundedRectangle (bounds, cornerSize);

        // g.setColour (outlineColour);
        // g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&, bool , bool) override
    {
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto fontHeight = boundsF.getHeight() * 0.675f;
        const auto textColour = findColour (textColourId);

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (findColour (highlightColourId));
            g.fillRoundedRectangle (boundsF, boundsF.getHeight() * highlightCornerSizePOH);
        }

        g.setColour ((shouldDrawButtonAsHighlighted ? textColour.withBrightness(2.f) : textColour).withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        g.setFont (AAFonts::interMedium (fontHeight));
        g.drawFittedText (button.getButtonText(), boundsF.reduced (boundsF.getWidth() * 0.05f, boundsF.getHeight() * 0.04f).toNearestInt(), juce::Justification::centred, 1);
    }

    float outlineThicknessPOW {0.014f};
    float highlightCornerSizePOH {0.22f};

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPopupLAF)
};
