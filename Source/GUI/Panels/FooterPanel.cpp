#include "FooterPanel.h"
#include "GUI/LAF/LookAndFeel.h"

FooterPanel::FooterPanel (GuitarToolsAudioProcessor& processor)
    : bypassButton ("Bypass Button", processor.treeState, juce::ParameterID("Plugin Bypass", 1)),
      inputGainSlider ("Input Gain", processor.treeState, juce::ParameterID("Comp Gain In", 1), " dB [IN]", juce::Slider::TextEntryBoxPosition::TextBoxRight),
      outputGainSlider ("Output Gain", processor.treeState, juce::ParameterID("Comp Gain Out", 1), " dB [OUT]", juce::Slider::TextEntryBoxPosition::TextBoxLeft),
      audioProcessor (processor),
      presetManager (processor, presetBox)
{
    addAndMakeVisible(presetBox);
    addAndMakeVisible(savePresetButton);
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(outputGainSlider);

    savePresetButton.setClickingTogglesState(true);
    savePresetButton.setColour(juce::TextButton::ColourIds::buttonColourId, GTColors::controlBackground);
    savePresetButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, GTColors::controlBackground);
    savePresetButton.setColour(juce::ComboBox::outlineColourId, GTColors::transparent);
    savePresetButton.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    savePresetButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
    savePresetButton.onClick = [this] { presetManager.savePreset(); };

    presetBox.setTextWhenNothingSelected("Select Preset");
    presetBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, GTColors::controlBackground);
    presetBox.setColour(juce::ComboBox::ColourIds::outlineColourId, GTColors::transparent);
    presetBox.setLookAndFeel(ComboBoxLookAndFeel::get());
    presetBox.onChange = [this] { presetManager.presetSelected(); };
    presetManager.refreshPresetList();
    presetManager.displayCurrentPresetName();
}

FooterPanel::~FooterPanel()
{
    presetBox.setLookAndFeel(nullptr);
}

void FooterPanel::paint (juce::Graphics& g)
{
    g.fillAll(GTColors::footerBackground);
}

void FooterPanel::resized()
{
    auto bounds = getLocalBounds();
    auto boundsWidth = bounds.getWidth();
    auto boundsHeight = bounds.getHeight();

    auto centreY = [boundsHeight] (int componentHeight) { return (boundsHeight - componentHeight) / 2; };

    bypassButton.setBounds(boundsWidth * 0.93f, centreY(bypassButton.getHeight()), bypassButton.getWidth(), bypassButton.getHeight());

    presetBox.setBounds(boundsWidth * 0.31f, centreY(20), 150 * 0.8f, 20);
    savePresetButton.setBounds(presetBox.getRight() * 1.05f, centreY(presetBox.getHeight()), presetBox.getWidth() * 0.5f, presetBox.getHeight());

    inputGainSlider.setBounds(8, centreY(inputGainSlider.getHeight()), inputGainSlider.getWidth(), inputGainSlider.getHeight());
    outputGainSlider.setBounds(boundsWidth * 0.585f, centreY(outputGainSlider.getHeight()), outputGainSlider.getWidth(), outputGainSlider.getHeight());
}
