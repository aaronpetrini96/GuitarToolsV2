//
// Created by Aaron Petrini on 03/09/2026.
//
#include "HeaderPanel.h"


HeaderPanel::HeaderPanel (GuitarToolsAudioProcessor& processor) :
    presetManager (processor, presetBox),
    bypassAttachment(processor.treeState, ParamIDs::getPluginBypassActiveName(), bypassButton),
    undoManager (processor.undoManager),
    audioProcessor (processor)
{
    setupIconButton (savePresetButton, BinaryData::saveIcon_svg, BinaryData::saveIcon_svgSize);
    savePresetButton.setTooltip("Save preset");
    setupIconButton (bypassButton, BinaryData::bypassIcon_svg, BinaryData::bypassIcon_svgSize);
    bypassButton.setTooltip (ParamHelpers::getTooltipForParam (ParamIDs::getPluginBypassActiveName()));
    bypassButton.setClickingTogglesState(true);
    bypassButton.onStateChange = [this] { if (onBypassChanged != nullptr) onBypassChanged(); };

    setupIconButton(settingsButton, BinaryData::settingsLines_svg, BinaryData::settingsLines_svgSize);
    settingsButton.setClickingTogglesState (true);
    settingsButton.setTooltip("Settings");
    settingsButton.onClick = [this] { if (onSettingsClicked != nullptr) onSettingsClicked(); };

    setupIconButton (undoButton, BinaryData::undoIcon_svg, BinaryData::undoIcon_svgSize);
    setupIconButton (redoButton, BinaryData::redoIcon_svg, BinaryData::redoIcon_svgSize);
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };
    savePresetButton.onClick = [this] { if (onSavePresetClicked != nullptr) onSavePresetClicked(); };

    setupIconButton (stateAButton, BinaryData::A_svg, BinaryData::A_svgSize);
    setupIconButton (stateBButton, BinaryData::B_svg, BinaryData::B_svgSize);
    stateAButton.setTooltip ("Switch to sound A");
    stateBButton.setTooltip ("Switch to sound B");

    for (auto* stateButton : {&stateAButton, &stateBButton})
    {
        stateButton->setClickingTogglesState (true);
        stateButton->setRadioGroupId (1);
    }

    stateAButton.setToggleState (audioProcessor.isStateAActive(), juce::dontSendNotification);
    stateBButton.setToggleState (! audioProcessor.isStateAActive(), juce::dontSendNotification);

    stateAButton.onClick = [this]
    {
        if (! audioProcessor.isStateAActive())
        {
            audioProcessor.switchABState();
            presetManager.displayCurrentPresetName();
        }
    };

    stateBButton.onClick = [this]
    {
        if (audioProcessor.isStateAActive())
        {
            audioProcessor.switchABState();
            presetManager.displayCurrentPresetName();
        }
    };

    undoManager.addChangeListener (this);
    changeListenerCallback (&undoManager);

    setupPresetBox();
    presetBox.onChange = [this] { presetManager.presetSelected(); };
    presetManager.refreshPresetList();
    presetManager.displayCurrentPresetName();

    // setupLogoButton();
    // logoButton.onClick = [this] { if (onInfoClicked != nullptr) onInfoClicked(); };
}

HeaderPanel::~HeaderPanel()
{
    undoManager.removeChangeListener (this);
    presetBox.setLookAndFeel (nullptr);
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
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();
    const auto dividerHalfThickness = boundsFWidth * 0.003f;

    g.setGradientFill(juce::ColourGradient::horizontal(GTColors::headerPanelStart, 0.f, GTColors::gradientNavy, boundsFWidth));
    g.fillRect(boundsF);
    // g.setColour (GTColors::dividerLinesStart);
    // g.fillRect (0.0f, boundsF.getHeight() - dividerHalfThickness, boundsFWidth, dividerHalfThickness);

    const auto reducedB = boundsF.withTrimmedBottom(dividerHalfThickness);
    const auto logoH = reducedB.getHeight() * JUCE_LIVE_CONSTANT(0.55f);
    const auto logoW = logoH * (float) guitarToolsLogo.getWidth() / (float) guitarToolsLogo.getHeight();
    const auto logoBounds = reducedB.withSize (logoW, logoH).withCentre({boundsFWidth * 0.33f * 0.5f, reducedB.getCentreY()});
    g.setOpacity(0.9f);
    g.drawImage (guitarToolsLogo, logoBounds, juce::RectanglePlacement::fillDestination);
}

