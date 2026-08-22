#include "MainPanel.h"
#include "GUI/LAF/LookAndFeel.h"

MainPanel::MainPanel (GuitarToolsAudioProcessor& processor)
    : audioProcessor (processor)
{
//  PRESENCE BUTTONS
    presence1.onClick = [this]() {setShelfFilterFreq("Presence Freq", 0);};
    presence2.onClick = [this]() {setShelfFilterFreq("Presence Freq", 1);};
    presence3.onClick = [this]() {setShelfFilterFreq("Presence Freq", 2);};
    int indexPresence = static_cast<int>(audioProcessor.treeState.getRawParameterValue("Presence Freq")->load());
    updatePresenceButtons(indexPresence);

//  DEPTH BUTTONS
    depth1.onClick = [this]() {setShelfFilterFreq("Depth Freq", 0);};
    depth2.onClick = [this]() {setShelfFilterFreq("Depth Freq", 1);};
    depth3.onClick = [this]() {setShelfFilterFreq("Depth Freq", 2);};
    int indexDepth = static_cast<int>(audioProcessor.treeState.getRawParameterValue("Depth Freq")->load());
    updateDepthButtons(indexDepth);

//   GROUP TITLES
    for (auto* groupLabel : { &cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel })
    {
        groupLabel->setJustificationType (juce::Justification::centred);
        groupLabel->setColour (juce::Label::textColourId, GTColors::sectionTitleText);
        groupLabel->setFont (GTFonts::chivoBold (16.0f).withExtraKerningFactor (0.15f));
        addAndMakeVisible (*groupLabel);
    }

//   CUT FILTERS
    lowCutFreqLabel.setText ("Low Cut", juce::dontSendNotification);
    lowCutFreqLabel.setJustificationType (juce::Justification::centred);
    lowCutFreqLabel.attachToComponent (&lowCutFreqKnob, false);
    addAndMakeVisible (lowCutFreqKnob);
    addAndMakeVisible (lowCutFreqLabel);

    highCutFreqLabel.setText ("High Cut", juce::dontSendNotification);
    highCutFreqLabel.setJustificationType (juce::Justification::centred);
    highCutFreqLabel.attachToComponent (&highCutFreqKnob, false);
    addAndMakeVisible (highCutFreqKnob);
    addAndMakeVisible (highCutFreqLabel);

    addAndMakeVisible(lowCutSlopeBox);
    addAndMakeVisible(highCutSlopeBox);

//    BUTTONS
//    addAndMakeVisible(resoButton);
//    addAndMakeVisible(resoFreqSlider);
    resoFreqSlider.setVisible(resoButton.getToggleState());
    resoButton.onClick = [this](){resoFreqSlider.setVisible(resoButton.getToggleState());};

//    addAndMakeVisible(mudButton);
//    addAndMakeVisible(mudFreqSlider);
    mudFreqSlider.setVisible(mudButton.getToggleState());
    mudButton.onClick = [this](){mudFreqSlider.setVisible(mudButton.getToggleState());};

//    addAndMakeVisible(compBypassButton);
//    addAndMakeVisible(compThresholdSlider);

    compThresholdSlider.setVisible(compBypassButton.getToggleState());
    compRatioBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
    compRatioBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    compRatioBox.setLookAndFeel(ComboBoxLookAndFeel::get());
//    addAndMakeVisible(compRatioBox);
    compRatioBox.setVisible(compBypassButton.getToggleState());
    compBypassButton.onClick = [this]()
    {
        bool state = compBypassButton.getToggleState();
        compThresholdSlider.setVisible(state);
        compRatioBox.setVisible(state);
    };

//    SHELF FILTERS
    highShelfGainLabel.setText ("Presence", juce::dontSendNotification);
    highShelfGainLabel.setJustificationType (juce::Justification::centred);
    highShelfGainLabel.attachToComponent (&highShelfGainKnob, false);
    addAndMakeVisible(highShelfGainKnob);
    addAndMakeVisible(highShelfGainLabel);

    lowShelfGainLabel.setText ("Depth", juce::dontSendNotification);
    lowShelfGainLabel.setJustificationType (juce::Justification::centred);
    lowShelfGainLabel.attachToComponent (&lowShelfGainKnob, false);
    addAndMakeVisible(lowShelfGainKnob);
    addAndMakeVisible(lowShelfGainLabel);
    for (int i {0}; i < shelfFiltersButtons.size(); ++i)
    {
        setShelfFilterButtonStyle(*shelfFiltersButtons[i]);
        addAndMakeVisible(*shelfFiltersButtons[i]);
    }
}

MainPanel::~MainPanel()
{
    compRatioBox.setLookAndFeel(nullptr);
}

void MainPanel::paint (juce::Graphics& g)
{
}

void MainPanel::paintOverChildren(Graphics& g)
{

}

void MainPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();
    const auto cutFiltersArea = boundsF.withSize (boundsFWidth * 0.315f, boundsFHeight);
    const auto shelfFiltersArea = cutFiltersArea.withRightX(boundsFWidth);
    const auto buttonsArea = boundsF.withSizeKeepingCentre (boundsFWidth - boundsFWidth * 0.315f * 2.f, boundsFHeight);

    //    GROUP TITLES
    const auto groupTitleHeight = boundsFHeight * 0.06f;
    const auto groupTitleY = boundsFHeight * JUCE_LIVE_CONSTANT (0.0f);
    cutFiltersGroupLabel.setBounds (cutFiltersArea          .withHeight (groupTitleHeight)        .withY (groupTitleY).toNearestInt());
    magicButtonsGroupLabel.setBounds (buttonsArea           .withHeight (groupTitleHeight)         .withY (groupTitleY).toNearestInt());
    shelfFiltersGroupLabel.setBounds (shelfFiltersArea      .withHeight (groupTitleHeight)    .withY (groupTitleY).toNearestInt());




