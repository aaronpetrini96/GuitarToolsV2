#include "MainPanel.h"

MainPanel::MainPanel (GuitarToolsAudioProcessor& processor)
    : audioProcessor (processor)

{
    //================  GROUP TITLES  ================//
    for (auto* l : {&cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel})
        setupLabels(l, GTColors::sectionTitleText, juce::Justification::centred);

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

    //================  LEVEL KNOBS  ================//
    for (auto* l : {&inputGainLabel, &outputGainLabel})
        setupLabels (l, textWhiteColor, juce::Justification::centredTop);

    for (auto* l : {&matchLabel, &levelsLabel})
        setupLabels (l, textWhiteColor, juce::Justification::centred);
    matchLabel.setVisible(false);

    addAndMakeVisible (inputGainKnob);
    addAndMakeVisible (outputGainKnob);
    for (auto* knob : {&inputGainKnob, &outputGainKnob})
    {
        knob->setTooltip (ParamHelpers::getTooltipForParam (knob->getComponentID()));
        knob->setUndoManager (audioProcessor.undoManager);
    }


    levelMatchButton.setClickingTogglesState (true);
    levelMatchButton.setColour (juce::DrawableButton::backgroundColourId, GTColors::transparent);
    levelMatchButton.setColour (juce::DrawableButton::backgroundOnColourId, GTColors::transparent);
    levelMatchButton.setTooltip (ParamHelpers::getTooltipForParam (ParamIDs::getLoudnessMatchActiveName()));
    addAndMakeVisible(levelMatchButton);

    if (const auto linkOnIcon = juce::Drawable::createFromImageData (BinaryData::linkON_svg, BinaryData::linkON_svgSize))
    {
        const auto linkOffIcon = linkOnIcon->createCopy();
        const auto linkOffIconOver = linkOnIcon->createCopy();
        linkOffIconOver->replaceColour (GTColors::activeNeonBlue, GTColors::iconOverColor);
        linkOffIcon->replaceColour (GTColors::activeNeonBlue, GTColors::iconNormalColor);
        levelMatchButton.setImages (linkOffIcon.get(), linkOffIconOver.get(),  linkOnIcon.get(), nullptr, linkOnIcon.get());
    }

    levelMatchButton.onStateChange = [this]
    {
        const auto isMatchEnabled = levelMatchButton.getToggleState();
        outputGainKnob.setEnabled (! isMatchEnabled);
        matchLabel.setColour (juce::Label::textColourId, isMatchEnabled ? GTColors::activeNeonBlue.withAlpha(0.85f) : GTColors::transparent);
    };
    levelMatchButton.onStateChange();
}

void MainPanel::paint(juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto dividerThickness = boundsFWidth * 0.006f;
    const auto dividerHalfThickness = dividerThickness * 0.5f;
    g.setColour (GTColors::dividerLinesStart);
    g.fillRect (0.0f, 0.0f, boundsFWidth, dividerHalfThickness);

    for (const auto& sectionArea : { cutFiltersArea, buttonsArea, shelfFiltersArea })
        UITools::drawRoundedDivider (g, sectionArea.withSizeKeepingCentre (sectionArea.getWidth() * 0.9f, dividerThickness).withY (sectionTitleH));
    UITools::drawRoundedDivider (g, magicDividerBounds);

    const auto verticalDividerBounds = boundsF.withSizeKeepingCentre (dividerThickness, boundsF.getHeight() * 0.97f).translated(0.f, dividerHalfThickness);
    for (const auto& dividerCentreX : { cutFiltersArea.getRight(), shelfFiltersArea.getX() })
        UITools::drawRoundedDivider (g, verticalDividerBounds.withCentre ({ dividerCentreX, verticalDividerBounds.getCentreY() }));


    // g.fillRect (0.0f, boundsF.getHeight() - dividerHalfThickness, boundsFWidth, dividerHalfThickness);
}

void MainPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

