#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
GuitarToolsAudioProcessorEditor::GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setWantsKeyboardFocus(true);
    setOpaque(true);
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(mainPanel);
    addChildComponent (infoPanel);
    addChildComponent (tooltipsPanel);
    tooltipsPanel.setVisible (audioProcessor.showTooltips);

    headerPanel.onInfoClicked = [this]
    {
        infoPanel.setVisible (! infoPanel.isVisible());
    };

    infoPanel.addComponentListener (this);

    headerPanel.onBypassChanged = [this] { updateMainPanelAlpha(); };
    updateMainPanelAlpha();

    addMouseListener (this, true);

    headerPanel.onSettingsClicked = [this]
    {
        showSettingsOptionsPopup();
    };

    addAndMakeVisible(resizeButton);
    resizeButton.setColour(juce::DrawableButton::ColourIds::backgroundOnColourId, GTColors::transparent);
    resizeButton.setColour(juce::DrawableButton::ColourIds::backgroundColourId, GTColors::transparent);
    if (const auto icon = juce::Drawable::createFromImageData (BinaryData::resizeLines_svg, BinaryData::resizeLines_svgSize))
    {
        const auto iconNormal = icon->createCopy();
        const auto iconOver = icon->createCopy();
        const auto iconActive = icon->createCopy();
        iconNormal->replaceColour (GTColors::iconSVGColor, GTColors::iconOverColor);
        iconOver->replaceColour (GTColors::iconSVGColor, GTColors::iconNormalColor.withAlpha(0.9f));
        iconActive->replaceColour (GTColors::iconSVGColor, GTColors::activeNeonBlue);
        resizeButton.setImages (iconNormal.get(), iconOver.get(), iconActive.get(), nullptr, iconActive.get());
    }
    resizeButton.setToggleable(true);
    resizeButton.setClickingTogglesState(false);
    resizeButton.onClick = [this] { showUISizeOptions(); };

    addChildComponent (settingsOptionsPopup);
    settingsOptionsPopup.addComponentListener (this);
    addChildComponent (sizeOptionsPopup);
    addChildComponent (presetBrowser);
    addChildComponent (savePresetPopup);
    addChildComponent (deletePresetPopup);

    settingsOptionsPopup.onTooltipsToggled = [this]
    {
        tooltipsPanel.setVisible (audioProcessor.showTooltips);
        updateEditorSize();
    };

    settingsOptionsPopup.onAboutClicked = [this]
    {
        infoPanel.setVisible (true);
        infoPanel.toFront (true);
    };

    sizeOptionsPopup.onItemClicked = [this] (const int itemID)
    {
        resizeButton.setToggleState (false, juce::dontSendNotification);

        if (uiResize.handleSizeOption (itemID))
            updateEditorSize();
    };

    headerPanel.presetBox.onShowPopup = [this] { showPresetBrowser(); };
    presetBrowser.onRowsChanged = [this] { resized(); };

    headerPanel.onSavePresetClicked = [this]
    {
        savePresetPopup.setVisible (true);
        savePresetPopup.toFront (false);
        savePresetPopup.prepare (headerPanel.presetManager.getCurrentPresetName());
    };

    savePresetPopup.onConfirm = [this] (const juce::String& presetName)
    {
        headerPanel.presetManager.savePreset (presetName);
        savePresetPopup.setVisible (false);
        presetBrowser.showUserPresets();
    };

    savePresetPopup.presetExists = [this] (const juce::String& presetName)
    {
        return headerPanel.presetManager.getUserPresetFile (presetName).existsAsFile();
    };

    presetBrowser.onDeleteRequested = [this] (const juce::File& presetFile)
    {
        presetToDelete = presetFile;
        deletePresetPopup.prepare (presetFile.getFileNameWithoutExtension());
        deletePresetPopup.setVisible (true);
        deletePresetPopup.toFront (false);
    };

    deletePresetPopup.onConfirm = [this]
    {
        headerPanel.presetManager.deletePreset (presetToDelete);
        deletePresetPopup.setVisible (false);
        presetBrowser.refresh();
        resized();
    };

    setResizable (false, false);
    updateEditorSize();
}

GuitarToolsAudioProcessorEditor::~GuitarToolsAudioProcessorEditor()
{}

