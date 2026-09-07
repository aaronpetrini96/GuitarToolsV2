//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "../../Utils/GTLook.h"
#include "../../Utils/ParamIDs.h"
#include "PluginProcessor.h"
#include "FadersPlatePanel.h"
#include "GUI/Components/AALightStrokeKnob.h"
#include "GUI/Components/AANotchSSLKnob.h"
#include "GUI/Components/AANeveRadioButtons.h"
#include "GUI/Components/AASquareSSLButton.h"


class MainPanel : public juce::Component
{
public:
    explicit MainPanel (GuitarToolsAudioProcessor& processor);

    void paint (juce::Graphics& g) override;
    void resized() override;

    float sectionTitleH {0};

private:
    GuitarToolsAudioProcessor& audioProcessor;

    void setupLabels(juce::Label*, juce::Colour, juce::Justification);
    void drawDivider(juce::Graphics&, juce::Rectangle<float>);
    juce::Rectangle<float> cutFiltersArea, buttonsArea, shelfFiltersArea, plateBounds, magicDividerBounds;

    juce::Label cutFiltersGroupLabel {{}, "CUT FILTERS"}, magicButtonsGroupLabel {{}, "MAGIC BUTTONS"}, shelfFiltersGroupLabel {{}, "SHELF FILTERS"};
    juce::Label levelsGroupLabel {{}, "LEVELS"};
    juce::Label inputGainLabel {{}, "INPUT"}, outputGainLabel {{}, "OUTPUT"};

    AALightStrokeKnob lowCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutFreqName()), "LOW CUT"};
    AALightStrokeKnob highCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutFreqName()), "HIGH CUT"};
    AALightStrokeKnob lowShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getDepthGainName()), "DEPTH", true, true};
    AALightStrokeKnob highShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceGainName()), "PRESENCE", true, true};

    AANeveRadioButtons presenceFreqButtons {audioProcessor.treeState, juce::ParameterID (ParamIDs::getPresenceFreqName(), 1)};
    AANeveRadioButtons depthFreqButtons {audioProcessor.treeState, juce::ParameterID (ParamIDs::getDepthFreqName(), 1)};

    juce::Label resoBypassLabel{"resoBypassLabel", "KILL\nHARSH"};
    juce::Label compBypassLabel{"compBypassLabel", "TIGHTEN\nLOWS"};
    juce::Label mudBypassLabel{"mudBypassLabel", "CLEAR\nMUD"};
    juce::Label resoButtonCentreLabel, compButtonCentreLabel, mudButtonCentreLabel;

    AANeveRadioButtons lowCutSlopeButtons {audioProcessor.treeState, juce::ParameterID(ParamIDs::getLowCutSlopeName(), 1)};
    AANeveRadioButtons highCutSlopeButtons {audioProcessor.treeState, juce::ParameterID(ParamIDs::getHighCutSlopeName(), 1)};

    AASquareSSLButton resoButton {audioProcessor.treeState, juce::ParameterID(ParamIDs::getResoBypassName(), 1), 1, false};
    AASquareSSLButton mudButton {audioProcessor.treeState, juce::ParameterID(ParamIDs::getMudBypassName(), 1), 1, false};
    AASquareSSLButton compBypassButton {audioProcessor.treeState, juce::ParameterID(ParamIDs::getCompBypassName(), 1), 1, false};

    AANotchSSLKnob inputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getCompGainInName())};
    AANotchSSLKnob outputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getCompGainOutName())};

    FadersPlatePanel fadersPlatePanel {audioProcessor};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
