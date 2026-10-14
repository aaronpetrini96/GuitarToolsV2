#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../PluginProcessor.h"

class PresetManager : private juce::ChangeListener
{
public:
    explicit PresetManager(GuitarToolsAudioProcessor& processor);
    ~PresetManager() override;

    enum class PresetType
    {
        factory,
        user
    };

    void savePreset(const juce::String& presetName);
    bool deletePreset(const juce::File& presetFile);
    void loadPresetFile(const juce::File& presetFile);
    void loadAdjacentPreset(int indexOffset);
    juce::Array<juce::File> getPresetFiles(PresetType presetType) const;
    juce::File getUserPresetFile(const juce::String& presetName) const;
    bool isFactoryPresetName(const juce::String& presetName) const;
    juce::String getCurrentPresetName() const;

    static juce::File getPresetFolder();
    static void installFactoryPresets();

    std::function<void()> onPresetNameChanged;

private:
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    GuitarToolsAudioProcessor& audioProcessor;
};
