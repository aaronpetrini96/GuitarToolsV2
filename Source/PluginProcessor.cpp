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
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    
    smoothedBypassValue.reset(sampleRate, DefaultValues::parameterSmoothingTime);
    smoothedBypassValue.setCurrentAndTargetValue(pluginBypassActiveParam->get() ? 0.f : 1.f);

//==============================================================================
//    Compressor Section
    midBandCompressor.setActive(compActiveParam->get());
    midBandCompressor.setExtremeMode(compExtremeActiveParam->get());
    midBandCompressor.setLowTuning(lowTuningActiveParam->get());
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

    for (auto* smoother : {&resoGainDbSmoother, &mudGainDbSmoother, &resoQSmoother, &mudQSmoother, &mudFreqSmoother,
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
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
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
        buffer.copyFrom (i, 0, buffer, 0, 0, buffer.getNumSamples());

    dryBuffer.makeCopyOf(buffer, true); // Save a dry copy before processing

    updateGainState();
    midBandCompressor.setActive(compActiveParam->get());
    midBandCompressor.setExtremeMode(compExtremeActiveParam->get());
    midBandCompressor.setLowTuning(lowTuningActiveParam->get());

    const auto bypassActive = pluginBypassActiveParam->get(); // Update smoothed bypass target
    smoothedBypassValue.setTargetValue(bypassActive ? 0.0f : 1.0f);

    constexpr int parameterUpdateInterval {32};
    const auto numSamples = buffer.getNumSamples();
//    auto inputSquareSum = 0.f;

    for (int chunkStart {0}; chunkStart < numSamples; chunkStart += parameterUpdateInterval)
    {
        const auto chunkLength = juce::jmin(parameterUpdateInterval, numSamples - chunkStart);
        juce::AudioBuffer<float> chunk {buffer.getArrayOfWritePointers(), buffer.getNumChannels(), chunkStart, chunkLength};

        updateSmootherFromParams(chunkLength, SmootherUpdateMode::liveInRealTime);
        midBandCompressor.setAmount(compAmountParamSmoother.getCurrentValue() / 100.f);
        filters.update(getChainSettings());

//========================    LEVELS part    ========================
        applyGain(chunk, inputGain);
        inputLevelLeft.store(juce::jmax(inputLevelLeft.load(), chunk.getMagnitude(0, 0, chunkLength)));
        if (chunk.getNumChannels() > 1)
            inputLevelRight.store(juce::jmax(inputLevelRight.load(), chunk.getMagnitude(1, 0, chunkLength)));
        else
            inputLevelRight.store(inputLevelLeft.load());
//        for (int channel {0}; channel < chunk.getNumChannels(); ++channel)
//            inputSquareSum += juce::square (chunk.getRMSLevel (channel, 0, chunkLength)) * static_cast<float> (chunkLength);
//========================    EQ part    ========================
        filters.process(chunk);

//========================    COMP part    ========================
        midBandCompressor.process(chunk);
    }
//    inputMeanSquare.store (inputSquareSum / static_cast<float> (juce::jmax (1, numSamples * buffer.getNumChannels())));

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

    outputLevelLeft.store(juce::jmax(outputLevelLeft.load(), buffer.getMagnitude(0, 0, numSamples)));
    if (buffer.getNumChannels() > 1)
        outputLevelRight.store(juce::jmax(outputLevelRight.load(), buffer.getMagnitude(1, 0, numSamples)));
    else
        outputLevelRight.store(outputLevelLeft.load());

//    auto outputSquareSum = 0.f;
//    for (int channel {0}; channel < buffer.getNumChannels(); ++channel)
//        outputSquareSum += juce::square (buffer.getRMSLevel (channel, 0, numSamples));
//    outputMeanSquare.store (outputSquareSum / static_cast<float> (juce::jmax (1, buffer.getNumChannels())));
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
    state.setProperty("CurrentPresetName", getCurrentPresetName(), nullptr);
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
            setCurrentPresetName (presetNameProp->toString()); // restore preset name
        if (auto* uiScaleProp = state.getPropertyPointer("UIScale"))
            uiScale = static_cast<float>(*uiScaleProp);
        if (auto* showTooltipsProp = state.getPropertyPointer("ShowTooltips"))
            showTooltips = static_cast<bool>(*showTooltipsProp);

        state.removeAllProperties (nullptr);
        treeState.replaceState(state);
        stateRestoredBroadcaster.sendChangeMessage();
    }
}

