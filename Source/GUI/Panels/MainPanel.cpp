#include "MainPanel.h"

MainPanel::MainPanel (GuitarToolsAudioProcessor& processor)
    : audioProcessor (processor),
      loudnessMatchAttachment (processor.treeState, ParamIDs::getLoudnessMatchName(), levelMatchButton)
{
    //================  GROUP TITLES  ================//
    for (auto* l : {&cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel})
        setupLabels(l, GTColors::sectionTitleText, juce::Justification::centred);

    //================  CUT FILTERS ================//
    addAndMakeVisible (lowCutFreqKnob);
    lowCutFreqKnob.setTooltip(ParamTooltips::getLowCutKnobTooltip());
    addAndMakeVisible (highCutFreqKnob);
    addAndMakeVisible (lowCutSlopeButtons);
    addAndMakeVisible (highCutSlopeButtons);

    setupLabels (&lowCutSlopeLabel,     textWhiteColor, juce::Justification::centred);
    setupLabels (&highCutSlopeLabel,    textWhiteColor, juce::Justification::centred);

    //================  SHELF FILTERS  ================//
    addAndMakeVisible(highShelfGainKnob);
    addAndMakeVisible(lowShelfGainKnob);
    addAndMakeVisible (presenceFreqButtons);
    addAndMakeVisible (depthFreqButtons);
    setupLabels (&presenceModeLabel,    textWhiteColor, juce::Justification::centred);
    setupLabels (&depthModeLabel,       textWhiteColor, juce::Justification::centred);

    //================  MAGIC BUTTONS  ================//
    for (auto* l : {&resoBypassLabel, &compBypassLabel, &mudBypassLabel})
        setupLabels (l, textWhiteColor, juce::Justification::centredTop);
    addAndMakeVisible (resoButton);
    addAndMakeVisible (mudButton);
    addAndMakeVisible (compBypassButton);
    addAndMakeVisible (fadersPlatePanel);

    resoButton.setOnStateChange ([this]      { fadersPlatePanel.resoFreqFader.setEnabled (resoButton.getToggleState()); });
    mudButton.setOnStateChange ([this]       { fadersPlatePanel.mudFreqFader.setEnabled (mudButton.getToggleState()); });
    compBypassButton.setOnStateChange ([this]{ fadersPlatePanel.compThresholdFader.setEnabled (compBypassButton.getToggleState()); });


    //================  LEVEL KNOBS  ================//
    for (auto* l : {&inputGainLabel, &outputGainLabel})
        setupLabels (l, textWhiteColor, juce::Justification::centredTop);
    for (auto* l : {&matchLabel, &levelsLabel})
        setupLabels (l, textWhiteColor, juce::Justification::centred);
    matchLabel.setVisible(false);
    addAndMakeVisible (inputGainKnob);
    addAndMakeVisible (outputGainKnob);


    levelMatchButton.setClickingTogglesState (true);
    levelMatchButton.setColour (juce::DrawableButton::backgroundColourId, GTColors::transparent);
    levelMatchButton.setColour (juce::DrawableButton::backgroundOnColourId, GTColors::transparent);
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
        matchLabel.setColour (juce::Label::textColourId, isMatchEnabled ? GTColors::activeNeonBlue : textWhiteColor);
    };
    levelMatchButton.onStateChange();
}

void MainPanel::paint(juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto dividerThickness = boundsFWidth * 0.006f;

    g.setColour (GTColors::dividerLinesEnd);
    g.fillRect (0.0f, 0.0f, boundsFWidth, dividerThickness * 0.5f);

    for (const auto& sectionArea : { cutFiltersArea, buttonsArea, shelfFiltersArea })
        drawDivider (g, sectionArea.withSizeKeepingCentre (sectionArea.getWidth() * 0.9f, dividerThickness).withY (sectionTitleH));

    drawDivider (g, magicDividerBounds);

    const auto verticalDividerBounds = boundsF.withSizeKeepingCentre (dividerThickness, boundsF.getHeight() * 0.975f);

    g.setColour (GTColors::dividerLinesEnd);
    for (const auto& dividerCentreX : { cutFiltersArea.getRight(), shelfFiltersArea.getX() })
        g.fillRoundedRectangle (verticalDividerBounds.withCentre ({ dividerCentreX, verticalDividerBounds.getCentreY() }), dividerThickness * 0.5f);

}

void MainPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

//      SECTIONS
    const auto sideSectionWidth = boundsFWidth * 0.33f;
    // const auto sideSectionWidth = boundsFWidth * 0.317f;
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

    const auto slopeButtonsWidth = knobSizeReference * 0.75f;
    const auto slopeButtonsHeight = slopeButtonsWidth / lowCutSlopeButtons.getImageAspectRatio();
    const auto slopeButtonsBounds = knobBounds.withSize (slopeButtonsWidth, slopeButtonsHeight);
    const auto slopeGap = knobSizeReference * 0.5f + slopeButtonsHeight * 0.5f;
    const auto slopeButtonsTopCY = topKnobsCY + slopeGap;
    const auto slopeButtonsBottomCY = bottomKnobsCY + slopeGap;
    lowCutSlopeButtons.setBounds (slopeButtonsBounds.withCentre({cutFiltersArea.getCentreX(), slopeButtonsTopCY}).toNearestInt());
    highCutSlopeButtons.setBounds (slopeButtonsBounds.withCentre({cutFiltersArea.getCentreX(), slopeButtonsBottomCY}).toNearestInt());

    const auto slopeLabelBounds = cutFiltersArea.withSizeKeepingCentre(slopeButtonsWidth, slopeButtonsHeight * 0.5f);
    lowCutSlopeLabel.setFont (GTFonts::ibmPlexSansMedium (slopeButtonsHeight * 0.4f));
    highCutSlopeLabel.setFont (GTFonts::ibmPlexSansMedium (slopeButtonsHeight * 0.4f));
    presenceModeLabel.setFont (GTFonts::ibmPlexSansMedium (slopeButtonsHeight * 0.4f));
    depthModeLabel.setFont (GTFonts::ibmPlexSansMedium (slopeButtonsHeight * 0.4f));
    lowCutSlopeLabel.setBounds(slopeLabelBounds.withBottomY(slopeButtonsTopCY + slopeButtonsHeight).toNearestInt());
    highCutSlopeLabel.setBounds(slopeLabelBounds.withBottomY(slopeButtonsBottomCY + slopeButtonsHeight).toNearestInt());

    //SHELF FILTERS
    highShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    lowShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());
    presenceFreqButtons.setBounds (slopeButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), slopeButtonsTopCY }).toNearestInt());
    depthFreqButtons.setBounds (slopeButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), slopeButtonsBottomCY }).toNearestInt());
    presenceModeLabel.setBounds (slopeLabelBounds.withX (shelfFiltersArea.getCentreX() - slopeLabelBounds.getWidth() * 0.5f).withBottomY (slopeButtonsTopCY + slopeButtonsHeight).toNearestInt());
    depthModeLabel.setBounds (slopeLabelBounds.withX (shelfFiltersArea.getCentreX() - slopeLabelBounds.getWidth() * 0.5f).withBottomY (slopeButtonsBottomCY + slopeButtonsHeight).toNearestInt());

//  MAGIC BUTTONS
    const auto magicButtonSize = knobSizeReference * 0.3f;
    const auto magicButtonBounds = buttonsArea.withSizeKeepingCentre (magicButtonSize, magicButtonSize).withY(boundsFHeight * 0.18f);
    const auto gap = magicButtonSize * 0.15f;
    mudButton.setBounds (magicButtonBounds.toNearestInt());
    resoButton.setBounds (magicButtonBounds.withRightX(magicButtonBounds.getX() - gap).toNearestInt());
    compBypassButton.setBounds (magicButtonBounds.withX(magicButtonBounds.getRight() + gap).toNearestInt());

    const auto magicLabelsBounds = buttonsArea.withSize (buttonsArea.getWidth() * 0.33f, buttonsArea.getWidth() * 0.2f).withY (boundsFHeight * 0.105f);
    for (const auto l : { &mudBypassLabel, &resoBypassLabel, &compBypassLabel })
        l->setFont (GTFonts::ibmPlexSansMedium (knobSizeReference * 0.092f).withExtraKerningFactor(-0.025f));

    mudBypassLabel.setBounds    (magicLabelsBounds.withX(mudButton.getBounds().toFloat().getCentreX() - magicLabelsBounds.getWidth() * 0.5f).toNearestInt());
    resoBypassLabel.setBounds   (magicLabelsBounds.withX(resoButton.getBounds().toFloat().getCentreX() - magicLabelsBounds.getWidth() * 0.5f).toNearestInt());
    compBypassLabel.setBounds   (magicLabelsBounds.withX(compBypassButton.getBounds().toFloat().getCentreX() - magicLabelsBounds.getWidth() * 0.5f).toNearestInt());

    magicDividerBounds = buttonsArea.withSizeKeepingCentre (buttonsArea.getWidth() * 0.9f, boundsFWidth * 0.006f).withY (buttonsArea.getHeight() * 0.8f);

    // MAGIC FADERS
    plateBounds = buttonsArea.withHeight(buttonsArea.proportionOfHeight (0.475f)).withCentre({buttonsArea.getCentreX(), buttonsArea.getHeight() * 0.535f});
    fadersPlatePanel.setFaderSpacing (magicButtonSize + gap);
    fadersPlatePanel.setBounds (plateBounds.toNearestInt());

