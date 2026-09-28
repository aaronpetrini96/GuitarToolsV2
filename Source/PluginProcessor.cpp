#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <array>
#include <cmath>
#include <memory>

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
    linkParameters();
    PresetManager::installFactoryPresets();
    loadDefaultPreset();
}

GuitarToolsAudioProcessor::~GuitarToolsAudioProcessor()
{
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
    return 0.5;
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
    juce::dsp::ProcessSpec spec{};
    const auto numSamples = samplesPerBlock;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.sampleRate = sampleRate;
    spec.numChannels = static_cast<juce::uint32>(getTotalNumInputChannels());
    
    smoothedBypassValue.reset(sampleRate, DefaultValues::parameterSmoothingTime);
    smoothedBypassValue.setCurrentAndTargetValue(pluginBypassActiveParam->get() ? 0.f : 1.f);

//==============================================================================
//    Compressor Section
    midBandCompressor.setActive(compActiveParam->get());
    midBandCompressor.setExtremeMode(compExtremeActiveParam->get());
    midBandCompressor.prepare(spec);

//==============================================================================
//    Levels Section
    levelMatch.prepare(spec);
    updateGainState();

    inputGain.prepare(spec);
    inputGain.setRampDurationSeconds(DefaultValues::parameterSmoothingTime);
    outputGain.prepare(spec);
    outputGain.setRampDurationSeconds(DefaultValues::parameterSmoothingTime);

    dryBuffer.setSize(static_cast<int>(spec.numChannels), numSamples);

//==============================================================================
//    EQ Section
    for (const auto& [param, smoother] : smootherBindings)
        smoother->reset(sampleRate, DefaultValues::parameterSmoothingTime);

    for (auto* smoother : {&resoGainDbSmoother, &mudGainDbSmoother, &resoQSmoother, &mudQSmoother,
                           &presenceFreqSmoother, &presenceResonanceSmoother, &depthFreqSmoother, &depthResonanceSmoother})
        smoother->reset(sampleRate, DefaultValues::parameterSmoothingTime);

    updateSmootherFromParams(1, SmootherUpdateMode::initialize);

    filters.prepare(spec, getChainSettings());

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

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void GuitarToolsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    dryBuffer.makeCopyOf(buffer, true); // Save a dry copy before processing

    updateGainState();
    midBandCompressor.setActive(compActiveParam->get());
    midBandCompressor.setExtremeMode(compExtremeActiveParam->get());

    const auto bypassActive = pluginBypassActiveParam->get(); // Update smoothed bypass target
    smoothedBypassValue.setTargetValue(bypassActive ? 0.0f : 1.0f);

    constexpr int parameterUpdateInterval {32};
    const auto numSamples = buffer.getNumSamples();

    for (int chunkStart {0}; chunkStart < numSamples; chunkStart += parameterUpdateInterval)
    {
        const auto chunkLength = juce::jmin(parameterUpdateInterval, numSamples - chunkStart);
        juce::AudioBuffer<float> chunk {buffer.getArrayOfWritePointers(), buffer.getNumChannels(), chunkStart, chunkLength};

        updateSmootherFromParams(chunkLength, SmootherUpdateMode::liveInRealTime);
        midBandCompressor.setAmount(compAmountParamSmoother.getCurrentValue() / DefaultValues::compAmountRangeEnd);
        filters.update(getChainSettings());

//========================    LEVELS part    ========================
        applyGain(chunk, inputGain);

//========================    EQ part    ========================
        filters.process(chunk);

//========================    COMP part    ========================
        midBandCompressor.process(chunk);
    }
    
    levelMatch.update (dryBuffer, buffer);
    applyGain(buffer, outputGain);

    for (int ch {0}; ch < buffer.getNumChannels(); ++ch)     // Smooth crossfade
    {
        auto channelBypassValue = smoothedBypassValue;
        auto* wet = buffer.getWritePointer(ch);
        const auto* dry = dryBuffer.getReadPointer(ch);
        for (int n {0}; n < buffer.getNumSamples(); ++n)
        {
            const auto mix = channelBypassValue.getNextValue();
            wet[n] = wet[n] * mix + dry[n] * (1.f - mix);
        }
    }
    smoothedBypassValue.skip(buffer.getNumSamples());
}

