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
#include "GUI/Panels/TooltipsPanel.h"
#include "GUI/Panels/HeaderPanel.h"
#include "GUI/Panels/InfoPanel.h"
#include "GUI/Panels/MainPanel.h"
// #include "GUI/Panels/SettingsPanel.h"
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
    void showSettingsMenu(juce::DrawableButton& button);
    void updateMainPanelAlpha();
    void updateEditorSize();
    const juce::StringArray uiSizeNames {"X-SMALL", "SMALL", "DEFAULT", "LARGE", "X-LARGE", "HUGE"};
    const std::array<float, 7> uiScales {0.6f, 0.8f, 1.0f, 1.2f, 1.4f, 1.75f};

    const int defaultUIWidth = 770;
    const int defaultUIHeight = 660;
    const float headerHeightRatio = 0.065f;
    float tooltipsHeightRatio = 0.5f;

    HeaderPanel headerPanel {audioProcessor};
    MainPanel mainPanel {audioProcessor};
    InfoPanel infoPanel;
    TooltipsPanel tooltipsPanel;
    bool dismissInfoPanelOnMouseUp {false};

    OptionsPopupLAF settingsMenuLAF;
    std::unique_ptr<juce::Drawable> metalBackground =  juce::Drawable::createFromImageData(BinaryData::uiBackgroundTexture_png, BinaryData::uiBackgroundTexture_pngSize);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
