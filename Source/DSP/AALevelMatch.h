#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <cmath>

class AALevelMatch
{
public:
    AALevelMatch() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        smoothedInputMeanSquare = 0.f;
        smoothedOutputMeanSquare = 0.f;
        gainDb = 0.f;
    }

    void update(const juce::AudioBuffer<float>& dryBuffer, const juce::AudioBuffer<float>& processedBuffer)
    {
        const auto blocksPerSecond = sampleRate / (double) processedBuffer.getNumSamples();
        const auto levelSmoothingCoefficient = 1.f - std::exp (-1.f / ((float) blocksPerSecond * averagingTime));
        const auto minimumMeanSquare = juce::square (juce::Decibels::decibelsToGain (minimumLevelFloorDb));

        const auto inputMeanSquare = getBufferMeanSquare (dryBuffer);

        if (inputMeanSquare > minimumMeanSquare)
        {
            const auto outputMeanSquare = getBufferMeanSquare (processedBuffer);

            smoothedInputMeanSquare += levelSmoothingCoefficient * (inputMeanSquare - smoothedInputMeanSquare);
            smoothedOutputMeanSquare += levelSmoothingCoefficient * (outputMeanSquare - smoothedOutputMeanSquare);

            if (smoothedOutputMeanSquare > minimumMeanSquare)
            {
                const auto targetGainDb = juce::jlimit (-maxGainDb, maxGainDb,
                                                        juce::Decibels::gainToDecibels (std::sqrt (smoothedInputMeanSquare / smoothedOutputMeanSquare)));

                if (std::abs (targetGainDb - gainDb) > stepDb)
                    gainDb = targetGainDb;
            }
        }
    }

    float getGainDb() const noexcept { return gainDb; }

private:
    static float getBufferMeanSquare (const juce::AudioBuffer<float>& buffer)
    {
        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();

        if (numChannels <= 0 || numSamples <= 0)
            return 0.f;

        float sumOfSquares {0.f};

        for (int channel = 0; channel < numChannels; ++channel)
        {
            const auto channelRms = buffer.getRMSLevel (channel, 0, numSamples);
            sumOfSquares += channelRms * channelRms;
        }

        return sumOfSquares / (float) numChannels;
    }

    static constexpr float minimumLevelFloorDb {-60.f};
    static constexpr float maxGainDb {24.f};
    static constexpr float stepDb {0.5f};
    static constexpr float averagingTime {1.f};

    double sampleRate {44100.0};
    float smoothedInputMeanSquare {0.f};
    float smoothedOutputMeanSquare {0.f};
    float gainDb {0.f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AALevelMatch)
};
