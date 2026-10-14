//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "../../AALook.h"
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
    void resized() override;
    void resizeLevelsSectionCentred(const juce::Rectangle<float>&);
    void resizeKnobs(const juce::Rectangle<float>&);
    void resizeMagicButtons();

    float sectionTitleH {0.f};
    float sideSectionPOW {0.32f};

private:
    GuitarToolsAudioProcessor& audioProcessor;
    float knobSizeReference {0.f};
    const juce::StringArray levelScaleLabels {"  0", " -9", "-20", "-48"};

    const juce::Colour textWhiteColor = GTColors::almostWhite.withAlpha(0.9f);

    juce::Rectangle<float> cutFiltersArea, buttonsArea, shelfFiltersArea, plateBounds;

    AALightStrokeKnob lowCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutFreqName()), "LOW CUT", false, false};
    AALightStrokeKnob highCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutFreqName()), "HIGH CUT", false, true};
    AALightStrokeKnob lowShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getDepthGainName()), "DEPTH", true, false};
    AALightStrokeKnob highShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceGainName()), "PRESENCE", true, false};

    AANeveRadioButtons presenceFreqButtons {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceModeName()), {"A", "B", "C", "D"}};
    AANeveRadioButtons depthFreqButtons {*audioProcessor.treeState.getParameter (ParamIDs::getDepthModeName()), {"A", "B", "C", "D"}};
    AANeveRadioButtons lowCutSlopeButtons {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutSlopeName()), {"12", "24", "36", "48"}};
    AANeveRadioButtons highCutSlopeButtons {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutSlopeName()), {"12", "24", "36", "48"}};

    AAUITools::PrintedText lowCutSlopeLabel  {"SLOPE", textWhiteColor};
    AAUITools::PrintedText highCutSlopeLabel {"SLOPE", textWhiteColor};
    AAUITools::PrintedText presenceModeLabel {"MODE", textWhiteColor};
    AAUITools::PrintedText depthModeLabel    {"MODE", textWhiteColor};
    AAUITools::PrintedText levelsTitle {"LEVELS", GTColors::almostWhite};

    AASquareSSLButton resoActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getResoActiveName()), "KILL\nRESO"};
    AASquareSSLButton mudActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getMudActiveName()), "CLEAR\nMUD"};
    AASquareSSLButton compActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getCompActiveName()), "TAME\nLOWS"};

    AARectSSLButton resoModeSwitch {*audioProcessor.treeState.getParameter (ParamIDs::getResoExtremeActiveName()), "MAX"};
    AARectSSLButton compModeSwitch {*audioProcessor.treeState.getParameter (ParamIDs::getCompExtremeActiveName()), "MAX"};
    AARectSSLButton mudModeSwitch {*audioProcessor.treeState.getParameter (ParamIDs::getMudExtremeActiveName()), "MAX"};

    AANotchSSLKnob inputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getInputGainName()), "IN"};
    AANotchSSLKnob outputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getOutputGainName()), "OUT"};

    GTLevelMeter outputLevelMeter{audioProcessor.outputLevelLeft, audioProcessor.outputLevelRight, GTLevelMeter::ScalePlacement::none};
    GTLevelMeter inputLevelMeter{audioProcessor.inputLevelLeft, audioProcessor.inputLevelRight, GTLevelMeter::ScalePlacement::none};
    std::unique_ptr<juce::Drawable> levelMatchButtonOn {juce::Drawable::createFromImageData (BinaryData::matchButton_on_png, BinaryData::matchButton_on_pngSize)};
    std::unique_ptr<juce::Drawable> levelMatchButtonOff {juce::Drawable::createFromImageData (BinaryData::matchButton_off_png, BinaryData::matchButton_off_pngSize)};
    juce::DrawableButton levelMatchButton {"levelMatchButton", juce::DrawableButton::ImageFitted};
    std::unique_ptr<juce::ButtonParameterAttachment> levelMatchButtonAttachment;
    FadersPlatePanel fadersPlatePanel {audioProcessor};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
