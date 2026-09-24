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

    headerPanel.onResizeClicked = [this] { showUISizeOptions(); };

    addChildComponent (settingsOptionsPopup);
    settingsOptionsPopup.addComponentListener (this);
    addChildComponent (resizeOptionsPopup);
    resizeOptionsPopup.addComponentListener (this);
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

    presetBrowser.onRowsChanged = [this] { resized(); };

    presetBox.onNameClicked = [this]
    {
        if (presetBrowser.isVisible())
        {
            presetBrowser.setVisible (false);
            return;
        }

        presetBrowser.refresh();
        showPopup (presetBrowser);
    };

    presetBox.onSaveClicked = [this]
    {
        showPopup (savePresetPopup);
        savePresetPopup.prepare (presetManager.getCurrentPresetName());
    };

    presetManager.onPresetNameChanged = [this] { presetBox.setPresetName (presetManager.getCurrentPresetName()); };
    presetBox.setPresetName (presetManager.getCurrentPresetName());

    savePresetPopup.onConfirm = [this] (const juce::String& presetName)
    {
        presetManager.savePreset (presetName);
        savePresetPopup.setVisible (false);
        presetBrowser.showUserPresets();
    };

    savePresetPopup.presetExists = [this] (const juce::String& presetName)
    {
        return presetManager.getUserPresetFile (presetName).existsAsFile();
    };

    presetBrowser.onDeleteRequested = [this] (const juce::File& presetFile)
    {
        presetToDelete = presetFile;
        deletePresetPopup.prepare (presetFile.getFileNameWithoutExtension());
        showPopup (deletePresetPopup);
    };

    deletePresetPopup.onConfirm = [this]
    {
        presetManager.deletePreset (presetToDelete);
        deletePresetPopup.setVisible (false);
        presetBrowser.refresh();
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
    else if (&component == &resizeOptionsPopup)
        headerPanel.resizeButton.setToggleState (resizeOptionsPopup.isVisible(), juce::dontSendNotification);
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

    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImage (metalBackground, boundsF, juce::RectanglePlacement::fillDestination);
    const auto alpha = 0.4f;
    g.setGradientFill (juce::ColourGradient::vertical (GTColors::mainPanelTop.withAlpha(alpha), 0.0f, GTColors::mainPanelBottom.brighter(0.1f).withAlpha(alpha), boundsFHeight));
    g.fillRect (boundsF);

    // g.fillAll(juce::Colours::black.withAlpha(0.12f));

}

void GuitarToolsAudioProcessorEditor::resized()
{
    const auto boundsF =  getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto contentHeight = boundsFWidth * (float) uiResize.defaultHeight / (float) uiResize.defaultWidth;
    const float headerH = contentHeight * headerHeightRatio;
    const auto tooltipsPanelH = tooltipsPanel.isVisible() ? headerH * tooltipsHeightRatio : 0.0f;

    headerPanel.setBounds (boundsF.withHeight (headerH).toNearestInt());

    const auto mainPanelBounds = boundsF.withHeight (contentHeight - headerH).withY (headerH);
    mainPanel.sectionTitleH = mainPanelBounds.getHeight() * 0.065f;
    mainPanel.setBounds (mainPanelBounds.toNearestInt());
    tooltipsPanel.setBounds (boundsF.withHeight (tooltipsPanelH).withY (contentHeight).toNearestInt());

    const auto rowH = headerH * 0.5f;
    const auto sizeOptionsWidth = boundsFWidth * 0.175f;
    const auto sizeOptionsHeight = rowH * static_cast<float>(resizeOptionsPopup.getNumRows());
    const auto resizeButtonBounds = getLocalArea (&headerPanel, headerPanel.resizeButton.getBounds().toFloat());
    resizeOptionsPopup.setBounds (juce::Rectangle<float> (sizeOptionsWidth, sizeOptionsHeight).withPosition (resizeButtonBounds.getRight() - sizeOptionsWidth, mainPanelBounds.getY()).toNearestInt());

    const auto settingsOptionsWidth = boundsFWidth * 0.25f;
    const auto settingsOptionsHeight = rowH * static_cast<float>(settingsOptionsPopup.getNumRows());
    settingsOptionsPopup.setBounds (mainPanelBounds.withSize(settingsOptionsWidth, settingsOptionsHeight)
                                                        .withCentre({boundsFWidth * 0.835f, mainPanelBounds.getY() + settingsOptionsHeight * 0.5f}).toNearestInt());

    const auto presetBoxBounds = getLocalArea (&headerPanel, presetBox.getBounds().toFloat());
    const auto presetBrowserBounds = presetBoxBounds.withSizeKeepingCentre(boundsFWidth * 0.34f, boundsF.getHeight() * 0.34f).withY(mainPanelBounds.getY());
    presetBrowser.setBounds (presetBrowserBounds.toNearestInt());

    const auto presetPopupBounds = presetBrowserBounds.toFloat().withHeight (presetBrowserBounds.getHeight() * 0.5f);
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

    const auto hidePopupUnlessClickedIn = [&isClickInside] (juce::Component& popup, const auto&... keepOpenFor)
    {
        if (popup.isVisible() && ! isClickInside (popup) && ! (isClickInside (keepOpenFor) || ...))
            popup.setVisible (false);
    };

    hidePopupUnlessClickedIn (settingsOptionsPopup, headerPanel.settingsButton);
    hidePopupUnlessClickedIn (resizeOptionsPopup, headerPanel.resizeButton);
    hidePopupUnlessClickedIn (presetBrowser, presetBox, deletePresetPopup);
    hidePopupUnlessClickedIn (savePresetPopup);
    hidePopupUnlessClickedIn (deletePresetPopup);
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
    showPopup (settingsOptionsPopup);
}

void GuitarToolsAudioProcessorEditor::showUISizeOptions()
{
    if (resizeOptionsPopup.isVisible())
    {
        resizeOptionsPopup.setVisible (false);
        return;
    }

    resizeOptionsPopup.setItems (uiResize.getSizeOptions ([this] { updateEditorSize(); }));
    showPopup (resizeOptionsPopup);
}

void GuitarToolsAudioProcessorEditor::showPopup (juce::Component& popup)
{
    resized();
    popup.setVisible (true);
    popup.toFront (false);
}
