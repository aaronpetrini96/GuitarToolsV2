#include "MainPanel.h"

MainPanel::MainPanel (GuitarToolsAudioProcessor& processor)
    : audioProcessor (processor)

{
    //================  GROUP TITLES  ================//
    // for (auto* l : {&cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel})
    //     setupLabels(l, GTColors::sectionTitleText.withAlpha(0.9f), juce::Justification::centred);

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

    const auto setupModeSwitch = [this] (juce::DrawableButton& modeSwitch, std::unique_ptr<juce::ButtonParameterAttachment>& attachment, const juce::String& paramID)
    {
        modeSwitch.setImages (modeSwitchOff.get(), nullptr, nullptr, nullptr, modeSwitchOn.get());
        modeSwitch.setClickingTogglesState (true);
        modeSwitch.setColour (juce::DrawableButton::backgroundColourId, GTColors::transparent);
        modeSwitch.setColour (juce::DrawableButton::backgroundOnColourId, GTColors::transparent);
        modeSwitch.setTooltip (ParamHelpers::getTooltipForParam (paramID));
        attachment = std::make_unique<juce::ButtonParameterAttachment> (*audioProcessor.treeState.getParameter (paramID), modeSwitch, &audioProcessor.undoManager);
        attachment->sendInitialUpdate();
        addAndMakeVisible (modeSwitch);
    };

    setupModeSwitch (resoModeSwitch, resoModeSwitchAttachment, ParamIDs::getResoExtremeActiveName());
    setupModeSwitch (mudModeSwitch, mudModeSwitchAttachment, ParamIDs::getMudExtremeActiveName());
    setupModeSwitch (compModeSwitch, compModeSwitchAttachment, ParamIDs::getCompExtremeActiveName());

    //================  LEVEL KNOBS  ================//
    for (auto* l : {&inputGainLabel, &outputGainLabel})
        setupLabels (l, textWhiteColor, juce::Justification::centredTop);

    // for (auto* l : {&levelsLabel, &inputTitleLabel, &outputTitleLabel})
    //     setupLabels (l, textWhiteColor, juce::Justification::centred);
    // inputTitleLabel.setVisible(false);
    // outputTitleLabel.setVisible(false);

    addAndMakeVisible (inputGainKnob);
    addAndMakeVisible (outputGainKnob);
    for (auto* knob : {&inputGainKnob, &outputGainKnob})
    {
        knob->setTooltip (ParamHelpers::getTooltipForParam (knob->getComponentID()));
        knob->setUndoManager (audioProcessor.undoManager);
    }

    addAndMakeVisible(outputLevelMeter);
    addAndMakeVisible(inputLevelMeter);

    levelMatchButton.setImages (levelMatchButtonOff.get(), nullptr, nullptr, nullptr, levelMatchButtonOn.get());
    levelMatchButton.setClickingTogglesState (true);
    levelMatchButton.setColour (juce::DrawableButton::backgroundColourId, GTColors::transparent);
    levelMatchButton.setColour (juce::DrawableButton::backgroundOnColourId, GTColors::transparent);
    levelMatchButton.setTooltip (ParamHelpers::getTooltipForParam (ParamIDs::getLoudnessMatchActiveName()));
    levelMatchButtonAttachment = std::make_unique<juce::ButtonParameterAttachment> (*audioProcessor.treeState.getParameter (ParamIDs::getLoudnessMatchActiveName()), levelMatchButton, &audioProcessor.undoManager);
    levelMatchButtonAttachment->sendInitialUpdate();
    levelMatchButton.onClick = [this] { outputGainKnob.setEnabled (! levelMatchButton.getToggleState()); };
    outputGainKnob.setEnabled (! levelMatchButton.getToggleState());
    addAndMakeVisible (levelMatchButton);
}

