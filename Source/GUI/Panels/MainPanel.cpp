#include "MainPanel.h"

MainPanel::MainPanel (GuitarToolsAudioProcessor& processor)
    : audioProcessor (processor)

{

    //================  CUT FILTERS ================//

    addAndMakeVisible (lowCutSlopeButtons);
    addAndMakeVisible (highCutSlopeButtons);
    //================  SHELF FILTERS  ================//

    addAndMakeVisible (presenceFreqButtons);
    addAndMakeVisible (depthFreqButtons);

    for (auto* knob : {&lowCutFreqKnob, &highCutFreqKnob, &lowShelfGainKnob, &highShelfGainKnob})
    {
        addAndMakeVisible(knob);
        knob->setTooltip (ParamHelpers::getTooltipForParam (knob->getComponentID()));
        knob->setUndoManager (audioProcessor.undoManager);
    }

    for (auto* btn : {&lowCutSlopeButtons, &highCutSlopeButtons, &presenceFreqButtons, &depthFreqButtons})
    {
        addAndMakeVisible(btn);
        btn->setTooltip (ParamHelpers::getTooltipForParam (btn->getComponentID()));
        btn->setUndoManager (audioProcessor.undoManager);
    }

    for (auto* l : {&lowCutSlopeLabel, &highCutSlopeLabel, &presenceModeLabel, &depthModeLabel})
        addAndMakeVisible(l);

    //================  MAGIC BUTTONS  ================//
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
        addAndMakeVisible (modeSwitch);
        modeSwitch->setTooltip (ParamHelpers::getTooltipForParam (modeSwitch->getComponentID()));
        modeSwitch->setUndoManager (audioProcessor.undoManager);
    }

    //================  LEVEL KNOBS  ================//



    for (auto* knob : {&inputGainKnob, &outputGainKnob})
    {
        knob->setTooltip (ParamHelpers::getTooltipForParam (knob->getComponentID()));
        knob->setUndoManager (audioProcessor.undoManager);
    }

    addAndMakeVisible (inputGainKnob);
    addAndMakeVisible (outputGainKnob);
    addAndMakeVisible (inputLevelMeter);
    addAndMakeVisible (outputLevelMeter);
    addAndMakeVisible (levelsTitle);

    levelMatchButton.setImages (levelMatchButtonOff.get(), nullptr, nullptr, nullptr, levelMatchButtonOn.get());
    levelMatchButton.setClickingTogglesState (true);
    levelMatchButton.setColour (juce::DrawableButton::backgroundColourId, GTColors::transparent);
    levelMatchButton.setColour (juce::DrawableButton::backgroundOnColourId, GTColors::transparent);
    levelMatchButton.setTooltip (ParamHelpers::getTooltipForParam (ParamIDs::getLoudnessMatchActiveName()));
    levelMatchButtonAttachment = std::make_unique<juce::ButtonParameterAttachment> (*audioProcessor.treeState.getParameter (ParamIDs::getLoudnessMatchActiveName()), levelMatchButton, &audioProcessor.undoManager);
    levelMatchButtonAttachment->sendInitialUpdate();
    levelMatchButton.onClick = [this] { outputGainKnob.setEnabled (! levelMatchButton.getToggleState()); };
    outputGainKnob.setEnabled (! levelMatchButton.getToggleState());
    // addAndMakeVisible (levelMatchButton);
}

void MainPanel::paint(juce::Graphics& g)
{
    const auto meterBounds = inputLevelMeter.getBounds().toFloat();
    const auto meterHeight = meterBounds.getHeight();
    const auto levelScaleSpan = meterHeight * 0.68f;
    const auto levelScaleTopY = meterBounds.getCentreY() - levelScaleSpan * 0.5f;
    const auto levelScaleCentreX = (meterBounds.getCentreX() + outputLevelMeter.getBounds().toFloat().getCentreX()) * 0.5f;
    g.setColour (GTColors::almostWhite.withAlpha (0.85f));
    g.setFont (AAFonts::ibmPlexSansCondensedSemiBold (meterHeight * 0.11f));

    for (int labelIndex {0}; labelIndex < levelScaleLabels.size(); ++labelIndex)
    {
        const auto labelCentreY = levelScaleTopY + levelScaleSpan * static_cast<float> (labelIndex) / static_cast<float> (levelScaleLabels.size() - 1);
        g.drawText (levelScaleLabels[labelIndex], meterBounds.withHeight (meterHeight * 0.1f).withCentre ({levelScaleCentreX, labelCentreY}), juce::Justification::centred);
    }
}

void MainPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();
    const auto sideSectionWidth = boundsFWidth * sideSectionPOW;
    knobSizeReference = boundsFHeight * 0.33f;

    cutFiltersArea = boundsF.withSize (sideSectionWidth, boundsFHeight);
    shelfFiltersArea = cutFiltersArea.withRightX(boundsFWidth);
    buttonsArea = boundsF.withSizeKeepingCentre (boundsFWidth - sideSectionWidth * 2.f, boundsFHeight);

    resizeKnobs(boundsF);
    resizeMagicButtons();
    resizeLevelsSectionCentred(boundsF);
}

void MainPanel::resizeKnobs(const juce::Rectangle<float>& boundsF)
{
    const auto boundsFHeight = boundsF.getHeight();
    const auto topKnobsCY = boundsFHeight * 0.25f;
    const auto bottomKnobsCY= boundsFHeight * 0.71f;

    const auto knobBounds= boundsF.withSize (knobSizeReference, knobSizeReference);
    lowCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    highCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());

    lowShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    highShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());

    const auto knobButtonsWidth = knobSizeReference * 0.75f;
    const auto knobButtonsGap = knobSizeReference * 0.63f;
    const auto knobButtonsTopCY = topKnobsCY + knobButtonsGap;
    const auto knobButtonsBottomCY = bottomKnobsCY + knobButtonsGap;

    const auto knobButtonsBounds = knobBounds.withSize (knobButtonsWidth, knobButtonsWidth * 578.0f / 1880.0f);
    lowCutSlopeButtons.setBounds (knobButtonsBounds.withCentre({cutFiltersArea.getCentreX(), knobButtonsTopCY}).toNearestInt());
    highCutSlopeButtons.setBounds (knobButtonsBounds.withCentre({cutFiltersArea.getCentreX(), knobButtonsBottomCY}).toNearestInt());
    depthFreqButtons.setBounds (knobButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), knobButtonsTopCY }).toNearestInt());
    presenceFreqButtons.setBounds (knobButtonsBounds.withCentre ({ shelfFiltersArea.getCentreX(), knobButtonsBottomCY }).toNearestInt());

    const auto knobButtonsLabelBounds = cutFiltersArea.withSizeKeepingCentre(knobButtonsWidth, knobSizeReference * 0.115f);
    for (const auto l : {&lowCutSlopeLabel, &highCutSlopeLabel, &presenceModeLabel, &depthModeLabel})
        l->setFont (AAFonts::ibmPlexSansMedium (knobButtonsLabelBounds.getHeight() * 0.825f));

    const auto knobButtonsLabelGap = knobSizeReference * 0.212f;
    lowCutSlopeLabel.setBounds(knobButtonsLabelBounds.withBottomY(knobButtonsTopCY + knobButtonsLabelGap).toNearestInt());
    highCutSlopeLabel.setBounds(knobButtonsLabelBounds.withBottomY(knobButtonsBottomCY + knobButtonsLabelGap).toNearestInt());
    depthModeLabel.setBounds (knobButtonsLabelBounds.withX (shelfFiltersArea.getCentreX() - knobButtonsLabelBounds.getWidth() * 0.5f).withBottomY (knobButtonsTopCY + knobButtonsLabelGap).toNearestInt());
    presenceModeLabel.setBounds (knobButtonsLabelBounds.withX (shelfFiltersArea.getCentreX() - knobButtonsLabelBounds.getWidth() * 0.5f).withBottomY (knobButtonsBottomCY + knobButtonsLabelGap).toNearestInt());
}
void MainPanel::resizeMagicButtons()
{
    const auto magicButtonSize = knobSizeReference * 0.32f;
    const auto magicButtonsGap = knobSizeReference * 0.34125f;
    const auto magicButtonBounds = buttonsArea.withSizeKeepingCentre (magicButtonSize, magicButtonSize).withY(sectionTitleH * 1.25f);
    mudActiveButton.setBounds (magicButtonBounds.toNearestInt());
    resoActiveButton.setBounds (magicButtonBounds.translated(-magicButtonsGap, 0.f).toNearestInt());
    compActiveButton.setBounds (magicButtonBounds.translated(magicButtonsGap, 0.f).toNearestInt());

    //PLATE BOUNDS
    plateBounds = buttonsArea.withSize(buttonsArea.getWidth() * 0.9f, buttonsArea.getHeight() * 0.5f).withCentre({buttonsArea.getCentreX(), magicButtonBounds.getBottom() + buttonsArea.getHeight() * 0.25f});
    fadersPlatePanel.setFaderSpacing(magicButtonsGap);
    fadersPlatePanel.setBounds (plateBounds.toNearestInt());

    const auto modeSwitchWidth = magicButtonSize;
    const auto modeSwitchHeight = modeSwitchWidth * 683.0f / 1206.0f;
    const auto switchBounds = plateBounds.withSizeKeepingCentre (modeSwitchWidth, modeSwitchHeight).withY (buttonsArea.getHeight() * 0.7f);
    mudModeSwitch.setBounds (switchBounds.toNearestInt());
    resoModeSwitch.setBounds (switchBounds.translated (-magicButtonsGap, 0.0f).toNearestInt());
    compModeSwitch.setBounds (switchBounds.translated (magicButtonsGap, 0.0f).toNearestInt());
}

