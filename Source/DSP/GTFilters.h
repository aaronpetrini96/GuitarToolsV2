//
// Created by Aaron Petrini on 26/09/2026.
//

#pragma once
#include "Utils/AADSPIncludes.h"

enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48,
};

inline float getButterworthStageQ(Slope slope, size_t stage)
{
    const auto order = (static_cast<int>(slope) + 1) * 2;
    return static_cast<float>(1.0 / (2.0 * std::cos((2.0 * static_cast<double>(stage) + 1.0) * juce::MathConstants<double>::pi / (order * 2.0))));
}

struct ChainSettings
{
    float lowCutFreq {0}, highCutFreq{0};
    float resoFreq {0}, mudFreq {0}, presenceFreq {0}, depthFreq{0};
    float presenceGain {0}, depthGain{0}, presenceResonance {0}, depthResonance {0};
    float resoGainDb {0.f}, mudGainDb {0.f};
    Slope lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_12};
    float resoQ {0.f}, mudQ {0.f};
};

class GTFilters
{
public:
    GTFilters() = default;

    void prepare(const juce::dsp::ProcessSpec& spec, const ChainSettings& chainSettings);
    void update(const ChainSettings& chainSettings, bool forceUpdate = false);
    void process(juce::AudioBuffer<float>& buffer);

private:
    using EQFilter = juce::dsp::IIR::Filter<float>;
    using ArrayCoefficients = juce::dsp::IIR::ArrayCoefficients<float>;
    using CutFilter = juce::dsp::ProcessorChain<EQFilter, EQFilter, EQFilter, EQFilter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, CutFilter, EQFilter, EQFilter, EQFilter, EQFilter>;

    enum ChainPositions
    {
        LowCut,
        HighCut,
        Reso,
        Mud,
        Presence,
        Depth
    };

    template<int Position>
    void setCoefficients(const std::array<float, 6>& coefficients);

    template<int Position>
    void updateCutFilter(float frequency, Slope slope, bool isHighPass);

    MonoChain leftChain, rightChain;
    ChainSettings lastChainSettings;
    double sampleRate {44100.0};
    int numChannels {2};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GTFilters)
};