//==============================================================================
bool GuitarToolsAudioProcessor::hasEditor() const {return true;}

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
    state.setProperty("UIScale", uiScale, nullptr);
    state.setProperty("ShowTooltips", showTooltips, nullptr);

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
        if (auto* uiScaleProp = state.getPropertyPointer("UIScale"))
            uiScale = static_cast<float>(*uiScaleProp);
        if (auto* showTooltipsProp = state.getPropertyPointer("ShowTooltips"))
            showTooltips = static_cast<bool>(*showTooltipsProp);

        treeState.replaceState(state);
    }
}

void GuitarToolsAudioProcessor::savePreset(const juce::File& file)
{
    const auto state = treeState.copyState();
    auto xml = state.createXml();
    if (xml)
    {
        xml->writeTo(file);
    }
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
    }
}

//==============================================================================
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout GuitarToolsAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    constexpr auto versionHint {1};

    const auto frequencyStringFromValue = [] (float value, int) { return juce::String (juce::roundToInt (value)); };
    const auto decibelStringFromValue = [] (float value, int) { return juce::String (value, 1); };

    auto lowCutFrequencyRange = juce::NormalisableRange<float> (DefaultValues::lowCutRangeStart, DefaultValues::lowCutRangeEnd, DefaultValues::frequencyIntervalValue);
    auto highCutFrequencyRange = juce::NormalisableRange<float> (DefaultValues::highCutRangeStart, DefaultValues::highCutRangeEnd, DefaultValues::frequencyIntervalValue);
    auto gainRange = juce::NormalisableRange<float> (DefaultValues::gainRangeStart, DefaultValues::gainRangeEnd, DefaultValues::decibelIntervalValue, 1.f);

//    ========== HICUT LOW CUT ===========
    auto name = ParamIDs::getLowCutFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, lowCutFrequencyRange, DefaultValues::lowCutFreq, juce::AudioParameterFloatAttributes().withLabel ("Hz").withStringFromValueFunction (frequencyStringFromValue)));

    name = ParamIDs::getHighCutFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name,highCutFrequencyRange, DefaultValues::highCutFreq, juce::AudioParameterFloatAttributes().withLabel ("Hz").withStringFromValueFunction (frequencyStringFromValue)));

    juce::StringArray stringArray;
    for (int i {0}; i < 4; ++i)
        stringArray.add(juce::String(12 + 12*i)+ " dB/Oct");
    
    name = ParamIDs::getLowCutSlopeName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, stringArray, DefaultValues::lowCutSlope));
    name = ParamIDs::getHighCutSlopeName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, stringArray, DefaultValues::highCutSlope));

//    ========== HIGH/LOW SHELVE  ===========
    juce::StringArray presenceModeArray {"A", "B", "C", "D"};
    name = ParamIDs::getPresenceModeName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, presenceModeArray, DefaultValues::presenceMode));
    name = ParamIDs::getPresenceGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, gainRange, DefaultValues::presenceGain, juce::AudioParameterFloatAttributes().withLabel ("dB").withStringFromValueFunction (decibelStringFromValue)));

    juce::StringArray depthModeArray {"A", "B", "C", "D"};
    name = ParamIDs::getDepthModeName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, depthModeArray, DefaultValues::depthMode));
    name = ParamIDs::getDepthGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, gainRange, DefaultValues::depthGain, juce::AudioParameterFloatAttributes().withLabel ("dB").withStringFromValueFunction (decibelStringFromValue)));

//    ========== RESONANCE TAMER ===========
    name = ParamIDs::getResoFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name,juce::NormalisableRange<float>(DefaultValues::resoRangeStart, DefaultValues::resoRangeEnd, DefaultValues::frequencyIntervalValue, 1.f), DefaultValues::resoFreq, juce::AudioParameterFloatAttributes().withLabel ("Hz").withStringFromValueFunction (frequencyStringFromValue)));
    name = ParamIDs::getResoActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::resoActive));
    name = ParamIDs::getResoExtremeActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::resoExtremeActive));

