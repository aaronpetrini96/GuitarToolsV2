//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include "../../AALook.h"
#include "../../Utils/AAParamUtils.h"
#include "PluginProcessor.h"

#include "GUI/Components/AAFaderSSL.h"

class FadersPlatePanel : public juce::Component
{
public:
    explicit FadersPlatePanel (GuitarToolsAudioProcessor& processor)
        : resoFreqFader (*processor.treeState.getParameter (ParamIDs::getResoFreqName()), "Hz"),
          mudFreqFader (*processor.treeState.getParameter (ParamIDs::getMudFreqName()), "Hz"),
          compAmountFader (*processor.treeState.getParameter (ParamIDs::getCompAmountName()), "%")
    {
        for (auto* fader : {&resoFreqFader, &mudFreqFader, &compAmountFader})
        {
            addAndMakeVisible (*fader);
            fader->setTooltip (ParamHelpers::getTooltipForParam (fader->getComponentID()));
            fader->setUndoManager (&processor.undoManager);
        }
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * 0.05f;
        const auto border = boundsF.getHeight() * 0.007f;
        g.setGradientFill(juce::ColourGradient::vertical(GTColors::fadersPlateGradientEnd, 0.f, GTColors::fadersPlateGradientStart, boundsF.getHeight()));
        g.fillRoundedRectangle(boundsF, cornerSize);

        g.setOpacity(1.f);
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        for (const auto& trackBounds : {resoTrackBounds, mudTrackBounds, compTrackBounds})
            g.drawImage(trackImage, trackBounds, juce::RectanglePlacement::centred);

        if (screw.isValid())
        {
            const auto screwSize = boundsF.getHeight() * 0.055f;
            const auto screwBounds = boundsF.withSize(screwSize, screwSize);
            const auto screwGap = screwSize;

            g.drawImage(screw, screwBounds.withCentre({screwGap, screwGap}), juce::RectanglePlacement::stretchToFit);
            // g.drawImage(screw, screwBounds.withCentre({boundsF.getCentreX(), screwGap}), juce::RectanglePlacement::stretchToFit);
            g.drawImage(screw, screwBounds.withCentre({boundsF.getWidth() - screwGap, screwGap}), juce::RectanglePlacement::stretchToFit);

            const auto bottomCY = boundsF.getHeight() - screwGap;
            g.drawImage(screw, screwBounds.withCentre({screwGap,                      bottomCY}), juce::RectanglePlacement::stretchToFit);
            // g.drawImage(screw, screwBounds.withCentre({boundsF.getCentreX(),                bottomCY}), juce::RectanglePlacement::stretchToFit);
            g.drawImage(screw, screwBounds.withCentre({boundsF.getWidth() - screwGap, bottomCY}), juce::RectanglePlacement::stretchToFit);
        }

        g.setColour(GTColors::dividerLinesStart);
        g.drawRoundedRectangle(boundsF.reduced(border * 0.5f), cornerSize, border);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();

        const auto faderWidth = boundsFWidth * 0.3f;

        const auto faderBounds = boundsF.withSizeKeepingCentre(faderWidth, boundsFHeight * 0.925f);
        mudFreqFader.setBounds (faderBounds.toNearestInt());
        resoFreqFader.setBounds (faderBounds.translated(-faderSpacing, 0.f).toNearestInt());
        compAmountFader.setBounds (faderBounds.translated(faderSpacing, 0.f).toNearestInt());

        const auto trackHeight = faderBounds.getHeight() * 0.9f;
        const auto trackWidth = trackHeight * (float) trackImage.getWidth() / (float) trackImage.getHeight();
        const auto trackBounds = faderBounds.withSizeKeepingCentre(trackWidth, trackHeight);
        mudTrackBounds = trackBounds;
        resoTrackBounds = trackBounds.translated(-faderSpacing, 0.f);
        compTrackBounds = trackBounds.translated(faderSpacing, 0.f);
    }

    void setFaderSpacing (const float newFaderSpacing)
    {
        if (juce::approximatelyEqual(faderSpacing, newFaderSpacing))
            return;
        faderSpacing = newFaderSpacing;
        resized();
    }

    AAVerticalSSLFader resoFreqFader, mudFreqFader, compAmountFader;

private:
    float faderSpacing {0.f};

    juce::Image screw {juce::ImageCache::getFromMemory(BinaryData::screwDarkGreyv3_png, BinaryData::screwDarkGreyv3_pngSize)};
    juce::Image trackImage = juce::ImageCache::getFromMemory (BinaryData::faderTrackClippedShorter_png, BinaryData::faderTrackClippedShorter_pngSize);
    juce::Rectangle<float> resoTrackBounds, mudTrackBounds, compTrackBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FadersPlatePanel)
};
