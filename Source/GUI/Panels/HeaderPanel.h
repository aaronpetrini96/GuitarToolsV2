//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "../../GTLook.h"
#include "../../Utils/AAParamUtils.h"
#include "../Components/AAPresetBox.h"

class HeaderPanel : public juce::Component,
                    private juce::ChangeListener
{
public:
    HeaderPanel (GuitarToolsAudioProcessor& processor, PresetManager& presetManagerToUse);
    ~HeaderPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseUp (const juce::MouseEvent& event) override;

    bool isBypassed() const;

    std::function<void()> onInfoClicked;
    std::function<void()> onBypassChanged;
    std::function<void()> onSettingsClicked;
    std::function<void()> onSaveClicked;
    juce::DrawableButton settingsButton {"Settings", juce::DrawableButton::ImageFitted};
    AAPresetBox presetBox;
    juce::DrawableButton bypassButton {"Bypass", juce::DrawableButton::ImageFitted};

private:
    void setupIconButton (juce::DrawableButton& iconButton, const void* iconData, int iconDataSize);

    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    juce::DrawableButton undoButton {"Undo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton redoButton {"Redo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton savePresetButton {"Save", juce::DrawableButton::ImageFitted};

    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment;
    juce::UndoManager& undoManager;

    juce::Image guitarToolsLogo = juce::ImageCache::getFromMemory (BinaryData::GTLogoV2Shadow_png, BinaryData::GTLogoV2Shadow_pngSize);
    juce::Rectangle<float> logoBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
