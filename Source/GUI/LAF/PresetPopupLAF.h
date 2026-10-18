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
        setColour (juce::TextEditor::backgroundColourId, GTColors::presetBoxBackground);
        setColour (juce::TextEditor::textColourId, GTPopupTextColor);
    }

    void drawPopupBackground (juce::Graphics& g, juce::Rectangle<float> bounds, const float numRows)
    {
        cornerSize = bounds.getHeight() * GTCornerPOH;
        outlineThickness = bounds.getHeight() / numRows * GTOutlineThicknessPOH;
        AAUITools::drawOutlinedRoundedRectangle (g, bounds, cornerSize, outlineThickness, GTPopupBackgroundColor, GTPopupOutlineColor);
    }

    void drawPopupTitle (juce::Graphics& g, juce::Rectangle<float> titleBounds, const juce::String& title)
    {
        juce::Path titleShape;
        titleShape.addRoundedRectangle (titleBounds.getX(), titleBounds.getY(), titleBounds.getWidth(), titleBounds.getHeight(), cornerSize, cornerSize,
                                        true, true, false, false);
        g.setColour (GTColors::presetBoxBackground);
        g.fillPath (titleShape);

        g.setColour (GTPopupOutlineColor);
        g.fillRect (titleBounds.withTop (titleBounds.getBottom() - outlineThickness * 0.5f));

        g.setColour (GTPopupTextColor.withBrightness (2.f));
        g.setFont (GTFonts::montserratSemiBold (titleBounds.getHeight() * GTFontHeightPOH));
        g.drawText (title, titleBounds, juce::Justification::centred, true);
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

        g.setColour (GTColors::presetBoxOutlineColor.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        g.drawRoundedRectangle (boundsF.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);

        g.setColour ((shouldDrawButtonAsHighlighted ? GTPopupTextColor.withMultipliedBrightness (0.6f) : GTPopupTextColor).withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        g.setFont (GTFonts::montserratMedium (fontHeight));
        g.drawFittedText (button.getButtonText(), boundsF.reduced (boundsF.getWidth() * 0.05f, boundsF.getHeight() * 0.04f).toNearestInt(), juce::Justification::centred, 1);
    }

    void fillTextEditorBackground (juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        const auto boundsF = juce::Rectangle<int> (width, height).toFloat();
        cornerSize = boundsF.getHeight() * GTCornerPOH;
        AAUITools::drawOutlinedRoundedRectangle (g, boundsF, cornerSize, boundsF.getHeight() * GTOutlineThicknessPOH,
                                                 textEditor.findColour (juce::TextEditor::backgroundColourId), GTColors::presetBoxOutlineColor);
    }

    void drawTextEditorOutline (juce::Graphics&, int, int, juce::TextEditor&) override { }

    float textEditorOutlineThicknessPOH {GTOutlineThicknessPOH * 0.75f};
private:
    float cornerSize {0.0f};
    float outlineThickness {0.0f};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPopupLAF)
};
