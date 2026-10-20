//
// Created by Aaron Petrini on 03/09/2026.
//
#include "HeaderPanel.h"

HeaderPanel::HeaderPanel (GuitarToolsAudioProcessor& processor, PresetManager& presetManagerToUse) :
    presetBox (presetManagerToUse),
    bypassAttachment(processor.treeState, ParamIDs::getPluginBypassActiveName(), bypassButton),
    undoManager (processor.undoManager)
{
    setupIconButton (bypassButton, BinaryData::bypassIcon_svg, BinaryData::bypassIcon_svgSize);
    bypassButton.setTooltip (ParamHelpers::getTooltipForParam (ParamIDs::getPluginBypassActiveName()));
    bypassButton.setClickingTogglesState(true);
    bypassButton.onStateChange = [this] { if (onBypassChanged != nullptr) onBypassChanged(); };

    setupIconButton(settingsButton, BinaryData::settingsLines_svg, BinaryData::settingsLines_svgSize);
    settingsButton.setClickingTogglesState (true);
    settingsButton.setTooltip (ParamTooltips::getSettingsTooltip());
    settingsButton.onClick = [this] { if (onSettingsClicked != nullptr) onSettingsClicked(); };

    setupIconButton (undoButton, BinaryData::undoIcon_svg, BinaryData::undoIcon_svgSize);
    setupIconButton (redoButton, BinaryData::redoIcon_svg, BinaryData::redoIcon_svgSize);
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };
    undoButton.setTooltip (ParamTooltips::getUndoTooltip());
    redoButton.setTooltip (ParamTooltips::getRedoTooltip());

    undoManager.addChangeListener (this);
    changeListenerCallback (&undoManager);

    addAndMakeVisible (presetBox);

    setupIconButton (savePresetButton, BinaryData::saveIcon2_svg, BinaryData::saveIcon2_svgSize);
    savePresetButton.setTooltip (ParamTooltips::getSavePresetTooltip());
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
    g.fillAll(GTColors::almostBlack);
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImage (guitarToolsLogo, logoBounds, juce::RectanglePlacement::fillDestination);
}

void HeaderPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto logoH = boundsFHeight * 0.75f;
    const auto logoW = logoH * (float) guitarToolsLogo.getWidth() / (float) guitarToolsLogo.getHeight();
    logoBounds = boundsF.withSize (logoW, logoH).withCentre ({boundsFWidth * logoCentreXPOW, boundsF.getCentreY()});

    const auto presetBoxHeight = boundsFHeight * 0.6f;
    const auto presetBoxWidth = boundsFWidth * 0.45f;
    presetBoxBounds = boundsF.withSizeKeepingCentre (presetBoxWidth, presetBoxHeight);
    presetBox.setBounds (presetBoxBounds.toNearestInt());

    const auto iconButtonSize = boundsFHeight * 0.6f;
    const auto iconGap = boundsFWidth * 0.0125f;
    const auto iconButtonBounds = boundsF.withSizeKeepingCentre (iconButtonSize, iconButtonSize);

    const auto saveButtonBounds = presetBoxBounds.withSizeKeepingCentre (iconButtonSize, iconButtonSize).withRightX (presetBoxBounds.getX() + presetBoxWidth * 0.975f);
    savePresetButton.setBounds (saveButtonBounds.toNearestInt());

    const auto settingsButtonBounds = iconButtonBounds.withRightX (boundsFWidth - iconGap);
    settingsButton.setBounds (settingsButtonBounds.toNearestInt());

    const auto bypassButtonBounds = iconButtonBounds.withRightX (settingsButtonBounds.getX() - iconGap);
    bypassButton.setBounds (bypassButtonBounds.toNearestInt());

    const auto undoRedoCentreX = (presetBoxBounds.getRight() + bypassButtonBounds.getX()) * 0.5f;
    const auto undoRedoOffset = (iconButtonSize + iconGap) * 0.5f;
    undoButton.setBounds (iconButtonBounds.withCentre ({undoRedoCentreX - undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
    redoButton.setBounds (iconButtonBounds.withCentre ({undoRedoCentreX + undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
}

void HeaderPanel::mouseUp (const juce::MouseEvent& event)
{
    if (logoBounds.contains (event.position) && onInfoClicked != nullptr)
        onInfoClicked();
}

void HeaderPanel::setupIconButton (juce::DrawableButton& iconButton, const void* iconData, const int iconDataSize)
{
    addAndMakeVisible (iconButton);
    AAUITools::setIconButtonImages (iconButton, iconData, iconDataSize, GTColors::iconSVGColor, GTColors::iconNormalColor, GTColors::iconOverColor, GTColors::activeNeonBlue);
}
