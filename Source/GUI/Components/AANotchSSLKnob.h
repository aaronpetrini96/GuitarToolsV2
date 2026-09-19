//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>
#include <memory>

#include "../../Utils/AALook.h"

class AANotchSSLKnob : public juce::Slider
{
public:
    explicit AANotchSSLKnob (juce::RangedAudioParameter& parameterToUse, juce::Label* nameLabelToUse = nullptr)
        : parameter (parameterToUse), nameText (nameLabelToUse != nullptr ? nameLabelToUse->getText() : juce::String()), nameLabel (nameLabelToUse)
    {
        jassert (knobImage.isValid());
        setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setNumDecimalPlacesToDisplay (1);
        setTextValueSuffix (" " + parameter.getLabel());
        setMouseDragSensitivity (300);
        setRotaryParameters (startAngle, endAngle, true);
        setComponentID (parameter.getParameterID());
        createAttachment (nullptr);
    }

    void setUndoManager (juce::UndoManager& undoManager)
    {
        createAttachment (&undoManager);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (isEnabled() && ! isDragging && e.mouseWasDraggedSinceMouseDown())
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
        const auto boundsCentre = boundsF.getCentre();

        const auto sizeRef = juce::jmin (boundsF.getWidth(), boundsF.getHeight());
        const auto dotSizeRadius = sizeRef * dotSizeAdjuster;
        const auto dotRingRadius = sizeRef * 0.5f - dotSizeRadius;
        const auto knobBounds = boundsF.reduced (knobBoundsReducer);


        const auto sliderPos = static_cast<float> (valueToProportionOfLength (getValue()));
        const auto angle = startAngle + sliderPos * (endAngle - startAngle);
        g.beginTransparencyLayer (isEnabled() ? 0.9f : 0.45f);
        paintLightDots (g, boundsCentre, dotRingRadius, dotSizeRadius, angle);

        {
            juce::Graphics::ScopedSaveState state (g);
            g.addTransform (juce::AffineTransform::rotation (angle, boundsCentre.x, boundsCentre.y));

            g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
            g.drawImage (knobImage, knobBounds, juce::RectanglePlacement::centred);
        }

        // if (isDragging && nameLabel == nullptr)
        // {
        //     const auto valueBounds = knobBounds.withSizeKeepingCentre (knobBounds.getWidth() * 0.65f, knobBounds.getWidth() * 0.25f).withY(knobBounds.getCentreX());
        //     g.setColour (GTColors::dividerLinesEnd);
        //     g.fillRect (valueBounds);
        //     g.setColour (GTColors::almostWhite.withAlpha(0.9f));
        //     g.setFont (AAFonts::interMedium (valueBounds.getHeight() * 0.5f));
        //     g.drawFittedText (getTextFromValue (getValue()), valueBounds.toNearestInt(), juce::Justification::centredBottom, 1);
        // }

        g.endTransparencyLayer();
    }

    int numberOfDots {11};


private:

    void createAttachment (juce::UndoManager* undoManager)
    {
        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, undoManager);
    }

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

    void updateValueDisplay()
    {
        if (nameLabel != nullptr)
            nameLabel->setText (isDragging ? getTextFromValue(getValue()) : nameText, juce::dontSendNotification);
        else
            repaint();
    }

    juce::Image knobImage = juce::ImageCache::getFromMemory (BinaryData::sslKnotchKnobShadow2_png, BinaryData::sslKnotchKnobShadow2_pngSize);
    const float startAngle { 1.25f * juce::MathConstants<float>::pi };
    const float endAngle { 2.75f * juce::MathConstants<float>::pi };
    const float dotSizeAdjuster {0.03f};
    const float knobBoundsReducer {2.f};

    bool isDragging {false};
    juce::RangedAudioParameter& parameter;
    const juce::String nameText;
    juce::Label* const nameLabel {nullptr};
    std::unique_ptr<juce::SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AANotchSSLKnob)
};
