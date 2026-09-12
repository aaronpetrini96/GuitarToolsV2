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
    explicit AANotchSSLKnob (juce::RangedAudioParameter& parameter, juce::UndoManager* undoManager = nullptr) : attachment (parameter, *this, undoManager)
    {
        jassert (knobImage.isValid());
        setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setNumDecimalPlacesToDisplay (1);
        setTextValueSuffix (" " + parameter.getLabel());
        setMouseDragSensitivity (300);
        setRotaryParameters (startAngle, endAngle, true);
    }

    void startedDragging() override
    {
        isDragging = true;
        repaint();
    }

    void stoppedDragging() override
    {
        isDragging = false;
        repaint();
    }

    void enablementChanged() override
    {
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.beginTransparencyLayer (isEnabled() ? 1.0f : 0.5f);

        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsCentre = boundsF.getCentre();
        const auto sizeRef = juce::jmin (boundsF.getWidth(), boundsF.getHeight());

        const auto dotSizeRadius = sizeRef * dotSizeAdjuster;
        const auto dotRingRadius = sizeRef * 0.5f - dotSizeRadius;
        const auto knobBounds = boundsF.reduced (dotSizeRadius * knobBoundsReducer);

        const auto sliderPos = static_cast<float> (valueToProportionOfLength (getValue()));
        const auto angle = startAngle + sliderPos * (endAngle - startAngle);
        paintLightDots (g, boundsCentre, dotRingRadius, dotSizeRadius, angle);

        {
            juce::Graphics::ScopedSaveState state (g);
            g.addTransform (juce::AffineTransform::rotation (angle, boundsCentre.x, boundsCentre.y));
            g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
            g.drawImage (knobImage, knobBounds, juce::RectanglePlacement::centred);
        }

        if (isDragging)
        {
            const auto valueBounds = knobBounds.withSizeKeepingCentre (knobBounds.getWidth() * 0.65f, knobBounds.getWidth() * 0.25f).withY(knobBounds.getCentreX());
            g.setColour (GTColors::dividerLinesEnd);
            g.fillRect (valueBounds);
            g.setColour (GTColors::almostWhite);
            g.setFont (GTFonts::interMedium (valueBounds.getHeight() * 0.65f));
            g.drawFittedText (getTextFromValue (getValue()), valueBounds.toNearestInt(), juce::Justification::centred, 1);
        }

        g.endTransparencyLayer();
    }

    int numberOfDots {11};

private:

    void paintLightDots (juce::Graphics& g, juce::Point<float> centre, float dotRingRadius, float dotRadius, float angle)
    {
        const auto dotSpacing = (endAngle - startAngle) / static_cast<float> (numberOfDots - 1);
        const auto midAngle = (startAngle + endAngle) * 0.5f;
        const auto litFromAngle = juce::jmin (angle, midAngle);
        const auto litToAngle = juce::jmax (angle, midAngle);

        for (auto dotIndex = 0; dotIndex < numberOfDots; ++dotIndex)
        {
            const auto dotAngle = startAngle + static_cast<float> (dotIndex) * dotSpacing;
            const juce::Point<float> dotDirection (std::sin (dotAngle), -std::cos (dotAngle));
            const auto dotCentre = centre + dotDirection * dotRingRadius;

            const auto isCentreDot = dotIndex == (numberOfDots - 1) / 2;
            const auto isLit = isCentreDot || (dotAngle >= litFromAngle && dotAngle <= litToAngle);
            g.setColour (isLit ? GTColors::activeNeonBlue : GTColors::dividerLinesEnd);
            g.fillEllipse (dotCentre.x - dotRadius, dotCentre.y - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
        }
    }

    juce::Image knobImage = juce::ImageCache::getFromMemory (BinaryData::sslKnotchKnobShadow2_png, BinaryData::sslKnotchKnobShadow2_pngSize);
    const float startAngle { 1.25f * juce::MathConstants<float>::pi };
    const float endAngle { 2.75f * juce::MathConstants<float>::pi };
    const float dotSizeAdjuster {0.025f};
    const float knobBoundsReducer {1.f};

    bool isDragging {false};
    juce::SliderParameterAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AANotchSSLKnob)
};