void MainPanel::resizeLevelsSectionCentred(const juce::Rectangle<float>& boundsF)
{
    const auto levelsArea = buttonsArea.withTop (boundsF.getHeight() * 0.775f);
    const auto levelsAreaHeight = levelsArea.getHeight();
    const auto gainKnobSize = levelsAreaHeight * 0.55f;
    const auto meterHeight = levelsAreaHeight * 0.85f;
    const auto meterWidth = meterHeight * 0.25f;
    const auto meterCY = buttonsArea.getHeight() * 0.905f;
    const auto meterBounds = levelsArea.withSize (meterWidth, meterHeight).withCentre({levelsArea.getCentreX(), meterCY});
    const auto meterGap = meterHeight * 0.25f;
    inputLevelMeter.setBounds (meterBounds.translated (-meterGap, 0.f).toNearestInt());
    outputLevelMeter.setBounds (meterBounds.translated (meterGap, 0.f).toNearestInt());

    const auto gainKnobGap = levelsArea.getCentreX() - (buttonsArea.getX() + meterBounds.getX() - meterGap) * 0.5f;
    const auto gainKnobBounds = levelsArea.withSize (gainKnobSize, gainKnobSize * 1.17f).withCentre ({levelsArea.getCentreX(), meterBounds.getCentreY()});
    inputGainKnob.setBounds (gainKnobBounds.translated (-gainKnobGap, 0.f).toNearestInt());
    outputGainKnob.setBounds (gainKnobBounds.translated (gainKnobGap, 0.f).toNearestInt());

    const auto levelsTitleHeight = knobSizeReference * 0.112f;
    const auto levelsTitleKerning = 0.03f;
    levelsTitle.setFont (AAFonts::ibmPlexSansMedium (levelsTitleHeight).withExtraKerningFactor (levelsTitleKerning));
    levelsTitle.setBounds (levelsArea.withHeight (levelsTitleHeight).withCentre ({levelsArea.getCentreX(), levelsArea.getY() + levelsTitleHeight * 0.5f}).toNearestInt());
}