void GuitarToolsAudioProcessor::savePreset(const juce::File& file)
{
    const auto state = treeState.copyState();
    auto xml = state.createXml();
    if (xml)
    {
        xml->setAttribute ("PluginVersion", JucePlugin_VersionString);
        xml->writeTo(file);
    }
}

void GuitarToolsAudioProcessor::loadPreset(const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse(file));
    if (xml && xml->hasTagName(treeState.state.getType()))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        state.removeAllProperties (nullptr);
        treeState.replaceState(state);
    }
}

//==============================================================================
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout GuitarToolsAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    constexpr auto versionHint {1};
    auto lowCutFrequencyRange = juce::NormalisableRange<float> (DefaultValues::lowCutRangeStart, DefaultValues::lowCutRangeEnd, DefaultValues::frequencyIntervalValue);
    auto highCutFrequencyRange = juce::NormalisableRange<float> (DefaultValues::highCutRangeStart, DefaultValues::highCutRangeEnd, DefaultValues::frequencyIntervalValue);
    auto gainRange = juce::NormalisableRange<float> (DefaultValues::gainRangeStart, DefaultValues::gainRangeEnd, DefaultValues::decibelIntervalValue, 1.f);
    auto resoFrequencyRange = juce::NormalisableRange<float> (DefaultValues::resoRangeStart, DefaultValues::resoRangeEnd, DefaultValues::frequencyIntervalValue);
    resoFrequencyRange.setSkewForCentre (std::sqrt (DefaultValues::resoRangeStart * DefaultValues::resoRangeEnd));
    auto amountRange = juce::NormalisableRange<float> (0.f, 100.f, DefaultValues::amountIntervalValue);

//    ========== HICUT LOW CUT ===========
    auto name = ParamIDs::getLowCutFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, lowCutFrequencyRange, DefaultValues::lowCutFreq, juce::AudioParameterFloatAttributes().withLabel ("Hz").withStringFromValueFunction (ParamHelpers::getStringFromValue (0))));

    name = ParamIDs::getHighCutFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name,highCutFrequencyRange, DefaultValues::highCutFreq, juce::AudioParameterFloatAttributes().withLabel ("Hz").withStringFromValueFunction (ParamHelpers::getStringFromValue (0))));

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
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, gainRange, DefaultValues::presenceGain, juce::AudioParameterFloatAttributes().withLabel ("dB").withStringFromValueFunction (ParamHelpers::getStringFromValue (1))));

    juce::StringArray depthModeArray {"A", "B", "C", "D"};
    name = ParamIDs::getDepthModeName();
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, depthModeArray, DefaultValues::depthMode));
    name = ParamIDs::getDepthGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, gainRange, DefaultValues::depthGain, juce::AudioParameterFloatAttributes().withLabel ("dB").withStringFromValueFunction (ParamHelpers::getStringFromValue (1))));

//    ========== RESONANCE TAMER ===========
    name = ParamIDs::getResoFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, resoFrequencyRange, DefaultValues::resoFreq, juce::AudioParameterFloatAttributes().withLabel ("Hz").withStringFromValueFunction (ParamHelpers::getStringFromValue (0))));
    name = ParamIDs::getResoActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::resoActive));
    name = ParamIDs::getResoExtremeActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::resoExtremeActive));

//    ========== MUD KILLER TAMER ===========
    name = ParamIDs::getMudAmountName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, amountRange, DefaultValues::mudAmount, juce::AudioParameterFloatAttributes().withLabel ("%").withStringFromValueFunction (ParamHelpers::getStringFromValue (0))));
    name = ParamIDs::getMudActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::mudActive));
    name = ParamIDs::getMudExtremeActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::mudExtremeActive));

//   ========== BYPASS ===========
    name = ParamIDs::getPluginBypassActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::pluginBypassActive));

