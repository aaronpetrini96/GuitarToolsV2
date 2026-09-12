//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "PluginProcessor.h"
#include "Utils/PresetManager.h"


class HeaderPanel : public juce::Component,
                    private juce::ChangeListener
{
public:
    explicit HeaderPanel (GuitarToolsAudioProcessor& processor);
    ~HeaderPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void setSettingsButtonOn (bool shouldBeOn);
    bool isBypassed() const;

    juce::ComboBox presetBox;
    std::function<void()> onSettingsClicked;
    std::function<void()> onInfoClicked;
    std::function<void()> onBypassChanged;

private:
    void setupIconButton (juce::DrawableButton& iconButton, const void* iconData, int iconDataSize);
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    juce::DrawableButton logoButton {"Logo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton savePresetButton {"Save", juce::DrawableButton::ImageFitted};
    juce::DrawableButton undoButton {"Undo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton redoButton {"Redo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton settingsButton {"Settings", juce::DrawableButton::ImageFitted};
    juce::DrawableButton bypassButton {"Bypass", juce::DrawableButton::ImageFitted};

    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment;
    juce::UndoManager& undoManager;

    float logoAspectRatio {1.0f};

    PresetManager presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
