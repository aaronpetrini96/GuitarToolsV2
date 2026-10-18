#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <mutex>

class PresetManager : public juce::ChangeBroadcaster
{
public:
    PresetManager (juce::AudioProcessorValueTreeState& treeStateToUse, juce::UndoManager& undoManagerToUse);

    enum class PresetType
    {
        factory,
        user
    };

    void savePreset(const juce::String& presetName);
    bool deletePreset(const juce::File& presetFile);
    bool renamePreset(const juce::File& presetFile, const juce::String& newPresetName);
    void loadPresetFile(const juce::File& presetFile);
    void loadAdjacentPreset(int indexOffset);
    juce::Array<juce::File> getPresetFiles(PresetType presetType) const;
    juce::File getUserPresetFile(const juce::String& presetName) const;
    bool isFactoryPresetName(const juce::String& presetName) const;
    juce::String getCurrentPresetName() const;
    void setCurrentPresetName (const juce::String& presetName);
    void loadDefaultPreset();

    static juce::File getPresetFolder();
    static void installFactoryPresets();

private:
    juce::AudioProcessorValueTreeState& treeState;
    juce::UndoManager& undoManager;
    juce::String currentPresetName;
    mutable std::mutex currentPresetNameMutex;

    void applyPreset (const juce::ValueTree& presetState, const juce::String& presetName);

    class PresetChangeAction : public juce::UndoableAction
    {
    public:
        PresetChangeAction (PresetManager& presetManagerToUse,
                            const juce::ValueTree& oldPresetState, const juce::String& oldPresetName,
                            const juce::ValueTree& newPresetState, const juce::String& newPresetName)
            : presetManager (presetManagerToUse), oldState (oldPresetState), newState (newPresetState), oldName (oldPresetName), newName (newPresetName)
        {
        }

        bool perform() override { presetManager.applyPreset (newState, newName); return true; }
        bool undo() override    { presetManager.applyPreset (oldState, oldName); return true; }

    private:
        PresetManager& presetManager;
        juce::ValueTree oldState, newState;
        juce::String oldName, newName;
    };
};