void GuitarToolsAudioProcessorEditor::updateMainPanelAlpha()
{
    if (infoPanel.isVisible() || headerPanel.isBypassed())
        mainPanel.setAlpha (0.4f);
    else
        mainPanel.setAlpha (1.0f);
}

void GuitarToolsAudioProcessorEditor::componentVisibilityChanged (juce::Component& component)
{
    if (&component == &settingsOptionsPopup)
        headerPanel.settingsButton.setToggleState (settingsOptionsPopup.isVisible(), juce::dontSendNotification);
    else if (&component == &infoPanel)
        updateMainPanelAlpha();
}

void GuitarToolsAudioProcessorEditor::updateEditorSize()
{
    const auto footerRatio = audioProcessor.showTooltips ? headerHeightRatio * tooltipsHeightRatio : 0.0f;
    const auto editorSize = uiResize.getEditorSize (getScreenBounds(), footerRatio);
    setSize (editorSize.getWidth(), editorSize.getHeight());
}

//==============================================================================
void GuitarToolsAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFHeight = boundsF.getHeight();

    g.drawImage (metalBackground, boundsF, juce::RectanglePlacement::fillDestination);

    g.setGradientFill (juce::ColourGradient::vertical (GTColors::mainPanelTop.withAlpha(0.45f), 0.0f, GTColors::mainPanelBottom.brighter(0.12f).withAlpha(0.45f), boundsFHeight));
    g.fillRect (boundsF);

    g.fillAll(juce::Colours::black.withAlpha(0.15f));
}

void GuitarToolsAudioProcessorEditor::resized()
{
    const auto boundsF =  getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto contentHeight = boundsFWidth * (float) uiResize.defaultHeight / (float) uiResize.defaultWidth;
    const float headerH = contentHeight * headerHeightRatio;
    const auto tooltipsPanelH = tooltipsPanel.isVisible() ? headerH * tooltipsHeightRatio : 0.0f;
    const auto rowH = contentHeight * 0.05f;

    headerPanel.setBounds (boundsF.withHeight (headerH).toNearestInt());
    const auto mainPanelBounds = boundsF.withHeight (contentHeight - headerH).withY (headerH);
    mainPanel.sectionTitleH = mainPanelBounds.getHeight() * 0.07f;
    mainPanel.setBounds (mainPanelBounds.toNearestInt());
    tooltipsPanel.setBounds (boundsF.withHeight (tooltipsPanelH).withY (contentHeight).toNearestInt());

    const auto resizeButtonSize = contentHeight * 0.045f;
    const auto resizeButtonGap = resizeButtonSize * 0.55f;
    const juce::Point<float> resizeButtonCentre {boundsFWidth - resizeButtonGap, contentHeight - resizeButtonGap};
    resizeButton.setBounds (boundsF.withSize(resizeButtonSize, resizeButtonSize).withCentre(resizeButtonCentre).toNearestInt());

    const auto sizeOptionsWidth = resizeButtonSize * 4.0f;
    const auto sizeOptionsHeight = rowH * (float) juce::jmax (3, sizeOptionsPopup.getNumRows());
    sizeOptionsPopup.setBounds (juce::Rectangle<float> (sizeOptionsWidth, sizeOptionsHeight).withPosition (resizeButtonCentre.x - sizeOptionsWidth, resizeButtonCentre.y - sizeOptionsHeight).toNearestInt());

    const auto settingsOptionsWidth = boundsFWidth * 0.3f;
    const auto settingsOptionsHeight = rowH * (float) juce::jmax (3, settingsOptionsPopup.getNumRows());
    settingsOptionsPopup.setBounds (boundsF.withSize(settingsOptionsWidth, settingsOptionsHeight).withY(mainPanelBounds.getY() + mainPanel.dividerThickness).withRightX(sizeOptionsPopup.getBounds().toFloat().getRight()).toNearestInt());

    const auto presetBoxBounds = getLocalArea (&headerPanel, headerPanel.presetBox.getBounds().toFloat());
    const auto presetBrowserRows = (float) juce::jmin (presetBrowser.getNumRows(), presetBrowser.maxVisibleRows) + 1.0f;
    presetBrowser.setBounds (presetBoxBounds.withHeight (presetBoxBounds.getHeight() * presetBrowserRows).withY(mainPanelBounds.getY() + mainPanel.dividerThickness).toNearestInt());

    const auto presetPopupBounds = mainPanelBounds.withSizeKeepingCentre (presetBoxBounds.getWidth(), presetBoxBounds.getHeight() * 4.0f);
    savePresetPopup.setBounds (presetPopupBounds.toNearestInt());
    deletePresetPopup.setBounds (presetPopupBounds.toNearestInt());

    infoPanel.setBounds (mainPanelBounds.withSizeKeepingCentre (boundsFWidth * 0.525f, mainPanelBounds.getHeight() * 0.5f).toNearestInt());
}

