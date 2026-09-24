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
#include "GUI/Panels/SettingsOptionsPanel.h"
#include "Utils/AAResizeUI.h"
#include "GUI/Components/AAListOptionsPopup.h"
#include "PresetFolder/PresetManager.h"
#include "PresetFolder/AAPresetBox.h"
#include "PresetFolder/AAPresetBrowser.h"
#include "PresetFolder/AAPresetPopups.h"
//==============================================================================
/**
*/
class GuitarToolsAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         private juce::ComponentListener

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
    void showSettingsOptionsPopup();
    void showUISizeOptions();
    void showPopup (juce::Component& popup);
    void updateMainPanelAlpha();
    void componentVisibilityChanged (juce::Component& component) override;
    void updateEditorSize();

    const float headerHeightRatio = 0.06f;
    float tooltipsHeightRatio = 0.55f;
    AAResizeUI uiResize {audioProcessor.uiScale, 700, 650};

    PresetManager presetManager {audioProcessor};
    AAPresetBox presetBox {presetManager};
    AAPresetBrowser presetBrowser {presetManager};
    AASavePresetPopup savePresetPopup;
    AADeletePresetPopup deletePresetPopup;
    juce::File presetToDelete;
    HeaderPanel headerPanel {audioProcessor, presetBox};
    MainPanel mainPanel {audioProcessor};
    InfoPanel infoPanel;
    TooltipsPanel tooltipsPanel;
    bool dismissInfoPanelOnMouseUp {false};

    SettingsOptionsPanel settingsOptionsPopup {uiResize, audioProcessor.showTooltips};
    AAListOptionsPopup resizeOptionsPopup;
    juce::Image metalBackground = juce::ImageCache::getFromMemory (BinaryData::uiBackgroundTexture_png, BinaryData::uiBackgroundTexture_pngSize);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
