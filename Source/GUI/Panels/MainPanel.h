//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "../../Utils/GTLook.h"
#include "../../Utils/ParamUtils.h"
#include "PluginProcessor.h"
#include "../Components/FadersPlateComponent.h"
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

    const juce::Colour textWhiteColor = GTColors::almostWhite.withAlpha(0.9f);
    void setupLabels(juce::Label*, juce::Colour, juce::Justification);
    void drawDivider(juce::Graphics&, juce::Rectangle<float>);
    juce::Rectangle<float> cutFiltersArea, buttonsArea, shelfFiltersArea, plateBounds, magicDividerBounds;

    juce::Label cutFiltersGroupLabel {{}, "CUT FILTERS"}, magicButtonsGroupLabel {{}, "MAGIC BUTTONS"}, shelfFiltersGroupLabel {{}, "SHELF FILTERS"};
    juce::Label inputGainLabel {{}, "IN"}, outputGainLabel {{}, "OUT"}, levelsLabel {{}, "LEVELS"}, matchLabel {{}, "MATCH"};

    AALightStrokeKnob lowCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutFreqName()), "LOW CUT", true, false, false, &audioProcessor.undoManager};
    AALightStrokeKnob highCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutFreqName()), "HIGH CUT", true, false, true, &audioProcessor.undoManager};
    AALightStrokeKnob lowShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getDepthGainName()), "DEPTH", true, true, false, &audioProcessor.undoManager};
    AALightStrokeKnob highShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceGainName()), "PRESENCE", true, true, false, &audioProcessor.undoManager};

    AANeveRadioButtons presenceFreqButtons {audioProcessor.treeState, juce::ParameterID (ParamIDs::getPresenceFreqName(), 1), {"1", "2", "3", "4"}, &audioProcessor.undoManager};
    AANeveRadioButtons depthFreqButtons {audioProcessor.treeState, juce::ParameterID (ParamIDs::getDepthFreqName(), 1), {"1", "2", "3", "4"}, &audioProcessor.undoManager};

    juce::Label resoBypassLabel{"resoBypassLabel", "KILL\nRESO"};
    juce::Label compBypassLabel{"compBypassLabel", "TIGHTEN\nLOWS"};
    juce::Label mudBypassLabel{"mudBypassLabel", "CLEAR\nMUD"};

    AANeveRadioButtons lowCutSlopeButtons {audioProcessor.treeState, juce::ParameterID (ParamIDs::getLowCutSlopeName(), 1), {"12", "24", "36", "48"}, &audioProcessor.undoManager};
    AANeveRadioButtons highCutSlopeButtons {audioProcessor.treeState, juce::ParameterID (ParamIDs::getHighCutSlopeName(), 1), {"12", "24", "36", "48"}, &audioProcessor.undoManager};
    juce::Label lowCutSlopeLabel {"lowCutSlopeLabel", "SLOPE"};
    juce::Label highCutSlopeLabel {"highCutSlopeLabel", "SLOPE"};
    juce::Label presenceModeLabel {"presenceModeLabel", "MODE"};
    juce::Label depthModeLabel {"depthModeLabel", "MODE"};

    AASquareSSLButton resoButton {audioProcessor.treeState, juce::ParameterID(ParamIDs::getResoBypassName(), 1), 1, false, &audioProcessor.undoManager};
    AASquareSSLButton mudButton {audioProcessor.treeState, juce::ParameterID(ParamIDs::getMudBypassName(), 1), 1, false, &audioProcessor.undoManager};
    AASquareSSLButton compBypassButton {audioProcessor.treeState, juce::ParameterID(ParamIDs::getCompBypassName(), 1), 1, false, &audioProcessor.undoManager};

    AANotchSSLKnob inputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getCompGainInName()), &audioProcessor.undoManager};
    AANotchSSLKnob outputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getCompGainOutName()), &audioProcessor.undoManager};
    juce::DrawableButton levelMatchButton {"Match", juce::DrawableButton::ImageFitted};
    juce::AudioProcessorValueTreeState::ButtonAttachment loudnessMatchAttachment;

    FadersPlateComponent fadersPlatePanel {audioProcessor};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