void GuitarToolsAudioProcessorEditor::mouseDown (const juce::MouseEvent& event)
{
    dismissInfoPanelOnMouseUp = infoPanel.isVisible()
        && event.eventComponent != &infoPanel
        && ! infoPanel.isParentOf (event.eventComponent);

    const auto isClickInside = [&event] (const juce::Component& component)
    {
        return event.eventComponent == &component || component.isParentOf (event.eventComponent);
    };

    if (settingsOptionsPopup.isVisible() && ! isClickInside (settingsOptionsPopup) && ! isClickInside (headerPanel.settingsButton))
    {
        settingsOptionsPopup.setVisible (false);
        headerPanel.settingsButton.setToggleState (false, juce::dontSendNotification);
    }

    if (sizeOptionsPopup.isVisible() && ! isClickInside (sizeOptionsPopup) && ! isClickInside (resizeButton))
    {
        sizeOptionsPopup.setVisible (false);
        resizeButton.setToggleState (false, juce::dontSendNotification);
    }

    if (presetBrowser.isVisible() && ! isClickInside (presetBrowser) && ! isClickInside (headerPanel.presetBox) && ! isClickInside (deletePresetPopup))
        presetBrowser.setVisible (false);

    if (savePresetPopup.isVisible() && ! isClickInside (savePresetPopup))
        savePresetPopup.setVisible (false);

    if (deletePresetPopup.isVisible() && ! isClickInside (deletePresetPopup))
        deletePresetPopup.setVisible (false);
}

void GuitarToolsAudioProcessorEditor::mouseUp (const juce::MouseEvent&)
{
    if (dismissInfoPanelOnMouseUp)
        infoPanel.setVisible (false);

    dismissInfoPanelOnMouseUp = false;
}

void GuitarToolsAudioProcessorEditor::mouseEnter (const juce::MouseEvent& event)
{
    juce::String tooltipText;

    for (auto* hoveredComponent = event.eventComponent; hoveredComponent != nullptr && hoveredComponent != this && tooltipText.isEmpty();
         hoveredComponent = hoveredComponent->getParentComponent())
    {
        if (auto* tooltipClient = dynamic_cast<juce::TooltipClient*> (hoveredComponent))
            tooltipText = tooltipClient->getTooltip();
    }

    tooltipsPanel.setTooltipText (tooltipText);
}

void GuitarToolsAudioProcessorEditor::mouseExit (const juce::MouseEvent&)
{
    tooltipsPanel.setTooltipText ({});
}

void GuitarToolsAudioProcessorEditor::showSettingsOptionsPopup()
{
    if (!headerPanel.settingsButton.getToggleState())
    {
        settingsOptionsPopup.setVisible (false);
        return;
    }

    settingsOptionsPopup.updateItems();

    resized();
    settingsOptionsPopup.setVisible (true);
    settingsOptionsPopup.toFront (false);
}

void GuitarToolsAudioProcessorEditor::showUISizeOptions()
{
    const auto shouldShow = ! sizeOptionsPopup.isVisible();
    resizeButton.setToggleState (shouldShow, juce::dontSendNotification);

    if (! shouldShow)
    {
        sizeOptionsPopup.setVisible (false);
        return;
    }

    sizeOptionsPopup.setItems (uiResize.getSizeOptions());

    resized();
    sizeOptionsPopup.setVisible (true);
    sizeOptionsPopup.toFront (false);
}

void GuitarToolsAudioProcessorEditor::showPresetBrowser()
{
    if (presetBrowser.isVisible())
    {
        presetBrowser.setVisible (false);
        return;
    }

    presetBrowser.refresh();

    resized();
    presetBrowser.setVisible (true);
    presetBrowser.toFront (false);
}
