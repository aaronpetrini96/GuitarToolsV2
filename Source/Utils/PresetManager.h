#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

class PresetManager
{
public:
    PresetManager(GuitarToolsAudioProcessor& processor, juce::ComboBox& presetBox);
    
    void savePreset();
    void loadPreset();
    void presetSelected();
    void refreshPresetList();
    int getItemIdForText(const juce::ComboBox&, const juce::String&);
    void applyPresetSelection(const juce::String&);
    void displayCurrentPresetName();
    
private:
    juce::File getPresetFolder();
    GuitarToolsAudioProcessor& audioProcessor;
    juce::ComboBox& presetBox;
    std::unique_ptr<juce::FileChooser> fileChooser;
};
