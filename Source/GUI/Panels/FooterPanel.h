//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "../../Utils/GTLook.h"
#include "PluginProcessor.h"
#include "GUI/Components/GainSlider.h"
// #include "GUI/Components/AAImageKnobs.h"
// #include "GUI/Components/AAFaderSSL.h"
#include "../../Utils/ParamIDs.h"
#include "Utils/PresetManager.h"

class FooterPanel : public juce::Component
{
public:
    explicit FooterPanel (GuitarToolsAudioProcessor& processor);
    ~FooterPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    GainSlider inputGainSlider;
    GainSlider outputGainSlider;

private:
    GuitarToolsAudioProcessor& audioProcessor;

    juce::DrawableButton settingsButton {"Settings", juce::DrawableButton::ImageFitted};
    juce::DrawableButton infoButton {"Info", juce::DrawableButton::ImageFitted};

    // AAFullCircleFillKnob inputGainKnob {*audioProcessor.treeState.getParameter (ParamIDs::getCompGainInName()), ""};
    // AAVerticalSSLFader outputGainFader {audioProcessor.treeState, juce::ParameterID (ParamIDs::getCompGainOutName(), 1), " dB", 1};

    juce::ComboBox presetBox;
    juce::DrawableButton savePresetButton {"Save", juce::DrawableButton::ImageFitted};
    PresetManager presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FooterPanel)
};