//    CUT FILTERS
    const auto knobSize = cutFiltersArea.getWidth() * 0.7f;
    const auto cutKnobsBounds = cutFiltersArea.withSizeKeepingCentre (knobSize, knobSize);
    lowCutFreqKnob.setBounds (cutKnobsBounds.withY (boundsFHeight * 0.125f).toNearestInt());
    highCutFreqKnob.setBounds (cutKnobsBounds.withY (boundsFHeight * 0.525f).toNearestInt());
    highShelfGainKnob.setBounds (cutKnobsBounds.withCentre ({shelfFiltersArea.getCentreX(), lowCutFreqKnob.getBounds().toFloat().getCentreY()}).toNearestInt());
    lowShelfGainKnob.setBounds (cutKnobsBounds.withCentre ({shelfFiltersArea.getCentreX(), highCutFreqKnob.getBounds().toFloat().getCentreY()}).toNearestInt());

    const auto knobLabelYOffset = boundsFHeight * JUCE_LIVE_CONSTANT (0.0f);
    for (auto* label : { &lowCutFreqLabel, &highCutFreqLabel, &highShelfGainLabel, &lowShelfGainLabel })
        label->setBounds (label->getBounds().withY (label->getY() + juce::roundToInt (knobLabelYOffset)));

    const auto slopeBoxY = boundsFHeight * JUCE_LIVE_CONSTANT (0.0f);
    lowCutSlopeBox.setBounds (cutFiltersArea.withSizeKeepingCentre ((float) lowCutSlopeBox.getWidth(), (float) lowCutSlopeBox.getHeight()).withY ((float) lowCutFreqKnob.getBottom() + slopeBoxY).toNearestInt());
    highCutSlopeBox.setBounds (cutFiltersArea.withSizeKeepingCentre ((float) highCutSlopeBox.getWidth(), (float) highCutSlopeBox.getHeight()).withY ((float) highCutFreqKnob.getBottom() + slopeBoxY).toNearestInt());

//    SHELF FILTERS
    const auto shelfButtonGap = 0.f;
    const auto shelfButtonSize = (float) presence1.getWidth();
    const auto shelfButtonsRowWidth = shelfButtonSize * 3.0f + shelfButtonGap * 2.0f;
    const auto shelfButtonsY = boundsFHeight * JUCE_LIVE_CONSTANT (0.0f);

    auto presenceButtonsBounds = shelfFiltersArea.withSizeKeepingCentre (shelfButtonsRowWidth, shelfButtonSize).withY ((float) highShelfGainKnob.getBottom() + shelfButtonsY);
    presence1.setBounds (presenceButtonsBounds.removeFromLeft (shelfButtonSize).toNearestInt());
    presenceButtonsBounds.removeFromLeft (shelfButtonGap);
    presence2.setBounds (presenceButtonsBounds.removeFromLeft (shelfButtonSize).toNearestInt());
    presenceButtonsBounds.removeFromLeft (shelfButtonGap);
    presence3.setBounds (presenceButtonsBounds.removeFromLeft (shelfButtonSize).toNearestInt());

    auto depthButtonsBounds = shelfFiltersArea.withSizeKeepingCentre (shelfButtonsRowWidth, shelfButtonSize).withY ((float) lowShelfGainKnob.getBottom() + shelfButtonsY);
    depth1.setBounds (depthButtonsBounds.removeFromLeft (shelfButtonSize).toNearestInt());
    depthButtonsBounds.removeFromLeft (shelfButtonGap);
    depth2.setBounds (depthButtonsBounds.removeFromLeft (shelfButtonSize).toNearestInt());
    depthButtonsBounds.removeFromLeft (shelfButtonGap);
    depth3.setBounds (depthButtonsBounds.removeFromLeft (shelfButtonSize).toNearestInt());
}

void MainPanel::setShelfFilterButtonStyle(juce::TextButton& button)
{
    button.setClickingTogglesState(true);
//    button.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(100, 100, 110).darker(0.5f));
//    button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(230, 165, 70));
    button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(0xFF40C4FF).darker(1.f));
    button.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    button.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
    button.setSize(25, 25);
}

void MainPanel::setShelfFilterFreq(const juce::String& parameterName, const int& index)
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.treeState.getParameter(parameterName)))
    {
        param->beginChangeGesture();
        choiceCount = static_cast<float>(param->choices.size() - 1);
        normalizedValue = static_cast<float>(index) / choiceCount;
        param->setValueNotifyingHost(normalizedValue);
        param->endChangeGesture();
        if(parameterName == "Presence Freq")
            updatePresenceButtons(index);
        if(parameterName == "Depth Freq")
            updateDepthButtons(index);
    }
}

void MainPanel::updatePresenceButtons(const int& selectedIndex)
{
    presence1.setToggleState(selectedIndex == 0, juce::dontSendNotification);
    presence2.setToggleState(selectedIndex == 1, juce::dontSendNotification);
    presence3.setToggleState(selectedIndex == 2, juce::dontSendNotification);
}

void MainPanel::updateDepthButtons(const int& selectedIndex)
{
    depth1.setToggleState(selectedIndex == 0, juce::dontSendNotification);
    depth2.setToggleState(selectedIndex == 1, juce::dontSendNotification);
    depth3.setToggleState(selectedIndex == 2, juce::dontSendNotification);
}
