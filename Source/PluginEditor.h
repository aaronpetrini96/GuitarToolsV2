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
#include "GUI/Panels/MainPanel.h"
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

private:
    GuitarToolsAudioProcessor& audioProcessor;

    const int defaultUIWidth = 700;
    const int defaultUIHeight = 600;

    HeaderPanel headerPanel {audioProcessor};
    MainPanel mainPanel {audioProcessor};
    FooterPanel footerPanel {audioProcessor};
    // std::unique_ptr<juce::Drawable> metalBackground =  juce::Drawable::createFromImageData(BinaryData::chassisV2_png, BinaryData::chassisV2_pngSize);
    std::unique_ptr<juce::Drawable> metalBackground =  juce::Drawable::createFromImageData(BinaryData::uiBackgroundTexture_png, BinaryData::uiBackgroundTexture_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
