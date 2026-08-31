/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include <array>
#include <atomic>

#include "DSP/CompressorBand.h"
#include "DSP/SingleChannelSampleFifo.h"


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
    float presenceGain {0}, depthGain{0};
    int presenceIndex{0}, depthIndex{0};
    
    bool resoBypass {false}, mudBypass {false}, pluginBypass {false}, compBypass {false};

    Slope lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_12};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& treeState);

//==============================================================================
/**
*/
class GuitarToolsAudioProcessor  : public juce::AudioProcessor,
                                    private juce::AudioProcessorValueTreeState::Listener
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
    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState treeState {*this, nullptr, "Parameters", createParameterLayout()};
    
    
    SingleChannelSampleFifo <juce::AudioBuffer<float>> leftChannelFifo {Channel::Left};
    SingleChannelSampleFifo <juce::AudioBuffer<float>> rightChannelFifo {Channel::Right};
    
    std::array<CompressorBand,3> compressors;
    CompressorBand& lowBandComp = compressors[0];
    CompressorBand& midBandComp = compressors[1];
    CompressorBand& highBandComp = compressors[2];

    std::atomic<bool> clipFlagIn { false };
    std::atomic<bool> clipFlagOut { false };
    
    juce::String currentPresetName;
    
private:
    
    juce::LinearSmoothedValue<float> smoothedBypassValue {1.f};
    //==============================================================================
//    EQ PART
    //==============================================================================
    
//    Declare Filter types and Process Chains
    using EQFilter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<EQFilter, EQFilter, EQFilter, EQFilter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, EQFilter, EQFilter, EQFilter, EQFilter, CutFilter>;
    MonoChain leftChain, rightChain;
    
    enum ChainPositions
    {
        LowCut,
        Reso,
        Mud,
        Presence,
        Depth,
        HighCut
    };
    
//    =====  UPDATE COEFFICIENTS =====
    
    using Coefficients = EQFilter::CoefficientsPtr; //alias for type needed to update coef
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);
    
//    =====  UPDATE FILTERS =====
    void updateBellFitlers(const ChainSettings& chainSettings);
    void updateShelfFilters(const ChainSettings& chainSettings);
    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);
    void updateFilters();
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    std::atomic<bool> filtersNeedUpdate { true };

    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& cutCoefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, cutCoefficients[Index]);
        chain.template setBypassed<Index>(false);
    }
    
    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chain, const CoefficientType& cutCoefficients, const Slope& slope)
    {

        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);
        
        
        switch (slope)
        {
            case Slope_48:
            {
                update<3>(chain, cutCoefficients);
            }
                
            case Slope_36:
            {
                update<2>(chain, cutCoefficients);
            }

            case Slope_24:
            {
                update<1>(chain, cutCoefficients);
            }

            case Slope_12:
            {
                update<0>(chain, cutCoefficients);
            }
            
        }
        
    }

    //==============================================================================
//    COMPRESSOR PART
    //==============================================================================
    
    using CompFilter = juce::dsp::LinkwitzRileyFilter<float>;
    CompFilter LP1, HP1, AP2, LP2, HP2;

//    juce::AudioParameterFloat* compRangeStartParameter {nullptr};
//    juce::AudioParameterFloat* compRangeEndParameter {nullptr};
    
    std::array<juce::AudioBuffer<float>, 3> filterBuffers;
    
    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam {nullptr};
    juce::AudioParameterFloat* outputGainParam {nullptr};

    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }
    
    void updateCompState();
    void splitBands(const juce::AudioBuffer<float>& inputBuffer);
    
    void linkCompParameters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessor)
};
