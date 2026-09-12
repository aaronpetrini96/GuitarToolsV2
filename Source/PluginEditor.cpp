#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
GuitarToolsAudioProcessorEditor::GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setWantsKeyboardFocus(true);
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(mainPanel);
    addChildComponent (settingsPanel);
    addChildComponent (infoPanel);
    addChildComponent (footerPanel);
    footerPanel.setVisible (audioProcessor.showTooltips);
    settingsPanel.setShowTooltips (audioProcessor.showTooltips);
    settingsPanel.onShowTooltipsChanged = [this] (const bool shouldShow)
    {
        audioProcessor.showTooltips = shouldShow;
        footerPanel.setVisible (shouldShow);
        updateEditorSize();
    };
    headerPanel.onSettingsClicked = [this] { updatePanelsVisibility (! settingsPanel.isVisible(), false); };
    settingsPanel.onHidden = [this]
    {
        updateMainPanelAlpha();
        headerPanel.setSettingsButtonOn (false);
    };
    headerPanel.onInfoClicked = [this] { updatePanelsVisibility (false, ! infoPanel.isVisible()); };
    infoPanel.onHidden = [this]
    {
        updateMainPanelAlpha();
    };
    headerPanel.onBypassChanged = [this] { updateMainPanelAlpha(); };
    updateMainPanelAlpha();
    addMouseListener (this, true);

    addAndMakeVisible(resizeButton);
    resizeButton.setColour(juce::DrawableButton::ColourIds::backgroundOnColourId, GTColors::transparent);
    resizeButton.setColour(juce::DrawableButton::ColourIds::backgroundColourId, GTColors::transparent);
    if (const auto icon = juce::Drawable::createFromImageData (BinaryData::resizeLines_svg, BinaryData::resizeLines_svgSize))
    {
        const auto iconNormal = icon->createCopy();
        const auto iconOver = icon->createCopy();
        const auto iconActive = icon->createCopy();
        iconNormal->replaceColour (GTColors::iconSVGColor, GTColors::iconOverColor);
        iconOver->replaceColour (GTColors::iconSVGColor, GTColors::iconNormalColor);
        iconActive->replaceColour (GTColors::iconSVGColor, GTColors::activeNeonBlue);
        resizeButton.setImages (iconNormal.get(), iconOver.get(), iconActive.get(), nullptr, iconActive.get());
    }
    resizeButton.setToggleable(true);
    resizeButton.setClickingTogglesState(false);
    resizeButton.onClick = [this] 
    {
        resizeButton.setToggleState(true, juce::dontSendNotification); 
        showUISizeMenu(); 
    };

    setResizable (false, false);
    updateEditorSize();
}

GuitarToolsAudioProcessorEditor::~GuitarToolsAudioProcessorEditor()
{
}

void GuitarToolsAudioProcessorEditor::updateMainPanelAlpha()
{
    if (settingsPanel.isVisible() || infoPanel.isVisible() || headerPanel.isBypassed())
        mainPanel.setAlpha (0.4f);
    else
        mainPanel.setAlpha (1.0f);
}

void GuitarToolsAudioProcessorEditor::updateEditorSize()
{
    const auto contentHeight = (float) defaultUIHeight * audioProcessor.uiScale;
    const auto footerHeight = audioProcessor.showTooltips ? contentHeight * headerHeightRatio * footerHeightRatio : 0.0f;
    setSize (juce::roundToInt ((float) defaultUIWidth * audioProcessor.uiScale), juce::roundToInt (contentHeight + footerHeight));
}

void GuitarToolsAudioProcessorEditor::updatePanelsVisibility (bool shouldShowSettings, bool shouldShowInfo)
{
    settingsPanel.setVisible (shouldShowSettings);
    infoPanel.setVisible (shouldShowInfo);
    updateMainPanelAlpha();
}

//==============================================================================
void GuitarToolsAudioProcessorEditor::paint (juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFHeight = boundsF.getHeight();

    g.fillAll(juce::Colours::black);
    if (metalBackground != nullptr)
        metalBackground->drawWithin (g, boundsF, juce::RectanglePlacement::fillDestination, 1.f);

    g.setGradientFill (juce::ColourGradient::vertical (GTColors::mainPanelTop.withAlpha(0.45f), 0.0f, GTColors::mainPanelBottom.brighter(0.12f).withAlpha(0.45f), boundsFHeight));
    g.fillRect (boundsF);
    g.fillAll(juce::Colours::black.withAlpha(0.1f));
}

void GuitarToolsAudioProcessorEditor::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto contentHeight = boundsFWidth * (float) defaultUIHeight / (float) defaultUIWidth;
    const float headerH = contentHeight * headerHeightRatio;
    const float footerH = headerH * footerHeightRatio;

    headerPanel.setBounds(boundsF.withHeight(headerH).toNearestInt());
    const auto mainPanelBounds = boundsF.withHeight(contentHeight - headerH).withY(headerH);
    mainPanel.sectionTitleH = mainPanelBounds.getHeight() * 0.07f;
    mainPanel.setBounds(mainPanelBounds.toNearestInt());
    footerPanel.setBounds(boundsF.withHeight(footerH).withY(contentHeight).toNearestInt());

    const auto resizeButtonSize = contentHeight * 0.045f;
    const auto resizeButtonGap = resizeButtonSize * 0.52f;
    resizeButton.setBounds (boundsF.withSize(resizeButtonSize, resizeButtonSize)
                            .withCentre({boundsFWidth - resizeButtonGap, contentHeight - resizeButtonGap}).toNearestInt());
    const auto settingsPanelSize = boundsFWidth * 0.4f;
    settingsPanel.setBounds (mainPanelBounds.withSizeKeepingCentre (settingsPanelSize, settingsPanelSize).toNearestInt());
    infoPanel.setBounds (mainPanelBounds.withSizeKeepingCentre (settingsPanelSize, settingsPanelSize).toNearestInt());
}

void GuitarToolsAudioProcessorEditor::mouseDown (const juce::MouseEvent& event)
{
    dismissInfoPanelOnMouseUp = infoPanel.isVisible()
        && event.eventComponent != &infoPanel
        && ! infoPanel.isParentOf (event.eventComponent);
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

    footerPanel.setTooltipText (tooltipText);
}

void GuitarToolsAudioProcessorEditor::mouseExit (const juce::MouseEvent&)
{
    footerPanel.setTooltipText ({});
}

void GuitarToolsAudioProcessorEditor::showUISizeMenu()
{
    juce::PopupMenu uiSizeMenu;
    uiSizeMenu.setLookAndFeel (&resizeButtonLAF);
    resizeButtonLAF.menuFontHeight = (float) resizeButton.getHeight() * 0.675f;

    for (int optionIndex = 0; optionIndex < uiSizeNames.size(); ++optionIndex)
    {
        const auto isSelectedSize = juce::approximatelyEqual (uiScales[(size_t) optionIndex], audioProcessor.uiScale);
        uiSizeMenu.addColouredItem (optionIndex + 1, uiSizeNames[optionIndex], isSelectedSize ? GTColors::activeNeonBlue : GTColors::iconNormalColor);
    }

    uiSizeMenu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (resizeButton)
                                                        .withPreferredPopupDirection (juce::PopupMenu::Options::PopupDirection::upwards),
                              [this] (const int selectedItemIndex)
                              {
                                  resizeButton.setToggleState(false, juce::dontSendNotification); 
                                  if (selectedItemIndex == 0)
                                      return;

                                  audioProcessor.uiScale = uiScales[(size_t) (selectedItemIndex - 1)];
                                  updateEditorSize();
                              });
}
