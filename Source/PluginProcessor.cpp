/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParamIDs.h"

#include <cmath>
#include <memory>
#include <vector>

//==============================================================================
static const juce::StringArray eqParameterIDs
{
    ParamIDs::getLowCutFreqName(), ParamIDs::getLowCutSlopeName(), ParamIDs::getHighCutFreqName(), ParamIDs::getHighCutSlopeName(),
    ParamIDs::getResoFreqName(), ParamIDs::getResoBypassName(), ParamIDs::getMudFreqName(), ParamIDs::getMudBypassName(),
    ParamIDs::getPresenceFreqName(), ParamIDs::getPresenceGainName(), ParamIDs::getDepthFreqName(), ParamIDs::getDepthGainName()
};

//==============================================================================
GuitarToolsAudioProcessor::GuitarToolsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    linkCompParameters();

    for (auto& paramID : eqParameterIDs)
        treeState.addParameterListener(paramID, this);
}

GuitarToolsAudioProcessor::~GuitarToolsAudioProcessor()
{
    for (auto& paramID : eqParameterIDs)
        treeState.removeParameterListener(paramID, this);
}

//==============================================================================



//==============================================================================
const juce::String GuitarToolsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarToolsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarToolsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarToolsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarToolsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarToolsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarToolsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarToolsAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuitarToolsAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuitarToolsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuitarToolsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumInputChannels();
    
    smoothedBypassValue.reset(sampleRate, 0.01);
    
//==============================================================================
//    Compressor Section
    compressors[1].prepare(spec);
    
    LP1.prepare(spec);
    HP1.prepare(spec);
    AP2.prepare(spec);
    LP2.prepare(spec);
    HP2.prepare(spec);
    
    inputGain.prepare(spec);
    inputGain.setRampDurationSeconds(0.05);
    outputGain.prepare(spec);
    outputGain.setRampDurationSeconds(0.05);
    
    for (auto& buffer : filterBuffers)
        buffer.setSize(spec.numChannels, samplesPerBlock);
    
    leftChannelFifo.prepare(samplesPerBlock);
    rightChannelFifo.prepare(samplesPerBlock);

//==============================================================================

    spec.numChannels = 1;
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    updateFilters();
    filtersNeedUpdate.store(false);
//==============================================================================
}

void GuitarToolsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarToolsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


bool clipDetection(juce::AudioBuffer<float>& buffer)
{
// CLIP DETECTION
    bool isClipping = false;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getReadPointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            auto sampleAbs = std::abs(samples[i]);
            if (sampleAbs > 1.0f)
            {
                isClipping = true;
                break; // you can also break out of both loops early if you want
            }
        }
    }
    
    return isClipping;
}

void GuitarToolsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

        
// Save a dry copy before processing
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

//========================    COMP part    ========================
    updateCompState();
    
    leftChannelFifo.update(buffer);
    if (buffer.getNumChannels() > 1)
        rightChannelFifo.update(buffer);
    
    applyGain(buffer, inputGain);
    
//    INPUT CLIP DETECTION
    if (clipDetection(buffer))
        clipFlagIn.store(true); // atomic flag to be safe for GUI thread
    
    splitBands(buffer);
    compressors[1].process(filterBuffers[1]); //only process midband
    
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    buffer.clear();
    
    auto addFilterBand = [nc=numChannels, ns=numSamples](auto& inputBuffer, const auto& source)
    {
        for(auto i{0};i<nc;++i)
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
    };
    
    for(size_t i{0}; i < compressors.size(); ++i)
        addFilterBand(buffer, filterBuffers[i]);
    
//    applyGain(buffer, outputGain);
    
//========================    EQ part    ========================
    auto chainSettings = getChainSettings(treeState);

    if (filtersNeedUpdate.exchange(false))
        updateFilters();

    juce::dsp::AudioBlock<float> block {buffer};
       
    // Update smoothed bypass target
    bool bypassActive = (chainSettings.pluginBypass != 0);
    smoothedBypassValue.setTargetValue(bypassActive ? 0.0f : 1.0f);
    
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(0);
    if (totalNumInputChannels > 1)
        rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext {leftBlock};
    juce::dsp::ProcessContextReplacing<float> rightContext {rightBlock};

    leftChain.process(leftContext);
    rightChain.process(rightContext);
    
    applyGain(buffer, outputGain);
    
    // Smooth crossfade
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* wet = buffer.getWritePointer(channel);
        auto* dry = dryBuffer.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float mix = smoothedBypassValue.getNextValue();
            wet[sample] = wet[sample] * mix + dry[sample] * (1.0f - mix);
        }
    }
    
