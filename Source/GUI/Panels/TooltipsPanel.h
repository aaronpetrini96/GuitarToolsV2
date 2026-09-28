//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "../../AALook.h"

class TooltipsPanel : public juce::Component
{
public:
    TooltipsPanel() = default;

    void setTooltipText (const juce::String& newTooltipText)
    {
        tooltipText = newTooltipText.toUpperCase();
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        g.fillAll (juce::Colours::black);
        g.setGradientFill (juce::ColourGradient::horizontal (GTColors::headerPanelEnd, 0.f, GTColors::headerPanelStart, boundsFWidth));
        g.setOpacity (0.9f);
        g.fillRect (boundsF);

        g.setColour (GTColors::almostWhite.withAlpha(0.9f));
        g.setFont (AAFonts::interBold (boundsFHeight * 0.62f).withExtraKerningFactor(0.06F));
        g.drawText (tooltipText, boundsF, juce::Justification::centred);
    }

private:
    juce::String tooltipText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipsPanel)
};
