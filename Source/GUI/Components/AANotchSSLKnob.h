//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>

#include "../../Utils/GTLook.h"

class AANotchSSLKnob : public juce::Slider
{
public:
    explicit AANotchSSLKnob (juce::RangedAudioParameter& parameter) : attachment (parameter, *this)
    {
        jassert (knobImage.isValid());
        setSliderStyle (RotaryHorizontalVerticalDrag);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setMouseDragSensitivity (450);
        setRotaryParameters (startAngle, endAngle, true);
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsCentre = boundsF.getCentre();
        const auto sizeRef = juce::jmin (boundsF.getWidth(), boundsF.getHeight());

        const auto dotSizeRadius = sizeRef * dotRadiusAdjuster;
        const auto dotRingRadius = sizeRef * 0.5f - dotSizeRadius;

        const auto sliderPos = static_cast<float> (valueToProportionOfLength (getValue()));
        const auto angle = startAngle + sliderPos * (endAngle - startAngle);
        const auto dotSpacing = (endAngle - startAngle) / static_cast<float> (numberOfDots - 1);

        g.setColour(juce::Colours::whitesmoke);
        for (auto dotIndex = 0; dotIndex < numberOfDots; ++dotIndex)
        {
            const auto dotAngle = startAngle + static_cast<float> (dotIndex) * dotSpacing;
            const juce::Point<float> dotDirection (std::sin (dotAngle), -std::cos (dotAngle));
            const auto dotCentre = boundsCentre + dotDirection * dotRingRadius;
            g.fillEllipse (dotCentre.x - dotSizeRadius, dotCentre.y - dotSizeRadius, dotSizeRadius * 2.0f, dotSizeRadius * 2.0f);
        }

        juce::Graphics::ScopedSaveState state (g);
        g.addTransform (juce::AffineTransform::rotation (angle, boundsCentre.x, boundsCentre.y));
        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        g.drawImage (knobImage, boundsF.reduced (dotSizeRadius * knobBoundsReducer), juce::RectanglePlacement::centred);
    }

    int numberOfDots {9};

private:

    juce::Image knobImage = juce::ImageCache::getFromMemory (BinaryData::sslKnotchKnobShadow_png, BinaryData::sslKnotchKnobShadow_pngSize);

    const float startAngle { 1.25f * juce::MathConstants<float>::pi };
    const float endAngle { 2.75f * juce::MathConstants<float>::pi };
    const float dotRadiusAdjuster {0.025f};
    const float knobBoundsReducer {1.33f};

    juce::SliderParameterAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AANotchSSLKnob)
};
