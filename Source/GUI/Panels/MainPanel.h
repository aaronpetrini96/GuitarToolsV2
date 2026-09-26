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
#include "GUI/Components/AAIconAndTextButton.h"
#include "GUI/Components/AAImgSwitch.h"


class MainPanel : public juce::Component
{
public:
    explicit MainPanel (GuitarToolsAudioProcessor& processor);

    void paint (juce::Graphics& g) override;
    void resized() override;
    void resizeLevelsSection(const juce::Rectangle<float>&);
    void resizeKnobs(const juce::Rectangle<float>&);
    void resizeMagicButtons(const juce::Rectangle<float>&);

    float sectionTitleH {0};
    float dividerThickness{0.f};
    float sideSectionPOW {0.32f};

private:
    GuitarToolsAudioProcessor& audioProcessor;

    const juce::Colour textWhiteColor = GTColors::almostWhite.withAlpha(0.9f);
    void setupLabels(juce::Label*, juce::Colour, juce::Justification);

    juce::Rectangle<float> cutFiltersArea, buttonsArea, shelfFiltersArea, plateBounds, magicDividerBounds;
    juce::Label cutFiltersGroupLabel {{}, "TONE RANGE"}, magicButtonsGroupLabel {{}, "CLEAN-UP"}, shelfFiltersGroupLabel {{}, "COLOR"};

    AALightStrokeKnob lowCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutFreqName()), "LOW CUT", false, false};
    AALightStrokeKnob highCutFreqKnob {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutFreqName()), "HIGH CUT", false, true};
    AALightStrokeKnob lowShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getDepthGainName()), "DEPTH", true, false};
    AALightStrokeKnob highShelfGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceGainName()), "PRESENCE", true, false};

    AANeveRadioButtons presenceFreqButtons {*audioProcessor.treeState.getParameter (ParamIDs::getPresenceModeName()), {"A", "B", "C", "D"}};
    AANeveRadioButtons depthFreqButtons {*audioProcessor.treeState.getParameter (ParamIDs::getDepthModeName()), {"A", "B", "C", "D"}};
    AANeveRadioButtons lowCutSlopeButtons {*audioProcessor.treeState.getParameter (ParamIDs::getLowCutSlopeName()), {"12", "24", "36", "48"}};
    AANeveRadioButtons highCutSlopeButtons {*audioProcessor.treeState.getParameter (ParamIDs::getHighCutSlopeName()), {"12", "24", "36", "48"}};

    juce::Label lowCutSlopeLabel {"lowCutSlopeLabel", "SLOPE"};
    juce::Label highCutSlopeLabel {"highCutSlopeLabel", "SLOPE"};
    juce::Label presenceModeLabel {"presenceModeLabel", "MODE"};
    juce::Label depthModeLabel {"depthModeLabel", "MODE"};

    juce::Label resoBypassLabel{"resoBypassLabel", "KILL\nRESO"};
    juce::Label compBypassLabel{"compBypassLabel", "TAME\nLOWS"};
    juce::Label mudBypassLabel{"mudBypassLabel", "CLEAR\nMUD"};
    AASquareSSLButton resoActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getResoActiveName()), resoBypassLabel.getText()};
    AASquareSSLButton mudActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getMudActiveName()), mudBypassLabel.getText()};
    AASquareSSLButton compActiveButton {*audioProcessor.treeState.getParameter (ParamIDs::getCompActiveName()), compBypassLabel.getText()};

    AAImgSwitch resoModeSwitch {*audioProcessor.treeState.getParameter (ParamIDs::getResoExtremeActiveName())};
    AAImgSwitch compModeSwitch {*audioProcessor.treeState.getParameter (ParamIDs::getCompExtremeActiveName())};
    AAImgSwitch mudModeSwitch {*audioProcessor.treeState.getParameter (ParamIDs::getMudExtremeActiveName())};

    juce::Label inputGainLabel {{}, "IN"};
    juce::Label outputGainLabel {{}, "OUT"};
    AANotchSSLKnob inputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getInputGainName()), &inputGainLabel};
    AANotchSSLKnob outputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getOutputGainName()), &outputGainLabel};
    juce::Label levelsLabel {{}, "LEVELS"};

    AAIconAndTextButton levelMatchControl {*audioProcessor.treeState.getParameter (ParamIDs::getLoudnessMatchActiveName()), BinaryData::linkON_svg, BinaryData::linkON_svgSize, "MATCH", "MATCHED"};

    FadersPlatePanel fadersPlatePanel {audioProcessor};
    // juce::Image screwA = juce::ImageCache::getFromMemory (BinaryData::allenKeyScrewDarkGrey_png, BinaryData::allenKeyScrewDarkGrey_pngSize);
    // juce::Image screw = juce::ImageCache::getFromMemory (BinaryData::screwDarkGrey_png, BinaryData::screwDarkGrey_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
