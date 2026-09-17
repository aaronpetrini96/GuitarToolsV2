//
// Created by Aaron Petrini on 03/09/2026.
//
#include "HeaderPanel.h"


HeaderPanel::HeaderPanel (GuitarToolsAudioProcessor& processor) :
     bypassAttachment(processor.treeState, ParamIDs::getPluginBypassActiveName(), bypassButton),
    undoManager (processor.undoManager),
    presetManager (processor, presetBox)
{
    setupIconButton (savePresetButton, BinaryData::saveIcon_svg, BinaryData::saveIcon_svgSize);
    savePresetButton.setTooltip("Save preset");
    setupIconButton (bypassButton, BinaryData::bypassIcon_svg, BinaryData::bypassIcon_svgSize);
    bypassButton.setTooltip (ParamHelpers::getTooltipForParam (ParamIDs::getPluginBypassActiveName()));
    bypassButton.setClickingTogglesState(true);
    bypassButton.onStateChange = [this] { if (onBypassChanged != nullptr) onBypassChanged(); };

    setupIconButton(settingsButton, BinaryData::settingsWheel_svg, BinaryData::settingsWheel_svgSize);
    settingsButton.setClickingTogglesState (true);
    settingsButton.setTooltip("Settings");
    settingsButton.onClick = [this] { if (onSettingsClicked != nullptr) onSettingsClicked(); };

    setupIconButton (undoButton, BinaryData::undoIcon_svg, BinaryData::undoIcon_svgSize);
    setupIconButton (redoButton, BinaryData::redoIcon_svg, BinaryData::redoIcon_svgSize);
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };

    undoManager.addChangeListener (this);
    changeListenerCallback (&undoManager);

    setupPresetBox();
    presetBox.onChange = [this] { presetManager.presetSelected(); };
    presetManager.refreshPresetList();
    presetManager.displayCurrentPresetName();

    setupLogoButton();
    logoButton.onClick = [this] { if (onInfoClicked != nullptr) onInfoClicked(); };
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
    g.setGradientFill(juce::ColourGradient::horizontal(GTColors::dividerLinesEnd, 0.f, GTColors::presetBox.darker(0.775f), boundsFWidth));
    g.fillRect(boundsF);
    const auto dividerHalfThickness = boundsFWidth * 0.003f;
    g.setColour (GTColors::dividerLinesStart);
    g.fillRect (0.0f, boundsF.getHeight() - dividerHalfThickness, boundsFWidth, dividerHalfThickness);
}

void HeaderPanel::resized()
{
    const auto b = getLocalBounds().toFloat();
    const auto boundsF = b.withBottom(b.getHeight() - b.getWidth() * 0.0015f);
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto presetBoxHeight = boundsFHeight * 0.625f;
    const auto presetBoxWidth = boundsF.getWidth() * 0.33f;
    const auto presetBoxBounds = boundsF.withSizeKeepingCentre (presetBoxWidth, presetBoxHeight);
    presetBox.setBounds (presetBoxBounds.toNearestInt());

    const auto controlGap = boundsFWidth * 0.012f;
    const auto guitarToolsLogoHeight = boundsFHeight * 0.625f;
    const auto logoBounds = boundsF.withSizeKeepingCentre (guitarToolsLogoHeight * logoGTAspectRatio, guitarToolsLogoHeight);
    logoButton.setBounds (logoBounds.withX(presetBoxBounds.getX() * 0.5f - logoBounds.getWidth() * 0.5f).toNearestInt());

    const auto iconButtonSize = boundsFHeight * 0.625f;
    const auto iconButtonBounds = boundsF.withSizeKeepingCentre (iconButtonSize, iconButtonSize);
    const auto savePresetButtonBounds = iconButtonBounds.withX (presetBoxBounds.getRight() + controlGap);
    savePresetButton.setBounds (savePresetButtonBounds.toNearestInt());

    const auto bypassButtonBounds = iconButtonBounds.withRightX (boundsFWidth - controlGap);
    bypassButton.setBounds (bypassButtonBounds.toNearestInt());
    const auto settingsButtonBounds = iconButtonBounds.withRightX (bypassButtonBounds.getX() - controlGap);
    settingsButton.setBounds (settingsButtonBounds.toNearestInt());

    const auto undoRedoCentreX = (savePresetButtonBounds.getRight() + settingsButtonBounds.getX()) * 0.5f;
    const auto undoRedoOffset = (iconButtonSize + controlGap) * 0.5f;
    const auto undoButtonBounds = iconButtonBounds.withCentre ({undoRedoCentreX - undoRedoOffset, boundsF.getCentreY()});
    const auto redoButtonBounds = iconButtonBounds.withCentre ({undoRedoCentreX + undoRedoOffset, boundsF.getCentreY()});
    undoButton.setBounds (undoButtonBounds.toNearestInt());
    redoButton.setBounds (redoButtonBounds.toNearestInt());
}

void HeaderPanel::setupIconButton (juce::DrawableButton& iconButton, const void* iconData, const int iconDataSize)
{
    addAndMakeVisible (iconButton);
    UITools::setIconButtonImages (iconButton, iconData, iconDataSize);
}

void HeaderPanel::setupPresetBox ()
{
    addAndMakeVisible(presetBox);
    presetBox.setTextWhenNothingSelected ("NothingSelected");
    presetBox.setJustificationType(juce::Justification::centred);
    presetBox.setLookAndFeel (PresetBrowserLAF::get());
    presetBox.setColour(juce::ComboBox::ColourIds::textColourId, GTColors::sectionTitleText);
    presetBox.setColour(juce::PopupMenu::ColourIds::textColourId, GTColors::sectionTitleText);
}

void HeaderPanel::setupLogoButton ()
{
    if (guitarsToolLogoImg != nullptr && guitarsToolLogoImgOver != nullptr)
    {
        addAndMakeVisible (logoButton);
        const auto logoBounds = guitarsToolLogoImg->getDrawableBounds();
        logoGTAspectRatio = logoBounds.getWidth() / logoBounds.getHeight();
        logoButton.setImages (guitarsToolLogoImg.get(), guitarsToolLogoImgOver.get());
    }
}
