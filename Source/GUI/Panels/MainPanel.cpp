#include "MainPanel.h"

MainPanel::MainPanel (GuitarToolsAudioProcessor& processor) : audioProcessor (processor)
{
    //================  GROUP TITLES  ================//
    for (auto* l : {&cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel})
        setupLabels(l, GTColors::sectionTitleText, juce::Justification::centred);

    //================  CUT FILTERS ================//
    addAndMakeVisible (lowCutFreqKnob);
    addAndMakeVisible (highCutFreqKnob);
    addAndMakeVisible (lowCutSlopeButtons);
    addAndMakeVisible (highCutSlopeButtons);

    //================  SHELF FILTERS  ================//
    addAndMakeVisible(highShelfGainKnob);
    addAndMakeVisible(lowShelfGainKnob);
    addAndMakeVisible (presenceFreqButtons);
    addAndMakeVisible (depthFreqButtons);

    //================  MAGIC BUTTONS  ================//
    // Labels
    for (auto* l : {&resoBypassLabel, &compBypassLabel, &mudBypassLabel})
        setupLabels (l, GTColors::clearGrey.withAlpha(0.95f), juce::Justification::centredTop);
    //buttons
    addAndMakeVisible (resoButton);
    addAndMakeVisible (mudButton);
    addAndMakeVisible (compBypassButton);
    addAndMakeVisible (fadersPlatePanel);

    resoButton.setOnStateChange ([this]      { fadersPlatePanel.resoFreqFader.setEnabled (resoButton.getToggleState()); });
    mudButton.setOnStateChange ([this]       { fadersPlatePanel.mudFreqFader.setEnabled (mudButton.getToggleState()); });
    compBypassButton.setOnStateChange ([this]{ fadersPlatePanel.compThresholdFader.setEnabled (compBypassButton.getToggleState()); });

    //================  LEVEL KNOBS  ================//
    for (auto* l : {&inputGainLabel, &outputGainLabel})
        setupLabels (l, GTColors::clearGrey.withAlpha(0.95f), juce::Justification::centred);
    addAndMakeVisible (inputGainKnob);
    addAndMakeVisible (outputGainKnob);
}

void MainPanel::paint(juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto dividerThickness = boundsFWidth * 0.006f;

    // const auto lineThickness = boundsFWidth * 0.004f;
    // g.setColour (GTColors::dividerLinesEnd);
    // g.fillRect (0.0f, boundsF.getHeight() - lineThickness, boundsFWidth, lineThickness);

    const auto cutFiltersDivider   = cutFiltersArea.withSizeKeepingCentre (cutFiltersArea.getWidth() * 0.9f, dividerThickness).withY (sectionTitleH);
    const auto buttonsDivider      = buttonsArea.withSizeKeepingCentre (buttonsArea.getWidth() * 0.9f, dividerThickness).withY (sectionTitleH);
    const auto shelfFiltersDivider = shelfFiltersArea.withSizeKeepingCentre (shelfFiltersArea.getWidth() * 0.9f, dividerThickness).withY (sectionTitleH);

    for (const auto& sectionDivider : { cutFiltersDivider, buttonsDivider, shelfFiltersDivider })
        drawDivider (g, sectionDivider);

    drawDivider (g, magicDividerBounds);

    const auto verticalDividerBounds = boundsF.withSizeKeepingCentre (dividerThickness, boundsF.getHeight() * 0.975f);

    g.setColour (GTColors::dividerLinesEnd);
    for (const auto& dividerCentreX : { (cutFiltersDivider.getRight() + buttonsDivider.getX()) * 0.5f,
                                        (buttonsDivider.getRight() + shelfFiltersDivider.getX()) * 0.5f })
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
    const auto topKnobsCY = boundsFHeight * 0.275f;
    const auto bottomKnobsCY= boundsFHeight * 0.72f;
    const auto knobSizeReference = cutFiltersArea.getWidth() * 0.85f;
    const auto knobBounds= boundsF.withSize (knobSizeReference, knobSizeReference);
    lowCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    highCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());

    const auto slopeButtonsWidth = knobSizeReference * 0.75f;
    const auto slopeButtonsHeight = slopeButtonsWidth / lowCutSlopeButtons.getImageAspectRatio();
    const auto slopeButtonsBounds = knobBounds.withSize (slopeButtonsWidth, slopeButtonsHeight);
    const auto slopeGap = knobSizeReference * 0.5f + slopeButtonsHeight * 0.75f;
    const auto slopeButtonsTopCY = topKnobsCY + slopeGap;
    const auto slopeButtonsBottomCY = bottomKnobsCY + slopeGap;
    lowCutSlopeButtons.setBounds (slopeButtonsBounds.withCentre({cutFiltersArea.getCentreX(), slopeButtonsTopCY}).toNearestInt());
    highCutSlopeButtons.setBounds (slopeButtonsBounds.withCentre({cutFiltersArea.getCentreX(), slopeButtonsBottomCY}).toNearestInt());

    //SHELF FILTERS
    highShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    lowShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());
    presenceFreqButtons.setBounds (slopeButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), slopeButtonsTopCY }).toNearestInt());
    depthFreqButtons.setBounds (slopeButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), slopeButtonsBottomCY }).toNearestInt());

