//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "GTLook.h"

class HeaderPanel : public juce::Component
{
public:
    HeaderPanel() = default;

    void paint(Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight= boundsF.getHeight();

        g.setColour(GTColors::footerBackground);
        g.fillRect(boundsF);
    }
    void resized() override
    {

    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};