//  LEVEL KNOBS
    const auto gainKnobsArea = buttonsArea.withTop (magicDividerBounds.getBottom());
    const auto gainLabelBounds = gainKnobsArea.withSizeKeepingCentre(gainKnobsArea.getWidth() * 0.5f, gainKnobsArea.getHeight() * 0.2f);
    levelsLabel.setFont (GTFonts::ibmPlexSansMedium (gainLabelBounds.getHeight() * 0.875f));
    levelsLabel.setBounds(gainLabelBounds.withY(buttonsArea.getHeight() * 0.81f).toNearestInt());

    const auto gainKnobSize = gainKnobsArea.getHeight() * 0.675f;
    const auto gainKnobBounds = gainKnobsArea.withSize (gainKnobSize, gainKnobSize);
    const auto gainKnobCY = buttonsArea.getHeight() * 0.91f;
    inputGainKnob.setBounds    (gainKnobBounds.withCentre({resoButton.getBounds().toFloat().getCentreX(), gainKnobCY}).toNearestInt());
    outputGainKnob.setBounds (gainKnobBounds.withCentre({compBypassButton.getBounds().toFloat().getCentreX(), gainKnobCY}).toNearestInt());

    const auto matchButtonSize = buttonsArea.getWidth() * 0.115f;
    levelMatchButton.setBounds (gainKnobsArea.withSize(matchButtonSize, matchButtonSize).withCentre({gainKnobsArea.getCentreX(), gainKnobCY}).toNearestInt());

    for (const auto l : { &inputGainLabel, &outputGainLabel})
        l->setFont (GTFonts::ibmPlexSansMedium (gainLabelBounds.getHeight() * 0.65f));

    matchLabel.setFont (GTFonts::ibmPlexSansMedium (gainLabelBounds.getHeight() * 0.65f));

    const auto gainLabelY = gainKnobCY * 1.053f;
    inputGainLabel.setBounds    (gainKnobBounds.withCentre({inputGainKnob.getBounds().toFloat().getCentre()}).withY(gainLabelY) .toNearestInt());
    outputGainLabel.setBounds   (gainKnobBounds.withCentre({outputGainKnob.getBounds().toFloat().getCentre()}).withY(gainLabelY) .toNearestInt());
}

void MainPanel::drawDivider(juce::Graphics& g, juce::Rectangle<float> dividerBounds)
{
    const auto cornerSize = dividerBounds.getHeight() * 0.5f;
    const auto outlineThickness = dividerBounds.getHeight() * 0.125f;
    const auto outlineExpansion = outlineThickness * 0.5f;

    g.setColour (GTColors::dividerLinesEnd);
    g.fillRoundedRectangle (dividerBounds, cornerSize);
    g.setColour (GTColors::dividerLinesEnd.brighter (0.1f));
    g.drawRoundedRectangle (dividerBounds.expanded (outlineExpansion), cornerSize + outlineExpansion, outlineThickness);
}

void MainPanel::setupLabels(juce::Label* l, juce::Colour color, juce::Justification justification)
{
    l->setJustificationType (justification);
    l->setColour (juce::Label::textColourId, color);
    l->setInterceptsMouseClicks(false, false);
    l->setMinimumHorizontalScale(1.f);
    addAndMakeVisible (*l);
}
