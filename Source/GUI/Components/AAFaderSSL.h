//
// Created by Aaron Petrini on 30/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include "../../GTLook.h"

class ValuePopupLAF : public juce::LookAndFeel_V4
{
public:
    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        const auto boundsF = label.getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        AAUITools::drawOutlinedRoundedRectangle (g, boundsF, boundsFHeight * GTCornerPOH, boundsFHeight * GTOutlineThicknessPOH, GTPopupBackgroundColor, GTPopupOutlineColor);
        g.setColour (GTPopupTextColor);
        g.setFont (GTFonts::montserratSemiBold (boundsFHeight * GTFontHeightPOH));
        g.drawText (label.getText(), boundsF, juce::Justification::centred, false);
    }
};

class AAVerticalSSLFader : public juce::Slider
{
public:
    explicit AAVerticalSSLFader (juce::RangedAudioParameter& parameterToUse, const juce::String& suffix = {})
        : parameter (parameterToUse)
    {
        jassert (thumbON.isValid() && thumbOFF.isValid());
        setSliderStyle (LinearVertical);
        setTextBoxStyle (NoTextBox, false, 0, 0);
        setSliderSnapsToMousePosition (false);
        setMouseDragSensitivity(250);
        setTextValueSuffix (suffix);
        setComponentID(parameter.getParameterID());
        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, nullptr);
    }

    void paint (juce::Graphics& g) override
    {
        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        const auto thumbCY = thumbTravel.getEnd() - static_cast<float> (valueToProportionOfLength (getValue())) * thumbTravel.getLength();
        g.drawImage (thumbActive ? thumbON : thumbOFF, thumbBounds.withCentre ({ thumbBounds.getCentreX(), thumbCY }),
                     juce::RectanglePlacement::fillDestination);
    }

    void resized() override
    {
        juce::Slider::resized();
        const auto boundsF = getLocalBounds().toFloat();
        const auto thumbHeight = boundsF.getHeight() * 0.22f;
        const auto thumbWidth = thumbHeight / ((float) thumbOFF.getHeight() / (float) thumbOFF.getWidth());
        thumbBounds = boundsF.withSizeKeepingCentre (thumbWidth, thumbHeight);
        const auto gap = thumbHeight * 0.525f;
        thumbTravel = { boundsF.getY() + gap, boundsF.getBottom() - gap};
    }

    void mouseDown (const juce::MouseEvent& event) override
    {
        const auto thumbValueCY = thumbTravel.getEnd() - static_cast<float>(valueToProportionOfLength(getValue())) * thumbTravel.getLength();
        const auto positionedThumbBounds = thumbBounds.withCentre ({ thumbBounds.getCentreX(), thumbValueCY }).reduced (0.0f, thumbBounds.getHeight() * 0.07f);

        if (event.mods.isLeftButtonDown() && ! positionedThumbBounds.contains (event.position))
        {
            const auto proportion = juce::jlimit (0.0f, 1.0f, (thumbTravel.getEnd() - event.position.y) / thumbTravel.getLength());
            parameter.beginChangeGesture();
            parameter.setValueNotifyingHost(parameter.convertTo0to1(static_cast<float>(proportionOfLengthToValue(proportion))));
            parameter.endChangeGesture();
        }

        juce::Slider::mouseDown (event);
    }

    void setThumbActive (const bool shouldBeActive)
    {
        if (shouldBeActive != thumbActive)
        {
            thumbActive = shouldBeActive;
            repaint();
        }
    }

    bool hitTest (int x, int) override
    {
        return thumbBounds.getHorizontalRange().contains (static_cast<float> (x));
    }


private:
    juce::Rectangle<float> thumbBounds;
    juce::Range<float> thumbTravel;
    bool thumbActive {true};

    juce::Image thumbOFF = juce::ImageCache::getFromMemory (BinaryData::thumbOffShadow_png, BinaryData::thumbOffShadow_pngSize);
    juce::Image thumbON = juce::ImageCache::getFromMemory (BinaryData::thumbBlue2_png, BinaryData::thumbBlue2_pngSize);

    juce::RangedAudioParameter& parameter;
    std::unique_ptr<juce::SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAVerticalSSLFader)
};