//    ========== MUD KILLER TAMER ===========
    name = ParamIDs::getMudFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name,juce::NormalisableRange<float>(DefaultValues::mudRangeStart, DefaultValues::mudRangeEnd, DefaultValues::frequencyIntervalValue, 1.f), DefaultValues::mudFreq, juce::AudioParameterFloatAttributes().withLabel ("Hz").withStringFromValueFunction (frequencyStringFromValue)));
    name = ParamIDs::getMudActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::mudActive));
    name = ParamIDs::getMudExtremeActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::mudExtremeActive));

//   ========== BYPASS ===========
    name = ParamIDs::getPluginBypassActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::pluginBypassActive));

//  ========== COMP ===========
    auto compAmountRange = juce::NormalisableRange<float>(DefaultValues::compAmountRangeStart, DefaultValues::compAmountRangeEnd, DefaultValues::compAmountIntervalValue, 1.f);
    name = ParamIDs::getCompAmountName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name, compAmountRange, DefaultValues::compAmount, juce::AudioParameterFloatAttributes().withLabel ("%").withStringFromValueFunction (frequencyStringFromValue)));
    name = ParamIDs::getCompActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::compActive));
    name = ParamIDs::getCompExtremeActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::compExtremeActive));

//  ========== LEVELS ===========
    name = ParamIDs::getInputGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, gainRange, DefaultValues::inputGainValue, juce::AudioParameterFloatAttributes().withLabel ("dB").withStringFromValueFunction (decibelStringFromValue)));
    name = ParamIDs::getOutputGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, gainRange, DefaultValues::outputGainValue, juce::AudioParameterFloatAttributes().withLabel ("dB").withStringFromValueFunction (decibelStringFromValue)));
    name = ParamIDs::getLoudnessMatchActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::loudnessMatchActive));

//    ========== RETURN LAYOUT ===========
    return layout;
}

//==============================================================================
ChainSettings GuitarToolsAudioProcessor::getChainSettings() const
{
    ChainSettings settings;

    settings.lowCutFreq = lowCutFreqParamSmoother.getCurrentValue();
    settings.lowCutSlope = static_cast<Slope>(lowCutSlopeParam->getIndex());
    settings.highCutFreq = highCutFreqParamSmoother.getCurrentValue();
    settings.highCutSlope = static_cast<Slope>(highCutSlopeParam->getIndex());

    settings.resoFreq = resoFreqParamSmoother.getCurrentValue();
    settings.resoGainDb = resoGainDbSmoother.getCurrentValue();
    settings.resoQ = resoQSmoother.getCurrentValue();
    settings.mudFreq = mudFreqParamSmoother.getCurrentValue();
    settings.mudGainDb = mudGainDbSmoother.getCurrentValue();
    settings.mudQ = mudQSmoother.getCurrentValue();

    settings.presenceGain = presenceGainParamSmoother.getCurrentValue();
    settings.presenceFreq = presenceFreqSmoother.getCurrentValue();
    settings.presenceResonance = presenceResonanceSmoother.getCurrentValue();

    settings.depthGain = depthGainParamSmoother.getCurrentValue();
    settings.depthFreq = depthFreqSmoother.getCurrentValue();
    settings.depthResonance = depthResonanceSmoother.getCurrentValue();

    return settings;
}

void GuitarToolsAudioProcessor::loadDefaultPreset()
{
    const auto defaultPresetFile = PresetManager::getPresetFolder().getChildFile ("DefaultStarter.xml");
    if (! defaultPresetFile.existsAsFile())
        return;
    loadPreset (defaultPresetFile);
    currentPresetName = "DefaultStarter";
}

//==============================================================================
//==============================================================================

