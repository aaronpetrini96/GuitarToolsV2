//
// Created by Aaron Petrini on 30/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "../../Utils/GTLook.h"

class AAVerticalSSLFader : public juce::Slider
{
public:
    AAVerticalSSLFader (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID,
                        const juce::String& suffix = {})
        : attachment (apvts, parameterID.getParamID(), *this)
    {
        jassert (thumbON.isValid() && thumbOFF.isValid() && trackImage.isValid());
        setSliderStyle (LinearVertical);
        setTextBoxStyle (NoTextBox, false, 0, 0);
        setSliderSnapsToMousePosition (false);
        setMouseDragSensitivity(250);
        setTextValueSuffix (suffix);
    }

    void resized() override
    {
        juce::Slider::resized();

        const auto boundsF = getLocalBounds().toFloat();
        const auto slotAspectRatio = (float) trackImage.getHeight() / (float) trackImage.getWidth();
        const auto slotWidth = boundsF.getHeight() / (slotAspectRatio + screwToTrackWidthRatio + thumbToTrackWidthRatio * thumbAspectRatio);
        const auto screwSize = slotWidth * screwToTrackWidthRatio;
        const auto thumbWidth = slotWidth * thumbToTrackWidthRatio;
        const auto thumbHeight = thumbWidth * thumbAspectRatio;
        const auto screwGap = slotWidth * 0.25f;

        slotBounds = boundsF.withSizeKeepingCentre (slotWidth, slotWidth * slotAspectRatio);
        thumbBounds = boundsF.withSizeKeepingCentre (thumbWidth, thumbHeight);

        const auto screwArea = boundsF.withSizeKeepingCentre (screwSize, screwSize);
        topScrewBounds = screwArea.withBottomY (slotBounds.getY() - screwGap);
        bottomScrewBounds = screwArea.withY (slotBounds.getBottom() + screwGap);

        thumbTravel = { slotBounds.getY(), slotBounds.getBottom() };
    }

    void paint (juce::Graphics& g) override
    {
        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        g.setOpacity(isEnabled() ? 0.95f : 0.85f);
        g.drawImage (trackImage, slotBounds, juce::RectanglePlacement::centred);
        g.drawImage (screw, topScrewBounds, juce::RectanglePlacement::centred);
        g.drawImage (screw, bottomScrewBounds, juce::RectanglePlacement::centred);
        const auto thumbCY = thumbTravel.getEnd() -  static_cast<float>(valueToProportionOfLength(getValue())) * thumbTravel.getLength();
        const auto positionedThumbBounds = thumbBounds.withCentre ({ thumbBounds.getCentreX(), thumbCY});
        g.setOpacity(0.95f);
        g.drawImage (isEnabled() ? thumbON : thumbOFF, positionedThumbBounds, juce::RectanglePlacement::fillDestination);
    }

private:
    void mouseDown (const juce::MouseEvent& event) override
    {
        const auto positionedThumbBounds = thumbBounds.withCentre ({ thumbBounds.getCentreX(),
                                              thumbTravel.getEnd() - (float) valueToProportionOfLength (getValue()) * thumbTravel.getLength() });

        if (isEnabled() && event.mods.isLeftButtonDown() && ! positionedThumbBounds.contains (event.position))
        {
            const auto proportion = juce::jlimit (0.0f, 1.0f, (thumbTravel.getEnd() - event.position.y) / thumbTravel.getLength());
            setValue (proportionOfLengthToValue (proportion), juce::sendNotificationSync);
        }

        juce::Slider::mouseDown (event);
    }

    juce::Rectangle<float> slotBounds;
    juce::Rectangle<float> topScrewBounds;
    juce::Rectangle<float> bottomScrewBounds;
    juce::Rectangle<float> thumbBounds;
    juce::Range<float> thumbTravel;

//    juce::Rectangle<float> faderTrackBounds;

    juce::Image thumbOFF = juce::ImageCache::getFromMemory (BinaryData::thumbDarkGrey_png, BinaryData::thumbDarkGrey_pngSize);
    juce::Image thumbON = juce::ImageCache::getFromMemory (BinaryData::thumbBlue_png, BinaryData::thumbBlue_pngSize);
    juce::Image trackImage = juce::ImageCache::getFromMemory (BinaryData::faderTrackClipped_png, BinaryData::faderTrackClipped_pngSize);
    juce::Image screw = juce::ImageCache::getFromMemory (BinaryData::screwDarkGreyv3_png, BinaryData::screwDarkGreyv3_pngSize);

    // the thumb artwork must be drawn with this height to width ratio
    const float thumbAspectRatio {1.5f};
    const float thumbToTrackWidthRatio {4.25f};
    const float screwToTrackWidthRatio {2.47};

//    const float trackWidthRatio {0.095f};

    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAVerticalSSLFader)
};
