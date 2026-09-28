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
        setColour (backgroundColourId, GTColors::presetBox.darker(0.1f));
        setColour (textColourId, GTColors::sectionTitleText);
        setColour (outlineColourId, GTColors::dividerLinesStart);
        setColour (highlightColourId, GTColors::presetPopupHighlight);
    }

    static PresetPopupLAF* get()
    {
        static PresetPopupLAF instance;
        return &instance;
    }

    void drawPopupBackground (juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        const auto cornerSize = bounds.getWidth() * 0.0f;
        const auto outlineThickness = bounds.getWidth() * outlineThicknessPOW;
        // g.setGradientFill(juce::ColourGradient::vertical(GTColors::presetBox.darker(0.1f), 0, GTColors::headerPanelEnd, bounds.getHeight()));
        g.setColour(findColour (backgroundColourId));
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
            g.fillRoundedRectangle (boundsF, boundsF.getHeight() * highlightCornerSizePOH);
        }

        g.setColour ((shouldDrawButtonAsHighlighted ? textColour.withBrightness(2.f) : textColour).withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        g.setFont (AAFonts::montserratMedium (fontHeight));
        g.drawFittedText (button.getButtonText(), boundsF.reduced (boundsF.getWidth() * 0.05f, boundsF.getHeight() * 0.04f).toNearestInt(), juce::Justification::centred, 1);
    }

    void fillTextEditorBackground (juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        const auto boundsF = juce::Rectangle<int> (width, height).toFloat();
        g.setColour (textEditor.findColour (juce::TextEditor::backgroundColourId));
        g.fillRoundedRectangle (boundsF, boundsF.getHeight() * textEditorCornerSizePOH);
        g.setColour(findColour (outlineColourId));
    }

    void drawTextEditorOutline (juce::Graphics&, int, int, juce::TextEditor&) override { }

    float outlineThicknessPOW {0.007f};
    float highlightCornerSizePOH {0.22f};
    float textEditorCornerSizePOH {0.22f};
    float fontPOH {0.65f};
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPopupLAF)
};
