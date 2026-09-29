//
// Created by Aaron Petrini on 03/09/2026.
//
#include "HeaderPanel.h"

HeaderPanel::HeaderPanel (GuitarToolsAudioProcessor& processor, AAPresetBox& presetBoxToUse) :
    bypassAttachment(processor.treeState, ParamIDs::getPluginBypassActiveName(), bypassButton),
    undoManager (processor.undoManager),
    presetBox (presetBoxToUse)
{
    setupIconButton (bypassButton, BinaryData::bypassIcon_svg, BinaryData::bypassIcon_svgSize);
    bypassButton.setTooltip (ParamHelpers::getTooltipForParam (ParamIDs::getPluginBypassActiveName()));
    bypassButton.setClickingTogglesState(true);
    bypassButton.onStateChange = [this] { if (onBypassChanged != nullptr) onBypassChanged(); };

    setupIconButton(settingsButton, BinaryData::settingsLines_svg, BinaryData::settingsLines_svgSize);
    settingsButton.setClickingTogglesState (true);
    settingsButton.setTooltip ("Settings: Show settings panel");
    settingsButton.onClick = [this] { if (onSettingsClicked != nullptr) onSettingsClicked(); };

    setupIconButton (undoButton, BinaryData::undoIcon_svg, BinaryData::undoIcon_svgSize);
    setupIconButton (redoButton, BinaryData::redoIcon_svg, BinaryData::redoIcon_svgSize);
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };

    undoManager.addChangeListener (this);
    changeListenerCallback (&undoManager);

    addAndMakeVisible (presetBox);

    setupIconButton (savePresetButton, BinaryData::saveIcon_svg, BinaryData::saveIcon_svgSize);
    savePresetButton.setTooltip ("Save: Save the current settings as a preset");
    savePresetButton.onClick = [this] { if (onSaveClicked != nullptr) onSaveClicked(); };
}

HeaderPanel::~HeaderPanel()
{
    undoManager.removeChangeListener (this);
}

bool HeaderPanel::isBypassed() const
{
    return bypassButton.getToggleState();
}

void HeaderPanel::changeListenerCallback (juce::ChangeBroadcaster*)
{
    undoButton.setEnabled (undoManager.canUndo());
    redoButton.setEnabled (undoManager.canRedo());
}

void HeaderPanel::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    const auto boundsF = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient::horizontal(GTColors::headerPanelStart, 0.f, GTColors::headerPanelEnd, boundsF.getWidth()));
    g.setOpacity(0.9f);
    g.fillRect(boundsF);
    const auto logoH = boundsF.getHeight() * 0.6f;
    const auto logoW = logoH * (float) guitarToolsLogo.getWidth() / (float) guitarToolsLogo.getHeight();
    const auto logoBounds = boundsF.withSizeKeepingCentre (logoW, logoH).withX(boundsF.getWidth() * 0.012f);
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.setOpacity(0.85f);
    g.drawImage (guitarToolsLogo, logoBounds, juce::RectanglePlacement::fillDestination);
}

void HeaderPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto presetBoxHeight = boundsFHeight * 0.65f;
    const auto presetBoxWidth = boundsFWidth * 0.45f;
    const auto presetBoxBounds = boundsF.withSizeKeepingCentre (presetBoxWidth, presetBoxHeight);
    presetBox.setBounds (presetBoxBounds.toNearestInt());

    const auto saveIconSize = presetBoxHeight * 0.88f;
    const auto saveButtonBounds = presetBoxBounds.withSizeKeepingCentre (saveIconSize, saveIconSize).withRightX (presetBoxBounds.getX() + presetBoxWidth * 0.975f);
    savePresetButton.setBounds (saveButtonBounds.toNearestInt());

    const auto iconButtonSize = boundsFHeight * 0.6f;
    const auto iconGap = boundsFWidth * 0.0125f;
    const auto iconButtonBounds = boundsF.withSizeKeepingCentre (iconButtonSize, iconButtonSize);
    const auto bypassButtonBounds = iconButtonBounds.withRightX (boundsFWidth - iconGap);
    settingsButton.setBounds (bypassButtonBounds.toNearestInt());
    const auto settingsButtonBounds = iconButtonBounds.withRightX (bypassButtonBounds.getX() - iconGap);
    bypassButton.setBounds (settingsButtonBounds.toNearestInt());

    const auto undoRedoCentreX = (presetBoxBounds.getRight() + settingsButtonBounds.getX()) * 0.5f;
    const auto undoRedoOffset = (iconButtonSize + iconGap) * 0.5f;
    undoButton.setBounds (iconButtonBounds.withCentre ({undoRedoCentreX - undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
    redoButton.setBounds (iconButtonBounds.withCentre ({undoRedoCentreX + undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
}

void HeaderPanel::setupIconButton (juce::DrawableButton& iconButton, const void* iconData, const int iconDataSize)
{
    addAndMakeVisible (iconButton);
    AAUITools::setIconButtonImages (iconButton, iconData, iconDataSize);
}