void HeaderPanel::resized()
{
    const auto b = getLocalBounds().toFloat();
    const auto boundsF = b.withBottom(b.getHeight() - b.getWidth() * 0.0015f);
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto presetBoxHeight = boundsFHeight * 0.6f;
    const auto presetBoxWidth = boundsF.getWidth() * 0.3f;
    const auto presetBoxBounds = boundsF.withSizeKeepingCentre (presetBoxWidth, presetBoxHeight);
    presetBox.setBounds (presetBoxBounds.toNearestInt());

    // const auto logoBounds = boundsF.withSizeKeepingCentre (logoH * logoGTAspectRatio, logoH).withX(boundsF.getX());
    // logoButton.setBounds (logoBounds.toNearestInt());


    const auto iconButtonSize = boundsFHeight * 0.55f;
    const auto iconGap = boundsFWidth * 0.012f;
    const auto iconButtonBounds = boundsF.withSizeKeepingCentre (iconButtonSize, iconButtonSize);
    const auto savePresetButtonBounds = iconButtonBounds.withX (presetBoxBounds.getRight() + iconGap);
    savePresetButton.setBounds (savePresetButtonBounds.toNearestInt());

    const auto stateButtonSize = iconButtonSize * 0.75f;
    const auto stateBButtonBounds = boundsF.withSizeKeepingCentre (stateButtonSize, stateButtonSize).withRightX (presetBoxBounds.getX() - iconGap);
    stateBButton.setBounds (stateBButtonBounds.toNearestInt());
    stateAButton.setBounds (stateBButtonBounds.withRightX (stateBButtonBounds.getX()).toNearestInt());

    const auto bypassButtonBounds = iconButtonBounds.withRightX (boundsFWidth - iconGap);
    bypassButton.setBounds (bypassButtonBounds.toNearestInt());
    const auto settingsButtonBounds = iconButtonBounds.withRightX (bypassButtonBounds.getX() - iconGap);
    settingsButton.setBounds (settingsButtonBounds.toNearestInt());

    const auto undoRedoCentreX = (savePresetButtonBounds.getRight() + settingsButtonBounds.getX()) * 0.5f;
    const auto undoRedoOffset = (iconButtonSize + iconGap) * 0.5f;
    undoButton.setBounds (iconButtonBounds.withCentre ({undoRedoCentreX - undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
    redoButton.setBounds (iconButtonBounds.withCentre ({undoRedoCentreX + undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
}

void HeaderPanel::setupIconButton (juce::DrawableButton& iconButton, const void* iconData, const int iconDataSize)
{
    addAndMakeVisible (iconButton);
    AAUITools::setIconButtonImages (iconButton, iconData, iconDataSize);
}

void HeaderPanel::setupPresetBox ()
{
    addAndMakeVisible(presetBox);
    presetBox.setTextWhenNothingSelected ("NothingSelected");
    presetBox.setJustificationType(juce::Justification::centred);
    presetBox.setLookAndFeel (PresetBrowserLAF::get());
    presetBox.setColour(juce::ComboBox::ColourIds::textColourId, GTColors::sectionTitleText);
}

// void HeaderPanel::setupLogoButton ()
// {
//     if (guitarsToolLogoImg != nullptr && guitarsToolLogoImgOver != nullptr)
//     {
//         addAndMakeVisible (logoButton);
//         const auto logoBounds = guitarsToolLogoImg->getDrawableBounds();
//         logoGTAspectRatio = logoBounds.getWidth() / logoBounds.getHeight();
//         logoButton.setImages (guitarsToolLogoImg.get(), guitarsToolLogoImgOver.get());
//     }
// }
