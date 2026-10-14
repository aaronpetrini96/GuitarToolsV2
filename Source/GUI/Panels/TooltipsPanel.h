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
        const auto boundsFHeight = boundsF.getHeight();
        g.setColour(GTColors::popupDarkBackground);
        g.fillRect (boundsF);
        g.setColour (GTColors::sectionTitleText);
        g.setFont (AAFonts::ibmPlexSansMedium (boundsFHeight * 0.65f).withExtraKerningFactor(0.04f));
        g.drawText (tooltipText, boundsF, juce::Justification::centred);
    }

private:
    juce::String tooltipText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipsPanel)
};
