//
// Created by Aaron Petrini on 18/09/2026.
//

#pragma once
#include "../../AALook.h"

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
        setColour (backgroundColourId, GTColors::popupDarkBackground);
        setColour (textColourId, GTColors::sectionTitleText);
        setColour (outlineColourId, GTColors::dividerLinesStart);
        setColour (highlightColourId, GTColors::presetPopupHighlight);
        setColour (juce::TextEditor::backgroundColourId, GTColors::dividerLinesEnd);
        setColour (juce::TextEditor::textColourId, GTColors::sectionTitleText);
    }

    void drawPopupBackground (juce::Graphics& g, juce::Rectangle<float> bounds, const float numRows)
    {
        cornerSize = bounds.getHeight() * cornerSizePOH;
        const auto outlineThickness = bounds.getHeight() / numRows * outlineThicknessPOH;
        g.setGradientFill(juce::ColourGradient::vertical(GTColors::popupDarkBackground, 0, GTColors::presetBox.darker(0.15f), bounds.getHeight()));
        g.fillRoundedRectangle (bounds, cornerSize);

        g.setColour (findColour (outlineColourId));
        g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);
    }

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool , bool) override { }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto fontHeight = boundsF.getHeight() * fontPOH;
        const auto textColour = findColour (textColourId);

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (findColour (highlightColourId));
            g.fillRoundedRectangle (boundsF, cornerSize);
        }

        g.setColour ((shouldDrawButtonAsHighlighted ? textColour.withBrightness(2.f) : textColour).withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        g.setFont (AAFonts::montserratMedium (fontHeight));
        g.drawFittedText (button.getButtonText(), boundsF.reduced (boundsF.getWidth() * 0.05f, boundsF.getHeight() * 0.04f).toNearestInt(), juce::Justification::centred, 1);
    }

    void fillTextEditorBackground (juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        const auto boundsF = juce::Rectangle<int> (width, height).toFloat();
        cornerSize = boundsF.getHeight() * textEditorCornerSizePOH;
        const auto outlineThickness = boundsF.getHeight() * textEditorOutlineThicknessPOH;
        g.setColour (textEditor.findColour (juce::TextEditor::backgroundColourId));
        g.fillRoundedRectangle (boundsF, cornerSize);
    }

    void drawTextEditorOutline (juce::Graphics&, int, int, juce::TextEditor&) override { }

    float textEditorCornerSizePOH {AAUITools::cornerPOH};
    float textEditorOutlineThicknessPOH {AAUITools::outlineThicknessPOH * 0.75f};
    float fontPOH {AAUITools::fontHeightPOH};
    float cornerSizePOH {AAUITools::cornerPOH};
    float outlineThicknessPOH {AAUITools::outlineThicknessPOH};
private:
    float cornerSize {0.0f};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPopupLAF)
};