//      SECTIONS
    const auto sideSectionWidth = boundsFWidth * 0.33f;
    cutFiltersArea = boundsF.withSize (sideSectionWidth, boundsFHeight);
    shelfFiltersArea = cutFiltersArea.withRightX(boundsFWidth);
    buttonsArea = boundsF.withSizeKeepingCentre (boundsFWidth - sideSectionWidth * 2.f, boundsFHeight);

    // SECTION TITLES
    for (const auto l : {&cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel,})
       l->setFont (GTFonts::ibmPlexSansCondensedSemiBold (boundsFHeight * 0.038f).withExtraKerningFactor(0.16f));
    cutFiltersGroupLabel    .setBounds (cutFiltersArea.withHeight (sectionTitleH).toNearestInt());
    magicButtonsGroupLabel  .setBounds (buttonsArea.withHeight (sectionTitleH).toNearestInt());
    shelfFiltersGroupLabel  .setBounds (shelfFiltersArea.withHeight (sectionTitleH).toNearestInt());

//    CUT FILTERS
    const auto topKnobsCY = boundsFHeight * 0.26f;
    const auto bottomKnobsCY= boundsFHeight * 0.71f;
    const auto knobSizeReference = cutFiltersArea.getWidth() * 0.85f;
    const auto knobBounds= boundsF.withSize (knobSizeReference, knobSizeReference);
    lowCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    highCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());
    lowShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    highShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());

    const auto knobButtonsWidth = knobSizeReference * 0.75f;
    const auto knobButtonsHeight = knobButtonsWidth / lowCutSlopeButtons.getImageAspectRatio();
    const auto knobButtonsBounds = knobBounds.withSize (knobButtonsWidth, knobButtonsHeight);
    const auto knobButtonsGap = (knobSizeReference + knobButtonsHeight) * 0.4725f;
    const auto knobButtonsTopCY = topKnobsCY + knobButtonsGap;
    const auto knobButtonsBottomCY = bottomKnobsCY + knobButtonsGap;
    lowCutSlopeButtons.setBounds (knobButtonsBounds.withCentre({cutFiltersArea.getCentreX(), knobButtonsTopCY}).toNearestInt());
    highCutSlopeButtons.setBounds (knobButtonsBounds.withCentre({cutFiltersArea.getCentreX(), knobButtonsBottomCY}).toNearestInt());
    depthFreqButtons.setBounds (knobButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), knobButtonsTopCY }).toNearestInt());
    presenceFreqButtons.setBounds (knobButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), knobButtonsBottomCY }).toNearestInt());

    const auto knobButtonsLabelBounds = cutFiltersArea.withSizeKeepingCentre(knobButtonsWidth, knobButtonsHeight * 0.5f);
    for (const auto l : {&lowCutSlopeLabel, &highCutSlopeLabel, &presenceModeLabel, &depthModeLabel})
        l->setFont (GTFonts::ibmPlexSansMedium (knobButtonsLabelBounds.getHeight() * 0.9f));

    const auto knobButtonsLabelGap = knobButtonsHeight * 1.05f;
    lowCutSlopeLabel.setBounds(knobButtonsLabelBounds.withBottomY(knobButtonsTopCY + knobButtonsLabelGap).toNearestInt());
    highCutSlopeLabel.setBounds(knobButtonsLabelBounds.withBottomY(knobButtonsBottomCY + knobButtonsLabelGap).toNearestInt());
    depthModeLabel.setBounds (knobButtonsLabelBounds.withX (shelfFiltersArea.getCentreX() - knobButtonsLabelBounds.getWidth() * 0.5f).withBottomY (knobButtonsTopCY + knobButtonsLabelGap).toNearestInt());
    presenceModeLabel.setBounds (knobButtonsLabelBounds.withX (shelfFiltersArea.getCentreX() - knobButtonsLabelBounds.getWidth() * 0.5f).withBottomY (knobButtonsBottomCY + knobButtonsLabelGap).toNearestInt());