void MainPanel::paint(juce::Graphics& g)
{
    // const auto boundsF = getLocalBounds().toFloat();
    // const auto dividerHalf = dividerThickness / 2.0f;
    // const auto dividerGap = dividerThickness * 4.f;
    //
    // for (const auto& sectionArea : { cutFiltersArea, shelfFiltersArea })
    //     AAUITools::drawRoundedDivider (g, sectionArea.withSizeKeepingCentre (sectionArea.getWidth() - dividerHalf - dividerGap, dividerThickness).withY (sectionTitleH - dividerHalf));
    //
    // const juce::Rectangle<float> buttonsAreaDividerBounds {buttonsArea.withSizeKeepingCentre(buttonsArea.getWidth() - dividerGap * 0.5f, dividerThickness)};
    // AAUITools::drawRoundedDivider(g, buttonsAreaDividerBounds.withY(sectionTitleH - dividerHalf));
    // AAUITools::drawRoundedDivider (g, magicDividerBounds.withSizeKeepingCentre(buttonsAreaDividerBounds.getWidth(), dividerThickness));
    //
    // const auto titleLineThickness = dividerThickness * 0.31f;
    //
    // g.setColour (GTColors::sectionTitleText.withAlpha (0.9f));
    // g.fillRect(cutFiltersArea.withSizeKeepingCentre( cutFiltersArea.getWidth() * 0.925f, titleLineThickness).withY(sectionTitleH));
}

void MainPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();
    // dividerThickness = boundsF.getHeight() * 0.008f;
    const auto sideSectionWidth = boundsFWidth * sideSectionPOW;
    cutFiltersArea = boundsF.withSize (sideSectionWidth, boundsFHeight);
    shelfFiltersArea = cutFiltersArea.withRightX(boundsFWidth);
    buttonsArea = boundsF.withSizeKeepingCentre (boundsFWidth - sideSectionWidth * 2.f, boundsFHeight);

    // for (const auto l : {&cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel})
    //    l->setFont (AAFonts::ibmPlexSansSemiBold (boundsFHeight * 0.04f).withExtraKerningFactor(0.1f));
    //
    // cutFiltersGroupLabel    .setBounds (cutFiltersArea.withHeight (sectionTitleH).toNearestInt());
    // magicButtonsGroupLabel  .setBounds (buttonsArea.withHeight (sectionTitleH).toNearestInt());
    // shelfFiltersGroupLabel  .setBounds (shelfFiltersArea.withHeight (sectionTitleH).toNearestInt());

    resizeKnobs(boundsF);
    resizeMagicButtons(boundsF);
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
    const auto magicButtonSize = knobSizeReference * 0.325f;
    const auto magicButtonsGap = magicButtonSize * 1.05f;
    const auto magicButtonBounds = buttonsArea.withSize (magicButtonSize, magicButtonSize).withCentre({buttonsArea.getCentreX(), lowCutFreqKnob.getBounds().toFloat().getCentreY() * 0.55f});
    mudActiveButton.setBounds (magicButtonBounds.toNearestInt());
    resoActiveButton.setBounds (magicButtonBounds.translated(-magicButtonsGap, 0.f).toNearestInt());
    compActiveButton.setBounds (magicButtonBounds.translated(magicButtonsGap, 0.f).toNearestInt());

    //PLATE BOUNDS
    plateBounds = buttonsArea.withSizeKeepingCentre(buttonsArea.getWidth() * 0.9f, buttonsArea.getHeight() * 0.475f).withY(magicButtonBounds.getBottom() * 1.05f);
    fadersPlatePanel.setFaderSpacing(magicButtonsGap);
    fadersPlatePanel.setBounds (plateBounds.toNearestInt());

    const auto modeSwitchHeight = knobSizeReference * 0.2f;
    const auto modeSwitchWidth = modeSwitchHeight * modeSwitchOff->getDrawableBounds().getAspectRatio();
    const auto switchBounds = plateBounds.withSizeKeepingCentre (modeSwitchWidth, modeSwitchHeight).withY (plateBounds.getBottom() * 0.98f);
    mudModeSwitch.setBounds (switchBounds.toNearestInt());
    resoModeSwitch.setBounds (switchBounds.translated (-magicButtonsGap, 0.0f).toNearestInt());
    compModeSwitch.setBounds (switchBounds.translated (magicButtonsGap, 0.0f).toNearestInt());

    // magicDividerBounds = buttonsArea.withSizeKeepingCentre (plateBounds.getWidth(), dividerThickness).withY (switchBounds.getBottom());
}

