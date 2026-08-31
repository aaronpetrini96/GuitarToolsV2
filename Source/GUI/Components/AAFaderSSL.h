//
// Created by Aaron Petrini on 30/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "GTLook.h"

class AAFaderLAF  : public juce::LookAndFeel_V4
{
public:
    AAFaderLAF()
    {
        setColour (juce::Slider::textBoxTextColourId, GTColors::clearGrey);
        setColour (juce::Slider::backgroundColourId, GTColors::dividerLinesStart);
        setColour (juce::Slider::trackColourId, GTColors::dividerLinesEnd);

        jassert (thumbImage.isValid());
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float, float,
                           const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        const juce::Rectangle<float> sliderBounds ((float) x, (float) y, (float) width, (float) height);

        const auto trackThickness = sliderBounds.getWidth() * trackToWidthRatio;
        const auto thumbHeight = (float) slider.getHeight() * thumbToHeightRatio;
        const auto thumbWidth = thumbImage.isValid()
                                  ? thumbHeight * (float) thumbImage.getWidth() / (float) thumbImage.getHeight()
                                  : 0.0f;
        const auto trackBounds = sliderBounds.withSizeKeepingCentre (trackThickness, sliderBounds.getHeight());

        g.setGradientFill (juce::ColourGradient::vertical (slider.findColour (juce::Slider::backgroundColourId), trackBounds.getY(),
                                                           slider.findColour (juce::Slider::trackColourId), trackBounds.getBottom()));
        g.fillRoundedRectangle (trackBounds, trackThickness * 0.5f);

        if (! thumbImage.isValid())
            return;

        const auto thumbBounds = sliderBounds.withSizeKeepingCentre (thumbWidth, thumbHeight).withCentre ({ sliderBounds.getCentreX(), sliderPos });

        g.drawImage (thumbImage, thumbBounds, juce::RectanglePlacement::centred);
    }

    juce::Slider::SliderLayout getSliderLayout (juce::Slider& slider) override
    {
        const auto boundsF = slider.getLocalBounds().toFloat();

        if (! thumbImage.isValid())
            return juce::LookAndFeel_V4::getSliderLayout (slider);

        const auto thumbHeight = boundsF.getHeight() * thumbToHeightRatio;
        const auto sliderBoundsHeight = boundsF.getHeight() * trackToHeightRatio;
        const auto sliderBoundsBottom = boundsF.getBottom() - thumbHeight * 0.5f;

        juce::Slider::SliderLayout layout;

        layout.sliderBounds = boundsF.withHeight (sliderBoundsHeight)
                                     .withY (sliderBoundsBottom - sliderBoundsHeight)
                                     .toNearestInt();
        layout.textBoxBounds = boundsF.withHeight (juce::jmax(thumbHeight, boundsF.getHeight() - sliderBoundsHeight)).toNearestInt();

        return layout;
    }

    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        if (label.isBeingEdited())
            return;
        const auto labelBounds = label.getLocalBounds().toFloat();

        g.setFont (GTFonts::jetBrainsMonoMedium (labelBounds.getHeight() * valueLabelFontPOH));
        g.setColour (label.findColour (juce::Label::textColourId));
        g.drawText (label.getText(), labelBounds, juce::Justification::centred, true);
    }

    void fillTextEditorBackground (juce::Graphics&, int, int, juce::TextEditor&) override {}
    void drawTextEditorOutline (juce::Graphics&, int, int, juce::TextEditor&) override {}

    float trackToWidthRatio {0.1f};
    float trackToHeightRatio {0.75f};
    float thumbToHeightRatio {0.2f};
    float valueLabelFontPOH {0.7f};

private:
    juce::Image thumbImage = juce::ImageCache::getFromMemory (BinaryData::darkThumb_png, BinaryData::darkThumb_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AAFaderLAF)
};

class AAFaderSSL : public  juce::Slider
{
public:
    AAFaderSSL (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID,
                const juce::String& suffix = {}, const int numberOfDecimalPlaces = 0)
        : attachment (apvts, parameterID.getParamID(), *this)
    {
        setSliderStyle (juce::Slider::LinearVertical);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setNumDecimalPlacesToDisplay (numberOfDecimalPlaces);
        setTextValueSuffix (suffix);
        setLookAndFeel (&laf);
    }

    ~AAFaderSSL() override
    {
        setLookAndFeel (nullptr);
    }

    juce::String getTextFromValue (double value) override
    {
        if (getNumDecimalPlacesToDisplay() == 0)
            return juce::String (juce::roundToInt (value)) + getTextValueSuffix();

        return juce::String (value, getNumDecimalPlacesToDisplay()) + getTextValueSuffix();
    }

private:
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    AAFaderLAF laf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AAFaderSSL)
};
