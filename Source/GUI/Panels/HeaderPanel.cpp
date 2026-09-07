//
// Created by Aaron Petrini on 03/09/2026.
//
#include "HeaderPanel.h"
#include "BinaryData.h"
#include "../../Utils/GTLook.h"
#include "GUI/LAF/PresetBrowserLAF.h"

HeaderPanel::HeaderPanel (GuitarToolsAudioProcessor& processor)
    : presetManager (processor, presetBox)
{
    addAndMakeVisible (logoButton);
    addAndMakeVisible (presetBox);
    addAndMakeVisible (savePresetButton);
    addAndMakeVisible (settingsButton);
    addAndMakeVisible (infoButton);

    if (auto logo = juce::Drawable::createFromImageData (BinaryData::logo_png, BinaryData::logo_pngSize))
        logoButton.setImages (logo.get());

    if (auto saveIcon = juce::Drawable::createFromImageData (BinaryData::saveIcon_svg, BinaryData::saveIcon_svgSize))
    {
        saveIcon->replaceColour (juce::Colours::black, GTColors::clearGrey);
        savePresetButton.setImages (saveIcon.get());
    }

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

HeaderPanel::~HeaderPanel()
{
    presetBox.setLookAndFeel (nullptr);
}

void HeaderPanel::paint (juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    g.fillAll (juce::Colours::black.withAlpha (0.2f));

    const auto lineThickness = boundsFWidth * 0.002f;
    g.setColour (GTColors::dividerLinesEnd);
    g.fillRect (0.0f, boundsF.getHeight() - lineThickness, boundsFWidth, lineThickness);
}

void HeaderPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto presetBoxHeight = boundsFHeight * 0.6f;
    const auto presetBoxWidth = boundsFWidth * 0.3f;
    const auto controlGap = boundsFWidth * 0.012f;
    const auto iconButtonSize = presetBoxHeight;
    const auto logoHeight = boundsFHeight * 0.8f;
    const auto presetBoxBounds = boundsF.withSizeKeepingCentre (presetBoxWidth, presetBoxHeight);
    presetBox.setBounds (presetBoxBounds.toNearestInt());

    const auto iconButtonBounds = boundsF.withSizeKeepingCentre (iconButtonSize, iconButtonSize);
    const auto logoBounds = boundsF.withSizeKeepingCentre (logoHeight * logoAspectRatio, logoHeight).withX (controlGap);
    logoButton.setBounds (logoBounds.toNearestInt());

    savePresetButton.setBounds (iconButtonBounds.withX (presetBoxBounds.getRight() + controlGap).toNearestInt());
    const auto infoButtonBounds = iconButtonBounds.withRightX (boundsFWidth - controlGap);
    const auto settingsButtonBounds = iconButtonBounds.withRightX (infoButtonBounds.getX() - controlGap);

    settingsButton.setBounds (settingsButtonBounds.toNearestInt());
    infoButton.setBounds (infoButtonBounds.toNearestInt());
}
