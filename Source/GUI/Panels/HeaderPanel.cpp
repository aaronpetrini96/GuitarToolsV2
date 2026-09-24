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
    settingsButton.setTooltip("Show settings panel");
    settingsButton.onClick = [this] { if (onSettingsClicked != nullptr) onSettingsClicked(); };

    setupIconButton (resizeButton, BinaryData::resizeIcon2_svg, BinaryData::resizeIcon2_svgSize);
    resizeButton.setToggleable (true);
    resizeButton.setClickingTogglesState (false);
    resizeButton.onClick = [this] { if (onResizeClicked != nullptr) onResizeClicked(); };

    setupIconButton (undoButton, BinaryData::undoIcon_svg, BinaryData::undoIcon_svgSize);
    setupIconButton (redoButton, BinaryData::redoIcon_svg, BinaryData::redoIcon_svgSize);
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };

    undoManager.addChangeListener (this);
    changeListenerCallback (&undoManager);

    addAndMakeVisible (presetBox);

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
    const auto boundsF = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient::horizontal(GTColors::headerPanelStart, 0.f, GTColors::headerPanelEnd.darker(0.1f), boundsF.getWidth()));
    g.fillRect(boundsF);

    const auto logoH = boundsF.getHeight() * 0.5f;
    const auto logoW = logoH * (float) guitarToolsLogo.getWidth() / (float) guitarToolsLogo.getHeight();
    const auto logoBounds = boundsF.withSizeKeepingCentre (logoW, logoH).withX(boundsF.getWidth() * 0.012f);
    g.setOpacity(0.9f);
    g.drawImage (guitarToolsLogo, logoBounds, juce::RectanglePlacement::fillDestination);

}

void HeaderPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto presetBoxHeight = boundsFHeight * 0.6f;
    const auto presetBoxWidth = boundsFWidth * 0.45f;
    const auto presetBoxBounds = boundsF.withSizeKeepingCentre (presetBoxWidth, presetBoxHeight);
    presetBox.setBounds (presetBoxBounds.toNearestInt());

    const auto iconButtonSize = boundsFHeight * 0.55f;
    const auto iconGap = boundsFWidth * 0.0125f;
    const auto iconButtonBounds = boundsF.withSizeKeepingCentre (iconButtonSize, iconButtonSize);

    const auto bypassButtonBounds = iconButtonBounds.withRightX (boundsFWidth - iconGap);
    bypassButton.setBounds (bypassButtonBounds.toNearestInt());
    const auto settingsButtonBounds = iconButtonBounds.withRightX (bypassButtonBounds.getX() - iconGap);
    settingsButton.setBounds (settingsButtonBounds.toNearestInt());
    const auto resizeButtonBounds = iconButtonBounds.withRightX (settingsButtonBounds.getX() - iconGap);
    resizeButton.setBounds (resizeButtonBounds.toNearestInt());

    const auto undoRedoCentreX = (presetBoxBounds.getRight() + resizeButtonBounds.getX()) * 0.5f;
    const auto undoRedoOffset = (iconButtonSize + iconGap) * 0.5f;
    undoButton.setBounds (iconButtonBounds.withCentre ({undoRedoCentreX - undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
    redoButton.setBounds (iconButtonBounds.withCentre ({undoRedoCentreX + undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
}

void HeaderPanel::setupIconButton (juce::DrawableButton& iconButton, const void* iconData, const int iconDataSize)
{
    addAndMakeVisible (iconButton);
    AAUITools::setIconButtonImages (iconButton, iconData, iconDataSize);
}
