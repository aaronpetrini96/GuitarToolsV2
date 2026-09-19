//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "../../Utils/AALook.h"

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
        g.setGradientFill (juce::ColourGradient::horizontal (GTColors::gradientNavy, 0.f, GTColors::headerPanelStart, boundsFWidth));
        g.fillRect (boundsF);
        const auto dividerHalfThickness = boundsFWidth * 0.003f;
        g.setColour (GTColors::dividerLinesStart);
        g.fillRect (0.0f, 0.0f, boundsFWidth, dividerHalfThickness);

        g.setColour (GTColors::almostWhite.withAlpha(0.9f));
        g.setFont (AAFonts::interMedium (boundsFHeight * 0.65f).withExtraKerningFactor(0.02f));
        g.drawText (tooltipText, boundsF, juce::Justification::centred);
    }

private:
    juce::String tooltipText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipsPanel)
};
