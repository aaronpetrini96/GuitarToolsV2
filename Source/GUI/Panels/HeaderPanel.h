//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "GTLook.h"
#include "PluginProcessor.h"
#include "GUI/Components/ImgButton.h"

class HeaderPanel : public juce::Component
{
public:
    explicit HeaderPanel (GuitarToolsAudioProcessor& processor)
        : bypassButton ("Bypass Button", processor.treeState, juce::ParameterID("Plugin Bypass", 1))
    {
        addAndMakeVisible (bypassButton);
    }

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
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto buttonSize = (float) bypassButton.getWidth();
        bypassButton.setBounds (boundsF.withSize (buttonSize, buttonSize)
                                       .withCentre ({boundsFWidth * 0.95f, boundsFHeight * 0.5f}).toNearestInt());
    }

    ImgButton bypassButton;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};