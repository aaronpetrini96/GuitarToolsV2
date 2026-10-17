#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

#include "BinaryData.h"
#include "PluginProcessor.h"
#include "GUI/Panels/TooltipsPanel.h"
#include "GUI/Panels/HeaderPanel.h"
#include "GUI/PopupPanels/InfoPopupPanel.h"
#include "GUI/Panels/MainPanel.h"
#include "GUI/PopupPanels/SettingsOptionsPanel.h"
#include "GUI/PopupPanels/UISizeOptionsPanel.h"
#include "GUI/PopupPanels/AAPresetBrowser.h"
#include "GUI/PopupPanels/AAPresetPopups.h"
#include "Utils/PresetManager.h"
#include "Utils/AAResizeUI.h"
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
    void componentVisibilityChanged (juce::Component& component) override;

private:
    GuitarToolsAudioProcessor& audioProcessor;
    void showSettingsOptionsPopup();
    void showUISizeOptions();
    void showPopup (juce::Component& popup);
    void updateMainPanelDarkening();
    void updateEditorSize();

    const float headerHeightRatio = 0.0575f;
    float tooltipsHeightRatio = 0.625f;
    AAResizeUI uiResize {audioProcessor.uiScale, 780, 720};
    juce::Rectangle<float> imageBounds{};

    PresetManager presetManager {audioProcessor};
    AAPresetBrowser presetBrowser {presetManager};
    AAPresetPopup presetPopup;
    juce::Component modalBlocker;
    juce::File targetPresetFile;
    HeaderPanel headerPanel {audioProcessor, presetManager};
    MainPanel mainPanel {audioProcessor};
    InfoPopupPanel infoPanel;
    TooltipsPanel tooltipsPanel;

    SettingsOptionsPanel settingsOptionsPopup {uiResize, audioProcessor.showTooltips, *audioProcessor.treeState.getParameter (ParamIDs::getLoudnessMatchActiveName()), *audioProcessor.treeState.getParameter (ParamIDs::getLowTuningActiveName())};
    UISizeOptionsPanel resizeOptionsPopup {uiResize};
    juce::DrawableButton resizeButton {"GUI Size", juce::DrawableButton::ImageFitted};
    juce::Image backgroundWithDividers = juce::ImageCache::getFromMemory (BinaryData::mainPanelBackgroundLight_png, BinaryData::mainPanelBackgroundLight_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
