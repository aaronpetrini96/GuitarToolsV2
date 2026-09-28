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

    static constexpr float crossoverLowFreq {70.f};
    static constexpr float crossoverHighFreq {220.f};
    static constexpr float attackMs {20.f};
    static constexpr float releaseMs {30.f};
    static constexpr float extremeAttackMs {15.f};
    static constexpr float extremeReleaseMs {10.f};
    static constexpr float minRatio {2.f};
    static constexpr float maxRatio {4.f};
    static constexpr float extremeMinRatio {8.f};
    static constexpr float extremeMaxRatio {12.f};
    static constexpr float ratioSweepStart {0.35f};
    static constexpr float maxThresholdOffsetDb {15.f};
    static constexpr double levelTrackingTime {300.0};
    static constexpr float levelFloorDb {-60.f};
    static constexpr double activeRampSeconds {0.05};

    using CrossoverFilter = juce::dsp::LinkwitzRileyFilter<float>;
    CrossoverFilter LP1, HP1, AP2, LP2, HP2;
    std::array<juce::AudioBuffer<float>, 3> bandBuffers;

    juce::dsp::BallisticsFilter<float> envelopeFilter;
    juce::dsp::BallisticsFilter<float> bandLevelFilter;

    float amountProportion {0.f};
    float thresholdOffsetGain {1.f};
    float makeupGain {1.f};
    float levelFloorGain {juce::Decibels::decibelsToGain(levelFloorDb)};
    juce::LinearSmoothedValue<float> activeAmount {0.f};
    bool extremeMode {false};
    float extremeMakeupGain {1.f};
    float compressionSlope {0.5f};
    float extremeCompressionSlope {0.875f};
    juce::LinearSmoothedValue<float> extremeAmount {0.f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GTMidBandCompressor)
};
