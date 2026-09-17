//
// Created by Aaron Petrini on 30/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include "../../Utils/GTLook.h"

class AAVerticalSSLFader : public juce::Slider
{
public:
    explicit AAVerticalSSLFader (juce::RangedAudioParameter& parameterToUse, const juce::String& suffix = {})
        : parameter (parameterToUse)
    {
        jassert (thumbON.isValid() && thumbOFF.isValid() && trackImage.isValid());
        setSliderStyle (LinearVertical);
        setTextBoxStyle (NoTextBox, false, 0, 0);
        setSliderSnapsToMousePosition (false);
        setMouseDragSensitivity(250);
        setTextValueSuffix (suffix);
        setComponentID(parameter.getParameterID());
        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, nullptr);
    }

    void setUndoManager (juce::UndoManager* undoManager)
    {
        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, undoManager);
    }

    void setThumbActive (const bool shouldBeActive)
    {
        if (shouldBeActive != thumbActive)
        {
            thumbActive = shouldBeActive;
            repaint();
        }
    }

    void resized() override
    {
        juce::Slider::resized();

        const auto boundsF = getLocalBounds().toFloat();
        const auto slotAspectRatio = (float) trackImage.getHeight() / (float) trackImage.getWidth();
        const auto slotWidth = boundsF.getHeight() / (slotAspectRatio + thumbToTrackWidthRatio * thumbAspectRatio);
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
        g.setOpacity(0.95f);
        g.drawImage (trackImage, slotBounds, juce::RectanglePlacement::fillDestination);
        g.drawImage (screw, topScrewBounds, juce::RectanglePlacement::fillDestination);
        g.drawImage (screw, bottomScrewBounds, juce::RectanglePlacement::fillDestination);
        const auto thumbCY = thumbTravel.getEnd() -  static_cast<float>(valueToProportionOfLength(getValue())) * thumbTravel.getLength();
        const auto positionedThumbBounds = thumbBounds.withCentre ({ thumbBounds.getCentreX(), thumbCY});
        g.drawImage (thumbActive ? thumbON : thumbOFF, positionedThumbBounds, juce::RectanglePlacement::fillDestination);
    }

private:

    void mouseDown (const juce::MouseEvent& event) override
    {
        const auto thumbValueCY = thumbTravel.getEnd() - static_cast<float>(valueToProportionOfLength(getValue())) * thumbTravel.getLength();
        const auto positionedThumbBounds = thumbBounds.withCentre ({ thumbBounds.getCentreX(), thumbValueCY });

        if (event.mods.isLeftButtonDown() && ! positionedThumbBounds.contains (event.position))
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
    bool thumbActive {true};

    juce::Image thumbOFF = juce::ImageCache::getFromMemory (BinaryData::thumbDarkGrey_png, BinaryData::thumbDarkGrey_pngSize);
    juce::Image thumbON = juce::ImageCache::getFromMemory (BinaryData::thumbBlue_png, BinaryData::thumbBlue_pngSize);
    juce::Image trackImage = juce::ImageCache::getFromMemory (BinaryData::faderTrackClippedShort_png, BinaryData::faderTrackClippedShort_pngSize);
    juce::Image screw = juce::ImageCache::getFromMemory (BinaryData::screwDarkGreyv3_png, BinaryData::screwDarkGreyv3_pngSize);

    const float thumbAspectRatio {1.5f}; // the thumb artwork must be drawn with this height to width ratio
    const float thumbToTrackWidthRatio {2.75f};
    const float screwToTrackWidthRatio {1.5f};

    juce::RangedAudioParameter& parameter;
    std::unique_ptr<juce::SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAVerticalSSLFader)
};
