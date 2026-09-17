#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
GuitarToolsAudioProcessorEditor::GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setWantsKeyboardFocus(true);
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(mainPanel);
    addChildComponent (infoPanel);
    addChildComponent (tooltipsPanel);
    tooltipsPanel.setVisible (audioProcessor.showTooltips);
    headerPanel.onInfoClicked = [this]
    {
        infoPanel.setVisible (! infoPanel.isVisible());
        updateMainPanelAlpha();
    };
    infoPanel.onHidden = [this]
    {
        updateMainPanelAlpha();
    };
    headerPanel.onBypassChanged = [this] { updateMainPanelAlpha(); };
    updateMainPanelAlpha();
    addMouseListener (this, true);

    headerPanel.onSettingsClicked = [this]
    {
        showSettingsMenu(headerPanel.settingsButton);
    };

    setResizable (false, false);
    updateEditorSize();
}

GuitarToolsAudioProcessorEditor::~GuitarToolsAudioProcessorEditor()
{
}

void GuitarToolsAudioProcessorEditor::updateMainPanelAlpha()
{
    if (infoPanel.isVisible() || headerPanel.isBypassed())
        mainPanel.setAlpha (0.4f);
    else
        mainPanel.setAlpha (1.0f);
}

void GuitarToolsAudioProcessorEditor::updateEditorSize()
{
    const auto footerRatio = audioProcessor.showTooltips ? headerHeightRatio * tooltipsHeightRatio : 0.0f;
    const auto defaultEditorHeight = (float) defaultUIHeight * (1.0f + footerRatio);
    auto effectiveScale = audioProcessor.uiScale;

    if (const auto* display = juce::Desktop::getInstance().getDisplays().getDisplayForRect (getScreenBounds()))
    {
        const auto usableBounds = display->userBounds.reduced (display->userBounds.getWidth() * 0.05f,
                                                                display->userBounds.getHeight() * 0.05f);
        const auto widthScaleLimit = usableBounds.getWidth() / (float) defaultUIWidth;
        const auto heightScaleLimit = usableBounds.getHeight() / defaultEditorHeight;
        effectiveScale = juce::jmin (effectiveScale, widthScaleLimit, heightScaleLimit);
    }

    const auto contentHeight = (float) defaultUIHeight * effectiveScale;
    const auto footerHeight = audioProcessor.showTooltips ? contentHeight * headerHeightRatio * tooltipsHeightRatio : 0.0f;
    setSize (juce::roundToInt ((float) defaultUIWidth * effectiveScale), juce::roundToInt (contentHeight + footerHeight));
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

    // const auto dividerThickness = boundsF.getWidth() * 0.003f;
    // g.setColour (GTColors::dividerLinesStart);
    // g.drawRect (boundsF.reduced(dividerThickness* 0.5f), dividerThickness);
}

void GuitarToolsAudioProcessorEditor::resized()
{
    const auto boundsF =  getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto contentHeight = boundsFWidth * (float) defaultUIHeight / (float) defaultUIWidth;
    const float headerH = contentHeight * headerHeightRatio;
    const auto tooltipsPanelH = tooltipsPanel.isVisible() ? headerH * tooltipsHeightRatio : 0.0f;

    headerPanel.setBounds (boundsF.withHeight (headerH).toNearestInt());
    const auto mainPanelBounds = boundsF.withHeight (contentHeight - headerH).withY (headerH);
    mainPanel.sectionTitleH = mainPanelBounds.getHeight() * 0.07f;
    mainPanel.setBounds (mainPanelBounds.toNearestInt());
    tooltipsPanel.setBounds (boundsF.withHeight (tooltipsPanelH).withY (contentHeight).toNearestInt());

    const auto infoPanelSize = boundsFWidth * 0.4f;
    infoPanel.setBounds (mainPanelBounds.withSizeKeepingCentre (infoPanelSize, infoPanelSize).toNearestInt());
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

    tooltipsPanel.setTooltipText (tooltipText);
}

void GuitarToolsAudioProcessorEditor::mouseExit (const juce::MouseEvent&)
{
    tooltipsPanel.setTooltipText ({});
}

void GuitarToolsAudioProcessorEditor::showSettingsMenu(juce::DrawableButton& button)
{
    juce::PopupMenu settingsMenu;
    juce::PopupMenu uiSizeMenu;
    settingsMenu.setLookAndFeel (&settingsMenuLAF);
    uiSizeMenu.setLookAndFeel (&settingsMenuLAF);
    settingsMenuLAF.menuFontHeight = (float) button.getHeight() * 0.675f;

    settingsMenu.addSectionHeader ("SETTINGS");
    settingsMenu.addColouredItem (1, audioProcessor.showTooltips ? "Hide Tooltips" : "Show Tooltips", GTColors::iconNormalColor);

    for (int optionIndex = 0; optionIndex < uiSizeNames.size(); ++optionIndex)
    {
        const auto isSelectedSize = juce::approximatelyEqual (uiScales[(size_t) optionIndex], audioProcessor.uiScale);
        uiSizeMenu.addColouredItem (optionIndex + 100, uiSizeNames[optionIndex], isSelectedSize ? GTColors::activeNeonBlue : GTColors::iconNormalColor);
    }

    settingsMenu.addSubMenu ("UI Size", uiSizeMenu);
    settingsMenu.addColouredItem (2, "Show Info", GTColors::iconNormalColor);

    const auto menuWidth = button.getHeight() * 4;
    const auto buttonScreenBounds = button.getScreenBounds();
    const auto menuTargetArea = buttonScreenBounds.withX (buttonScreenBounds.getX() - menuWidth);

    settingsMenu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (button)
                                                         .withTargetScreenArea (menuTargetArea)
                                                         .withMinimumWidth (menuWidth)
                                                         .withPreferredPopupDirection (juce::PopupMenu::Options::PopupDirection::downwards),
                              [this] (const int selectedItemIndex)
                              {
                                  headerPanel.settingsButton.setToggleState (false, juce::dontSendNotification);

                                  if (selectedItemIndex == 1)
                                  {
                                      audioProcessor.showTooltips = ! audioProcessor.showTooltips;
                                      tooltipsPanel.setVisible (audioProcessor.showTooltips);
                                      updateEditorSize();
                                  }
                                  else if (selectedItemIndex == 2)
                                  {
                                      infoPanel.setVisible (true);
                                      infoPanel.toFront (true);
                                      updateMainPanelAlpha();
                                  }
                                  else if (selectedItemIndex >= 100)
                                  {
                                      audioProcessor.uiScale = uiScales[(size_t) (selectedItemIndex - 100)];
                                      updateEditorSize();
                                  }

                              });
}
