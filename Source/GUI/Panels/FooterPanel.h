//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "GTLook.h"
#include "PluginProcessor.h"
#include "GUI/Components/GainSlider.h"
#include "GUI/Components/ImgButton.h"
#include "Utils/PresetManager.h"

class FooterPanel : public juce::Component
{
public:
    explicit FooterPanel (GuitarToolsAudioProcessor& processor);
    ~FooterPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    juce::ComboBox presetBox;
    juce::TextButton savePresetButton {"Save"};
    GainSlider inputGainSlider;
    GainSlider outputGainSlider;

private:
    GuitarToolsAudioProcessor& audioProcessor;
    PresetManager presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FooterPanel)
};
