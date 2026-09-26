//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "../../AALook.h"
#include "../../Utils/AAParamUtils.h"
#include "../Components/AAPresetBox.h"

class HeaderPanel : public juce::Component,
                    private juce::ChangeListener
{
public:
    HeaderPanel (GuitarToolsAudioProcessor& processor, AAPresetBox& presetBoxToUse);
    ~HeaderPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    bool isBypassed() const;

    std::function<void()> onInfoClicked;
    std::function<void()> onBypassChanged;
    std::function<void()> onSettingsClicked;
    std::function<void()> onSaveClicked;
    // std::function<void()> onResizeClicked;
    juce::DrawableButton settingsButton {"Settings", juce::DrawableButton::ImageFitted};
    // juce::DrawableButton resizeButton {"GUI Size", juce::DrawableButton::ImageFitted};
    juce::DrawableButton bypassButton {"Bypass", juce::DrawableButton::ImageFitted};

private:
    void setupIconButton (juce::DrawableButton& iconButton, const void* iconData, int iconDataSize);
    void setupPresetBox ();
    juce::Rectangle<float> presetBoxBoundsExtended;

    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    juce::DrawableButton undoButton {"Undo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton redoButton {"Redo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton savePresetButton {"Save", juce::DrawableButton::ImageFitted};

    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment;
    juce::UndoManager& undoManager;
    AAPresetBox& presetBox;

    juce::Image guitarToolsLogo = juce::ImageCache::getFromMemory (BinaryData::guitarTools2Logo2_png, BinaryData::guitarTools2Logo2_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
