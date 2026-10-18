/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include <utility>
#include <vector>

#include "DSP/AALevelMatch.h"
#include "DSP/GTMidBandCompressor.h"
#include "DSP/GTFilters.h"
#include "Utils/AAParamUtils.h"

//==============================================================================
/**
*/
class GuitarToolsAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GuitarToolsAudioProcessor();
    ~GuitarToolsAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorParameter* getBypassParameter() const override { return pluginBypassActiveParam; }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);
    void loadDefaultPreset();
    void setCurrentPresetName(const juce::String& name) { const std::scoped_lock lock (currentPresetNameMutex); currentPresetName = name; }
    juce::String getCurrentPresetName() const { const std::scoped_lock lock (currentPresetNameMutex); return currentPresetName; }
    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::UndoManager undoManager { 100000, 100 };
    juce::AudioProcessorValueTreeState treeState {*this, &undoManager, "Parameters", createParameterLayout()};

    juce::String currentPresetName;
    mutable std::mutex currentPresetNameMutex;
    juce::ChangeBroadcaster stateRestoredBroadcaster;
    float uiScale {0.0f};
    bool showTooltips {false};

    std::atomic<float> inputLevelLeft {0.f}, inputLevelRight {0.f};
    std::atomic<float> outputLevelLeft {0.f}, outputLevelRight {0.f};

private:

    //========== EQ PART ==========//
    GTFilters filters;
    ChainSettings getChainSettings() const;

// PARAMETER POINTERS
    juce::AudioParameterFloat* lowCutFreqParam {nullptr};
    juce::AudioParameterChoice* lowCutSlopeParam {nullptr};
    juce::AudioParameterFloat* highCutFreqParam {nullptr};
    juce::AudioParameterChoice* highCutSlopeParam {nullptr};
    juce::AudioParameterFloat* resoFreqParam {nullptr};
    juce::AudioParameterBool* resoActiveParam {nullptr};
    juce::AudioParameterBool* resoExtremeActiveParam {nullptr};
    juce::AudioParameterFloat* mudAmountParam {nullptr};
    juce::AudioParameterBool* mudActiveParam {nullptr};
    juce::AudioParameterBool* mudExtremeActiveParam {nullptr};
    juce::AudioParameterChoice* presenceModeParam {nullptr};
    juce::AudioParameterFloat* presenceGainParam {nullptr};
    juce::AudioParameterChoice* depthModeParam {nullptr};
    juce::AudioParameterFloat* depthGainParam {nullptr};

    //SMOOTHERS
    juce::SmoothedValue<float>
    lowCutFreqParamSmoother,
    highCutFreqParamSmoother,
    resoFreqParamSmoother,
    presenceGainParamSmoother,
    depthGainParamSmoother,
    compAmountParamSmoother;

    juce::SmoothedValue<float>
    resoGainDbSmoother,
    mudGainDbSmoother,
    resoQSmoother,
    mudQSmoother,
    mudFreqSmoother,
    presenceFreqSmoother,
    presenceResonanceSmoother,
    depthFreqSmoother,
    depthResonanceSmoother;

    std::vector<std::pair<juce::AudioParameterFloat*, juce::SmoothedValue<float>*>> smootherBindings;
    //========== COMPRESSOR PART ==========//
    GTMidBandCompressor midBandCompressor;
    juce::AudioParameterFloat* compAmountParam {nullptr};
    juce::AudioParameterBool* compActiveParam {nullptr};
    juce::AudioParameterBool* compExtremeActiveParam {nullptr};
    juce::AudioParameterBool* lowTuningActiveParam {nullptr};

    //========== LEVELS PART ==========//

    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam {nullptr};
    juce::AudioParameterFloat* outputGainParam {nullptr};
    juce::AudioParameterBool* loudnessMatchActiveParam {nullptr};
    juce::AudioParameterBool* pluginBypassActiveParam {nullptr};
    juce::LinearSmoothedValue<float> smoothedBypassValue {1.f};
    AALevelMatch levelMatch;
    juce::AudioBuffer<float> dryBuffer {};

    void updateGainState();


    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }


    /// PARAM FUNCTIONS
    void linkParameters();
    template<typename ParamType, typename Params, typename Funcs>
    void initCachedParams(Params paramsArray, Funcs funcsArray)
    {
        for (size_t i = 0; i < paramsArray.size(); ++i)
        {
            auto ptrToParamPtr = paramsArray[i];
            *ptrToParamPtr = dynamic_cast<ParamType>(treeState.getParameter(funcsArray[i]()));
            jassert(*ptrToParamPtr != nullptr);
        }
    }

    enum class SmootherUpdateMode
    {
        initialize,
        liveInRealTime
    };

    void updateSmootherFromParams(int numSamplesToSkip, SmootherUpdateMode mode);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessor)
};
