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
#include "GUI/PopupPanels/AAListOptionsPopup.h"
#include "Utils/PresetManager.h"
#include "GUI/Components/AAPresetBox.h"
#include "GUI/PopupPanels/AAPresetBrowser.h"
#include "GUI/PopupPanels/AAPresetPopups.h"
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

    const float headerHeightRatio = 0.0575f;
    float tooltipsHeightRatio = 0.625f;
    AAResizeUI uiResize {audioProcessor.uiScale, 780, 720};
    juce::Rectangle<float> imageBounds{};

    PresetManager presetManager {audioProcessor};
    AAPresetBox presetBox {presetManager};
    AAPresetBrowser presetBrowser {presetManager};
    AAPresetPopup savePresetPopup {AAPresetPopup::Action::save};
    AAPresetPopup deletePresetPopup {AAPresetPopup::Action::remove};
    juce::Component modalBlocker;
    juce::File presetToDelete;
    HeaderPanel headerPanel {audioProcessor, presetBox};
    MainPanel mainPanel {audioProcessor};
    InfoPanel infoPanel;
    TooltipsPanel tooltipsPanel;
    bool dismissInfoPanelOnMouseUp {false};

    SettingsOptionsPanel settingsOptionsPopup {uiResize, audioProcessor.showTooltips, *audioProcessor.treeState.getParameter (ParamIDs::getLoudnessMatchActiveName()), *audioProcessor.treeState.getParameter (ParamIDs::getGainLimitActiveName()), *audioProcessor.treeState.getParameter (ParamIDs::getLowTuningActiveName())};
    AAListOptionsPopup resizeOptionsPopup {"UI SIZE"};
    juce::DrawableButton resizeButton {"GUI Size", juce::DrawableButton::ImageFitted};
    juce::Image backgroundWithDividers = juce::ImageCache::getFromMemory (BinaryData::mainPanelBackgroundLevelsDarkNoText_png, BinaryData::mainPanelBackgroundLevelsDarkNoText_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
