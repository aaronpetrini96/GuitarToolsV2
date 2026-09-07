//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include "../../Utils/GTLook.h"
#include "../../Utils/ParamIDs.h"
#include "PluginProcessor.h"

#include "GUI/Components/AAFaderSSL.h"

class FadersPlatePanel : public juce::Component
{
public:
    explicit FadersPlatePanel (GuitarToolsAudioProcessor& processor)
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
        // const auto boundsF = getLocalBounds().toFloat();
        // const auto boundsFHeight= boundsF.getHeight();
        //
        // const auto plateCorners      = boundsFHeight * plateCornerRatio;
        // const auto borderThickness   = boundsFHeight * plateBorderRatio;
        // const auto borderBounds = boundsF.reduced (borderThickness * 0.5f);

        // g.setColour (GTColors::dividerLinesStart.withAlpha (0.125f));
        // g.setColour (GTColors::dividerLinesStart.withAlpha(JUCE_LIVE_CONSTANT(1.f)));
        // g.fillRoundedRectangle (boundsF, plateCorners);
        // g.setColour (juce::Colours::black.withAlpha (0.9f));
        // g.drawRoundedRectangle (borderBounds, plateCorners - borderThickness * 0.5f, borderThickness);

        // const auto screwSize= boundsFHeight * screwToHeightRatio;
        // const auto screwGap = screwSize * 0.8f;
        // auto leftScrewCenterX = borderBounds.getY() + screwGap;
        // const auto rightScrewCenterX= borderBounds.getRight() - screwGap;
        // const auto topScrewsCY      = borderBounds.getY() + screwGap;
        // const auto bottomScrewsCY   = borderBounds.getBottom() - screwGap;
        //
        // const juce::Rectangle<float> screwBounds (screwSize, screwSize);
        // // g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        // // g.drawImage (screw, screwBounds.withCentre ({   leftScrewCenterX,   topScrewsCY     }), juce::RectanglePlacement::centred);
        // // g.drawImage (screw, screwBounds.withCentre ({   rightScrewCenterX,  topScrewsCY     }), juce::RectanglePlacement::centred);
        // // g.drawImage (screw, screwBounds.withCentre ({   leftScrewCenterX,   bottomScrewsCY  }), juce::RectanglePlacement::centred);
        // // g.drawImage (screw, screwBounds.withCentre ({   rightScrewCenterX,  bottomScrewsCY  }), juce::RectanglePlacement::centred);
        // g.drawImage (screw, screwBounds.withCentre ({   boundsF.getCentreX(),  topScrewsCY  }), juce::RectanglePlacement::centred);
        // g.drawImage (screw, screwBounds.withCentre ({   boundsF.getCentreX(),  bottomScrewsCY  }), juce::RectanglePlacement::centred);
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
    float plateCornerRatio {0.09f};
    float plateBorderRatio {0.00f};
    float screwToHeightRatio {0.075f};
    juce::Image screw = juce::ImageCache::getFromMemory (BinaryData::screw_png, BinaryData::screw_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FadersPlatePanel)
};
