#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../PluginProcessor.h"

class PresetManager
{
public:
    explicit PresetManager(GuitarToolsAudioProcessor& processor);
    
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

    static juce::File getPresetFolder();
    static void installFactoryPresets();

    std::function<void()> onPresetNameChanged;

private:
    GuitarToolsAudioProcessor& audioProcessor;
};
