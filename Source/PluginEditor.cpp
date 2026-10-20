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

    headerPanel.onInfoClicked = [this] { showPopup (infoPanel); };

    infoPanel.addComponentListener (this);

    headerPanel.onBypassChanged = [this] { updateMainPanelDarkening(); };
    updateMainPanelDarkening();

    addMouseListener (this, true);

    headerPanel.onSettingsClicked = [this]
    {
        showSettingsOptionsPopup();
    };

    addAndMakeVisible(resizeButton);
    AAUITools::setIconButtonImages (resizeButton, BinaryData::resizeLines_svg, BinaryData::resizeLines_svgSize, GTColors::iconSVGColor, GTColors::iconNormalColor.withAlpha (0.9f), GTColors::iconOverColor, GTColors::activeNeonBlue);
    resizeButton.setToggleable(true);
    resizeButton.setClickingTogglesState(false);
    resizeButton.onClick = [this] { showUISizeOptions(); };
    resizeButton.setTooltip (ParamTooltips::getResizeUITooltip());

    addChildComponent (settingsOptionsPopup);
    settingsOptionsPopup.addComponentListener (this);
    addChildComponent (resizeOptionsPopup);
    resizeOptionsPopup.addComponentListener (this);
    resizeOptionsPopup.onSizeChanged = [this] { updateEditorSize(); };
    addChildComponent (presetBrowser);
    presetBrowser.addComponentListener (this);
    addChildComponent (presetPopup);
    presetPopup.addComponentListener (this);
    addChildComponent (modalBlocker);

    settingsOptionsPopup.onTooltipsToggled = [this]
    {
        tooltipsPanel.setVisible (audioProcessor.showTooltips);
        updateEditorSize();
    };

    settingsOptionsPopup.onAboutClicked = [this] { showPopup (infoPanel); };

    presetBrowser.onRowsChanged = [this] { resized(); };

    headerPanel.presetBox.onNameClicked = [this]
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
        showPopup (presetPopup);
        presetPopup.prepare (AAPresetPopup::Action::save, presetManager.getCurrentPresetName());
    };

    presetManager.addChangeListener (this);
    headerPanel.presetBox.setPresetName (presetManager.getCurrentPresetName());

    presetPopup.onConfirm = [this] (const AAPresetPopup::Action action, const juce::String& presetName)
    {
        presetPopup.setVisible (false);

        if (action == AAPresetPopup::Action::save)
        {
            presetManager.savePreset (presetName);
            presetBrowser.showUserPresets();
            return;
        }

        if (action == AAPresetPopup::Action::rename)
            presetManager.renamePreset (targetPresetFile, presetName);
        else
            presetManager.deletePreset (targetPresetFile);

        presetBrowser.refresh();
    };

    presetPopup.presetExists = [this] (const juce::String& presetName)
    {
        return presetManager.getUserPresetFile (presetName).existsAsFile();
    };

    presetPopup.isNameReserved = [this] (const juce::String& presetName)
    {
        return presetManager.isFactoryPresetName (presetName);
    };

    presetBrowser.onRenameRequested = [this] (const juce::File& presetFile)
    {
        targetPresetFile = presetFile;
        showPopup (presetPopup);
        presetPopup.prepare (AAPresetPopup::Action::rename, presetFile.getFileNameWithoutExtension());
    };

    presetBrowser.onDeleteRequested = [this] (const juce::File& presetFile)
    {
        targetPresetFile = presetFile;
        showPopup (presetPopup);
        presetPopup.prepare (AAPresetPopup::Action::remove, presetFile.getFileNameWithoutExtension());
    };

    setResizable (false, false);
    updateEditorSize();
}

GuitarToolsAudioProcessorEditor::~GuitarToolsAudioProcessorEditor()
{
    presetManager.removeChangeListener (this);
}

void GuitarToolsAudioProcessorEditor::changeListenerCallback (juce::ChangeBroadcaster*)
{
    headerPanel.presetBox.setPresetName (presetManager.getCurrentPresetName());
}