//  MAGIC BUTTONS
    const auto magicButtonSize = knobSizeReference * 0.275f;
    const auto magicButtonBounds = buttonsArea.withSizeKeepingCentre (magicButtonSize, magicButtonSize).withY(boundsFHeight * 0.2f);
    const auto gap = magicButtonSize * 0.125f;
    mudButton.setBounds (magicButtonBounds.toNearestInt());
    resoButton.setBounds (magicButtonBounds.withRightX(magicButtonBounds.getX() - gap).toNearestInt());
    compBypassButton.setBounds (magicButtonBounds.withX(magicButtonBounds.getRight() + gap).toNearestInt());

    const auto magicLabelsBounds = buttonsArea.withSize (buttonsArea.getWidth() * 0.33f, buttonsArea.getWidth() * 0.2f).withY (boundsFHeight * 0.111f);
    for (const auto l : { &mudBypassLabel, &resoBypassLabel, &compBypassLabel })
        l->setFont (GTFonts::ibmPlexSansMedium (knobSizeReference * 0.1f).withExtraKerningFactor(-0.025f));

    mudBypassLabel.setBounds    (magicLabelsBounds.withX(mudButton.getBounds().toFloat().getCentreX() - magicLabelsBounds.getWidth() * 0.5f).toNearestInt());
    resoBypassLabel.setBounds   (magicLabelsBounds.withX(resoButton.getBounds().toFloat().getCentreX() - magicLabelsBounds.getWidth() * 0.5f).toNearestInt());
    compBypassLabel.setBounds   (magicLabelsBounds.withX(compBypassButton.getBounds().toFloat().getCentreX() - magicLabelsBounds.getWidth() * 0.5f).toNearestInt());

    magicDividerBounds = buttonsArea.withSizeKeepingCentre (buttonsArea.getWidth() * 0.9f, boundsFWidth * 0.006f).withY (buttonsArea.getHeight() * 0.79f);

    // MAGIC FADERS
    plateBounds = buttonsArea.withHeight(buttonsArea.proportionOfHeight (0.5f))
                .withCentre({buttonsArea.getCentreX(), (magicButtonBounds.getBottom() + magicDividerBounds.getY()) * 0.5f});
    fadersPlatePanel.setFaderSpacing (magicButtonSize + gap);
    fadersPlatePanel.setBounds (plateBounds.toNearestInt());

//    GAIN KNOBS
    auto gainKnobsArea = buttonsArea.withTop (magicDividerBounds.getBottom());
    const auto gainLabelBounds = gainKnobsArea.removeFromTop(gainKnobsArea.getHeight() * 0.2f).withWidth(gainKnobsArea.getWidth() * 0.33f);
    const auto gainKnobSize = gainKnobsArea.getHeight() * 0.85f;
    const auto gainKnobBounds = gainKnobsArea.withSizeKeepingCentre (gainKnobSize, gainKnobSize);
    const auto gainKnobOffsetX = gainKnobSize * 0.85f;
    inputGainKnob.setBounds (gainKnobBounds.translated (-gainKnobOffsetX, 0.0f).toNearestInt());
    outputGainKnob.setBounds (gainKnobBounds.translated (gainKnobOffsetX, 0.0f).toNearestInt());

    for (const auto l : { &inputGainLabel, &outputGainLabel })
        l->setFont (GTFonts::ibmPlexSansMedium (gainLabelBounds.getHeight() * 0.75f));
    inputGainLabel.setBounds (gainLabelBounds.withCentre({inputGainKnob.getBounds().toFloat().getCentreX(), gainLabelBounds.getCentreY()}) .toNearestInt());
    outputGainLabel.setBounds (gainLabelBounds.withCentre({outputGainKnob.getBounds().toFloat().getCentreX(), gainLabelBounds.getCentreY()}) .toNearestInt());
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