#include "MainPanel.h"

MainPanel::MainPanel (GuitarToolsAudioProcessor& processor)
    : audioProcessor (processor)

{
    //================  GROUP TITLES  ================//
    for (auto* l : {&cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel})
        setupLabels(l, GTColors::sectionTitleText.withAlpha(0.9f), juce::Justification::centred);

    //================  CUT FILTERS ================//
    addAndMakeVisible (lowCutFreqKnob);
    addAndMakeVisible (highCutFreqKnob);
    addAndMakeVisible (lowCutSlopeButtons);
    addAndMakeVisible (highCutSlopeButtons);

    for (auto* l : {&lowCutSlopeLabel, &highCutSlopeLabel})
       setupLabels (l, textWhiteColor, juce::Justification::centred);

    //================  SHELF FILTERS  ================//
    addAndMakeVisible(highShelfGainKnob);
    addAndMakeVisible(lowShelfGainKnob);
    addAndMakeVisible (presenceFreqButtons);
    addAndMakeVisible (depthFreqButtons);
    for (auto* knob : {&lowCutFreqKnob, &highCutFreqKnob, &lowShelfGainKnob, &highShelfGainKnob})
    {
        knob->setTooltip (ParamHelpers::getTooltipForParam (knob->getComponentID()));
        knob->setUndoManager (audioProcessor.undoManager);
    }
    for (auto* buttons : {&lowCutSlopeButtons, &highCutSlopeButtons, &presenceFreqButtons, &depthFreqButtons})
    {
        buttons->setTooltip (ParamHelpers::getTooltipForParam (buttons->getComponentID()));
        buttons->setUndoManager (audioProcessor.undoManager);
    }

    setupLabels (&presenceModeLabel,    textWhiteColor, juce::Justification::centred);
    setupLabels (&depthModeLabel,       textWhiteColor, juce::Justification::centred);

    //================  MAGIC BUTTONS  ================//
    for (auto* l : {&resoBypassLabel, &compBypassLabel, &mudBypassLabel})
        setupLabels (l, textWhiteColor, juce::Justification::centredTop);

    addAndMakeVisible (resoActiveButton);
    addAndMakeVisible (mudActiveButton);
    addAndMakeVisible (compActiveButton);
    for (auto* button : {&resoActiveButton, &mudActiveButton, &compActiveButton})
    {
        button->setTooltip (ParamHelpers::getTooltipForParam (button->getComponentID()));
        button->setUndoManager (audioProcessor.undoManager);
    }

    resoActiveButton.onClick = [this] { fadersPlatePanel.resoFreqFader.setThumbActive (resoActiveButton.getToggleState()); };
    mudActiveButton.onClick = [this] { fadersPlatePanel.mudFreqFader.setThumbActive (mudActiveButton.getToggleState()); };
    compActiveButton.onClick = [this] { fadersPlatePanel.compAmountFader.setThumbActive (compActiveButton.getToggleState()); };

    fadersPlatePanel.resoFreqFader.setThumbActive (resoActiveButton.getToggleState());
    fadersPlatePanel.mudFreqFader.setThumbActive (mudActiveButton.getToggleState());
    fadersPlatePanel.compAmountFader.setThumbActive (compActiveButton.getToggleState());

    addAndMakeVisible (fadersPlatePanel);

    for (auto* modeSwitch : {&resoModeSwitch, &mudModeSwitch, &compModeSwitch})
    {
        addAndMakeVisible (*modeSwitch);
        modeSwitch->setTooltip (ParamHelpers::getTooltipForParam (modeSwitch->getComponentID()));
        modeSwitch->setUndoManager (audioProcessor.undoManager);
        modeSwitch->setLabelText ("MODE");
    }

    //================  LEVEL KNOBS  ================//
    for (auto* l : {&inputGainLabel, &outputGainLabel})
        setupLabels (l, textWhiteColor, juce::Justification::centredTop);

    for (auto* l : {&levelsLabel})
        setupLabels (l, textWhiteColor, juce::Justification::centred);

    addAndMakeVisible (inputGainKnob);
    addAndMakeVisible (outputGainKnob);
    for (auto* knob : {&inputGainKnob, &outputGainKnob})
    {
        knob->setTooltip (ParamHelpers::getTooltipForParam (knob->getComponentID()));
        knob->setUndoManager (audioProcessor.undoManager);
    }

    levelMatchControl.setTooltip (ParamHelpers::getTooltipForParam (levelMatchControl.getComponentID()));
    levelMatchControl.setUndoManager (audioProcessor.undoManager);
    levelMatchControl.onClick = [this] { outputGainKnob.setEnabled (! levelMatchControl.getToggleState()); };
    outputGainKnob.setEnabled (! levelMatchControl.getToggleState());
    addAndMakeVisible (levelMatchControl);
}

