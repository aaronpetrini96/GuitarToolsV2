//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>

#include "../../Utils/GTLook.h"

class AALightStrokeKnob : public juce::Slider
{
public:
    AALightStrokeKnob (juce::RangedAudioParameter& parameter, const juce::String& nameText, const bool shouldDrawDots = true, const bool shouldDrawFromMiddle = false, const bool shouldFillFromEnd = false)
        : drawDots (shouldDrawDots), drawFromMiddle (shouldDrawFromMiddle), fillFromEnd (shouldFillFromEnd), attachment (parameter, *this)
    {
        jassert (knobImage.isValid());
        setSliderStyle (RotaryVerticalDrag);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setMouseDragSensitivity (400);
        setRotaryParameters (startAngle, endAngle, true);
        setTextValueSuffix (parameter.getLabel().isEmpty() ? juce::String() : " " + parameter.getLabel());
        // setDoubleClickReturnValue(slider);

        for (auto* label : { &nameLabel, &valueLabel })
        {
            label->setJustificationType (juce::Justification::centred);
            label->setColour (juce::Label::textColourId, GTColors::clearGrey);
            addAndMakeVisible (*label);
        }

        nameLabel.setText (nameText, juce::dontSendNotification);
        nameLabel.setInterceptsMouseClicks (false, false);

        valueLabel.setColour (juce::Label::ColourIds::outlineWhenEditingColourId, GTColors::transparent);
        valueLabel.setColour (juce::Label::ColourIds::backgroundWhenEditingColourId, GTColors::transparent);
        valueLabel.setColour (juce::Label::ColourIds::textWhenEditingColourId, GTColors::clearGrey);
        valueLabel.setInterceptsMouseClicks (true, false);
        valueLabel.setEditable (false, true);
        valueLabel.setKeyboardType (juce::TextInputTarget::decimalKeyboard);
        valueLabel.onEditorShow = [this]
        {
            if (auto* editor = valueLabel.getCurrentTextEditor())
            {
                editor->setBorder (juce::BorderSize<int>());
                editor->setIndents (valueLabel.getBorderSize().getLeft(), valueLabel.getBorderSize().getTop());
                editor->setJustification (valueLabel.getJustificationType());
                editor->setPopupMenuEnabled (false);
            }
        };
        valueLabel.onTextChange = [this]
        {
            setValue (getValueFromText (valueLabel.getText()), juce::sendNotificationSync);
            updateValueLabelText();
        };

        updateValueLabelText();
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto fontBounds = boundsF.withHeight (boundsF.getHeight() * 0.15f);
        nameLabel.setFont (GTFonts::ibmPlexSansMedium (fontBounds.getHeight() * 0.7f));
        valueLabel.setFont (GTFonts::ibmPlexMonoMedium (fontBounds.getHeight() * 0.675f));
        nameLabel.setBounds (fontBounds.toNearestInt());
        valueLabel.setBounds (fontBounds.withY (boundsF.getHeight() - fontBounds.getHeight()).toNearestInt());
    }

    void paintOverChildren (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        if (boundsF.isEmpty())
            return;

        const auto imageBounds = boundsF.reduced (boundsF.getWidth() * 0.0275f);

        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        g.setOpacity (0.8f);
        g.drawImage (knobImage, imageBounds, juce::RectanglePlacement::centred);

        const auto radiusReference = imageBounds.getWidth() * 0.25f;
        const auto centre = imageBounds.getCentre();
        const auto arcRadius = radiusReference * 1.5f;
        const juce::PathStrokeType stroke (radiusReference * 0.075f, juce::PathStrokeType::mitered, juce::PathStrokeType::butt);
        const auto sliderPos = static_cast<float> (valueToProportionOfLength (getValue()));

        const auto angle = startAngle + sliderPos * (endAngle - startAngle);
        const auto midAngle = (startAngle + endAngle) * 0.5f;
        auto fillAngleRange = juce::Range<float> { startAngle, angle };

        if (drawFromMiddle)
            fillAngleRange = { juce::jmin (angle, midAngle), juce::jmax (angle, midAngle) };
        else if (fillFromEnd)
            fillAngleRange = { angle, endAngle };

        if (drawDots)
            paintLightDots (g, centre, arcRadius, radiusReference, fillAngleRange);
        else
            paintFillStroke (g, centre, arcRadius, stroke, fillAngleRange);

        g.setColour (GTColors::accentActive);

        const juce::Point<float> tickDirection (std::sin (angle), -std::cos (angle));
        juce::Path tick;
        tick.startNewSubPath (centre + tickDirection * (radiusReference * 0.4f));
        tick.lineTo (centre + tickDirection * (radiusReference * 0.65f));
        g.strokePath (tick, stroke);
    }

private:
    void paintFillStroke (juce::Graphics& g, juce::Point<float> centre, float arcRadius, const juce::PathStrokeType& stroke, juce::Range<float> fillAngleRange)
    {
        juce::Path backgroundArc;
        backgroundArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, startAngle, endAngle, true);
        g.setColour (GTColors::dividerLinesStart);
        g.strokePath (backgroundArc, stroke);

        // keeps a stub as wide as the tick when the value sits on the arc's origin
        const auto minStubAngle = stroke.getStrokeThickness() * 0.5f / arcRadius;

        juce::Path valueArc;
        valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, fillAngleRange.getStart() - minStubAngle, fillAngleRange.getEnd() + minStubAngle, true);
        g.setColour (GTColors::accentActive);
        g.strokePath (valueArc, stroke);
    }

    void paintLightDots (juce::Graphics& g, juce::Point<float> centre, float arcRadius, float radiusReference, juce::Range<float> fillAngleRange)
    {
        const auto dotRadius = radiusReference * 0.05f;

        for (auto dotIndex = 0; dotIndex < numberOfDots; ++dotIndex)
        {
            const auto dotProportion = static_cast<float> (dotIndex) / static_cast<float> (numberOfDots - 1);
            const auto dotAngle = startAngle + dotProportion * (endAngle - startAngle);
            const juce::Point<float> dotDirection (std::sin (dotAngle), -std::cos (dotAngle));
            const auto dotCentre = centre + dotDirection * arcRadius * 0.77f;

            const auto isLit = dotAngle >= fillAngleRange.getStart() && dotAngle <= fillAngleRange.getEnd();
            g.setColour (isLit ? GTColors::accentActive : GTColors::dividerLinesStart);
            g.fillEllipse (dotCentre.x - dotRadius, dotCentre.y - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
        }
    }

    void valueChanged() override
    {
        updateValueLabelText();
    }

    void updateValueLabelText()
    {
        valueLabel.setText (getTextFromValue (getValue()), juce::dontSendNotification);
    }

    juce::Image knobImage = juce::ImageCache::getFromMemory (BinaryData::knob_preview3_png, BinaryData::knob_preview3_pngSize);

    const bool drawDots;
    const bool drawFromMiddle;
    const bool fillFromEnd;

    juce::Label nameLabel;
    juce::Label valueLabel;

    const float startAngle { 1.25f * juce::MathConstants<float>::pi };
    const float endAngle { 2.75f * juce::MathConstants<float>::pi };
    const int numberOfDots { 13 };

    juce::SliderParameterAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AALightStrokeKnob)
};
