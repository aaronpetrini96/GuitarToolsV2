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
    float logoCentreXPOW {0.16f};

private:
    void setupIconButton (juce::DrawableButton& iconButton, const void* iconData, int iconDataSize);
    void setupPresetBox ();

    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    juce::DrawableButton undoButton {"Undo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton redoButton {"Redo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton savePresetButton {"Save", juce::DrawableButton::ImageFitted};

    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment;
    juce::UndoManager& undoManager;
    AAPresetBox& presetBox;

    juce::Image guitarToolsLogo = juce::ImageCache::getFromMemory (BinaryData::GTLogoV2Shadow_png, BinaryData::GTLogoV2Shadow_pngSize);
    juce::Rectangle<float> presetBoxBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
