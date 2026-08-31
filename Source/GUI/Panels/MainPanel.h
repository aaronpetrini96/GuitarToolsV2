//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "GTLook.h"
#include "ParamIDs.h"
#include "PluginProcessor.h"
#include "GUI/Components/AAFaderSSL.h"
#include "GUI/Components/AAImgKnob.h"
#include "GUI/Components/AANeveStyleRadioButtons.h"
#include "GUI/Components/AASquareSSLButton.h"

class MainPanel : public juce::Component
{
public:
    explicit MainPanel (GuitarToolsAudioProcessor& processor);


    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    GuitarToolsAudioProcessor& audioProcessor;

    juce::Rectangle<float> cutFiltersArea, buttonsArea, shelfFiltersArea;
    float sectionTitleH {0};

    juce::Label cutFiltersGroupLabel {{}, "CUT FILTERS"}, magicButtonsGroupLabel {{}, "MAGIC BUTTONS"}, shelfFiltersGroupLabel {{}, "SHELF FILTERS"};

    AAImgKnob lowCutFreqKnob {audioProcessor.treeState, juce::ParameterID(ParamIDs::getLowCutFreqName(), 1), "LOW CUT", "Hz"};
    AAImgKnob highCutFreqKnob {audioProcessor.treeState, juce::ParameterID(ParamIDs::getHighCutFreqName(), 1), "HIGH CUT", "Hz", false, true};
    AAImgKnob lowShelfGainKnob {audioProcessor.treeState, juce::ParameterID(ParamIDs::getDepthGainName(), 1), "DEPTH", "dB", true};
    AAImgKnob highShelfGainKnob {audioProcessor.treeState, juce::ParameterID(ParamIDs::getPresenceGainName(), 1), "PRESENCE", "dB", true};

    AASquareSSLButton presenceFreqButtons {audioProcessor.treeState, juce::ParameterID(ParamIDs::getPresenceFreqName(), 1), 3};
    AASquareSSLButton depthFreqButtons {audioProcessor.treeState, juce::ParameterID(ParamIDs::getDepthFreqName(), 1), 3};

    juce::Label resoBypassLabel{"TAME RESO", "TAME RESO"};
    juce::Label compBypassLabel{"LOW CONTROL ", "LOW CONTROL "};
    juce::Label mudBypassLabel{"MUD KILL", "KILL MUD"};

    AANeveStyleRadioButtons lowCutSlopeButtons {audioProcessor.treeState, juce::ParameterID(ParamIDs::getLowCutSlopeName(), 1)};
    AANeveStyleRadioButtons highCutSlopeButtons {audioProcessor.treeState, juce::ParameterID(ParamIDs::getHighCutSlopeName(), 1)};

    AASquareSSLButton resoButton {audioProcessor.treeState, juce::ParameterID(ParamIDs::getResoBypassName(), 1), 1, false};
    AASquareSSLButton mudButton {audioProcessor.treeState, juce::ParameterID(ParamIDs::getMudBypassName(), 1), 1, false};
    AASquareSSLButton compBypassButton {audioProcessor.treeState, juce::ParameterID(ParamIDs::getCompBypassName(), 1), 1, false};

    juce::Label slope1Label {"slope1Label", "SLOPE"};

    AAFaderSSL resoFreqFader {audioProcessor.treeState, juce::ParameterID(ParamIDs::getResoFreqName(), 1), "Hz"};
    AAFaderSSL mudFreqFader {audioProcessor.treeState, juce::ParameterID(ParamIDs::getMudFreqName(), 1), "Hz"};
    AAFaderSSL compThresholdFader {audioProcessor.treeState, juce::ParameterID(ParamIDs::getThresholdName(), 1), "dB", 1};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
