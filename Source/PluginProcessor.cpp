/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
static const juce::StringArray eqParameterIDs
{
    "LowCut Freq", "LowCut Slope", "HighCut Freq", "HighCut Slope",
    "Reso Freq", "Reso Bypass", "Mud Freq", "Mud Bypass",
    "Presence Freq", "Presence Gain", "Depth Freq", "Depth Gain"
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

//    ========== HICUT LOW CUT ===========
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("LowCut Freq",1),"LowCut Freq",juce::NormalisableRange<float>(20.f, 4000.f, 1.f, 0.5f),40.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("HighCut Freq",1),"HighCut Freq",juce::NormalisableRange<float>(4000.f, 20000.f, 1.f, 1.f),14000.f));
    
    
    juce::StringArray stringArray;
    for (int i {0}; i < 4; ++i) {
        juce::String str;
        str << (12 + 12*i);
        str << " dB/Oct";
        stringArray.add(str);
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("LowCut Slope", 1),"LowCut Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("HighCut Slope", 1),"HighCut Slope", stringArray, 0));

//    ========== HIGH/LOW SHELVE  ===========
    juce::StringArray presenceFreqArray {"4.5 kHz", "6 kHz", "8 kHz"};
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Presence Freq", 1), "Presence Freq", presenceFreqArray, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Presence Gain", 1), "Presence Gain", juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f),3.f));
    

    juce::StringArray depthFreqArray {"60 Hz", "100 Hz", "170 Hz"};
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Depth Freq", 1), "Depth Freq", depthFreqArray, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Depth Gain", 1), "Depth Gain", juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f),0.f));
    
//    ========== RESONANCE TAMER ===========
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Reso Freq",1),"Reso Freq",juce::NormalisableRange<float>(2000.f, 6000.f, 1.f, 1.f),3850.f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("Reso Bypass", 1), "Tame Resonance", false));
    
//    ========== MUD KILLER TAMER ===========
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Mud Freq", 1),"Mud Freq",juce::NormalisableRange<float>(40.f, 500.f, 1.f, 1.f),120.f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("Mud Bypass", 1), "Kill Mud", false));
    
//    ========== BYPASS ===========
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("Plugin Bypass", 1), "Bypass", false));
    
//    ========== OVERSAMPLING ===========
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Oversampling", 1), "Oversampling", juce::StringArray {"Off", "2x", "4x", "8x"}, 0));
    
    //==============================================================================
    //==============================================================================
//    COMPRESSOR
    
    auto gainRange = juce::NormalisableRange<float>(-60.f, 12.f, 0.1f, 1.f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Comp Gain In", 1),"Comp Gain In", gainRange, 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Comp Gain Out", 1),"Comp Gain Out", gainRange, 0.f));
    
    auto thresholdRange = juce::NormalisableRange<float>(MIN_THRESHOLD, MAX_DECIBELS, 1.f, 1.f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Threshold", 1),"Threshold", thresholdRange, 0.f));
    
    auto attackReleaseRange = juce::NormalisableRange<float> (5.f, 500.f, 1.f, 1.f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Attack", 1),"Attack", attackReleaseRange, 30.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Release", 1),"Release", attackReleaseRange, 50.f));
    
    auto choices = std::vector<double>{1,1.4,2,3,4,8,10,20,50,100};
    juce::StringArray sa;
    for (auto choice: choices)
        sa.add(juce::String(choice, 1));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Ratio", 1),"Ratio", sa, 4));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("Comp Bypass", 1), "Control Low-End", true));
    
    //CROSSOVER FREQS
    auto lowMidCrossoverRange = juce::NormalisableRange<float>(MIN_FREQ, 149.9f, 1.f, 1.f);
    auto midHighCrossoverRange = juce::NormalisableRange<float>(150.f, MAX_FREQ, 1.f, 1.f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("LowMid Crossover Freq", 1),"LowMid Crossover Freq", lowMidCrossoverRange, 70.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MidHigh Crossover Freq", 1),"MidHigh Crossover Freq", lowMidCrossoverRange, 210.f));
    
    
    
//    ========== RETURN LAYOUT ===========
    return layout;
}

