//
// Created by Aaron Petrini on 18/10/2026.
//

#pragma once
#include "juce_audio_processors/juce_audio_processors.h"

class AAUndoManager : public juce::UndoManager,
                      private juce::AudioProcessorParameter::Listener

{
public:
    using juce::UndoManager::UndoManager;

    void trackParameters(juce::AudioProcessor& processor)
    {
        trackedParameters = processor.getParameters();
        for (auto* parameter : trackedParameters)
            parameter->addListener(this);
    }

private:
    class ParameterChangeAction : public juce::UndoableAction
    {
    public:
        ParameterChangeAction (juce::AudioProcessorParameter& parameterToChange, float oldNormalisedValue, float newNormalisedValue)
            : parameter (parameterToChange), oldValue (oldNormalisedValue), newValue (newNormalisedValue)
        {
        }

        bool perform() override { parameter.setValueNotifyingHost (newValue); return true; }
        bool undo() override    { parameter.setValueNotifyingHost (oldValue); return true; }

    private:
        juce::AudioProcessorParameter& parameter;
        float oldValue, newValue;
    };

    void parameterValueChanged (int, float) override {}

    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
    {
        auto* parameter = trackedParameters[parameterIndex];

        if (gestureIsStarting)
            gestureStartValue = parameter->getValue();
        else if (! juce::approximatelyEqual (parameter->getValue(), gestureStartValue))
        {
            beginNewTransaction();
            perform (new ParameterChangeAction (*parameter, gestureStartValue, parameter->getValue()));
        }
    }

    juce::Array<juce::AudioProcessorParameter*> trackedParameters;
    float gestureStartValue {0.f};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AAUndoManager)
};
