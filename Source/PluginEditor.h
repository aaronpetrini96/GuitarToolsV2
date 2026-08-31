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
#include "GUI/LAF/LookAndFeel.h"
#include "GUI/Panels/FooterPanel.h"
#include "GUI/Panels/HeaderPanel.h"
#include "GUI/Panels/MainPanel.h"
//==============================================================================
/**
*/
class GuitarToolsAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer

{
public:
    GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor&);
    ~GuitarToolsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void paintOverChildren (juce::Graphics& g) override;
    bool keyPressed (const juce::KeyPress& key) override;

private:
    GuitarToolsAudioProcessor& audioProcessor;

    const int minUIWidth = 640;
    const int minUIHeight = 480;

    HeaderPanel headerPanel {audioProcessor};
    MainPanel mainPanel {audioProcessor};
    FooterPanel footerPanel {audioProcessor};
    std::unique_ptr<juce::Drawable> metalBackground =  juce::Drawable::createFromImageData(BinaryData::chassisV2_png, BinaryData::chassisV2_pngSize);

   #if JUCE_DEBUG
    std::unique_ptr<juce::Drawable> draft = juce::Drawable::createFromImageData (BinaryData::draftUI_png, BinaryData::draftUI_pngSize);
    float drawDraft { 0.0f };
   #endif

    void timerCallback() override;

    MainLookAndFeel mainLF;

    int clipLightHoldCounter = 0; // counts down in timer ticks


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
