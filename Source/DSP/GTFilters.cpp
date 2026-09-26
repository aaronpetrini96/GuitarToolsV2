//
// Created by Aaron Petrini on 26/09/2026.
//
#include "GTFilters.h"

void GTFilters::prepare(const juce::dsp::ProcessSpec& spec, const ChainSettings& chainSettings)
{
    sampleRate = spec.sampleRate;
    numChannels = static_cast<int>(spec.numChannels);
    update(chainSettings, true);

    auto monoSpec = spec;
    monoSpec.numChannels = 1;
    leftChain.prepare(monoSpec);
    rightChain.prepare(monoSpec);
}

template<int Position>
void GTFilters::setCoefficients(const std::array<float, 6>& coefficients)
{
    for (auto* chain : {&leftChain, &rightChain})
        *chain->get<Position>().coefficients = coefficients;
}

template<int Position>
void GTFilters::updateCutFilter(float frequency, Slope slope, bool isHighPass)
{
    std::array<std::array<float, 6>, 4> stageCoefficients;
    stageCoefficients.fill({1.f, 0.f, 0.f, 1.f, 0.f, 0.f});

    for (size_t stage = 0; stage <= static_cast<size_t>(slope); ++stage)
    {
        const auto stageQ = getButterworthStageQ(slope, stage);
        stageCoefficients[stage] = isHighPass ? ArrayCoefficients::makeHighPass(sampleRate, frequency, stageQ)
                                              : ArrayCoefficients::makeLowPass(sampleRate, frequency, stageQ);
    }

    for (auto* chain : {&leftChain, &rightChain})
    {
        auto& cutFilter = chain->get<Position>();
        *cutFilter.template get<0>().coefficients = stageCoefficients[0];
        *cutFilter.template get<1>().coefficients = stageCoefficients[1];
        *cutFilter.template get<2>().coefficients = stageCoefficients[2];
        *cutFilter.template get<3>().coefficients = stageCoefficients[3];
    }
}

void GTFilters::update(const ChainSettings& chainSettings, bool forceUpdate)
{
    if (forceUpdate
     || ! juce::approximatelyEqual(chainSettings.lowCutFreq, lastChainSettings.lowCutFreq)
     || chainSettings.lowCutSlope != lastChainSettings.lowCutSlope)
        updateCutFilter<LowCut>(chainSettings.lowCutFreq, chainSettings.lowCutSlope, true);

    if (forceUpdate
     || ! juce::approximatelyEqual(chainSettings.highCutFreq, lastChainSettings.highCutFreq)
     || chainSettings.highCutSlope != lastChainSettings.highCutSlope)
        updateCutFilter<HighCut>(chainSettings.highCutFreq, chainSettings.highCutSlope, false);

    if (forceUpdate
     || ! juce::approximatelyEqual(chainSettings.resoFreq, lastChainSettings.resoFreq)
     || ! juce::approximatelyEqual(chainSettings.resoGainDb, lastChainSettings.resoGainDb)
     || ! juce::approximatelyEqual(chainSettings.resoQ, lastChainSettings.resoQ))
        setCoefficients<Reso>(ArrayCoefficients::makePeakFilter(sampleRate, chainSettings.resoFreq, chainSettings.resoQ, juce::Decibels::decibelsToGain(chainSettings.resoGainDb)));

    if (forceUpdate
     || ! juce::approximatelyEqual(chainSettings.mudFreq, lastChainSettings.mudFreq)
     || ! juce::approximatelyEqual(chainSettings.mudGainDb, lastChainSettings.mudGainDb)
     || ! juce::approximatelyEqual(chainSettings.mudQ, lastChainSettings.mudQ))
        setCoefficients<Mud>(ArrayCoefficients::makePeakFilter(sampleRate, chainSettings.mudFreq, chainSettings.mudQ, juce::Decibels::decibelsToGain(chainSettings.mudGainDb)));

    if (forceUpdate
     || ! juce::approximatelyEqual(chainSettings.presenceFreq, lastChainSettings.presenceFreq)
     || ! juce::approximatelyEqual(chainSettings.presenceResonance, lastChainSettings.presenceResonance)
     || ! juce::approximatelyEqual(chainSettings.presenceGain, lastChainSettings.presenceGain))
        setCoefficients<Presence>(ArrayCoefficients::makeHighShelf(sampleRate, chainSettings.presenceFreq, chainSettings.presenceResonance, juce::Decibels::decibelsToGain(chainSettings.presenceGain)));

    if (forceUpdate
     || ! juce::approximatelyEqual(chainSettings.depthFreq, lastChainSettings.depthFreq)
     || ! juce::approximatelyEqual(chainSettings.depthResonance, lastChainSettings.depthResonance)
     || ! juce::approximatelyEqual(chainSettings.depthGain, lastChainSettings.depthGain))
        setCoefficients<Depth>(ArrayCoefficients::makeLowShelf(sampleRate, chainSettings.depthFreq, chainSettings.depthResonance, juce::Decibels::decibelsToGain(chainSettings.depthGain)));

    lastChainSettings = chainSettings;
}

void GTFilters::process(juce::AudioBuffer<float>& buffer)
{
    const juce::dsp::AudioBlock<float> block {buffer};

    auto leftBlock = block.getSingleChannelBlock(0);
    juce::dsp::ProcessContextReplacing<float> leftContext {leftBlock};
    leftChain.process(leftContext);

    if (numChannels > 1)
    {
        auto rightBlock = block.getSingleChannelBlock(1);
        juce::dsp::ProcessContextReplacing<float> rightContext {rightBlock};
        rightChain.process(rightContext);
    }
}