//    OUTPUT CLIP DETECTION
    if (clipDetection(buffer))
        clipFlagOut.store(true); // atomic flag to be safe for GUI thread
}

//==============================================================================
bool GuitarToolsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarToolsAudioProcessor::createEditor()
{
    return new GuitarToolsAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void GuitarToolsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    state.setProperty("CurrentPresetName", currentPresetName, nullptr);
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GuitarToolsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(treeState.state.getType()))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        if (auto* presetNameProp = state.getPropertyPointer("CurrentPresetName"))
            currentPresetName = presetNameProp->toString(); // restore preset name
        treeState.replaceState(state);
        updateFilters();
    }
}

void GuitarToolsAudioProcessor::savePreset(const juce::File& file)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    if (xml) { xml->writeTo(file);}
}

void GuitarToolsAudioProcessor::loadPreset(const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse(file));
    if (xml && xml->hasTagName(treeState.state.getType()))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        if (auto* presetNameProp = state.getPropertyPointer("CurrentPresetName"))
            currentPresetName = presetNameProp->toString(); // restore preset name
        treeState.replaceState(state);
        updateFilters();
    }
}


//==============================================================================
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout GuitarToolsAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    constexpr auto versionHint {1};
    constexpr auto frequencyIntervalValue {0.01f};
    constexpr auto decibelIntervalValue {0.001f};

//    ========== HICUT LOW CUT ===========
    auto name = ParamIDs::getLowCutFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float> (20.f, 4000.f, frequencyIntervalValue, 1.f),20.f));

    name = ParamIDs::getHighCutFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name,juce::NormalisableRange<float>(4000.f, 20000.f, frequencyIntervalValue, 1.f),20000.f));
    
    
    juce::StringArray stringArray;
    for (int i {0}; i < 4; ++i)
    {
        juce::String str;
        str << (12 + 12*i);
        str << " dB/Oct";
        stringArray.add(str);
    }
    
    name = ParamIDs::getLowCutSlopeName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, stringArray, 0));

    name = ParamIDs::getHighCutSlopeName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, stringArray, 0));

//    ========== HIGH/LOW SHELVE  ===========
    juce::StringArray presenceFreqArray {"4.5 kHz", "6 kHz", "8 kHz"};
    name = ParamIDs::getPresenceFreqName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, presenceFreqArray, 1));

    name = ParamIDs::getPresenceGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-12.f, 12.f, decibelIntervalValue, 1.f),0.f));
    

    juce::StringArray depthFreqArray {"60 Hz", "100 Hz", "170 Hz"};
    name = ParamIDs::getDepthFreqName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, depthFreqArray, 1));

    name = ParamIDs::getDepthGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-12.f, 12.f, decibelIntervalValue, 1.f),0.f));
    
//    ========== RESONANCE TAMER ===========
    name = ParamIDs::getResoFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name,juce::NormalisableRange<float>(2000.f, 6000.f, frequencyIntervalValue, 1.f),3850.f));

    name = ParamIDs::getResoBypassName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    
//    ========== MUD KILLER TAMER ===========
    name = ParamIDs::getMudFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name,juce::NormalisableRange<float>(40.f, 500.f, frequencyIntervalValue, 1.f),120.f));

    name = ParamIDs::getMudBypassName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    
//    ========== BYPASS ===========
    name = ParamIDs::getPluginBypassName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    
    //==============================================================================
    //==============================================================================
//    COMPRESSOR
    
    auto gainRange = juce::NormalisableRange<float>(-60.f, 12.f, decibelIntervalValue, 1.f);
    name = ParamIDs::getCompGainInName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name, gainRange, 0.f));

    name = ParamIDs::getCompGainOutName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name, gainRange, 0.f));
    
    auto thresholdRange = juce::NormalisableRange<float>(MIN_THRESHOLD, MAX_DECIBELS, decibelIntervalValue, 1.f);
    name = ParamIDs::getThresholdName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name, thresholdRange, 0.f));
    
    auto attackReleaseRange = juce::NormalisableRange<float> (5.f, 500.f, 0.1f, 1.f);
    name = ParamIDs::getAttackName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name, attackReleaseRange, 30.f));

    name = ParamIDs::getReleaseName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name, attackReleaseRange, 50.f));
    
    auto choices = std::vector<double>{1,1.4,2,3,4,8,10,20,50,100};
    juce::StringArray sa;
    for (auto choice: choices)
        sa.add(juce::String(choice, 1));
    name = ParamIDs::getRatioName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint),name, sa, 4));
    
    name = ParamIDs::getCompBypassName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, true));

