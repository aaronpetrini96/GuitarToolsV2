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
#include "Utils/AAParamUtils.h"


enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48,
};


struct ChainSettings
{
    float lowCutFreq {0}, highCutFreq{0};
    float resoFreq {0}, mudFreq {0}, presenceFreq {0}, depthFreq{0};
    float presenceGain {0}, depthGain{0}, presenceResonance {0}, depthResonance {0};
    int presenceIndex{0}, depthIndex{0};
    float resoGainDb {0.f}, mudGainDb {0.f};
    Slope lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_12};
    bool resoExtremeActive{false}, mudExtremeActive{false};
};

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
    void setCurrentPresetName(const juce::String& name) {currentPresetName = name;}
    juce::String getCurrentPresetName() const {return currentPresetName;}
    void switchABState();
    bool isStateAActive() const { return stateAActive; }
    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::UndoManager undoManager { 100000, 100 };
    juce::AudioProcessorValueTreeState treeState {*this, &undoManager, "Parameters", createParameterLayout()};
    
    
    juce::String currentPresetName;
    float uiScale {0.0f};
    bool showTooltips {false};

private:
    juce::ValueTree stateA, stateB;
    bool stateAActive {true};

    //==============================================================================
//    EQ PART
    //==============================================================================
    
//    Declare Filter types and Process Chains
    using EQFilter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<EQFilter, EQFilter, EQFilter, EQFilter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, CutFilter, EQFilter, EQFilter, EQFilter, EQFilter>;
    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        LowCut,
        HighCut,
        Reso,
        Mud,
        Presence,
        Depth
    };
//    =====  UPDATE COEFFICIENTS =====
    
    using Coefficients = EQFilter::CoefficientsPtr; //alias for type needed to update coef
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);
    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);
    void updateResoFilters(const ChainSettings& chainSettings);
    void updateMudFilters(const ChainSettings& chainSettings);
    void updatePresenceFilters(const ChainSettings& chainSettings);
    void updateDepthFilters(const ChainSettings& chainSettings);
    void updateFilters();
    ChainSettings getChainSettings() const;
    ChainSettings lastChainSettings;


    juce::AudioParameterFloat* lowCutFreqParam {nullptr};
    juce::AudioParameterChoice* lowCutSlopeParam {nullptr};
    juce::AudioParameterFloat* highCutFreqParam {nullptr};
    juce::AudioParameterChoice* highCutSlopeParam {nullptr};
    juce::AudioParameterFloat* resoFreqParam {nullptr};
    juce::AudioParameterBool* resoActiveParam {nullptr};
    juce::AudioParameterBool* resoExtremeActiveParam {nullptr};
    juce::AudioParameterFloat* mudFreqParam {nullptr};
    juce::AudioParameterBool* mudActiveParam {nullptr};
    juce::AudioParameterBool* mudExtremeActiveParam {nullptr};
    juce::AudioParameterChoice* presenceModeParam {nullptr};
    juce::AudioParameterFloat* presenceGainParam {nullptr};
    juce::AudioParameterChoice* depthModeParam {nullptr};
    juce::AudioParameterFloat* depthGainParam {nullptr};

    juce::SmoothedValue<float>
    lowCutFreqParamSmoother,
    highCutFreqParamSmoother,
    resoFreqParamSmoother,
    mudFreqParamSmoother,
    presenceGainParamSmoother,
    depthGainParamSmoother,
    compAmountParamSmoother;

    juce::SmoothedValue<float> resoGainDbSmoother, mudGainDbSmoother;

    std::vector<std::pair<juce::AudioParameterFloat*, juce::SmoothedValue<float>*>> smootherBindings;



    template<int Index, typename ChainType, typename CoefficientType>
    void updateCutStage(ChainType& chain, const CoefficientType& cutCoefficients, const Slope& slope)
    {
        const auto stageIsUsed = Index <= static_cast<int>(slope);

        if (stageIsUsed)
        {
            if (chain.template isBypassed<Index>())
                chain.template get<Index>().reset();

            updateCoefficients(chain.template get<Index>().coefficients, cutCoefficients[Index]);
        }

        chain.template setBypassed<Index>(! stageIsUsed);
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chain, const CoefficientType& cutCoefficients, const Slope& slope)
    {
        updateCutStage<0>(chain, cutCoefficients, slope);
        updateCutStage<1>(chain, cutCoefficients, slope);
        updateCutStage<2>(chain, cutCoefficients, slope);
        updateCutStage<3>(chain, cutCoefficients, slope);
    }

    //==============================================================================
//    COMPRESSOR PART
    //==============================================================================
    
    GTMidBandCompressor midBandCompressor;
    juce::AudioParameterFloat* compAmountParam {nullptr};
    juce::AudioParameterBool* compActiveParam {nullptr};
    juce::AudioParameterBool* compExtremeActiveParam {nullptr};

    //==============================================================================
//    LEVELS PART
    //==============================================================================

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
    template<typename ParamType, typename Params, typename Funcs> // <-- new
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