//==============================================================================
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& treeState)
{
    ChainSettings settings;
    
    settings.lowCutFreq = treeState.getRawParameterValue("LowCut Freq")->load();
    settings.lowCutSlope = static_cast<Slope>(treeState.getRawParameterValue("LowCut Slope")->load());
    settings.highCutFreq = treeState.getRawParameterValue("HighCut Freq")->load();
    settings.highCutSlope = static_cast<Slope>(treeState.getRawParameterValue("HighCut Slope")->load());
    settings.mudFreq = treeState.getRawParameterValue("Mud Freq")->load();
    settings.mudBypass = treeState.getRawParameterValue("Mud Bypass")->load() < 0.5f;
    settings.presenceIndex = static_cast<int>(treeState.getRawParameterValue("Presence Freq")->load());
    settings.presenceGain = treeState.getRawParameterValue("Presence Gain")->load();
    settings.depthIndex = static_cast<int>(treeState.getRawParameterValue("Depth Freq")->load());
    settings.depthGain = treeState.getRawParameterValue("Depth Gain")->load();
    settings.resoBypass = treeState.getRawParameterValue("Reso Bypass")->load() < 0.5f;
    settings.resoFreq = treeState.getRawParameterValue("Reso Freq")->load();
    settings.oversamplingFactor = static_cast<OversamplingFactor>(treeState.getRawParameterValue("Oversampling")->load());
    settings.pluginBypass = treeState.getRawParameterValue("Plugin Bypass")->load() > 0.5;
    
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


void GuitarToolsAudioProcessor::updateOversampling(const ChainSettings &chainSettings)
{
    auto factor = chainSettings.oversamplingFactor;
    int newFactor = 1;

    switch (factor) {
        case Off: newFactor = 1; break;
        case x2:  newFactor = 2; break;
        case x4:  newFactor = 4; break;
        case x8:  newFactor = 8; break;
    }

    // Only recreate oversampling object if factor has changed
    if (newFactor != oversamplingFactor)
    {
        oversamplingFactor = newFactor;
        std::cout << "New Oversampling Factor: " << oversamplingFactor << "x" << std::endl;

        // Calculate number of stages (each stage doubles the rate)
        int numStages = 0;
        if (oversamplingFactor == 2) numStages = 1;
        else if (oversamplingFactor == 4) numStages = 2;
        else if (oversamplingFactor == 8) numStages = 3;
        else numStages = 0; // Off or 1x

        // Recreate oversampling object
        oversampling = std::make_unique<juce::dsp::Oversampling<float>>(
            2, // 2 channels (stereo)
            numStages, // number of stages
            juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR
        );

        // Initialize buffers with expected block size (use safe default)
        oversampling->reset();
        oversampling->initProcessing((size_t) getBlockSize());
    }
}

//==============================================================================
void GuitarToolsAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings(treeState);
    updateBellFitlers(chainSettings);
    updateShelfFilters(chainSettings);
    updateLowCutFilters(chainSettings);
    updateHighCutFilters(chainSettings);
//    updateOversampling(chainSettings);
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

    midBandComp.attack = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID("Attack", 1).getParamID()));
    midBandComp.release = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID("Release", 1).getParamID()));
    midBandComp.threshold = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID("Threshold", 1).getParamID()));
    midBandComp.ratio = dynamic_cast<juce::AudioParameterChoice*>(treeState.getParameter(juce::ParameterID("Ratio", 1).getParamID()));
    midBandComp.bypassed = dynamic_cast<juce::AudioParameterBool*>(treeState.getParameter(juce::ParameterID("Comp Bypass", 1).getParamID()));
    
    lowMidCrossover = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID("LowMid Crossover Freq", 1).getParamID()));
    midHighCrossover = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID("MidHigh Crossover Freq", 1).getParamID()));
    
    inputGainParam = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID("Comp Gain In", 1).getParamID()));
    outputGainParam = dynamic_cast<juce::AudioParameterFloat*>(treeState.getParameter(juce::ParameterID("Comp Gain Out", 1).getParamID()));
    
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
    
    auto lowMidCutoffFreq = lowMidCrossover -> get();
    LP1.setCutoffFrequency(lowMidCutoffFreq);
    HP1.setCutoffFrequency(lowMidCutoffFreq);
    
    auto midHighCutoffFreq = midHighCrossover -> get();
    AP2.setCutoffFrequency(midHighCutoffFreq);
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);
    
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