//    COMP RANGE PARAMETERS
//    auto compRangeStartRange = juce::NormalisableRange<float>(MIN_FREQ, 149.9f, frequencyIntervalValue, 1.f);
//    auto compRangeEndRange = juce::NormalisableRange<float>(150.f, MAX_FREQ, frequencyIntervalValue, 1.f);
//    name = ParamIDs::getCompRangeStartName();
//    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, compRangeStartRange, 80.f));
//
//    name = ParamIDs::getCompRangeEndName();
//    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, compRangeEndRange, 180.f));

//    ========== RETURN LAYOUT ===========
    return layout;
}

//==============================================================================
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& treeState)
{
    ChainSettings settings;
    
    settings.lowCutFreq = treeState.getRawParameterValue(ParamIDs::getLowCutFreqName())->load();
    settings.lowCutSlope = static_cast<Slope>(treeState.getRawParameterValue(ParamIDs::getLowCutSlopeName())->load());
    settings.highCutFreq = treeState.getRawParameterValue(ParamIDs::getHighCutFreqName())->load();
    settings.highCutSlope = static_cast<Slope>(treeState.getRawParameterValue(ParamIDs::getHighCutSlopeName())->load());
    settings.mudFreq = treeState.getRawParameterValue(ParamIDs::getMudFreqName())->load();
    settings.mudBypass = treeState.getRawParameterValue(ParamIDs::getMudBypassName())->load() < 0.5f;
    settings.presenceIndex = static_cast<int>(treeState.getRawParameterValue(ParamIDs::getPresenceFreqName())->load());
    settings.presenceGain = treeState.getRawParameterValue(ParamIDs::getPresenceGainName())->load();
    settings.depthIndex = static_cast<int>(treeState.getRawParameterValue(ParamIDs::getDepthFreqName())->load());
    settings.depthGain = treeState.getRawParameterValue(ParamIDs::getDepthGainName())->load();
    settings.resoBypass = treeState.getRawParameterValue(ParamIDs::getResoBypassName())->load() < 0.5f;
    settings.resoFreq = treeState.getRawParameterValue(ParamIDs::getResoFreqName())->load();
    settings.pluginBypass = treeState.getRawParameterValue(ParamIDs::getPluginBypassName())->load() > 0.5;
    
//    presence
    if (settings.presenceIndex == 0) {settings.presenceFreq = 4500.f;}
    else if (settings.presenceIndex == 1) { settings.presenceFreq = 6000.f;}
    else if (settings.presenceIndex == 2) {settings.presenceFreq = 8000.f;}

//    Depth
    if (settings.depthIndex == 0) {settings.depthFreq = 60.f;}
    else if (settings.depthIndex == 1) {settings.depthFreq = 100.f;}
    else if (settings.depthIndex == 2) {settings.depthFreq = 170.f;}

    return settings;
}

//==============================================================================
void GuitarToolsAudioProcessor::updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}
//==============================================================================
void GuitarToolsAudioProcessor::updateBellFitlers(const ChainSettings& chainSettings)
{

    auto resoCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.resoFreq, 10.f, juce::Decibels::decibelsToGain(-6.f));

    leftChain.setBypassed<ChainPositions::Reso>(chainSettings.resoBypass);
    rightChain.setBypassed<ChainPositions::Reso>(chainSettings.resoBypass);

    updateCoefficients(leftChain.get<ChainPositions::Reso>().coefficients, resoCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Reso>().coefficients, resoCoefficients);
    
    auto mudCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.mudFreq, 2.5f, juce::Decibels::decibelsToGain(-2.f));
    
    leftChain.setBypassed<ChainPositions::Mud>(chainSettings.mudBypass);
    rightChain.setBypassed<ChainPositions::Mud>(chainSettings.mudBypass);
    
    updateCoefficients(leftChain.get<ChainPositions::Mud>().coefficients, mudCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Mud>().coefficients, mudCoefficients);
}
//==============================================================================
void GuitarToolsAudioProcessor::updateShelfFilters(const ChainSettings& chainSettings)
{

    auto presCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), chainSettings.presenceFreq, 0.707f, juce::Decibels::decibelsToGain(chainSettings.presenceGain));
    
    updateCoefficients(leftChain.get<ChainPositions::Presence>().coefficients, presCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Presence>().coefficients, presCoefficients);
    
    auto depthCoefficientes = juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), chainSettings.depthFreq, 0.707f, juce::Decibels::decibelsToGain(chainSettings.depthGain));
    updateCoefficients(leftChain.get<ChainPositions::Depth>().coefficients, depthCoefficientes);
    updateCoefficients(rightChain.get<ChainPositions::Depth>().coefficients, depthCoefficientes);
}
//==============================================================================
void GuitarToolsAudioProcessor::updateLowCutFilters(const ChainSettings& chainSettings)
{
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq, getSampleRate(), (chainSettings.lowCutSlope + 1) * 2);
    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();
    updateCutFilter(leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
}
//==============================================================================
void GuitarToolsAudioProcessor::updateHighCutFilters(const ChainSettings& chainSettings)
{
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq, getSampleRate(), (chainSettings.highCutSlope + 1) * 2);
    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();
    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}

