//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "../../Utils/GTLook.h"
#include "../../Utils/AAParamUtils.h"
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

    const juce::Colour textWhiteColor = GTColors::almostWhite.withAlpha(0.9f);
    void setupLabels(juce::Label*, juce::Colour, juce::Justification);
    juce::Rectangle<float> cutFiltersArea, buttonsArea, shelfFiltersArea, plateBounds, magicDividerBounds;

    juce::Label cutFiltersGroupLabel {{}, "TONE RANGE"}, magicButtonsGroupLabel {{}, "CLEAN-UP"}, shelfFiltersGroupLabel {{}, "COLOR"};
    juce::Label inputGainLabel {{}, "IN"}, outputGainLabel {{}, "OUT"}, levelsLabel {{}, "LEVELS"}, matchLabel {{}, "MATCH"};

    AALightStrokeKnob lowCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutFreqName()), "LOW CUT", true, false, false};
    AALightStrokeKnob highCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutFreqName()), "HIGH CUT", true, false, true};
    AALightStrokeKnob lowShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getDepthGainName()), "DEPTH", true, true, false};
    AALightStrokeKnob highShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceGainName()), "PRESENCE", true, true, false};

    AANeveRadioButtons presenceFreqButtons {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceModeName()), {"A", "B", "C", "D"}};
    AANeveRadioButtons depthFreqButtons {*audioProcessor.treeState.getParameter (ParamIDs::getDepthModeName()), {"A", "B", "C", "D"}};

    juce::Label resoBypassLabel{"resoBypassLabel", "KILL\nRESO"};
    juce::Label compBypassLabel{"compBypassLabel", "TAME\nLOWS"};
    juce::Label mudBypassLabel{"mudBypassLabel", "CLEAR\nMUD"};

    AANeveRadioButtons lowCutSlopeButtons {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutSlopeName()), {"12", "24", "36", "48"}};
    AANeveRadioButtons highCutSlopeButtons {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutSlopeName()), {"12", "24", "36", "48"}};
    juce::Label lowCutSlopeLabel {"lowCutSlopeLabel", "AGGRESSION"};
    juce::Label highCutSlopeLabel {"highCutSlopeLabel", "AGGRESSION"};
    juce::Label presenceModeLabel {"presenceModeLabel", "MODE"};
    juce::Label depthModeLabel {"depthModeLabel", "MODE"};

    AASquareSSLButton resoActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getResoActiveName()), resoBypassLabel.getText()};
    AASquareSSLButton mudActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getMudActiveName()), mudBypassLabel.getText()};
    AASquareSSLButton compActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getCompActiveName()), compBypassLabel.getText()};

    AANotchSSLKnob inputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getInputGainName()), &inputGainLabel};
    AANotchSSLKnob outputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getOutputGainName()), &outputGainLabel};
    juce::DrawableButton levelMatchButton {"Match", juce::DrawableButton::ImageFitted};
    juce::AudioProcessorValueTreeState::ButtonAttachment loudnessMatchAttachment{audioProcessor.treeState, ParamIDs::getLoudnessMatchActiveName(), levelMatchButton};

    FadersPlatePanel fadersPlatePanel {audioProcessor};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
