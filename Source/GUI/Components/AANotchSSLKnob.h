//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>
#include <memory>

#include "../../AALook.h"

class AANotchSSLKnob : public juce::Slider
{
public:
    AANotchSSLKnob (juce::RangedAudioParameter& parameterToUse, const juce::String& nameTextToUse)
        : nameText (nameTextToUse), parameter (parameterToUse)
    {
        jassert (knobImage.isValid());
        setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
        setScrollWheelEnabled(false);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setMouseDragSensitivity (300);
        setRotaryParameters (startAngle, endAngle, true);
        setComponentID (parameter.getParameterID());

        addAndMakeVisible (valueLabel);
        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, nullptr);
        updateValueDisplay();
    }

    void setUndoManager (juce::UndoManager& undoManager)
    {
        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, &undoManager);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (isEnabled() && ! isDragging && (e.mouseWasDraggedSinceMouseDown()))
        {
            isDragging = true;
            updateValueDisplay();
        }

        juce::Slider::mouseDrag (e);
    }

    void stoppedDragging() override
    {
        isDragging = false;
        updateValueDisplay();
    }

    void valueChanged () override
    {
        updateValueDisplay();
    }

    void enablementChanged() override
    {
        repaint();
    }



    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto knobArea = boundsF.withHeight (boundsF.getWidth());
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

        valueLabel.setFont (AAFonts::ibmPlexSansMedium (labelBounds.getHeight() * 0.9f));
        valueLabel.setBounds (labelBounds.toNearestInt());
    }

    int numberOfTicks {11};
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

        g.setColour (GTColors::almostWhite.withAlpha (0.8f));
        g.strokePath (scalePath, juce::PathStrokeType (scaleThickness, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
    }

    void updateValueDisplay()
    {
        const auto roundedValue = juce::roundToInt (getValue() * 10.0) / 10.0;
        valueLabel.setText (isDragging ? (roundedValue > 0.0 ? "+" : "") + juce::String (roundedValue, 1) : nameText);
    }

    juce::Image knobImage = juce::ImageCache::getFromMemory (BinaryData::knotchKnobShadow2_png, BinaryData::knotchKnobShadow2_pngSize);
    const float startAngle { 1.25f * juce::MathConstants<float>::pi };
    const float endAngle { 2.75f * juce::MathConstants<float>::pi };
    const float scaleThicknessAdjuster {0.012f};
    const float tickLengthAdjuster {0.065f};
    const float knobBoundsReducer {0.5f};

    bool isDragging {false};
    const juce::String nameText;
    juce::RangedAudioParameter& parameter;
    AAUITools::PrintedText valueLabel {juce::String(), GTColors::almostWhite.withAlpha (0.85f), juce::Justification::centredTop};
    std::unique_ptr<juce::SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AANotchSSLKnob)
};
