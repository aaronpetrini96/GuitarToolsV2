//
// Created by Aaron Petrini on 07/09/2026.
//

#pragma once

#include "../../Utils/GTLook.h"

class OptionsPopupLAF : public juce::LookAndFeel_V4
{
public:
    OptionsPopupLAF()
    {
        setColour (juce::PopupMenu::backgroundColourId, GTColors::superDarkBackground.withAlpha(0.9f));
        setColour (juce::PopupMenu::textColourId, GTColors::iconNormalColor);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, GTColors::controlBackground);
        setColour (juce::PopupMenu::highlightedTextColourId, GTColors::activeNeonBlue);
    }

    float menuFontHeight {14.0f};
    float menuCornerRatio {0.5f};

    juce::Font getPopupMenuFont() override
    {
        return GTFonts::ibmPlexSansMedium (menuFontHeight);
    }

    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override
    {
        const auto boundsF = juce::Rectangle<int> (width, height).toFloat();
        const auto cornerSize = menuFontHeight * menuCornerRatio;

        g.setColour (GTColors::dividerLinesStart);
        g.fillRoundedRectangle (boundsF, cornerSize);

        g.setColour (GTColors::dividerLinesEnd);
        g.drawRoundedRectangle (boundsF.reduced (1.0f), cornerSize, 2.0f);
    }

    void preparePopupMenuWindow (juce::Component& menuWindow) override
    {
        const auto clickPosition = juce::Desktop::getMousePosition();

        menuWindow.setTopLeftPosition (clickPosition.x - menuWindow.getWidth(),
                                       clickPosition.y - menuWindow.getHeight());
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPopupLAF)
};