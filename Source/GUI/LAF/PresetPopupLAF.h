//
// Created by Aaron Petrini on 18/09/2026.
//

#pragma once
#include "../../Utils/AALook.h"

class PresetPopupLAF : public juce::LookAndFeel_V4
{
public:
    PresetPopupLAF() = default;

    static PresetPopupLAF* get()
    {
        static PresetPopupLAF instance;
        return &instance;
    }

    void drawPopupBackground (juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        const auto cornerSize = bounds.getWidth() * 0.02f;
        const auto outlineThickness = bounds.getWidth() * 0.001f;
        g.setGradientFill (juce::ColourGradient::vertical (GTColors::gradientNavyDark, bounds.getY(), GTColors::gradientNavy, bounds.getBottom()));
        g.fillRoundedRectangle (bounds, cornerSize);

        g.setColour (GTColors::sectionTitleText);
        g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);
    }

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override
    {
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto fontHeight = boundsF.getHeight() * 0.675f;

        g.setColour ((shouldDrawButtonAsHighlighted ? GTColors::sectionTitleText.withBrightness(2.f) : GTColors::sectionTitleText).withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        g.setFont (AAFonts::interMedium (fontHeight));
        g.drawFittedText (button.getButtonText(), boundsF.reduced (boundsF.getWidth() * 0.05f, boundsF.getHeight() * 0.04f).toNearestInt(), juce::Justification::centred, 1);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPopupLAF)
};
