/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

#include "BinaryData.h"
#include "PluginProcessor.h"
#include "GUI/Panels/FooterPanel.h"
#include "GUI/Panels/HeaderPanel.h"
#include "GUI/Panels/InfoPanel.h"
#include "GUI/Panels/MainPanel.h"
#include "GUI/Panels/SettingsPanel.h"
#include "GUI/LAF/OptionsPopupLAF.h"
//==============================================================================
/**
*/
class GuitarToolsAudioProcessorEditor  : public juce::AudioProcessorEditor

{
public:
    GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor&);
    ~GuitarToolsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
    void mouseEnter (const juce::MouseEvent& event) override;
    void mouseExit (const juce::MouseEvent& event) override;

private:
    GuitarToolsAudioProcessor& audioProcessor;

    void showUISizeMenu();
    void updateMainPanelAlpha();
    void updateEditorSize();
    void updatePanelsVisibility (bool shouldShowSettings, bool shouldShowInfo);
    const juce::StringArray uiSizeNames {"TINY", "EXTRA SMALL", "SMALL", "DEFAULT", "LARGE", "EXTRA LARGE", "HUGE"};
    const std::array<float, 7> uiScales {0.5f, 0.65f, 0.8f, 1.0f, 1.25f, 1.5f, 2.0f};

    const int defaultUIWidth = 700;
    const int defaultUIHeight = 600;
    const float headerHeightRatio = 0.07f;
    float footerHeightRatio = 0.5f;

    HeaderPanel headerPanel {audioProcessor};
    MainPanel mainPanel {audioProcessor};
    SettingsPanel settingsPanel;
    InfoPanel infoPanel;
    FooterPanel footerPanel;
    bool dismissInfoPanelOnMouseUp {false};

    OptionsPopupLAF resizeButtonLAF;
    juce::DrawableButton resizeButton {"GUI Size", juce::DrawableButton::ImageFitted};
    std::unique_ptr<juce::Drawable> metalBackground =  juce::Drawable::createFromImageData(BinaryData::uiBackgroundTexture_png, BinaryData::uiBackgroundTexture_pngSize);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
