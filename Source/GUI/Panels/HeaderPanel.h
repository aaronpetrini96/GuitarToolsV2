//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "PluginProcessor.h"
#include "Utils/PresetManager.h"
#include "BinaryData.h"
#include "../../Utils/AALook.h"
#include "../../Utils/AAParamUtils.h"
#include "GUI/LAF/PresetBrowserLAF.h"

class PresetBox : public juce::ComboBox
{
public:
    PresetBox() = default;

    std::function<void()> onShowPopup;

    void showPopup() override
    {
        hidePopup();

        if (isEnabled() && onShowPopup != nullptr)
            onShowPopup();
    }

    void mouseDown (const juce::MouseEvent& event) override
    {
        if (! event.mods.isPopupMenu())
            showPopup();
    }

    void mouseDrag (const juce::MouseEvent&) override {}
    void mouseUp (const juce::MouseEvent&) override {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBox)
};

class HeaderPanel : public juce::Component,
                    private juce::ChangeListener
{
public:
    explicit HeaderPanel (GuitarToolsAudioProcessor& processor);
    ~HeaderPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    bool isBypassed() const;

    PresetBox presetBox;
    PresetManager presetManager;
    std::function<void()> onInfoClicked;
    std::function<void()> onSavePresetClicked;
    std::function<void()> onBypassChanged;
    std::function<void()> onSettingsClicked;
    juce::DrawableButton settingsButton {"Settings", juce::DrawableButton::ImageFitted};
    juce::DrawableButton bypassButton {"Bypass", juce::DrawableButton::ImageFitted};

private:
    void setupIconButton (juce::DrawableButton& iconButton, const void* iconData, int iconDataSize);
    void setupPresetBox ();
    // void setupLogoButton ();
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    // juce::DrawableButton logoButton {"Logo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton savePresetButton {"Save", juce::DrawableButton::ImageFitted};
    juce::DrawableButton undoButton {"Undo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton redoButton {"Redo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton stateAButton {"A", juce::DrawableButton::ImageFitted};
    juce::DrawableButton stateBButton {"B", juce::DrawableButton::ImageFitted};


    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment;
    juce::UndoManager& undoManager;
    GuitarToolsAudioProcessor& audioProcessor;

    juce::Image guitarToolsLogo = juce::ImageCache::getFromMemory (BinaryData::guitarTools2Logo_png, BinaryData::guitarTools2Logo_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
