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
    
//==============================================================================
//    Compressor Section
    midBandCompressor.prepare(spec);

//==============================================================================
//    Levels Section
    inputGain.prepare(spec);
    inputGain.setRampDurationSeconds(DefaultValues::parameterSmoothingTime);
    outputGain.prepare(spec);
    outputGain.setRampDurationSeconds(DefaultValues::parameterSmoothingTime);

    levelMatch.prepare(spec);

    dryBuffer.setSize(static_cast<int>(spec.numChannels), numSamples);

//==============================================================================
//    EQ Section
    auto monospec = spec;
    monospec.numChannels = 1;
    leftChain.prepare(monospec);
    rightChain.prepare(monospec);
    lastChainSettings = ChainSettings{};

    for (const auto& [param, smoother] : smootherBindings)
        smoother->reset(sampleRate, DefaultValues::parameterSmoothingTime);

    resoGainDbSmoother.reset(sampleRate, DefaultValues::parameterSmoothingTime);
    mudGainDbSmoother.reset(sampleRate, DefaultValues::parameterSmoothingTime);

    updateSmootherFromParams(1, SmootherUpdateMode::initialize);
    updateFilters();
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

    updateSmootherFromParams(buffer.getNumSamples(), SmootherUpdateMode::liveInRealTime);
    updateGainState();
    midBandCompressor.setAmount(compAmountParamSmoother.getCurrentValue() / DefaultValues::compAmountRangeEnd);
    midBandCompressor.setActive(compActiveParam->get());
    midBandCompressor.setExtremeMode(compExtremeActiveParam->get());
    updateFilters();

    const auto bypassActive = pluginBypassActiveParam->get(); // Update smoothed bypass target
    smoothedBypassValue.setTargetValue(bypassActive ? 0.0f : 1.0f);

//========================    LEVELS part    ========================
    applyGain(buffer, inputGain);

//========================    COMP part    ========================
    midBandCompressor.process(buffer);

//========================    EQ part    ========================
    const juce::dsp::AudioBlock<float> block {buffer};

    auto leftBlock = block.getSingleChannelBlock(0);
    juce::dsp::ProcessContextReplacing<float> leftContext {leftBlock};
    leftChain.process(leftContext);

    if (totalNumInputChannels > 1)
    {
        auto rightBlock = block.getSingleChannelBlock(1);
        juce::dsp::ProcessContextReplacing<float> rightContext {rightBlock};
        rightChain.process(rightContext);
    }
    
    levelMatch.update (dryBuffer, buffer);
    applyGain(buffer, outputGain);
    

    for (int n {0}; n < buffer.getNumSamples(); ++n)     // Smooth crossfade
    {
        const auto mix = smoothedBypassValue.getNextValue();
        for (int ch {0}; ch < buffer.getNumChannels(); ++ch)
        {
            auto* wet = buffer.getWritePointer(ch);
            const auto* dry = dryBuffer.getReadPointer(ch);
            wet[n] = wet[n] * mix + dry[n] * (1.f - mix);
        }
    }

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
    state.setProperty ("StateAActive", stateAActive, nullptr);

    const auto& inactiveState = stateAActive ? stateB : stateA;
    if (inactiveState.isValid())
    {
        juce::ValueTree inactiveStateTree ("InactiveABState");
        inactiveStateTree.appendChild (inactiveState.createCopy(), nullptr);
        state.appendChild (inactiveStateTree, nullptr);
    }

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

        stateAActive = static_cast<bool> (state.getProperty ("StateAActive", true));
        stateA = {};
        stateB = {};

        const auto inactiveStateTree = state.getChildWithName ("InactiveABState");
        if (inactiveStateTree.isValid())
        {
            auto& inactiveState = stateAActive ? stateB : stateA;
            inactiveState = inactiveStateTree.getChild (0).createCopy();
            state.removeChild (inactiveStateTree, nullptr);
        }

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

void GuitarToolsAudioProcessor::switchABState()
{
    auto& stateToStore = stateAActive ? stateA : stateB;
    const auto& stateToLoad = stateAActive ? stateB : stateA;

    stateToStore = treeState.copyState();
    stateToStore.setProperty ("CurrentPresetName", currentPresetName, nullptr);

    if (stateToLoad.isValid())
    {
        undoManager.beginNewTransaction();

        for (const auto& storedParameter : stateToLoad)
        {
            const auto parameterID = storedParameter.getProperty ("id").toString();

            if (parameterID != ParamIDs::getPluginBypassActiveName())
                treeState.state.getChildWithProperty ("id", parameterID).setProperty ("value", storedParameter.getProperty ("value"), &undoManager);
        }

        currentPresetName = stateToLoad.getProperty ("CurrentPresetName").toString();
    }

    stateAActive = ! stateAActive;
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
    settings.resoExtremeActive = resoExtremeActiveParam->get();
    settings.mudFreq = mudFreqParamSmoother.getCurrentValue();
    settings.mudGainDb = mudGainDbSmoother.getCurrentValue();
    settings.mudExtremeActive = mudExtremeActiveParam->get();

    settings.presenceIndex = presenceModeParam->getIndex();
    settings.presenceGain = presenceGainParamSmoother.getCurrentValue();
    settings.presenceFreq = DefaultValues::presenceFreqByMode[(size_t) juce::jlimit (0, 3, settings.presenceIndex)];
    settings.presenceResonance = DefaultValues::presenceResonanceByMode[(size_t) juce::jlimit (0, 3, settings.presenceIndex)];

    settings.depthIndex = depthModeParam->getIndex();
    settings.depthGain = depthGainParamSmoother.getCurrentValue();
    settings.depthFreq = DefaultValues::depthFreqByMode[(size_t) juce::jlimit (0, 3, settings.depthIndex)];
    settings.depthResonance = DefaultValues::depthResonanceByMode[(size_t) juce::jlimit (0, 3, settings.depthIndex)];

    return settings;
}