void MainPanel::resizeLevelsSection(const juce::Rectangle<float>& boundsF)
{
    const auto gainKnobsArea = buttonsArea.withTop (boundsF.getHeight() * 0.725f);
    const auto gainLabelBounds = gainKnobsArea.withSizeKeepingCentre(gainKnobsArea.getWidth() * 0.5f,gainKnobsArea.getHeight() * 0.15f);

    // for (auto* l : {&levelsLabel, &inputTitleLabel, &outputTitleLabel})
    //     setIbmMediumFont (*l, gainLabelBounds.getHeight());
    //
    // const auto gainTitleBounds = gainLabelBounds.withY (gainKnobsArea.getY() * 1.01f);
    // levelsLabel.setBounds(gainTitleBounds.toNearestInt());

    const auto gainKnobSize = gainKnobsArea.getHeight() * 0.5f;
    const auto gainKnobBounds = gainKnobsArea.withSizeKeepingCentre (gainKnobSize, gainKnobSize);
    const auto gainKnobGap = gainKnobSize * JUCE_LIVE_CONSTANT(0.6f);
    const auto inputKnobBounds = gainKnobBounds.translated(-gainKnobGap, 0.f);
    const auto outputKnobBounds = gainKnobBounds.translated(gainKnobGap, 0.f);
    inputGainKnob.setBounds (inputKnobBounds.toNearestInt());
    outputGainKnob.setBounds(outputKnobBounds.toNearestInt());

    // const auto knobTitleLabelsCY = gainTitleBounds.getCentreY();
    // inputTitleLabel.setBounds (gainTitleBounds.withCentre ({gainKnobBounds.getCentreX() - gainKnobGap, knobTitleLabelsCY}).toNearestInt());
    // outputTitleLabel.setBounds (gainTitleBounds.withCentre ({gainKnobBounds.getCentreX() + gainKnobGap, knobTitleLabelsCY}).toNearestInt());

    // for (const auto l : { &inputGainLabel, &outputGainLabel})
    //     setIbmMediumFont (*l, gainLabelBounds.getHeight() * 0.65f);
    //
    // const auto gainValueLabelY = gainKnobBounds.getBottom() * 0.99f;
    // inputGainLabel.setBounds    (inputKnobBounds.withHeight(gainLabelBounds.getHeight()).withY(gainValueLabelY) .toNearestInt());
    // outputGainLabel.setBounds   (outputKnobBounds.withHeight(gainLabelBounds.getHeight()).withY(gainValueLabelY) .toNearestInt());

    const auto levelsBottomY = gainKnobsArea.getBottom() * 0.99f;
    const auto levelMeterBounds = gainKnobsArea.withSize(gainKnobsArea.getWidth() * 0.1f,levelsBottomY - gainKnobBounds.getY()).withBottomY(levelsBottomY);
    const auto inputMeterCX = buttonsArea.getX() + 0.5f * (inputKnobBounds.getX() - buttonsArea.getX());
    const auto outputMeterCX = outputKnobBounds.getRight() + 0.5f * (buttonsArea.getRight() - outputKnobBounds.getRight());
    inputLevelMeter.setBounds (levelMeterBounds.withX(inputMeterCX - levelMeterBounds.getWidth() * 0.5f).toNearestInt());
    outputLevelMeter.setBounds (levelMeterBounds.withX(outputMeterCX - levelMeterBounds.getWidth() * 0.5f).toNearestInt());

    const auto levelMatchW = levelMeterBounds.getWidth() * 2.5f;
    const auto levelMatchButtonHeight = levelMatchW / levelMatchButtonOn->getDrawableBounds().getAspectRatio();
    levelMatchButton.setBounds (gainKnobBounds.withSizeKeepingCentre (levelMatchW, levelMatchButtonHeight).withBottomY(levelsBottomY * 1.01f).toNearestInt());
}