//  ========== COMP ===========
    name = ParamIDs::getCompAmountName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint),name, amountRange, DefaultValues::compAmount, juce::AudioParameterFloatAttributes().withLabel ("%").withStringFromValueFunction (ParamHelpers::getStringFromValue (0))));
    name = ParamIDs::getCompActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::compActive));
    name = ParamIDs::getCompExtremeActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::compExtremeActive));
    name = ParamIDs::getLowTuningActiveName();
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, DefaultValues::lowTuningActive));

//  ========== LEVELS ===========
    name = ParamIDs::getInputGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, gainRange, DefaultValues::inputGainValue, juce::AudioParameterFloatAttributes().withLabel ("dB").withStringFromValueFunction (ParamHelpers::getStringFromValue (1))));
    name = ParamIDs::getOutputGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, gainRange, DefaultValues::outputGainValue, juce::AudioParameterFloatAttributes().withLabel ("dB").withStringFromValueFunction (ParamHelpers::getStringFromValue (1))));
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
    settings.mudFreq = mudFreqSmoother.getCurrentValue();
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
    const auto defaultPresetFile = PresetManager::getPresetFolder().getChildFile ("Default.xml");
    if (! defaultPresetFile.existsAsFile())
        return;
    loadPreset (defaultPresetFile);
    currentPresetName = "Default";
}

//==============================================================================
//==============================================================================

void GuitarToolsAudioProcessor::linkParameters()
{
    auto floatParams = std::array
    {
        &lowCutFreqParam, &highCutFreqParam, &resoFreqParam, &mudAmountParam,
        &presenceGainParam, &depthGainParam, &compAmountParam,
        &inputGainParam, &outputGainParam
    };

    auto floatParamIDs = std::array
    {
        &ParamIDs::getLowCutFreqName, &ParamIDs::getHighCutFreqName, &ParamIDs::getResoFreqName, &ParamIDs::getMudAmountName,
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
        &compActiveParam, &compExtremeActiveParam, &loudnessMatchActiveParam, &pluginBypassActiveParam,
        &lowTuningActiveParam
    };

    auto boolParamIDs = std::array
    {
        &ParamIDs::getResoActiveName, &ParamIDs::getResoExtremeActiveName, &ParamIDs::getMudActiveName, &ParamIDs::getMudExtremeActiveName,
        &ParamIDs::getCompActiveName, &ParamIDs::getCompExtremeActiveName, &ParamIDs::getLoudnessMatchActiveName, &ParamIDs::getPluginBypassActiveName,
        &ParamIDs::getLowTuningActiveName
    };

    initCachedParams<juce::AudioParameterBool*>(boolParams, boolParamIDs);

    smootherBindings =
    {
        {lowCutFreqParam, &lowCutFreqParamSmoother},
        {highCutFreqParam, &highCutFreqParamSmoother},
        {resoFreqParam, &resoFreqParamSmoother},
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
    updateSmoother(resoQSmoother, resoExtremeActive ? DefaultValues::resoExtremeQ : DefaultValues::resoQ);
    updateSmoother(mudGainDbSmoother, mudActiveParam->get() ? mudAmountParam->get() / 100.f * DefaultValues::mudMaxCutDb : 0.f);
    updateSmoother(mudQSmoother, mudExtremeActive ? DefaultValues::mudExtremeQ : DefaultValues::mudQ);
    updateSmoother(mudFreqSmoother, lowTuningActiveParam->get() ? DefaultValues::lowTuningMudFreq : DefaultValues::mudFreq);
    updateSmoother(presenceFreqSmoother, DefaultValues::presenceFreqByMode[presenceIndex]);
    updateSmoother(presenceResonanceSmoother, DefaultValues::presenceResonanceByMode[presenceIndex]);
    updateSmoother(depthFreqSmoother, DefaultValues::depthFreqByMode[depthIndex]);
    updateSmoother(depthResonanceSmoother, DefaultValues::depthResonanceByMode[depthIndex]);
}

void GuitarToolsAudioProcessor::updateGainState()
{
    inputGain.setGainDecibels(inputGainParam->get());
    const auto selectedOutputGainDb = loudnessMatchActiveParam->get() ? levelMatch.getGainDb() : outputGainParam->get();
    outputGain.setGainDecibels(selectedOutputGainDb);
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarToolsAudioProcessor();
}
