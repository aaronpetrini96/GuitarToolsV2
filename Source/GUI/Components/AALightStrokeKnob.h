//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <memory>
#include "../../AALook.h"

class AALightStrokeKnob : public juce::Slider
{
public:
    AALightStrokeKnob (juce::RangedAudioParameter& parameterToUse, const juce::String& nameText, const bool shouldDrawFromMiddle = false, const bool shouldFillFromEnd = false)
        : parameter (parameterToUse), drawFromMiddle (shouldDrawFromMiddle), fillFromEnd (shouldFillFromEnd)
    {
        jassert (knobImage.isValid());
        setSliderStyle (RotaryVerticalDrag);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setMouseDragSensitivity (400);
        setRotaryParameters (startAngle, endAngle, true);
        setComponentID (parameter.getParameterID());

        for (auto* label : { &nameLabel, &valueLabel })
        {
            label->setInterceptsMouseClicks (false, false);
            label->setColour (juce::Label::textColourId, GTColors::sectionTitleText.withAlpha(0.85f));
            label->setJustificationType(juce::Justification::centred);
            addAndMakeVisible (*label);
        }
        nameLabel.setText (nameText, juce::dontSendNotification);

        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, nullptr);
        updateValueLabelText();
    }

    void setUndoManager (juce::UndoManager& undoManager)
    {
        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, &undoManager);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto fontBounds = boundsF.withHeight (boundsF.getHeight() * 0.14f);
        nameLabel.setFont (AAFonts::ibmPlexSansMedium (fontBounds.getHeight() * 0.7f));
        nameLabel.setBounds (fontBounds.toNearestInt());
        valueLabel.setFont (AAFonts::interMedium (fontBounds.getHeight() * 0.625f));
        valueLabel.setBounds (fontBounds.withBottomY (boundsF.getHeight()).toNearestInt());
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        //========   KNOB IMAGE  ========//
        const auto imageBounds = boundsF.reduced (boundsF.getWidth() * knobReducePOW);
        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        g.setOpacity(0.85f);
        g.drawImage (knobImage, imageBounds, juce::RectanglePlacement::centred);
        g.setOpacity(1.f);
        const auto activeColor = GTColors::activeNeonBlue.withAlpha (0.85f);
        const auto offColor = GTColors::dividerLinesEnd.withAlpha (0.85f);

        //========   POINTS DRAWING  ========//
        const auto radiusReference = imageBounds.getWidth() * 0.255f;
        const auto centre = imageBounds.getCentre();
        const juce::PathStrokeType stroke (radiusReference * 0.075f, juce::PathStrokeType::mitered, juce::PathStrokeType::butt);
        const auto angle = startAngle + static_cast<float> (valueToProportionOfLength (getValue())) * (endAngle - startAngle);
        const auto midAngle = (startAngle + endAngle) * 0.5f;

        auto fillAngleRange = juce::Range<float> { startAngle, angle };
        if (drawFromMiddle)
            fillAngleRange = { juce::jmin (angle, midAngle), juce::jmax (angle, midAngle) };
        if (fillFromEnd)
            fillAngleRange = { angle, endAngle };

        const auto arcRadius = radiusReference * 1.5f;
        const auto dotRadius = radiusReference * 0.05f;
        for (auto dotIndex = 0; dotIndex < numberOfDots; ++dotIndex)
        {
            const auto dotProportion = static_cast<float> (dotIndex) / static_cast<float> (numberOfDots - 1);
            const auto dotAngle = startAngle + dotProportion * (endAngle - startAngle);
            const juce::Point<float> dotDirection (std::sin (dotAngle), -std::cos (dotAngle));
            const auto dotCentre = centre + dotDirection * arcRadius * 0.77f;
            const auto isLit = dotAngle >= fillAngleRange.getStart() && dotAngle <= fillAngleRange.getEnd();
            g.setColour (isLit ? activeColor: offColor);
            g.fillEllipse (dotCentre.x - dotRadius, dotCentre.y - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
        }

        //========   TICK PAINTING  ========//
        g.setColour (activeColor);
        const juce::Point<float> tickDirection (std::sin (angle), -std::cos (angle));
        juce::Path tick;
        tick.startNewSubPath (centre + tickDirection * (radiusReference * 0.4f));
        tick.lineTo (centre + tickDirection * (radiusReference * 0.65f));
        g.strokePath (tick, stroke);
    }

private:
    void valueChanged() override
    {
        updateValueLabelText();
    }

    void updateValueLabelText()
    {
        const auto sliderPosition = valueToProportionOfLength (getValue());
        juce::String valueText;

        if (drawFromMiddle)
        {
            const auto percentage = juce::roundToInt ((sliderPosition - 0.5) * 200.0);
            valueText = juce::String (percentage > 0 ? "+" : "") + juce::String (percentage);
        }
        else if (fillFromEnd)
        {
            const auto percentage = juce::roundToInt ((1.0 - sliderPosition) * 100.0);
            valueText = juce::String (percentage);
        }
        else
        {
            const auto percentage = juce::roundToInt (sliderPosition * 100.0);
            valueText = juce::String (percentage);
        }

        valueLabel.setText (valueText + "%", juce::dontSendNotification);
    }

    juce::Image knobImage = juce::ImageCache::getFromMemory(BinaryData::_23232_png, BinaryData::_23232_pngSize);
    // juce::Image knobImage = juce::ImageCache::getFromMemory(BinaryData::_2_png, BinaryData::_2_pngSize);
    // juce::Image knobImage = juce::ImageCache::getFromMemory(BinaryData::knob_preview3Shadow_png, BinaryData::knob_preview3Shadow_pngSize);
    juce::Label nameLabel;
    juce::Label valueLabel;
    juce::RangedAudioParameter& parameter;

    const bool drawFromMiddle;
    const bool fillFromEnd;
    const float startAngle { 1.25f * juce::MathConstants<float>::pi };
    const float endAngle { 2.75f * juce::MathConstants<float>::pi };
    const int numberOfDots { 13 };
    const float knobReducePOW { 0.f };

    std::unique_ptr<juce::SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AALightStrokeKnob)
};
