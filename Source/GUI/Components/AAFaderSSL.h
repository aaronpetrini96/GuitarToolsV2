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
//        jassert (thumbON.isValid() && thumbOFF.isValid() && screw.isValid());
        setSliderStyle (LinearVertical);
        setTextBoxStyle (NoTextBox, false, 0, 0);
        setSliderSnapsToMousePosition (false);
        setMouseDragSensitivity(400);
        setTextValueSuffix (suffix);

        slotImage = trackImage.getClippedImage ({ 0, juce::roundToInt ((float) trackImage.getHeight() * slotTopRatio),
                                                 trackImage.getWidth(), juce::roundToInt ((float) trackImage.getHeight() * (slotBottomRatio - slotTopRatio)) });
    }

    void resized() override
    {
        juce::Slider::resized();

        const auto boundsF = getLocalBounds().toFloat();
        const auto slotAspectRatio = (float) slotImage.getHeight() / (float) slotImage.getWidth();
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

        thumbTravel = { slotBounds.getY() + thumbHeight * 0.5f, slotBounds.getBottom() - thumbHeight * 0.5f };

//        const auto faderTrackWidth = boundsF.getWidth() * trackWidthRatio;
//        const auto screwSize = faderTrackWidth * screwToTrackWidthRatio;
//        const auto thumbWidth = faderTrackWidth * thumbToTrackWidthRatio;
//        const auto thumbHeight = thumbWidth * thumbAspectRatio;
//        const auto faderTrackHeight = boundsF.getHeight() - screwSize - thumbHeight;
//        const auto screwGap = faderTrackWidth * 0.25f;
//
//        faderTrackBounds = boundsF.withSizeKeepingCentre (faderTrackWidth, faderTrackHeight);
//
//        const auto screwArea = boundsF.withSizeKeepingCentre (screwSize, screwSize);
//        topScrewBounds = screwArea.withBottomY (faderTrackBounds.getY() - screwGap);
//        bottomScrewBounds = screwArea.withY (faderTrackBounds.getBottom() + screwGap);
//
//        thumbBounds = boundsF.withSizeKeepingCentre (thumbWidth, thumbHeight);
//        thumbTravel = { faderTrackBounds.getY(), faderTrackBounds.getBottom() };
    }

    void paint (juce::Graphics& g) override
    {
        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        g.drawImage (slotImage, slotBounds, juce::RectanglePlacement::centred);
        g.drawImage (screw, topScrewBounds, juce::RectanglePlacement::centred);
        g.drawImage (screw, bottomScrewBounds, juce::RectanglePlacement::centred);

//        const auto faderTrackWidth = faderTrackBounds.getWidth();
//        g.setColour (isEnabled() ? GTColors::dividerLinesEnd : GTColors::dividerLinesEnd.withAlpha(0.5f));
//        g.fillRoundedRectangle (faderTrackBounds, faderTrackWidth * 0.5f);
//        g.setColour (isEnabled() ? GTColors::dividerLinesEnd.brighter(0.1f) : GTColors::dividerLinesEnd.brighter(0.1f).withAlpha(0.5f));
//        g.drawRoundedRectangle (faderTrackBounds, faderTrackWidth * 0.5f, faderTrackWidth * 0.2f);
//
//        g.setOpacity (1.f);
//        g.drawImage (screw, topScrewBounds, juce::RectanglePlacement::fillDestination);
//        g.drawImage (screw, bottomScrewBounds, juce::RectanglePlacement::fillDestination);

        const auto positionedThumbBounds = thumbBounds.withCentre ({ thumbBounds.getCentreX(),
                                              thumbTravel.getEnd() - (float) valueToProportionOfLength (getValue()) * thumbTravel.getLength() });

        g.drawImage (isEnabled() ? thumbON : thumbOFF, positionedThumbBounds, juce::RectanglePlacement::fillDestination, false);
    }

    juce::MouseCursor getMouseCursor() override
    {
        const auto positionedThumbBounds = thumbBounds.withCentre ({ thumbBounds.getCentreX(),
                                              thumbTravel.getEnd() - (float) valueToProportionOfLength (getValue()) * thumbTravel.getLength() });

        return positionedThumbBounds.contains (getMouseXYRelative().toFloat()) ? juce::MouseCursor::DraggingHandCursor
                                                                              : juce::MouseCursor::NormalCursor;
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
    juce::Image thumbON = juce::ImageCache::getFromMemory (BinaryData::thumbDarkBlue_png, BinaryData::thumbDarkBlue_pngSize);
    juce::Image trackImage = juce::ImageCache::getFromMemory (BinaryData::faderWithScrews_png, BinaryData::faderWithScrews_pngSize);
    juce::Image screw = juce::ImageCache::getFromMemory (BinaryData::screwDarkGreyv3_png, BinaryData::screwDarkGreyv3_pngSize);
    juce::Image slotImage;

    // the thumb artwork must be drawn with this height to width ratio
    const float thumbAspectRatio {1.5f};
    const float thumbToTrackWidthRatio {4.f};

    // the slot runs between these fractions of the track artwork height
    const float slotTopRatio {0.04f};
    const float slotBottomRatio {0.96f};

    const float screwToTrackWidthRatio {2.25f};

//    const float trackWidthRatio {0.095f};

    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAVerticalSSLFader)
};
