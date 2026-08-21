/*
  ==============================================================================

    LookAndFeelHelpers.h
    Created: 6 May 2025 10:56:04pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
struct LookAndFeelHelpers
{
    
    LookAndFeelHelpers() = delete;

    static juce::Colour createBaseColour (juce::Colour buttonColour,
                                    bool hasKeyboardFocus,
                                    bool shouldDrawButtonAsHighlighted,
                                    bool shouldDrawButtonAsDown) noexcept
    {
        const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
        const juce::Colour baseColour (buttonColour.withMultipliedSaturation (sat));

        if (shouldDrawButtonAsDown)        return baseColour.contrasting (0.2f);
        if (shouldDrawButtonAsHighlighted) return baseColour.contrasting (0.1f);

        return baseColour;
    }

    static juce::TextLayout layoutTooltipText (const juce::String& text, juce::Colour colour) noexcept
    {
        const float tooltipFontSize = 13.0f;
        const int maxToolTipWidth = 400;

        juce::AttributedString s;
        s.setJustification (juce::Justification::centred);
        s.append (text, juce::Font (tooltipFontSize, juce::Font::FontStyleFlags::bold), colour);

        juce::TextLayout tl;
        tl.createLayoutWithBalancedLineLengths (s, (float) maxToolTipWidth);
        return tl;
    }
};