//  MAGIC BUTTONS
    const auto magicButtonSize = knobSizeReference * 0.325f;
    const auto magicButtonBounds = buttonsArea.withSizeKeepingCentre (magicButtonSize, magicButtonSize).withY(boundsFHeight * 0.095f);
    const auto magicButtonsGap = magicButtonSize * 0.05f;
    mudActiveButton.setBounds (magicButtonBounds.toNearestInt());
    resoActiveButton.setBounds (magicButtonBounds.withRightX(magicButtonBounds.getX() - magicButtonsGap).toNearestInt());
    compActiveButton.setBounds (magicButtonBounds.withX(magicButtonBounds.getRight() + magicButtonsGap).toNearestInt());

    magicDividerBounds = buttonsArea.withSizeKeepingCentre (buttonsArea.getWidth() * 0.9f, boundsFWidth * 0.006f).withY (buttonsArea.getHeight() * 0.82f);

    // MAGIC FADERS
    plateBounds = buttonsArea.withHeight(buttonsArea.getHeight() * 0.575f).withCentre({buttonsArea.getCentreX(), buttonsArea.getHeight() * 0.51f});
    fadersPlatePanel.setFaderSpacing (magicButtonSize + magicButtonsGap);
    fadersPlatePanel.setBounds (plateBounds.toNearestInt());

//  LEVEL KNOBS
    const auto gainKnobsArea = buttonsArea.withTop (magicDividerBounds.getBottom());
    const auto gainLabelBounds = gainKnobsArea.withSizeKeepingCentre(gainKnobsArea.getWidth() * 0.5f, gainKnobsArea.getHeight() * 0.2f);
    levelsLabel.setFont (GTFonts::ibmPlexSansMedium (gainLabelBounds.getHeight() * 0.925f));
    levelsLabel.setBounds(gainLabelBounds.withY(gainKnobsArea.getY() * 1.01f).toNearestInt());

    const auto gainKnobSize = gainKnobsArea.getHeight() * 0.65f;
    const auto gainKnobBounds = gainKnobsArea.withSize (gainKnobSize, gainKnobSize);
    const auto gainKnobCY = buttonsArea.getHeight() * 0.91f;
    inputGainKnob.setBounds    (gainKnobBounds.withCentre({resoActiveButton.getBounds().toFloat().getCentreX(), gainKnobCY}).toNearestInt());
    outputGainKnob.setBounds (gainKnobBounds.withCentre({compActiveButton.getBounds().toFloat().getCentreX(), gainKnobCY}).toNearestInt());

    for (const auto l : { &inputGainLabel, &outputGainLabel})
        l->setFont (GTFonts::ibmPlexSansMedium (gainLabelBounds.getHeight() * 0.8f));
    const auto gainLabelY = gainKnobCY * 1.053f;
    inputGainLabel.setBounds    (gainKnobBounds.withCentre({inputGainKnob.getBounds().toFloat().getCentre()}).withY(gainLabelY) .toNearestInt());
    outputGainLabel.setBounds   (gainKnobBounds.withCentre({outputGainKnob.getBounds().toFloat().getCentre()}).withY(gainLabelY) .toNearestInt());

    const auto matchButtonSize = buttonsArea.getWidth() * 0.12f;
    levelMatchButton.setBounds (gainKnobsArea.withSize(matchButtonSize, matchButtonSize).withCentre({gainKnobsArea.getCentreX(), gainKnobCY}).toNearestInt());
    matchLabel.setFont (GTFonts::ibmPlexSansMedium (gainLabelBounds.getHeight() * 0.65f));
    matchLabel.setBounds(gainLabelBounds.withY(gainKnobCY * 1.01f).toNearestInt());
}

void MainPanel::setupLabels(juce::Label* l, juce::Colour color, juce::Justification justification)
{
    l->setJustificationType (justification);
    l->setColour (juce::Label::textColourId, color);
    l->setInterceptsMouseClicks(false, false);
    l->setMinimumHorizontalScale(1.f);
    addAndMakeVisible (*l);
}
