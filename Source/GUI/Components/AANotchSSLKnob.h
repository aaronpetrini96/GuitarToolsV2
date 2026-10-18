//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>
#include <memory>

#include "../../GTLook.h"

class AANotchSSLKnob : public juce::Slider
{
public:
    explicit AANotchSSLKnob (juce::RangedAudioParameter& parameterToUse)
        : parameter (parameterToUse)
    {
        jassert (knobImage.isValid());
        setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setMouseDragSensitivity (300);
        setRotaryParameters (startAngle, endAngle, true);
        setComponentID (parameter.getParameterID());

        addAndMakeVisible (valueLabel);
        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, nullptr);
        updateValueLabelText();
    }

    void valueChanged () override
    {
        updateValueLabelText();
    }

    void enablementChanged() override
    {
        valueLabel.setAlpha(isEnabled() ? 1.0f : 0.4f);
        updateValueLabelText();
        juce::Slider::enablementChanged();
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        if (!isEnabled() && onDisabledClicked != nullptr)
            onDisabledClicked();
        juce::Slider::mouseDown(event);
    }

    void paint (juce::Graphics& g) override
    {
        const auto knobArea = getLocalBounds().toFloat().withHeight (getLocalBounds().toFloat().getWidth());
        const auto boundsCentre = knobArea.getCentre();
        const auto sizeRef = knobArea.getWidth();
        const auto scaleThickness = sizeRef * scaleThicknessAdjuster;
        const auto tickLength = sizeRef * tickLengthAdjuster;
        const auto scaleRadius = sizeRef * 0.5f - tickLength - scaleThickness * 0.5f;
        const auto knobBounds = knobArea.reduced ((scaleThickness + tickLength) * knobBoundsReducer);
        const auto sliderPos = static_cast<float> (valueToProportionOfLength (getValue()));
        const auto angle = startAngle + sliderPos * (endAngle - startAngle);
        g.beginTransparencyLayer (isEnabled() ? 0.9f : 0.4f);
        paintScale (g, boundsCentre, scaleRadius, tickLength, scaleThickness);
        {
            juce::Graphics::ScopedSaveState state (g);
            g.addTransform (juce::AffineTransform::rotation (angle, boundsCentre.x, boundsCentre.y));
            g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
            g.drawImage (knobImage, knobBounds, juce::RectanglePlacement::fillDestination);
        }
        g.endTransparencyLayer();
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto knobSize = boundsF.getWidth();
        const auto labelBounds = boundsF.withTop (knobSize * 0.92f);

        valueLabel.setFont (GTFonts::ibmPlexSansMedium (labelBounds.getHeight() * 0.92f).withExtraKerningFactor (0.03f));
        valueLabel.setBounds (labelBounds.toNearestInt());
    }

    int numberOfTicks {11};
    float scaleThicknessAdjuster {0.012f};
    float tickLengthAdjuster {0.065f};
    float knobBoundsReducer {0.5f};
    std::function<void()> onDisabledClicked;

private:
    void paintScale (juce::Graphics& g, juce::Point<float> centre, float scaleRadius, float tickLength, float scaleThickness)
    {
        juce::Path scalePath;
        scalePath.addCentredArc (centre.x, centre.y, scaleRadius, scaleRadius, 0.f, startAngle, endAngle, true);

        const auto tickSpacing = (endAngle - startAngle) / static_cast<float> (numberOfTicks - 1);

        for (auto tickIndex = 0; tickIndex < numberOfTicks; ++tickIndex)
        {
            const auto tickAngle = startAngle + static_cast<float> (tickIndex) * tickSpacing;
            const juce::Point<float> tickDirection (std::sin (tickAngle), -std::cos (tickAngle));
            scalePath.startNewSubPath (centre + tickDirection * scaleRadius);
            scalePath.lineTo (centre + tickDirection * (scaleRadius + tickLength));
        }

        g.setColour (GTColors::almostWhiteText.withAlpha (0.9f));
        g.strokePath (scalePath, juce::PathStrokeType (scaleThickness, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
    }

    void updateValueLabelText()
    {
        const auto roundedValue = juce::roundToInt (getValue() * 10.0) / 10.0;
        valueLabel.setText (isEnabled() ? (roundedValue > 0.0 ? "+" : "") + juce::String (roundedValue, 1) + " dB" : "Auto");
    }

    juce::Image knobImage = juce::ImageCache::getFromMemory (BinaryData::knotchKnobShadow2_png, BinaryData::knotchKnobShadow2_pngSize);
    const float startAngle { 1.25f * juce::MathConstants<float>::pi };
    const float endAngle { 2.75f * juce::MathConstants<float>::pi };

    juce::RangedAudioParameter& parameter;
    AAUITools::PrintedText valueLabel {juce::String(), GTColors::almostWhiteText, juce::Justification::centredTop};
    std::unique_ptr<juce::SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AANotchSSLKnob)
};
