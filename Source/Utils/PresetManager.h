#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>

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

    static juce::File getPresetFolder();
    static void installFactoryPresets();

private:
    void addPresetFolderToMenu (const juce::File& folder);

    GuitarToolsAudioProcessor& audioProcessor;
    juce::ComboBox& presetBox;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Array<juce::File> presetFiles;
};