//==============================================================================
void GuitarToolsAudioProcessor::updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}
//==============================================================================
void GuitarToolsAudioProcessor::updateResoFilters(const ChainSettings& chainSettings)
{
    const auto resoQ = chainSettings.resoExtremeActive ? DefaultValues::resoExtremeQ : DefaultValues::resoQ;
    const auto resoGain = juce::Decibels::decibelsToGain(chainSettings.resoGainDb);
    const auto resoCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.resoFreq, resoQ, resoGain);

    updateCoefficients(leftChain.get<ChainPositions::Reso>().coefficients, resoCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Reso>().coefficients, resoCoefficients);
}
//==============================================================================
void GuitarToolsAudioProcessor::updateMudFilters(const ChainSettings& chainSettings)
{
    const auto mudQ = chainSettings.mudExtremeActive ? DefaultValues::mudExtremeQ : DefaultValues::mudQ;
    const auto mudGain = juce::Decibels::decibelsToGain(chainSettings.mudGainDb);
    const auto mudCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.mudFreq, mudQ, mudGain);

    updateCoefficients(leftChain.get<ChainPositions::Mud>().coefficients, mudCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Mud>().coefficients, mudCoefficients);
}
//==============================================================================
void GuitarToolsAudioProcessor::updatePresenceFilters(const ChainSettings& chainSettings)
{
    const auto presCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), chainSettings.presenceFreq, chainSettings.presenceResonance, juce::Decibels::decibelsToGain(chainSettings.presenceGain));

    updateCoefficients(leftChain.get<ChainPositions::Presence>().coefficients, presCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Presence>().coefficients, presCoefficients);
}
//==============================================================================
void GuitarToolsAudioProcessor::updateDepthFilters(const ChainSettings& chainSettings)
{
    const auto depthCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), chainSettings.depthFreq, chainSettings.depthResonance, juce::Decibels::decibelsToGain(chainSettings.depthGain));

    updateCoefficients(leftChain.get<ChainPositions::Depth>().coefficients, depthCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Depth>().coefficients, depthCoefficients);
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
    const auto chainSettings = getChainSettings();

    if (chainSettings.lowCutFreq != lastChainSettings.lowCutFreq
     || chainSettings.lowCutSlope != lastChainSettings.lowCutSlope)
        updateLowCutFilters(chainSettings);

    if (chainSettings.highCutFreq != lastChainSettings.highCutFreq
     || chainSettings.highCutSlope != lastChainSettings.highCutSlope)
        updateHighCutFilters(chainSettings);

    if (chainSettings.resoFreq != lastChainSettings.resoFreq
     || chainSettings.resoGainDb != lastChainSettings.resoGainDb
     || chainSettings.resoExtremeActive != lastChainSettings.resoExtremeActive)
        updateResoFilters(chainSettings);

    if (chainSettings.mudFreq != lastChainSettings.mudFreq
     || chainSettings.mudGainDb != lastChainSettings.mudGainDb
     || chainSettings.mudExtremeActive != lastChainSettings.mudExtremeActive)
        updateMudFilters(chainSettings);

    if (chainSettings.presenceIndex != lastChainSettings.presenceIndex
     || chainSettings.presenceGain != lastChainSettings.presenceGain)
        updatePresenceFilters(chainSettings);

    if (chainSettings.depthIndex != lastChainSettings.depthIndex
     || chainSettings.depthGain != lastChainSettings.depthGain)
        updateDepthFilters(chainSettings);

    lastChainSettings = chainSettings;
}

void GuitarToolsAudioProcessor::loadDefaultPreset()
{
    const auto defaultPresetFile = PresetManager::getPresetFolder().getChildFile ("Start From Here.xml");
    if (! defaultPresetFile.existsAsFile())
        return;
    loadPreset (defaultPresetFile);
    currentPresetName = "Start From Here";
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
    for (const auto& [param, smoother] : smootherBindings)
    {
        if (mode == SmootherUpdateMode::initialize)
            smoother->setCurrentAndTargetValue(param->get());
        else
            smoother->setTargetValue(param->get());

        smoother->skip(numSamplesToSkip);
    }

    const auto resoGainDbTarget = resoActiveParam->get() ? (resoExtremeActiveParam->get() ? DefaultValues::resoExtremeGainDb : DefaultValues::resoGainDb) : 0.f;
    const auto mudGainDbTarget = mudActiveParam->get() ? (mudExtremeActiveParam->get() ? DefaultValues::mudExtremeGainDb : DefaultValues::mudGainDb) : 0.f;

    if (mode == SmootherUpdateMode::initialize)
    {
        resoGainDbSmoother.setCurrentAndTargetValue(resoGainDbTarget);
        mudGainDbSmoother.setCurrentAndTargetValue(mudGainDbTarget);
    }
    else
    {
        resoGainDbSmoother.setTargetValue(resoGainDbTarget);
        mudGainDbSmoother.setTargetValue(mudGainDbTarget);
    }

    resoGainDbSmoother.skip(numSamplesToSkip);
    mudGainDbSmoother.skip(numSamplesToSkip);
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
