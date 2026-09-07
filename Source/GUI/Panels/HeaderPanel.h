//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "PluginProcessor.h"
#include "Utils/PresetManager.h"

class HeaderPanel : public juce::Component
{
public:
    explicit HeaderPanel (GuitarToolsAudioProcessor& processor);
    ~HeaderPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    juce::ComboBox presetBox;

private:
    juce::DrawableButton logoButton {"Logo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton savePresetButton {"Save", juce::DrawableButton::ImageFitted};
    juce::DrawableButton settingsButton {"Settings", juce::DrawableButton::ImageFitted};
    juce::DrawableButton infoButton {"Info", juce::DrawableButton::ImageFitted};
    PresetManager presetManager;

    float logoAspectRatio {1.5f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