//==============================================================================
void GuitarToolsAudioProcessorEditor::paint (juce::Graphics& g)
{
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
    headerPanel.logoCentreXPOW = mainPanel.sideSectionPOW * 0.5f;
    headerPanel.setBounds (boundsF.withHeight (headerH).toNearestInt());
    tooltipsPanel.setBounds (boundsF.withHeight (tooltipsPanelH).withY (contentHeight).toNearestInt());
    imageBounds = boundsF.withHeight (contentHeight - headerH).withY (headerH);

    const auto mainPanelBounds = imageBounds.withSizeKeepingCentre(imageBounds.getWidth(), imageBounds.getHeight() * 0.977f);
    mainPanel.sectionTitleH = mainPanelBounds.getHeight() * 0.07f;
    mainPanel.setBounds (mainPanelBounds.toNearestInt());

    const auto resizeButtonSize = contentHeight * 0.0375f;
    const auto resizeBtnGap = resizeButtonSize * 0.5f;
    const auto resizeBtnBounds = mainPanelBounds.withSize(resizeButtonSize, resizeButtonSize)
                                                .withCentre({mainPanelBounds.getWidth() - resizeBtnGap, mainPanelBounds.getBottom() - resizeBtnGap});
    resizeButton.setBounds (resizeBtnBounds.toNearestInt());

    const auto rowH = headerH * 0.6f;
    const auto popupsY = headerH * 1.03f;

    const auto sizeOptionsWidth = boundsFWidth * 0.2f;
    const auto sizeOptionsHeight = rowH * resizeOptionsPopup.getNumRows();
    resizeOptionsPopup.setBounds (juce::Rectangle<float> (sizeOptionsWidth, sizeOptionsHeight).withRightX (static_cast<float> (headerPanel.settingsButton.getRight())).withBottomY (resizeBtnBounds.getCentreY()).toNearestInt());
    settingsOptionsPopup.setBounds (mainPanelBounds.withSize (boundsFWidth * 0.2925f, rowH * settingsOptionsPopup.getNumRows()).withY (popupsY).withRightX (static_cast<float> (headerPanel.settingsButton.getRight())).toNearestInt());

    const auto presetBoxBounds = getLocalArea (&headerPanel, headerPanel.presetBox.getBounds().toFloat());
    const auto presetBrowserH = rowH * presetBrowser.getNumRows();
    const auto presetBrowserBounds = presetBoxBounds.withSizeKeepingCentre(boundsFWidth * 0.34f, presetBrowserH).withY(popupsY);
    presetBrowser.setBounds (presetBrowserBounds.toNearestInt());


    const auto presetPopupBounds = mainPanelBounds.withSizeKeepingCentre(presetBrowserBounds.getWidth(), rowH * presetPopup.numRows).withY(popupsY);
    presetPopup.setBounds (presetPopupBounds.toNearestInt());

    infoPanel.setBounds (mainPanelBounds.withSizeKeepingCentre (boundsFWidth * 0.5f, mainPanelBounds.getHeight() * 0.475f).toNearestInt());
    modalBlocker.setBounds (getLocalBounds());
}

void GuitarToolsAudioProcessorEditor::mouseDown (const juce::MouseEvent& event)
{
    if (event.eventComponent == &modalBlocker)
    {
        presetPopup.setVisible (false);
        infoPanel.setVisible (false);
    }
}

void GuitarToolsAudioProcessorEditor::mouseUp (const juce::MouseEvent& event)
{
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
    hidePopupUnlessClickedIn (presetBrowser, headerPanel.presetBox, presetPopup, modalBlocker);
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

void GuitarToolsAudioProcessorEditor::updateMainPanelDarkening()
{
    mainPanel.shouldDim = headerPanel.isBypassed() || infoPanel.isVisible() || modalBlocker.isVisible() || settingsOptionsPopup.isVisible() || resizeOptionsPopup.isVisible() || presetBrowser.isVisible();
    mainPanel.repaint();
}

void GuitarToolsAudioProcessorEditor::componentVisibilityChanged (juce::Component& component)
{
    if (&component == &settingsOptionsPopup)
        headerPanel.settingsButton.setToggleState (settingsOptionsPopup.isVisible(), juce::dontSendNotification);
    else if (&component == &resizeOptionsPopup)
        resizeButton.setToggleState (resizeOptionsPopup.isVisible(), juce::dontSendNotification);
    else if (&component == &presetPopup || &component == &infoPanel)
    {
        modalBlocker.setVisible (presetPopup.isVisible() || infoPanel.isVisible());
        modalBlocker.toBehind (&component);
    }

    updateMainPanelDarkening();
}

void GuitarToolsAudioProcessorEditor::updateEditorSize()
{
    const auto footerRatio = audioProcessor.showTooltips ? headerHeightRatio * tooltipsHeightRatio : 0.0f;
    const auto editorSize = uiResize.getEditorSize (getScreenBounds(), footerRatio);
    setSize (editorSize.getWidth(), editorSize.getHeight());
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
    resizeOptionsPopup.updateItems();
    showPopup (resizeOptionsPopup);
}

void GuitarToolsAudioProcessorEditor::showPopup (juce::Component& popup)
{
    resized();
    popup.toFront (false);
    popup.setVisible (true);
}
