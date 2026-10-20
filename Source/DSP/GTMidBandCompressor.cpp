#include "GTMidBandCompressor.h"

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

    for (auto& bandBuffer : bandBuffers)
        bandBuffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));

    samplesPerMs = static_cast<float>(spec.sampleRate) / 1000.f;
    attackCoefficient = std::exp(-1.f / (attackTimeConstantMs * samplesPerMs));
    releaseCoefficient = std::exp(-1.f / (releaseTimeConstantMs * samplesPerMs));
    updateSettings();
    smoothedGainReductionDb = 0.f;

    activeAmount.reset(spec.sampleRate, activeRampSeconds);
    extremeAmount.reset(spec.sampleRate, activeRampSeconds);
}

void GTMidBandCompressor::setAmount(float newAmountProportion)
{
    amountProportion = juce::jlimit(0.f, 1.f, newAmountProportion);

    const auto thresholdOffsetDb = amountProportion * maxThresholdOffsetDb;
    const auto ratioSweepProportion = juce::jmax(0.f, (amountProportion - ratioSweepStart) / (1.f - ratioSweepStart));
    compressionSlope = 1.f - 1.f / juce::jmap(ratioSweepProportion, minRatio, maxRatio);
    extremeCompressionSlope = 1.f - 1.f / juce::jmap(ratioSweepProportion, extremeMinRatio, extremeMaxRatio);

    thresholdDb = referenceLevelDb - thresholdOffsetDb;
}

void GTMidBandCompressor::setActive(bool shouldBeActive)
{
    const auto wasFullyInactive = juce::approximatelyEqual(activeAmount.getTargetValue(), 0.f) && !activeAmount.isSmoothing();
    if (shouldBeActive && wasFullyInactive)
    {
        for (auto* filter : {&LP1, &HP1, &AP2, &LP2, &HP2})
            filter->reset();
        smoothedGainReductionDb = 0.f;
    }
    activeAmount.setTargetValue(shouldBeActive ? 1.f : 0.f);
}

void GTMidBandCompressor::setExtremeMode(bool shouldBeExtreme)
{
    if (extremeMode == shouldBeExtreme)
        return;

    extremeMode = shouldBeExtreme;
    extremeAmount.setTargetValue(extremeMode ? 1.f : 0.f);
}

void GTMidBandCompressor::process(juce::AudioBuffer<float>& buffer)
{
    if (juce::approximatelyEqual(activeAmount.getTargetValue(), 0.f) && !activeAmount.isSmoothing())
        return;

    auto* const* outputChannels = buffer.getArrayOfWritePointers();

    splitBands(buffer);

    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();
    auto* const* lowBandChannels = bandBuffers[0].getArrayOfReadPointers();
    auto* const* midBandChannels = bandBuffers[1].getArrayOfWritePointers();
    auto* const* highBandChannels = bandBuffers[2].getArrayOfReadPointers();
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        auto linkedInput = 0.f;

        for (int channel = 0; channel < numChannels; ++channel)
            linkedInput = juce::jmax(linkedInput, std::abs(midBandChannels[channel][sample]));

        const auto inputLevelDb = juce::Decibels::gainToDecibels(linkedInput);
        const auto activeProportion = activeAmount.getNextValue();
        const auto extremeProportion = extremeAmount.getNextValue();

        const auto slope = compressionSlope + (extremeCompressionSlope - compressionSlope) * extremeProportion;
        const auto kneeWidthDb = softKneeWidthDb * (1.f - extremeProportion);
        const auto overshootDb = inputLevelDb - thresholdDb;

        auto targetGainReductionDb = 0.f;

        if (2.f * overshootDb >= kneeWidthDb)
            targetGainReductionDb = -slope * overshootDb;
        else if (2.f * overshootDb > -kneeWidthDb)
            targetGainReductionDb = -slope * juce::square(overshootDb + kneeWidthDb * 0.5f) / (2.f * kneeWidthDb);

        const auto coefficient = targetGainReductionDb < smoothedGainReductionDb ? attackCoefficient : releaseCoefficient;
        smoothedGainReductionDb = targetGainReductionDb + coefficient * (smoothedGainReductionDb - targetGainReductionDb);

        const auto compressorGain = juce::Decibels::decibelsToGain(smoothedGainReductionDb);

        for (int channel = 0; channel < numChannels; ++channel)
        {
            const auto splitSample = lowBandChannels[channel][sample] + midBandChannels[channel][sample] * compressorGain + highBandChannels[channel][sample];
            auto& outputSample = outputChannels[channel][sample];
            outputSample += (splitSample - outputSample) * activeProportion;
        }
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

void GTMidBandCompressor::setLowTuning(bool shouldUseLowTuning)
{
    if (lowTuning == shouldUseLowTuning)
        return;

    lowTuning = shouldUseLowTuning;
    updateSettings();
}

void GTMidBandCompressor::updateSettings()
{
    const auto lowCrossoverFreq = lowTuning ? lowTuningCrossoverLowFreq : crossoverLowFreq;
    LP1.setCutoffFrequency(lowCrossoverFreq);
    HP1.setCutoffFrequency(lowCrossoverFreq);
    const auto highCrossoverFreq = lowTuning ? lowTuningCrossoverHighFreq : crossoverHighFreq;
    AP2.setCutoffFrequency(highCrossoverFreq);
    LP2.setCutoffFrequency(highCrossoverFreq);
    HP2.setCutoffFrequency(highCrossoverFreq);
}