void MainPanel::paint(juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();

    dividerThickness = boundsF.getHeight() * 0.006f;
    const auto dividerHalf = dividerThickness / 2.0f;
    const auto dividerGap = dividerThickness * 5.f;
    // AAUITools::drawRoundedDivider (g, boundsF.withHeight(dividerThickness));
    // AAUITools::drawRoundedDivider (g, boundsF.withHeight(dividerThickness).withBottomY(boundsF.getHeight()));
    const auto verticalDividerBounds = boundsF.withSizeKeepingCentre (dividerThickness, boundsF.getHeight() - dividerGap);
    AAUITools::drawRoundedDivider (g, verticalDividerBounds.withCentre ({cutFiltersArea.getRight() , verticalDividerBounds.getCentreY() }));
    AAUITools::drawRoundedDivider (g, verticalDividerBounds.withCentre ({ shelfFiltersArea.getX(), verticalDividerBounds.getCentreY() }));

    for (const auto& sectionArea : { cutFiltersArea, shelfFiltersArea })
        AAUITools::drawRoundedDivider (g, sectionArea.withSizeKeepingCentre (sectionArea.getWidth() - dividerHalf - dividerGap, dividerThickness).withY (sectionTitleH - dividerHalf));

    AAUITools::drawRoundedDivider(g, buttonsArea.withSizeKeepingCentre(buttonsArea.getWidth() - dividerGap, dividerThickness).withY(sectionTitleH - dividerHalf));

    // AAUITools::drawRoundedDivider (g, boundsF.withSize (boundsF.getWidth()/* - dividerHalf - dividerGap*/, dividerThickness).withY (sectionTitleH - dividerHalf));
    AAUITools::drawRoundedDivider (g, magicDividerBounds.withSizeKeepingCentre(buttonsArea.getWidth() - dividerHalf - dividerGap , magicDividerBounds.getHeight()));

    // const auto screwSize = boundsF.getHeight() * 0.033f;
    // const auto screwBounds = boundsF.withSize(screwSize, screwSize);
    // g.drawImage(screw, screwBounds.withCentre({screwSize, screwSize}), juce::RectanglePlacement::centred);
    // g.drawImage(screw, screwBounds.withCentre({boundsF.getWidth() - screwSize, screwSize}), juce::RectanglePlacement::centred);
    // g.drawImage(screw, screwBounds.withCentre({boundsF.getWidth() - screwSize, boundsF.getHeight() - screwSize}), juce::RectanglePlacement::centred);
    // g.drawImage(screw, screwBounds.withCentre({screwSize, boundsF.getHeight() - screwSize}), juce::RectanglePlacement::centred);
}

void MainPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto sideSectionWidth = boundsFWidth * sideSectionPOW;
    cutFiltersArea = boundsF.withSize (sideSectionWidth, boundsFHeight);
    shelfFiltersArea = cutFiltersArea.withRightX(boundsFWidth);
    buttonsArea = boundsF.withSizeKeepingCentre (boundsFWidth - sideSectionWidth * 2.f, boundsFHeight);

    for (const auto l : {&cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel,})
       l->setFont (AAFonts::ibmPlexSansCondensedSemiBold (boundsFHeight * 0.033f).withExtraKerningFactor(0.12f));
    cutFiltersGroupLabel    .setBounds (cutFiltersArea.withHeight (sectionTitleH).toNearestInt());
    magicButtonsGroupLabel  .setBounds (buttonsArea.withHeight (sectionTitleH).toNearestInt());
    shelfFiltersGroupLabel  .setBounds (shelfFiltersArea.withHeight (sectionTitleH).toNearestInt());

    resizeKnobs(boundsF);
    resizeMagicButtons(boundsF);
    magicDividerBounds = buttonsArea.withSizeKeepingCentre (plateBounds.getWidth(), boundsF.getWidth() * 0.006f).withY (resoModeSwitch.getBounds().toFloat().getBottom());
    resizeLevelsSection(boundsF);
}

void MainPanel::setupLabels(juce::Label* l, juce::Colour color, juce::Justification justification)
{
    l->setJustificationType (justification);
    l->setColour (juce::Label::textColourId, color);
    l->setInterceptsMouseClicks(false, false);
    addAndMakeVisible (*l);
}

void MainPanel::resizeKnobs(const juce::Rectangle<float>& boundsF)
{
    const auto boundsFHeight = boundsF.getHeight();
    const auto topKnobsCY = boundsFHeight * 0.25f;
    const auto bottomKnobsCY= boundsFHeight * 0.71f;
    const auto knobSizeReference = boundsFHeight * 0.33f;

    const auto knobBounds= boundsF.withSize (knobSizeReference, knobSizeReference);
    lowCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    highCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());

    lowShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    highShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());

    const auto knobButtonsWidth = knobSizeReference * 0.75f;
    const auto knobButtonsHeight = knobButtonsWidth / lowCutSlopeButtons.getImageAspectRatio();
    const auto knobButtonsGap = (knobSizeReference + knobButtonsHeight) * 0.535f;
    const auto knobButtonsTopCY = topKnobsCY + knobButtonsGap;
    const auto knobButtonsBottomCY = bottomKnobsCY + knobButtonsGap;

    const auto knobButtonsBounds = knobBounds.withSize (knobButtonsWidth, knobButtonsHeight);
    lowCutSlopeButtons.setBounds (knobButtonsBounds.withCentre({cutFiltersArea.getCentreX(), knobButtonsTopCY}).toNearestInt());
    highCutSlopeButtons.setBounds (knobButtonsBounds.withCentre({cutFiltersArea.getCentreX(), knobButtonsBottomCY}).toNearestInt());
    depthFreqButtons.setBounds (knobButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), knobButtonsTopCY }).toNearestInt());
    presenceFreqButtons.setBounds (knobButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), knobButtonsBottomCY }).toNearestInt());

    const auto knobButtonsLabelBounds = cutFiltersArea.withSizeKeepingCentre(knobButtonsWidth, knobButtonsHeight * 0.6f);
    for (const auto l : {&lowCutSlopeLabel, &highCutSlopeLabel, &presenceModeLabel, &depthModeLabel})
        l->setFont (AAFonts::ibmPlexSansMedium (knobButtonsLabelBounds.getHeight() * 0.825f));

    const auto knobButtonsLabelGap = knobButtonsHeight * 1.1f;
    lowCutSlopeLabel.setBounds(knobButtonsLabelBounds.withBottomY(knobButtonsTopCY + knobButtonsLabelGap).toNearestInt());
    highCutSlopeLabel.setBounds(knobButtonsLabelBounds.withBottomY(knobButtonsBottomCY + knobButtonsLabelGap).toNearestInt());
    depthModeLabel.setBounds (knobButtonsLabelBounds.withX (shelfFiltersArea.getCentreX() - knobButtonsLabelBounds.getWidth() * 0.5f).withBottomY (knobButtonsTopCY + knobButtonsLabelGap).toNearestInt());
    presenceModeLabel.setBounds (knobButtonsLabelBounds.withX (shelfFiltersArea.getCentreX() - knobButtonsLabelBounds.getWidth() * 0.5f).withBottomY (knobButtonsBottomCY + knobButtonsLabelGap).toNearestInt());
}
void MainPanel::resizeMagicButtons(const juce::Rectangle<float>& boundsF)
{
    const auto knobSizeReference = boundsF.getHeight() * 0.33f;
    const auto magicButtonSize = knobSizeReference * 0.33f;
    const auto magicButtonBounds = buttonsArea.withSize (magicButtonSize, magicButtonSize).withCentre({buttonsArea.getCentreX(), lowCutFreqKnob.getBounds().toFloat().getCentreY() * 0.55f});
    mudActiveButton.setBounds (magicButtonBounds.toNearestInt());
    resoActiveButton.setBounds (magicButtonBounds.withRightX(magicButtonBounds.getX()).toNearestInt());
    compActiveButton.setBounds (magicButtonBounds.withX(magicButtonBounds.getRight()).toNearestInt());
    //PLATE BOUNDS
    plateBounds = buttonsArea.withSizeKeepingCentre(buttonsArea.getWidth() * 0.9f, buttonsArea.getHeight() * 0.5f).withY(magicButtonBounds.getBottom() * 1.025f);
    fadersPlatePanel.setFaderSpacing(magicButtonSize);
    fadersPlatePanel.setBounds (plateBounds.toNearestInt());

    const auto switchBounds = plateBounds.withSizeKeepingCentre (plateBounds.getWidth() * 0.3f, knobSizeReference * 0.3f).withY (plateBounds.getBottom());
    mudModeSwitch.setBounds (switchBounds.toNearestInt());
    resoModeSwitch.setBounds (switchBounds.translated (-magicButtonSize, 0.0f).toNearestInt());
    compModeSwitch.setBounds (switchBounds.translated (magicButtonSize, 0.0f).toNearestInt());
}

