//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include "../../GTLook.h"
#include "../../Utils/AAParamUtils.h"
#include "PluginProcessor.h"

#include "GUI/Components/AAFaderSSL.h"

class FadersPlatePanel : public juce::Component
{
public:
    explicit FadersPlatePanel (GuitarToolsAudioProcessor& processor)
        : resoFreqFader (*processor.treeState.getParameter (ParamIDs::getResoFreqName()), "Hz"),
          mudAmountFader (*processor.treeState.getParameter (ParamIDs::getMudAmountName()), "%"),
          compAmountFader (*processor.treeState.getParameter (ParamIDs::getCompAmountName()), "%")
    {
        for (auto* fader : {&resoFreqFader, &mudAmountFader, &compAmountFader})
        {
            addAndMakeVisible (*fader);
            fader->setTooltip (ParamHelpers::getTooltipForParam (fader->getComponentID()));
            fader->setUndoManager (&processor.undoManager);
        }
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        for (const auto& trackBounds : {resoTrackBounds, mudTrackBounds, compTrackBounds})
            g.drawImage(trackImage, trackBounds, juce::RectanglePlacement::fillDestination);

        if (screw.isValid())
        {
            const auto screwBounds = boundsF.withSize(screwSize, screwSize);
            const auto screwsTopCY = screwSize;
            const auto screwsBottomCY = boundsF.getHeight() - screwSize;
            const auto faderCentreX = faderBounds.getCentreX();
            g.drawImage(screw, screwBounds.withCentre({faderCentreX,                screwsTopCY}), imgPlacement);
            g.drawImage(screw, screwBounds.withCentre({faderCentreX,                screwsBottomCY}), imgPlacement);
            g.drawImage(screw, screwBounds.withCentre({faderCentreX - faderSpacing, screwsTopCY}), imgPlacement);
            g.drawImage(screw, screwBounds.withCentre({faderCentreX - faderSpacing, screwsBottomCY}), imgPlacement);
            g.drawImage(screw, screwBounds.withCentre({faderCentreX + faderSpacing, screwsTopCY}), imgPlacement);
            g.drawImage(screw, screwBounds.withCentre({faderCentreX + faderSpacing, screwsBottomCY}), imgPlacement);
        }
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();

        faderBounds = boundsF.withSizeKeepingCentre(boundsFWidth * 0.3f , boundsFHeight);
        mudAmountFader.setBounds (faderBounds.toNearestInt());
        resoFreqFader.setBounds (faderBounds.translated(-faderSpacing, 0.f).toNearestInt());
        compAmountFader.setBounds (faderBounds.translated(faderSpacing, 0.f).toNearestInt());

        const auto trackHeight = boundsFHeight * 0.8f;
        const auto trackWidth = trackHeight * (float) trackImage.getWidth() / (float) trackImage.getHeight();
        const auto trackBounds = faderBounds.withSizeKeepingCentre(trackWidth, trackHeight);
        mudTrackBounds = trackBounds;
        resoTrackBounds = trackBounds.translated(-faderSpacing, 0.f);
        compTrackBounds = trackBounds.translated(faderSpacing, 0.f);
        screwSize = trackWidth * 2.5f;
    }

    void setFaderSpacing (const float newFaderSpacing)
    {
        if (juce::approximatelyEqual(faderSpacing, newFaderSpacing))
            return;
        faderSpacing = newFaderSpacing;
        resized();
    }

    AAVerticalSSLFader resoFreqFader, mudAmountFader, compAmountFader;
    float faderSpacing {0.f};
private:
    const juce::RectanglePlacement imgPlacement {juce::RectanglePlacement::fillDestination};

    float screwSize{0.f};
    juce::Rectangle<float> faderBounds;
    juce::Image screw {juce::ImageCache::getFromMemory(BinaryData::screwDirty_png, BinaryData::screwDirty_pngSize)};
    juce::Image trackImage = juce::ImageCache::getFromMemory (BinaryData::faderTrackV2_copy_png, BinaryData::faderTrackV2_copy_pngSize);
    juce::Rectangle<float> resoTrackBounds, mudTrackBounds, compTrackBounds;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FadersPlatePanel)
};