void GuitarToolsAudioProcessor::linkParameters()
{
    auto floatParams = std::array
    {
        &lowCutFreqParam, &highCutFreqParam, &resoFreqParam, &mudFreqParam,
        &presenceGainParam, &depthGainParam, &compAmountParam,
        &inputGainParam, &outputGainParam
    };

    auto floatParamIDs = std::array
    {
        &ParamIDs::getLowCutFreqName, &ParamIDs::getHighCutFreqName, &ParamIDs::getResoFreqName, &ParamIDs::getMudFreqName,
        &ParamIDs::getPresenceGainName, &ParamIDs::getDepthGainName, &ParamIDs::getCompAmountName,
        &ParamIDs::getInputGainName, &ParamIDs::getOutputGainName
    };

    initCachedParams<juce::AudioParameterFloat*>(floatParams, floatParamIDs);

    auto choiceParams = std::array
    {
        &lowCutSlopeParam, &highCutSlopeParam, &presenceModeParam, &depthModeParam
    };

    auto choiceParamIDs = std::array
    {
        &ParamIDs::getLowCutSlopeName, &ParamIDs::getHighCutSlopeName,
        &ParamIDs::getPresenceModeName, &ParamIDs::getDepthModeName
    };

    initCachedParams<juce::AudioParameterChoice*>(choiceParams, choiceParamIDs);

    auto boolParams = std::array
    {
        &resoActiveParam, &resoExtremeActiveParam, &mudActiveParam, &mudExtremeActiveParam,
        &compActiveParam, &compExtremeActiveParam, &loudnessMatchActiveParam, &pluginBypassActiveParam
    };

    auto boolParamIDs = std::array
    {
        &ParamIDs::getResoActiveName, &ParamIDs::getResoExtremeActiveName, &ParamIDs::getMudActiveName, &ParamIDs::getMudExtremeActiveName,
        &ParamIDs::getCompActiveName, &ParamIDs::getCompExtremeActiveName, &ParamIDs::getLoudnessMatchActiveName, &ParamIDs::getPluginBypassActiveName
    };

    initCachedParams<juce::AudioParameterBool*>(boolParams, boolParamIDs);

    smootherBindings =
    {
        {lowCutFreqParam, &lowCutFreqParamSmoother},
        {highCutFreqParam, &highCutFreqParamSmoother},
        {resoFreqParam, &resoFreqParamSmoother},
        {mudFreqParam, &mudFreqParamSmoother},
        {presenceGainParam, &presenceGainParamSmoother},
        {depthGainParam, &depthGainParamSmoother},
        {compAmountParam, &compAmountParamSmoother}
    };
}

void GuitarToolsAudioProcessor::updateSmootherFromParams(int numSamplesToSkip, SmootherUpdateMode mode)
{
    const auto updateSmoother = [mode, numSamplesToSkip] (juce::SmoothedValue<float>& smoother, float targetValue)
    {
        if (mode == SmootherUpdateMode::initialize)
            smoother.setCurrentAndTargetValue(targetValue);
        else
            smoother.setTargetValue(targetValue);

        smoother.skip(numSamplesToSkip);
    };

    for (const auto& [param, smoother] : smootherBindings)
        updateSmoother(*smoother, param->get());

    const auto resoExtremeActive = resoExtremeActiveParam->get();
    const auto mudExtremeActive = mudExtremeActiveParam->get();
    const auto presenceIndex = static_cast<size_t>(juce::jlimit(0, 3, presenceModeParam->getIndex()));
    const auto depthIndex = static_cast<size_t>(juce::jlimit(0, 3, depthModeParam->getIndex()));

    updateSmoother(resoGainDbSmoother, resoActiveParam->get() ? (resoExtremeActive ? DefaultValues::resoExtremeGainDb : DefaultValues::resoGainDb) : 0.f);
    updateSmoother(mudGainDbSmoother, mudActiveParam->get() ? (mudExtremeActive ? DefaultValues::mudExtremeGainDb : DefaultValues::mudGainDb) : 0.f);
    updateSmoother(resoQSmoother, resoExtremeActive ? DefaultValues::resoExtremeQ : DefaultValues::resoQ);
    updateSmoother(mudQSmoother, mudExtremeActive ? DefaultValues::mudExtremeQ : DefaultValues::mudQ);
    updateSmoother(presenceFreqSmoother, DefaultValues::presenceFreqByMode[presenceIndex]);
    updateSmoother(presenceResonanceSmoother, DefaultValues::presenceResonanceByMode[presenceIndex]);
    updateSmoother(depthFreqSmoother, DefaultValues::depthFreqByMode[depthIndex]);
    updateSmoother(depthResonanceSmoother, DefaultValues::depthResonanceByMode[depthIndex]);
}

void GuitarToolsAudioProcessor::updateGainState()
{
    inputGain.setGainDecibels(inputGainParam->get());
    outputGain.setGainDecibels(loudnessMatchActiveParam->get() ? levelMatch.getGainDb() : outputGainParam->get());
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarToolsAudioProcessor();
}
