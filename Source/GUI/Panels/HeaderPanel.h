//
// Created by Aaron Petrini on 20/08/2026.
//

#pragma once

#include "PluginProcessor.h"
#include "Utils/PresetManager.h"
#include "BinaryData.h"
#include "../../Utils/GTLook.h"
#include "../../Utils/AAParamUtils.h"
#include "GUI/LAF/PresetBrowserLAF.h"


class HeaderPanel : public juce::Component,
                    private juce::ChangeListener
{
public:
    explicit HeaderPanel (GuitarToolsAudioProcessor& processor);
    ~HeaderPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    bool isBypassed() const;

    juce::ComboBox presetBox;
    std::function<void()> onInfoClicked;
    std::function<void()> onBypassChanged;
    std::function<void()> onSettingsClicked;
    juce::DrawableButton settingsButton {"Settings", juce::DrawableButton::ImageFitted};

private:
    void setupIconButton (juce::DrawableButton& iconButton, const void* iconData, int iconDataSize);
    void setupPresetBox ();
    void setupLogoButton ();
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    juce::DrawableButton logoButton {"Logo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton savePresetButton {"Save", juce::DrawableButton::ImageFitted};
    juce::DrawableButton undoButton {"Undo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton redoButton {"Redo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton bypassButton {"Bypass", juce::DrawableButton::ImageFitted};


    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment;
    juce::UndoManager& undoManager;

    float logoGTAspectRatio {1.0f};
    std::unique_ptr<juce::Drawable> guitarsToolLogoImg  {juce::Drawable::createFromImageData(BinaryData::guitarTools2Logo_png, BinaryData::guitarTools2Logo_pngSize)};
    std::unique_ptr<juce::Drawable> guitarsToolLogoImgOver {juce::Drawable::createFromImageData(BinaryData::guitarTools2LogoOver_png, BinaryData::guitarTools2LogoOver_pngSize)};


    PresetManager presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
