//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "GTLook.h"
#include "PluginProcessor.h"
#include "GUI/Components/RotaryKnob.h"
#include "GUI/Components/ImgKnob.h"
#include "GUI/Components/OptionBox.h"
#include "GUI/Components/CustomButton.h"
#include "GUI/Components/HorizontalSlider.h"

class MainPanel : public juce::Component
{
public:
    explicit MainPanel (GuitarToolsAudioProcessor& processor);
    ~MainPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void paintOverChildren(Graphics& g) override;

private:
    GuitarToolsAudioProcessor& audioProcessor;

    RotaryKnob lowCutFreqKnob {"Low Cut", audioProcessor.treeState, juce::ParameterID("LowCut Freq", 1)," Hz"};
    RotaryKnob highCutFreqKnob {"High Cut", audioProcessor.treeState, juce::ParameterID("HighCut Freq", 1)," Hz"};

    RotaryKnob lowShelfGainKnob {"Depth", audioProcessor.treeState, juce::ParameterID("Depth Gain", 1), " dB", true};
    RotaryKnob highShelfGainKnob {"Presence", audioProcessor.treeState, juce::ParameterID("Presence Gain", 1), " dB", true};

    // Not linked to its own parameter yet — piggybacking on LowCut Freq just to preview the image knob in place.
    ImgKnob imgKnobPreview {audioProcessor.treeState, juce::ParameterID("LowCut Freq", 1), " Hz"};

    juce::TextButton presence1 {"1"}, presence2 {"2"}, presence3 {"3"}, depth1 {"1"}, depth2 {"2"}, depth3 {"3"};
    std::vector<juce::TextButton*> shelfFiltersButtons = {&presence1, &presence2, &presence3, &depth1, &depth2, &depth3};

    void updatePresenceButtons(const int&);
    void updateDepthButtons(const int&);
    void setShelfFilterButtonStyle(juce::TextButton&);
    float choiceCount {0}, normalizedValue{0};
    void setShelfFilterFreq(const juce::String&, const int&);

    std::vector<juce::String> slopeOptions {"12 dB/Oct", "24 dB/Oct", "36 dB/Oct", "48 dB/Oct"};
    OptionBox lowCutSlopeBox {slopeOptions, audioProcessor.treeState, juce::ParameterID("LowCut Slope", 1)};
    OptionBox highCutSlopeBox {slopeOptions, audioProcessor.treeState, juce::ParameterID("HighCut Slope", 1)};

    std::vector<juce::String> oversamplingOptions { "Off (OS)", "2x", "4x", "8x" };
    OptionBox oversamplingBox {oversamplingOptions, audioProcessor.treeState, juce::ParameterID("Oversampling",1)};

    std::vector<juce::String> ratioChoices = {"1:1","1.4:1","2:1","3:1","4:1","8:1","10:1","20:1","50:1","100:1"};
    OptionBox compRatioBox {ratioChoices, audioProcessor.treeState, juce::ParameterID("Ratio", 1)};

    CustomButton resoButton {"Tame Resonance", audioProcessor.treeState, juce::ParameterID("Reso Bypass", 1)};
    CustomButton mudButton {"Kill Mud", audioProcessor.treeState, juce::ParameterID("Mud Bypass", 1)};
    CustomButton compBypassButton {"Control Low-End", audioProcessor.treeState, juce::ParameterID("Comp Bypass",1)};

    HorizontalSlider resoFreqSlider {"Reso Freq", audioProcessor.treeState, juce::ParameterID("Reso Freq", 1), " Hz"};
    HorizontalSlider mudFreqSlider {"Mud Freq", audioProcessor.treeState, juce::ParameterID("Mud Freq", 1), " Hz"};
    HorizontalSlider compThresholdSlider {"Thresh", audioProcessor.treeState, juce::ParameterID("Threshold", 1), " dB"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
