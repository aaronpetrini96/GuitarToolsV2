#include "GTMidBandCompressor.h"

#include <cmath>

void GTMidBandCompressor::prepare(const juce::dsp::ProcessSpec& spec)
{
    using FilterType = juce::dsp::LinkwitzRileyFilterType;

    for (auto* filter : {&LP1, &HP1, &AP2, &LP2, &HP2})
        filter->prepare(spec);

    LP1.setType(FilterType::lowpass);
    HP1.setType(FilterType::highpass);
    AP2.setType(FilterType::allpass);
    LP2.setType(FilterType::lowpass);
    HP2.setType(FilterType::highpass);

    LP1.setCutoffFrequency(crossoverLowFreq);
    HP1.setCutoffFrequency(crossoverLowFreq);
    AP2.setCutoffFrequency(crossoverHighFreq);
    LP2.setCutoffFrequency(crossoverHighFreq);
    HP2.setCutoffFrequency(crossoverHighFreq);

    for (auto& bandBuffer : bandBuffers)
        bandBuffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));

    auto detectorSpec = spec;
    detectorSpec.numChannels = 1;
    envelopeFilter.prepare(detectorSpec);
    envelopeFilter.setAttackTime(extremeMode ? extremeAttackMs : attackMs);
    envelopeFilter.setReleaseTime(extremeMode ? extremeReleaseMs : releaseMs);

    bandLevelFilter.prepare(detectorSpec);
    bandLevelFilter.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::RMS);
    bandLevelFilter.setAttackTime(levelTrackingTime);
    bandLevelFilter.setReleaseTime(levelTrackingTime);

    activeAmount.reset(spec.sampleRate, activeRampSeconds);
}

void GTMidBandCompressor::setAmount(float newAmountProportion)
{
    amountProportion = juce::jlimit(0.f, 1.f, newAmountProportion);

    const auto thresholdOffsetDb = amountProportion * maxThresholdOffsetDb;
    const auto currentRatio = extremeMode ? extremeRatio : ratio;

    thresholdOffsetGain = juce::Decibels::decibelsToGain(-thresholdOffsetDb);
    makeupGain = juce::Decibels::decibelsToGain(thresholdOffsetDb * (1.f - 1.f / currentRatio));
}

void GTMidBandCompressor::setActive(bool shouldBeActive)
{
    activeAmount.setTargetValue(shouldBeActive ? 1.f : 0.f);
}

void GTMidBandCompressor::setExtremeMode(bool shouldBeExtreme)
{
    if (extremeMode == shouldBeExtreme)
        return;

    extremeMode = shouldBeExtreme;
    envelopeFilter.setAttackTime(extremeMode ? extremeAttackMs : attackMs);
    envelopeFilter.setReleaseTime(extremeMode ? extremeReleaseMs : releaseMs);
    setAmount(amountProportion);
}

void GTMidBandCompressor::process(juce::AudioBuffer<float>& buffer)
{
    splitBands(buffer);

    static_assert (ratio == 4.f, "The gain law below assumes a 4:1 ratio");
    static_assert (extremeRatio == 8.f, "The gain law below assumes an 8:1 extreme ratio");

    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();
    auto* const* midBandChannels = bandBuffers[1].getArrayOfWritePointers();


    for (int sample = 0; sample < numSamples; ++sample)
    {
        auto linkedInput = 0.f;

        for (int channel = 0; channel < numChannels; ++channel)
            linkedInput = juce::jmax(linkedInput, std::abs(midBandChannels[channel][sample]));

        const auto envelope = envelopeFilter.processSample(0, linkedInput);
        const auto bandLevel = juce::jmax(bandLevelFilter.processSample(0, linkedInput), levelFloorGain);

        const auto activeProportion = activeAmount.getNextValue();

        if (activeProportion == 0.f)
            continue;

        const auto threshold = bandLevel * thresholdOffsetGain;
        auto compressorGain = makeupGain;

        if (envelope >= threshold)
        {
            const auto overshoot = envelope / threshold;
            const auto overshootRoot = std::sqrt(overshoot);
            const auto overshootQuarterRoot = std::sqrt(overshootRoot);
            auto overshootPower = overshootRoot * overshootQuarterRoot;              // overshoot^0.75, a 4:1 ratio

            if (extremeMode)
                overshootPower *= std::sqrt(overshootQuarterRoot);                   // overshoot^0.875, an 8:1 ratio

            compressorGain /= overshootPower;
        }

        const auto gain = 1.f + (compressorGain - 1.f) * activeProportion;

        for (int channel = 0; channel < numChannels; ++channel)
            midBandChannels[channel][sample] *= gain;
    }

    for (int channel = 0; channel < numChannels; ++channel)
    {
        buffer.copyFrom(channel, 0, bandBuffers[0], channel, 0, numSamples);
        buffer.addFrom(channel, 0, bandBuffers[1], channel, 0, numSamples);
        buffer.addFrom(channel, 0, bandBuffers[2], channel, 0, numSamples);
    }
}

void GTMidBandCompressor::splitBands(const juce::AudioBuffer<float>& inputBuffer)
{
    bandBuffers[0].makeCopyOf(inputBuffer, true);
    bandBuffers[1].makeCopyOf(inputBuffer, true);

    auto lowBandBlock = juce::dsp::AudioBlock<float>(bandBuffers[0]);
    auto midBandBlock = juce::dsp::AudioBlock<float>(bandBuffers[1]);

    auto lowBandContext = juce::dsp::ProcessContextReplacing<float>(lowBandBlock);
    auto midBandContext = juce::dsp::ProcessContextReplacing<float>(midBandBlock);

    LP1.process(lowBandContext);
    AP2.process(lowBandContext);
    HP1.process(midBandContext);
    bandBuffers[2].makeCopyOf(bandBuffers[1], true);
    LP2.process(midBandContext);

    auto highBandBlock = juce::dsp::AudioBlock<float>(bandBuffers[2]);
    auto highBandContext = juce::dsp::ProcessContextReplacing<float>(highBandBlock);
    HP2.process(highBandContext);
}
