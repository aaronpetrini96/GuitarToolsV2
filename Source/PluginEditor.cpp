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

    // headerPanel.onResizeClicked = [this] { showUISizeOptions(); };

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
    addChildComponent (resizeOptionsPopup);
    resizeOptionsPopup.addComponentListener (this);
    addChildComponent (presetBrowser);
    addChildComponent (savePresetPopup);
    addChildComponent (deletePresetPopup);
    savePresetPopup.addComponentListener (this);
    deletePresetPopup.addComponentListener (this);
    addChildComponent (modalBlocker);

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

    headerPanel.onSaveClicked = [this]
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

    deletePresetPopup.onConfirm = [this] (const juce::String&)
    {
        presetManager.deletePreset (presetToDelete);
        deletePresetPopup.setVisible (false);
        presetBrowser.refresh();
    };

    setResizable (false, false);
    updateEditorSize();
}

GuitarToolsAudioProcessorEditor::~GuitarToolsAudioProcessorEditor() = default;

void GuitarToolsAudioProcessorEditor::updateMainPanelAlpha()
{
    if (infoPanel.isVisible() || headerPanel.isBypassed() || modalBlocker.isVisible())
        mainPanel.setAlpha (0.3f);
    else
        mainPanel.setAlpha (1.0f);
}

void GuitarToolsAudioProcessorEditor::componentVisibilityChanged (juce::Component& component)
{
    if (&component == &settingsOptionsPopup)
        headerPanel.settingsButton.setToggleState (settingsOptionsPopup.isVisible(), juce::dontSendNotification);
    else if (&component == &resizeOptionsPopup)
        resizeButton.setToggleState (resizeOptionsPopup.isVisible(), juce::dontSendNotification);
    else if (&component == &infoPanel)
        updateMainPanelAlpha();
    else if (&component == &savePresetPopup || &component == &deletePresetPopup)
    {
        modalBlocker.setVisible (savePresetPopup.isVisible() || deletePresetPopup.isVisible());
        modalBlocker.toBehind (&component);
        updateMainPanelAlpha();
    }
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
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImage(backgroundWithDividers, imageBounds, juce::RectanglePlacement::fillDestination);
}

void GuitarToolsAudioProcessorEditor::resized()
{
    const auto boundsF =  getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto contentHeight = boundsFWidth * (float) uiResize.defaultHeight / (float) uiResize.defaultWidth;
    const float headerH = contentHeight * headerHeightRatio;
    const auto tooltipsPanelH = tooltipsPanel.isVisible() ? headerH * tooltipsHeightRatio : 0.0f;

    headerPanel.setBounds (boundsF.withHeight (headerH).toNearestInt());
    tooltipsPanel.setBounds (boundsF.withHeight (tooltipsPanelH).withY (contentHeight).toNearestInt());

    imageBounds = boundsF.withHeight (contentHeight - headerH).withY (headerH);
    const auto mainPanelBounds = imageBounds.withSizeKeepingCentre(imageBounds.getWidth(), imageBounds.getHeight() * 0.98f);
    mainPanel.sectionTitleH = mainPanelBounds.getHeight() * 0.07f;
    mainPanel.setBounds (mainPanelBounds.toNearestInt());

    const auto resizeButtonSize = contentHeight * 0.04f;
    const auto resizeBtnBounds = mainPanelBounds.withSize(resizeButtonSize, resizeButtonSize).withRightX(mainPanelBounds.getWidth()).withBottomY(mainPanelBounds.getBottom());
    resizeButton.setBounds (resizeBtnBounds.toNearestInt());

    const auto rowH = headerH * 0.65f;
    const auto popupsY = mainPanelBounds.getHeight() * 0.06f;

    const auto sizeOptionsWidth = boundsFWidth * 0.175f;
    const auto sizeOptionsHeight = rowH * static_cast<float>(resizeOptionsPopup.getNumRows());
    resizeOptionsPopup.setBounds (juce::Rectangle<float> (sizeOptionsWidth, sizeOptionsHeight).withPosition (resizeBtnBounds.getCentreX() - sizeOptionsWidth, resizeBtnBounds.getCentreY() - sizeOptionsHeight).toNearestInt());

    const auto settingsOptionsWidth = boundsFWidth * 0.225f;
    const auto settingsOptionsHeight = rowH * static_cast<float>(settingsOptionsPopup.getNumRows());
    settingsOptionsPopup.setBounds (mainPanelBounds.withSize(settingsOptionsWidth, settingsOptionsHeight).withY(popupsY).withRightX(static_cast<float>(headerPanel.settingsButton.getRight())).toNearestInt());

    const auto presetBoxBounds = getLocalArea (&headerPanel, presetBox.getBounds().toFloat());
    const auto presetBrowserH = rowH * presetBrowser.getNumRows();
    const auto presetBrowserBounds = presetBoxBounds.withSizeKeepingCentre(boundsFWidth * 0.32f, presetBrowserH).withY(popupsY);
    presetBrowser.setBounds (presetBrowserBounds.toNearestInt());

    const auto saveDeletePopupBounds = mainPanelBounds.withSizeKeepingCentre(presetBoxBounds.getWidth(), rowH * 5.f);
    savePresetPopup.setBounds (saveDeletePopupBounds.toNearestInt());
    deletePresetPopup.setBounds (saveDeletePopupBounds.toNearestInt());

    infoPanel.setBounds (mainPanelBounds.withSizeKeepingCentre (boundsFWidth * 0.525f, mainPanelBounds.getHeight() * 0.5f).toNearestInt());
    modalBlocker.setBounds (getLocalBounds());
}

void GuitarToolsAudioProcessorEditor::mouseDown (const juce::MouseEvent& event)
{
    if (modalBlocker.isVisible())
    {
        if (event.eventComponent == &modalBlocker)
        {
            savePresetPopup.setVisible (false);
            deletePresetPopup.setVisible (false);
            presetBrowser.setVisible (false);
        }

        return;
    }

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
    hidePopupUnlessClickedIn (resizeOptionsPopup, resizeButton);
    hidePopupUnlessClickedIn (presetBrowser, presetBox, deletePresetPopup);
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
