#pragma once
#include "../Utils/AADSPIncludes.h"

class GTMidBandCompressor
{
public:
    GTMidBandCompressor() = default;
    void prepare(const juce::dsp::ProcessSpec& spec);
    void setAmount(float newAmountProportion);
    void setActive(bool shouldBeActive);
    void setExtremeMode(bool shouldBeExtreme);
    void process(juce::AudioBuffer<float>& buffer);

private:
    void splitBands(const juce::AudioBuffer<float>& inputBuffer);

    static constexpr float crossoverLowFreq {65.f};
    static constexpr float crossoverHighFreq {280.f};
    static constexpr float attackTimeConstantMs {16.f};
    static constexpr float releaseTimeConstantMs {50.f};
    static constexpr float referenceLevelDb {-18.f};
    static constexpr float softKneeWidthDb {6.f};
    static constexpr float minRatio {2.f};
    static constexpr float maxRatio {4.f};
    static constexpr float extremeMinRatio {8.f};
    static constexpr float extremeMaxRatio {12.f};
    static constexpr float ratioSweepStart {0.35f};
    static constexpr float maxThresholdOffsetDb {15.f};
    static constexpr double activeRampSeconds {0.05};

    using CrossoverFilter = juce::dsp::LinkwitzRileyFilter<float>;
    CrossoverFilter LP1, HP1, AP2, LP2, HP2;
    std::array<juce::AudioBuffer<float>, 3> bandBuffers;

    float thresholdDb {referenceLevelDb};
    float attackCoefficient {0.f};
    float releaseCoefficient {0.f};
    float smoothedGainReductionDb {0.f};

    float amountProportion {0.f};
    juce::LinearSmoothedValue<float> activeAmount {0.f};
    bool extremeMode {false};
    float compressionSlope {0.5f};
    float extremeCompressionSlope {0.875f};
    juce::LinearSmoothedValue<float> extremeAmount {0.f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GTMidBandCompressor)
};
