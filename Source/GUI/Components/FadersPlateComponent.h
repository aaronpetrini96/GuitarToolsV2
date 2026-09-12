//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include "../../Utils/GTLook.h"
#include "../../Utils/ParamUtils.h"
#include "PluginProcessor.h"

#include "GUI/Components/AAFaderSSL.h"

class FadersPlateComponent : public juce::Component
{
public:
    explicit FadersPlateComponent (GuitarToolsAudioProcessor& processor)
        : resoFreqFader (processor.treeState, juce::ParameterID (ParamIDs::getResoFreqName(), 1), "Hz"),
          mudFreqFader (processor.treeState, juce::ParameterID (ParamIDs::getMudFreqName(), 1), "Hz"),
          compThresholdFader (processor.treeState, juce::ParameterID (ParamIDs::getThresholdName(), 1), "dB")
    {
        addAndMakeVisible (resoFreqFader);
        addAndMakeVisible (mudFreqFader);
        addAndMakeVisible (compThresholdFader);
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto plateHeight= boundsF.getHeight();
        const auto plateArea = boundsF.withSizeKeepingCentre(boundsF.proportionOfWidth(0.9f), plateHeight);
        const auto plateCorners      = plateHeight * 0.1f;
        const auto borderThickness  = plateHeight * 0.007f;

        g.setGradientFill(juce::ColourGradient::vertical(juce::Colours::black.withAlpha(0.225f), juce::Colours::black.withAlpha(0.275f), plateArea));
        g.fillRoundedRectangle (plateArea, plateCorners);
        g.setColour(juce::Colours::black.withAlpha(0.55f));
        g.drawRoundedRectangle(plateArea.reduced(borderThickness * 0.5f), plateCorners - borderThickness * 0.5f, borderThickness);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto faderBounds = boundsF.withSizeKeepingCentre (boundsF.getWidth() * 0.3f, boundsF.getHeight());
        mudFreqFader.setBounds (faderBounds.toNearestInt());
        resoFreqFader.setBounds (faderBounds.withCentre ({faderBounds.getCentreX() - faderSpacing, faderBounds.getCentreY()}).toNearestInt());
        compThresholdFader.setBounds (faderBounds.withCentre ({faderBounds.getCentreX() + faderSpacing, faderBounds.getCentreY()}).toNearestInt());
    }

    void setFaderSpacing (float newFaderSpacing)
    {
        faderSpacing = newFaderSpacing;
        resized();
    }

    AAVerticalSSLFader resoFreqFader, mudFreqFader, compThresholdFader;

private:
    float faderSpacing {0.f};
    float screwToHeightRatio {0.075f};
    juce::Image screw = juce::ImageCache::getFromMemory (BinaryData::screw_png, BinaryData::screw_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FadersPlateComponent)
};
