//
// Created by Aaron Petrini on 03/09/2026.
//
#include "HeaderPanel.h"
#include "BinaryData.h"
#include "../../Utils/GTLook.h"
#include "GUI/LAF/PresetBrowserLAF.h"

HeaderPanel::HeaderPanel (GuitarToolsAudioProcessor& processor) :
     bypassAttachment(processor.treeState, ParamIDs::getPluginBypassName(), bypassButton),
    undoManager (processor.undoManager),
    presetManager (processor, presetBox)
{
    addAndMakeVisible (logoButton);
    addAndMakeVisible (presetBox);
    addAndMakeVisible(bypassButton);

    if (const auto logo = juce::Drawable::createFromImageData (BinaryData::guitarTools2Logo_png, BinaryData::guitarTools2Logo_pngSize))
    {
        const auto logoBounds = logo->getDrawableBounds();
        logoAspectRatio = logoBounds.getWidth() / logoBounds.getHeight();
        logoButton.setImages (logo.get());
    }

    setupIconButton (savePresetButton, BinaryData::saveIcon_svg, BinaryData::saveIcon_svgSize);
    setupIconButton (undoButton, BinaryData::undoIcon_svg, BinaryData::undoIcon_svgSize);
    setupIconButton (redoButton, BinaryData::redoIcon_svg, BinaryData::redoIcon_svgSize);
    setupIconButton (settingsButton, BinaryData::settings_svg, BinaryData::settings_svgSize);
    setupIconButton (bypassButton, BinaryData::bypassIcon_svg, BinaryData::bypassIcon_svgSize);
    bypassButton.setClickingTogglesState(true);
    bypassButton.onStateChange = [this] { if (onBypassChanged != nullptr) onBypassChanged(); };
    settingsButton.setClickingTogglesState(true);
    logoButton.onClick = [this] { if (onInfoClicked != nullptr) onInfoClicked(); };
    logoButton.onStateChange = [this]
    {
        logoButton.setAlpha (logoButton.isMouseOver() ? 0.65f : 1.0f);
    };
    savePresetButton.onClick = [this] { presetManager.savePreset(); };
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };
    settingsButton.onClick = [this] { if (onSettingsClicked != nullptr) onSettingsClicked(); };

    undoManager.addChangeListener (this);
    changeListenerCallback (&undoManager);

    presetBox.setTextWhenNothingSelected ("NothingSelected");
    presetBox.setJustificationType(juce::Justification::centred);
    presetBox.setLookAndFeel (PresetBrowserLAF::get());
    presetBox.setColour(juce::ComboBox::ColourIds::textColourId, GTColors::sectionTitleText);
    presetBox.setColour(juce::PopupMenu::ColourIds::textColourId, GTColors::sectionTitleText);
    presetBox.onChange = [this] { presetManager.presetSelected(); };
    presetManager.refreshPresetList();
    presetManager.displayCurrentPresetName();
}

HeaderPanel::~HeaderPanel()
{
    undoManager.removeChangeListener (this);
    presetBox.setLookAndFeel (nullptr);
}

void HeaderPanel::setSettingsButtonOn (bool shouldBeOn)
{
    settingsButton.setToggleState (shouldBeOn, juce::dontSendNotification);
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
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto presetBoxHeight = boundsFHeight * 0.6f;
    const auto presetBoxWidth = boundsFWidth * 0.3f;
    const auto controlGap = boundsFWidth * 0.012f;

    const auto presetBoxBounds = boundsF.withSizeKeepingCentre (presetBoxWidth, presetBoxHeight);
    presetBox.setBounds (presetBoxBounds.toNearestInt());

    const auto guitarToolsLogoHeight = boundsFHeight * 0.6f;
    const auto logoBounds = boundsF.withSizeKeepingCentre (guitarToolsLogoHeight * logoAspectRatio, guitarToolsLogoHeight);
    logoButton.setBounds (logoBounds.toNearestInt().withX (getLocalBounds().getX()));

    const auto iconButtonSize = boundsFHeight * 0.5f;
    const auto iconButtonBounds = boundsF.withSizeKeepingCentre (iconButtonSize, iconButtonSize);
    savePresetButton.setBounds (iconButtonBounds.withX (presetBoxBounds.getRight() + controlGap).toNearestInt());

    const auto bypassButtonBounds = iconButtonBounds.withRightX (boundsFWidth - controlGap);
    const auto settingsButtonBounds = iconButtonBounds.withRightX (bypassButtonBounds.getX() - controlGap);
    const auto redoButtonBounds = iconButtonBounds.withRightX (settingsButtonBounds.getX() - controlGap);
    const auto undoButtonBounds = iconButtonBounds.withRightX (redoButtonBounds.getX() - controlGap);
    undoButton.setBounds (undoButtonBounds.toNearestInt());
    redoButton.setBounds (redoButtonBounds.toNearestInt());
    settingsButton.setBounds (settingsButtonBounds.toNearestInt());
    bypassButton.setBounds (bypassButtonBounds.toNearestInt());
}

void HeaderPanel::setupIconButton (juce::DrawableButton& iconButton, const void* iconData, const int iconDataSize)
{
    addAndMakeVisible (iconButton);
    UITools::setIconButtonImages (iconButton, iconData, iconDataSize);
}