//==============================================================================
void GuitarToolsAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings(treeState);
    updateBellFitlers(chainSettings);
    updateShelfFilters(chainSettings);
    updateLowCutFilters(chainSettings);
    updateHighCutFilters(chainSettings);
}

void GuitarToolsAudioProcessor::parameterChanged(const juce::String&, float)
{
    filtersNeedUpdate.store(true);
}

void GuitarToolsAudioProcessor::loadDefaultPreset()
{
    loadPreset(juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("DefaultPreset.xml"));
               
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

void GuitarToolsAudioProcessor::linkCompParameters()
{

    midBandComp.attack = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID(ParamIDs::getAttackName(), 1).getParamID()));
    midBandComp.release = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID(ParamIDs::getReleaseName(), 1).getParamID()));
    midBandComp.threshold = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID(ParamIDs::getThresholdName(), 1).getParamID()));
    midBandComp.ratio = dynamic_cast<juce::AudioParameterChoice*>(treeState.getParameter(juce::ParameterID(ParamIDs::getRatioName(), 1).getParamID()));
    midBandComp.bypassed = dynamic_cast<juce::AudioParameterBool*>(treeState.getParameter(juce::ParameterID(ParamIDs::getCompBypassName(), 1).getParamID()));

//    compRangeStartParameter = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID(ParamIDs::getCompRangeStartName(), 1).getParamID()));
//    compRangeEndParameter = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID(ParamIDs::getCompRangeEndName(), 1).getParamID()));

    inputGainParam = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID(ParamIDs::getCompGainInName(), 1).getParamID()));
    outputGainParam = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID(ParamIDs::getCompGainOutName(), 1).getParamID()));
    
    using FilterType = juce::dsp::LinkwitzRileyFilterType;
    LP1.setType(FilterType::lowpass);
    HP1.setType(FilterType::highpass);
    AP2.setType(FilterType::allpass);
    LP2.setType(FilterType::lowpass);
    HP2.setType(FilterType::highpass);
    
}



void GuitarToolsAudioProcessor::updateCompState()
{
    compressors[1].updateCompressorSettings();

//    const auto compRangeStart = compRangeStartParameter->get();
//    const auto compRangeEnd = compRangeEndParameter->get();

    constexpr auto compRangeStart {80.0f};
    constexpr auto compRangeEnd {180.0f};

    LP1.setCutoffFrequency(compRangeStart);
    HP1.setCutoffFrequency(compRangeStart);

    AP2.setCutoffFrequency(compRangeEnd);
    LP2.setCutoffFrequency(compRangeEnd);
    HP2.setCutoffFrequency(compRangeEnd);
    
    inputGain.setGainDecibels(inputGainParam->get());
    outputGain.setGainDecibels(outputGainParam->get());
}

void GuitarToolsAudioProcessor::splitBands(const juce::AudioBuffer<float> &inputBuffer)
{
    for(auto& filterBuffer : filterBuffers)
        filterBuffer = inputBuffer;
    
    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]); //lowpass audio
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);
    
    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);
    
    LP1.process(fb0Ctx);
    AP2.process(fb0Ctx);
    HP1.process(fb1Ctx);
    filterBuffers[2]=filterBuffers[1];
    LP2.process(fb1Ctx);
    HP2.process(fb2Ctx);
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarToolsAudioProcessor();
}
