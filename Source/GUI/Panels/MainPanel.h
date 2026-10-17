//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "../../GTLook.h"
#include "../../Utils/AAParamUtils.h"
#include "PluginProcessor.h"
#include "FadersPlatePanel.h"
#include "GUI/Components/AALightStrokeKnob.h"
#include "GUI/Components/AANotchSSLKnob.h"
#include "GUI/Components/AANeveRadioButtons.h"
#include "GUI/Components/AASquareSSLButton.h"
#include "GUI/Components/AARectSSLButton.h"
#include "GUI/Components/GTLevelMeter.h"


class MainPanel : public juce::Component
{
public:
    explicit MainPanel (GuitarToolsAudioProcessor& processor);

    void paint (juce::Graphics& g) override;
    void paintOverChildren(juce::Graphics& g) override;
    void resized() override;

    bool shouldDim {false};
    float sectionTitleH {0.f};
    float sideSectionPOW {0.32f};

private:
    void resizeLevelsSectionCentred(const juce::Rectangle<float>&);
    void resizeKnobs(const juce::Rectangle<float>&);
    void resizeMagicButtons();

    GuitarToolsAudioProcessor& audioProcessor;
    float knobSizeReference {0.f};
    const juce::StringArray levelScaleLabels {"CLIP", "-6", "-12", "-18", "-24", "-48"};
    juce::Image grainTile = juce::ImageCache::getFromMemory (BinaryData::inkGrainTile_png, BinaryData::inkGrainTile_pngSize);


    juce::Rectangle<float> cutFiltersArea, buttonsArea, shelfFiltersArea, plateBounds;

    AALightStrokeKnob lowCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutFreqName()), "LOW CUT", false, false};
    AALightStrokeKnob highCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutFreqName()), "HIGH CUT", false, true};
    AALightStrokeKnob lowShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getDepthGainName()), "DEPTH", true, false};
    AALightStrokeKnob highShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceGainName()), "PRESENCE", true, false};

    AANeveRadioButtons presenceFreqButtons {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceModeName()), {"A", "B", "C", "D"}};
    AANeveRadioButtons depthFreqButtons {*audioProcessor.treeState.getParameter (ParamIDs::getDepthModeName()), {"A", "B", "C", "D"}};
    AANeveRadioButtons lowCutSlopeButtons {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutSlopeName()), {"12", "24", "36", "48"}};
    AANeveRadioButtons highCutSlopeButtons {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutSlopeName()), {"12", "24", "36", "48"}};

    AAUITools::PrintedText lowCutSlopeLabel  {"SLOPE", GTColors::almostWhite}, highCutSlopeLabel {"SLOPE", GTColors::almostWhite};
    AAUITools::PrintedText presenceModeLabel {"MODE", GTColors::almostWhite}, depthModeLabel {"MODE", GTColors::almostWhite};

    AASquareSSLButton resoActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getResoActiveName()), "KILL\nRESO"};
    AASquareSSLButton mudActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getMudActiveName()), "CLEAR\nMUD"};
    AASquareSSLButton compActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getCompActiveName()), "TAME\nLOWS"};
    AARectSSLButton resoModeSwitch {*audioProcessor.treeState.getParameter (ParamIDs::getResoExtremeActiveName()), "MAX"};
    AARectSSLButton compModeSwitch {*audioProcessor.treeState.getParameter (ParamIDs::getCompExtremeActiveName()), "MAX"};
    AARectSSLButton mudModeSwitch {*audioProcessor.treeState.getParameter (ParamIDs::getMudExtremeActiveName()), "MAX"};
    FadersPlatePanel fadersPlatePanel {audioProcessor};

    AAUITools::PrintedText inputLevelTitle {"INPUT", GTColors::almostWhite};
    AAUITools::PrintedText outputLevelTitle {"OUTPUT", GTColors::almostWhite};

    AANotchSSLKnob inputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getInputGainName())};
    AANotchSSLKnob outputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getOutputGainName())};

    GTLevelMeter outputLevelMeter{audioProcessor.outputLevelLeft, audioProcessor.outputLevelRight, GTLevelMeter::ScalePlacement::none};
    GTLevelMeter inputLevelMeter{audioProcessor.inputLevelLeft, audioProcessor.inputLevelRight, GTLevelMeter::ScalePlacement::none};
    std::unique_ptr<juce::ParameterAttachment> levelMatchAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
