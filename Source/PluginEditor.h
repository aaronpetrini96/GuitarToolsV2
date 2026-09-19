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
#include "GUI/Components/AAPresetBrowser.h"
#include "GUI/Components/AAPresetPopups.h"
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
    void showPresetBrowser();
    void updateMainPanelAlpha();
    void componentVisibilityChanged (juce::Component& component) override;
    void updateEditorSize();

    const float headerHeightRatio = 0.065f;
    float tooltipsHeightRatio = 0.55f;
    AAResizeUI uiResize {audioProcessor.uiScale, 770, 660};

    HeaderPanel headerPanel {audioProcessor};
    MainPanel mainPanel {audioProcessor};
    InfoPanel infoPanel;
    TooltipsPanel tooltipsPanel;
    bool dismissInfoPanelOnMouseUp {false};

    SettingsOptionsPanel settingsOptionsPopup {uiResize, audioProcessor.showTooltips};
    AAListOptionsPopup sizeOptionsPopup;
    AAPresetBrowser presetBrowser {headerPanel.presetManager};
    AASavePresetPopup savePresetPopup;
    AADeletePresetPopup deletePresetPopup;
    juce::File presetToDelete;
    juce::DrawableButton resizeButton {"GUI Size", juce::DrawableButton::ImageFitted};
    juce::Image metalBackground = juce::ImageCache::getFromMemory (BinaryData::uiBackgroundTexture_png, BinaryData::uiBackgroundTexture_pngSize);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
