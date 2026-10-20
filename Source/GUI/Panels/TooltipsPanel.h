//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "../../GTLook.h"

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
        const auto boundsFHeight = boundsF.getHeight();
        g.fillAll(GTColors::almostBlack);
        g.setColour (GTColors::almostWhiteText);
        g.setFont (GTFonts::ibmPlexSansMedium (boundsFHeight * GTFontHeightPOH).withExtraKerningFactor (0.03f));
        g.drawText (tooltipText, boundsF, juce::Justification::centred);
    }

private:
    juce::String tooltipText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipsPanel)
};
