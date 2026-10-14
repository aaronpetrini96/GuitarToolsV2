//
// Created by Aaron Petrini on 30/08/2026.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include "../../AALook.h"

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
        setMouseDragSensitivity(300);
        setTextValueSuffix (suffix);
        setComponentID(parameter.getParameterID());
        attachment = std::make_unique<juce::SliderParameterAttachment> (parameter, *this, nullptr);
    }

    void setUndoManager (juce::UndoManager* undoManager)
    {
        parameterUndoManager = undoManager;
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
        const auto thumbHeight = boundsF.getHeight() * 0.225f;
        const auto thumbWidth = thumbHeight / ((float) thumbOFF.getHeight() / (float) thumbOFF.getWidth());
        thumbBounds = boundsF.withSizeKeepingCentre (thumbWidth, thumbHeight);
        thumbTravel = { boundsF.getY() + thumbHeight * 0.5f, boundsF.getBottom() - thumbHeight * 0.5f };
    }


private:

    void mouseDown (const juce::MouseEvent& event) override
    {
        const auto thumbValueCY = thumbTravel.getEnd() - static_cast<float>(valueToProportionOfLength(getValue())) * thumbTravel.getLength();
        const auto positionedThumbBounds = thumbBounds.withCentre ({ thumbBounds.getCentreX(), thumbValueCY });

        if (event.mods.isLeftButtonDown() && ! positionedThumbBounds.contains (event.position))
        {
            const auto proportion = juce::jlimit (0.0f, 1.0f, (thumbTravel.getEnd() - event.position.y) / thumbTravel.getLength());
            if (parameterUndoManager != nullptr)
                parameterUndoManager->beginNewTransaction();
            parameter.beginChangeGesture();
            parameter.setValueNotifyingHost(parameter.convertTo0to1(static_cast<float>(proportionOfLengthToValue(proportion))));
            parameter.endChangeGesture();
        }

        juce::Slider::mouseDown (event);
    }

    juce::Rectangle<float> thumbBounds;
    juce::Range<float> thumbTravel;
    bool thumbActive {true};

    juce::Image thumbOFF = juce::ImageCache::getFromMemory (BinaryData::thumbOffShadow_png, BinaryData::thumbOffShadow_pngSize);
    juce::Image thumbON = juce::ImageCache::getFromMemory (BinaryData::thumbBlue2_png, BinaryData::thumbBlue2_pngSize);

    juce::RangedAudioParameter& parameter;
    juce::UndoManager* parameterUndoManager{nullptr};
    std::unique_ptr<juce::SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAVerticalSSLFader)
};
