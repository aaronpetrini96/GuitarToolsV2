//
// Created by Aaron Petrini on 19/10/2026.
//

#pragma once
#include "../../GTLook.h"

class ValuePopupLAF : public juce::LookAndFeel_V4
{
public:
    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        const auto boundsF = label.getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        AAUITools::drawOutlinedRoundedRectangle (g, boundsF, boundsFHeight * GTCornerPOH, boundsFHeight * GTOutlineThicknessPOH, GTPopupBackgroundColor, GTPopupOutlineColor);
        g.setColour (GTPopupTextColor);
        g.setFont (GTFonts::montserratSemiBold (boundsFHeight * GTFontHeightPOH));
        g.drawText (label.getText(), boundsF, juce::Justification::centred, false);
    }
};
