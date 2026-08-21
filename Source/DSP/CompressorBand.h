#pragma once
#include <JuceHeader.h>
//#include "../Utilities.h"
#define MIN_FREQ 20.f
#define MAX_FREQ 20000.f
#define MAX_DECIBELS 12.f
#define MIN_THRESHOLD -60.f

struct CompressorBand
{
    juce::AudioParameterFloat* attack {nullptr};
    juce::AudioParameterFloat* release {nullptr};
    juce::AudioParameterFloat* threshold {nullptr};
    juce::AudioParameterChoice* ratio {nullptr};
    juce::AudioParameterBool* bypassed {nullptr};
//    juce::AudioParameterBool* mute {nullptr};
//    juce::AudioParameterBool* solo {nullptr};

    void prepare(const juce::dsp::ProcessSpec& spec);

    void updateCompressorSettings();

    void process(juce::AudioBuffer<float>& buffer);

private:
    juce::dsp::Compressor<float> compressor;
};
