#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"

class PresetManager
{
public:
    PresetManager(GuitarToolsAudioProcessor& processor, juce::ComboBox& presetBox);
    
    enum class PresetType
    {
        factory,
        user
    };

    void savePreset(const juce::String& presetName);
    bool deletePreset(const juce::File& presetFile);
    void loadPresetFile(const juce::File& presetFile);
    juce::Array<juce::File> getPresetFiles(PresetType presetType) const;
    juce::File getUserPresetFile(const juce::String& presetName) const;
    juce::String getCurrentPresetName() const;

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
    juce::Array<juce::File> presetFiles;
};
