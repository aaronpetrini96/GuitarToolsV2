//
// Created by Aaron Petrini on 18/09/2026.
//

#pragma once
#include "../../GTLook.h"

class PresetPopupLAF : public juce::LookAndFeel_V4
{
public:
    PresetPopupLAF()
    {
        setColour (juce::TextEditor::backgroundColourId, GTColors::popupDarkBackground);
        setColour (juce::TextEditor::textColourId, GTPopupTextColor);
    }

    void drawPopupBackground (juce::Graphics& g, juce::Rectangle<float> bounds, const float numRows)
    {
        cornerSize = bounds.getHeight() * GTCornerPOH;
        const auto outlineThickness = bounds.getHeight() / numRows * GTOutlineThicknessPOH;
        AAUITools::drawOutlinedRoundedRectangle (g, bounds, cornerSize, outlineThickness, GTPopupBackgroundColor, GTPopupOutlineColor);
    }

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool , bool) override { }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto fontHeight = boundsF.getHeight() * GTFontHeightPOH;

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (GTColors::presetBoxBackground);
            g.fillRoundedRectangle (boundsF, cornerSize);
        }

        g.setColour ((shouldDrawButtonAsHighlighted ? GTPopupTextColor.withMultipliedBrightness (0.6f) : GTPopupTextColor).withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        g.setFont (GTFonts::montserratMedium (fontHeight));
        g.drawFittedText (button.getButtonText(), boundsF.reduced (boundsF.getWidth() * 0.05f, boundsF.getHeight() * 0.04f).toNearestInt(), juce::Justification::centred, 1);
    }

    void fillTextEditorBackground (juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        const auto boundsF = juce::Rectangle<int> (width, height).toFloat();
        cornerSize = boundsF.getHeight() * GTCornerPOH;
        g.setColour (textEditor.findColour (juce::TextEditor::backgroundColourId));
        g.fillRoundedRectangle (boundsF, cornerSize);
    }

    void drawTextEditorOutline (juce::Graphics&, int, int, juce::TextEditor&) override { }

    float textEditorOutlineThicknessPOH {GTOutlineThicknessPOH * 0.75f};
private:
    float cornerSize {0.0f};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPopupLAF)
};