void MainPanel::resizeLevelsSection(const juce::Rectangle<float>& boundsF)
{
    const auto gainKnobsArea = buttonsArea.withTop (magicDividerBounds.getBottom());
    const auto gainLabelBounds = gainKnobsArea.withSizeKeepingCentre(boundsF.getHeight() * 0.245f,gainKnobsArea.getHeight() * 0.2f);
    levelsLabel.setFont (AAFonts::ibmPlexSansMedium (gainLabelBounds.getHeight() * 0.925f));
    levelsLabel.setBounds(gainLabelBounds.withY(gainKnobsArea.getY() * 1.01f).toNearestInt());

    const auto gainKnobSize = gainKnobsArea.getHeight() * 0.65f;
    const auto gainKnobBounds = gainKnobsArea.withSize (gainKnobSize, gainKnobSize);
    const auto gainKnobCY = buttonsArea.getHeight() * 0.91f;
    inputGainKnob.setBounds    (gainKnobBounds.withCentre({resoActiveButton.getBounds().toFloat().getCentreX(), gainKnobCY}).toNearestInt());
    outputGainKnob.setBounds (gainKnobBounds.withCentre({compActiveButton.getBounds().toFloat().getCentreX(), gainKnobCY}).toNearestInt());

    for (const auto l : { &inputGainLabel, &outputGainLabel})
        l->setFont (AAFonts::ibmPlexSansMedium (gainLabelBounds.getHeight() * 0.65f));
    const auto gainLabelY = gainKnobCY * 1.053f;
    inputGainLabel.setBounds    (gainKnobBounds.withCentre({inputGainKnob.getBounds().toFloat().getCentre()}).withY(gainLabelY) .toNearestInt());
    outputGainLabel.setBounds   (gainKnobBounds.withCentre({outputGainKnob.getBounds().toFloat().getCentre()}).withY(gainLabelY) .toNearestInt());

    const auto matchY = gainKnobCY * JUCE_LIVE_CONSTANT(1.053f);
    const auto matchBounds = gainLabelBounds.withSizeKeepingCentre (gainKnobSize * 0.6f, gainLabelBounds.getHeight() * 2.0f).withY(matchY);
    levelMatchControl.setBounds (matchBounds.toNearestInt());
}
