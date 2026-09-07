#include "FooterPanel.h"
#include "BinaryData.h"
#include "GUI/LAF/PresetBrowserLAF.h"


FooterPanel::FooterPanel (GuitarToolsAudioProcessor& processor)
    : inputGainSlider ("Input Gain", processor.treeState, juce::ParameterID(ParamIDs::getCompGainInName(), 1), " dB [IN]", juce::Slider::TextEntryBoxPosition::TextBoxRight),
      outputGainSlider ("Output Gain", processor.treeState, juce::ParameterID(ParamIDs::getCompGainOutName(), 1), " dB [OUT]", juce::Slider::TextEntryBoxPosition::TextBoxLeft),
      audioProcessor (processor),
      presetManager (processor, presetBox)
{
    // addAndMakeVisible(inputGainKnob);
    // outputGainFader.setSliderStyle (juce::Slider::LinearHorizontal);
    // addAndMakeVisible(outputGainFader);
    addAndMakeVisible(settingsButton);
    addAndMakeVisible(infoButton);
    addAndMakeVisible(presetBox);
    addAndMakeVisible(savePresetButton);
    // addAndMakeVisible(inputGainSlider);
    // addAndMakeVisible(outputGainSlider);

    if (auto settingsIcon = juce::Drawable::createFromImageData (BinaryData::settings_svg, BinaryData::settings_svgSize))
    {
        settingsIcon->replaceColour (juce::Colours::black, GTColors::clearGrey);
        settingsButton.setImages (settingsIcon.get());
    }

    if (auto infoIcon = juce::Drawable::createFromImageData (BinaryData::info_svg, BinaryData::info_svgSize))
    {
        infoIcon->replaceColour (juce::Colours::black, GTColors::clearGrey);
        infoButton.setImages (infoIcon.get());
    }

    if (auto saveIcon = juce::Drawable::createFromImageData (BinaryData::saveIcon_svg, BinaryData::saveIcon_svgSize))
    {
        saveIcon->replaceColour (juce::Colours::black, GTColors::clearGrey);
        savePresetButton.setImages (saveIcon.get());
    }

    savePresetButton.onClick = [this] { presetManager.savePreset(); };

    presetBox.setTextWhenNothingSelected ("DEFAULT");
    presetBox.setColour (juce::ComboBox::backgroundColourId, GTColors::controlBackground);
    presetBox.setColour (juce::ComboBox::textColourId, GTColors::clearGrey);
    presetBox.setColour (juce::ComboBox::arrowColourId, GTColors::clearGrey);
    presetBox.setColour (juce::ComboBox::outlineColourId, GTColors::transparent);
    presetBox.setLookAndFeel (PresetBrowserLAF::get());
    presetBox.onChange = [this] { presetManager.presetSelected(); };

    presetManager.refreshPresetList();
    presetManager.displayCurrentPresetName();
}

FooterPanel::~FooterPanel()
{
    presetBox.setLookAndFeel (nullptr);
}

void FooterPanel::paint (juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    g.fillAll(juce::Colours::black.withAlpha(0.2f));
    const auto boundsFWidth = boundsF.getWidth();
    const auto lineThickness = boundsFWidth * 0.004f;
    g.setColour (GTColors::dividerLinesEnd);
    g.fillRect (0.0f, 0.f, boundsFWidth, lineThickness);
}

void FooterPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();
    const auto controlHeight = boundsFHeight * 0.5f;
    const auto controlGap = boundsFWidth * 0.012f;

    const auto iconButtonBounds = boundsF.withSizeKeepingCentre (controlHeight, controlHeight);
    const auto infoButtonBounds = iconButtonBounds.withRightX (boundsFWidth - controlGap);
    const auto settingsButtonBounds = iconButtonBounds.withRightX (infoButtonBounds.getX() - controlGap);

    settingsButton.setBounds (settingsButtonBounds.toNearestInt());
    infoButton.setBounds (infoButtonBounds.toNearestInt());

    const auto presetBoxBounds = boundsF.withSizeKeepingCentre (boundsFWidth * 0.3f, boundsFHeight * 0.425f);
    presetBox.setBounds (presetBoxBounds.toNearestInt());
    savePresetButton.setBounds (iconButtonBounds.withX (presetBoxBounds.getRight() + controlGap).toNearestInt());

    // outputGainFader.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.3f, boundsFHeight * 0.6f).toNearestInt());

    // const auto gainKnobSize = boundsFHeight;
    // inputGainKnob.setBounds (boundsF.withSizeKeepingCentre (gainKnobSize, gainKnobSize)
    //                                 .withX (controlGap).toNearestInt());
}
