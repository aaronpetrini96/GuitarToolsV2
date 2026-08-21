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

//    OVERSAMPLING
//    oversamplingBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
//    oversamplingBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
//    oversamplingBox.setLookAndFeel(ComboBoxLookAndFeel::get());
//    oversamplingBox.setSize(60, 25);
//    addAndMakeVisible(oversamplingBox);

//   CUT FILTERS
    // addAndMakeVisible(lowCutFreqKnob);
    addAndMakeVisible(highCutFreqKnob);
    addAndMakeVisible(lowCutSlopeBox);
    addAndMakeVisible(highCutSlopeBox);
    addAndMakeVisible(imgKnobPreview);

//    BUTTONS
    addAndMakeVisible(resoButton);
    addAndMakeVisible(resoFreqSlider);
    resoFreqSlider.setVisible(resoButton.getToggleState());
    resoButton.onClick = [this](){resoFreqSlider.setVisible(resoButton.getToggleState());};

    addAndMakeVisible(mudButton);
    addAndMakeVisible(mudFreqSlider);
    mudFreqSlider.setVisible(mudButton.getToggleState());
    mudButton.onClick = [this](){mudFreqSlider.setVisible(mudButton.getToggleState());};

    addAndMakeVisible(compBypassButton);
    addAndMakeVisible(compThresholdSlider);

    compThresholdSlider.setVisible(compBypassButton.getToggleState());
    compRatioBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
    compRatioBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    compRatioBox.setLookAndFeel(ComboBoxLookAndFeel::get());
    addAndMakeVisible(compRatioBox);
    compRatioBox.setVisible(compBypassButton.getToggleState());
    compBypassButton.onClick = [this]()
    {
        bool state = compBypassButton.getToggleState();
        compThresholdSlider.setVisible(state);
        compRatioBox.setVisible(state);
    };

//    SHELF FILTERS
    addAndMakeVisible(highShelfGainKnob);
    addAndMakeVisible(lowShelfGainKnob);
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
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFHeight = boundsF.getHeight();

    g.setGradientFill(juce::ColourGradient::vertical(GTColors::mainPanelTop.withAlpha(0.5f), boundsFHeight * 0.075f, GTColors::mainPanelBottom.withAlpha(0.5f), boundsFHeight - boundsFHeight * 0.15f));
    g.fillRect(boundsF);
}

void MainPanel::paintOverChildren(Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.25f));
}

void MainPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    auto y = boundsFHeight * 0.015f;
    auto height = boundsFHeight * 0.84f;
    auto leftMargin = boundsFWidth * 0.02f;
    auto presenceButtonsSize = leftMargin * 2.5f;
    auto groupWidth = boundsFWidth * 0.245f;

//    AREAS (formerly GroupComponents — no longer drawn, just layout anchors)
    juce::Rectangle<float> cutFiltersArea (leftMargin, y, groupWidth, height);
    juce::Rectangle<float> shelfFiltersArea (boundsFWidth * 0.74f, y, groupWidth, height);
    juce::Rectangle<float> buttonsArea (cutFiltersArea.getRight() + leftMargin, y,
                                       shelfFiltersArea.getX() - cutFiltersArea.getRight() - leftMargin * 2.0f, height);

//    CUT FILTERS
    auto lowCutFreqKnobLocalX = (cutFiltersArea.getWidth() - lowCutFreqKnob.getWidth()) * 0.5f;
    auto lowCutFreqKnobLocalY = leftMargin * 1.5f;
    lowCutFreqKnob.setBounds (juce::Rectangle<float> ((float) lowCutFreqKnob.getWidth(), (float) lowCutFreqKnob.getHeight())
        .withPosition (cutFiltersArea.getX() + lowCutFreqKnobLocalX, cutFiltersArea.getY() + lowCutFreqKnobLocalY)
        .toNearestInt());


    auto knobSize = groupWidth;
    imgKnobPreview.setBounds (cutFiltersArea.withSizeKeepingCentre(knobSize, knobSize).withY(boundsFHeight * 0.f).toNearestInt());


    auto highCutFreqKnobLocalY = lowCutFreqKnob.getHeight() * 1.5f;
    highCutFreqKnob.setBounds (juce::Rectangle<float> ((float) lowCutFreqKnob.getWidth(), (float) lowCutFreqKnob.getHeight())
        .withPosition (cutFiltersArea.getX() + lowCutFreqKnobLocalX, cutFiltersArea.getY() + highCutFreqKnobLocalY)
        .toNearestInt());

    lowCutSlopeBox.setBounds (juce::Rectangle<float> ((float) lowCutSlopeBox.getWidth(), (float) lowCutSlopeBox.getHeight())
        .withPosition (cutFiltersArea.getX() + (cutFiltersArea.getWidth() - lowCutSlopeBox.getWidth()) * 0.5f, cutFiltersArea.getY() + lowCutFreqKnob.getHeight() * 1.2f)
        .toNearestInt());

    auto highCutSlopeBoxLocalY = lowCutFreqKnob.getHeight() * 2.6f;
    highCutSlopeBox.setBounds (juce::Rectangle<float> ((float) highCutSlopeBox.getWidth(), (float) highCutSlopeBox.getHeight())
        .withPosition (cutFiltersArea.getX() + (cutFiltersArea.getWidth() - highCutSlopeBox.getWidth()) * 0.5f, cutFiltersArea.getY() + highCutSlopeBoxLocalY)
        .toNearestInt());

//    IMG KNOB PREVIEW

//    BUTTONS
    auto middleButtonsGroup = (buttonsArea.getWidth() - resoButton.getWidth()) * 0.53f;
    auto resoButtonLocalY = leftMargin * 2.1f;
    resoButton.setBounds (juce::Rectangle<float> ((float) resoButton.getWidth(), (float) resoButton.getHeight())
        .withPosition (buttonsArea.getX() + middleButtonsGroup, buttonsArea.getY() + resoButtonLocalY)
        .toNearestInt());

    auto resoButtonLocalBottom = resoButtonLocalY + resoButton.getHeight();
    auto resoFreqSliderLocalX = middleButtonsGroup * 0.6f;
    resoFreqSlider.setBounds (juce::Rectangle<float> (150.0f, 30.0f)
        .withPosition (buttonsArea.getX() + resoFreqSliderLocalX, buttonsArea.getY() + resoButtonLocalBottom * 0.89f)
        .toNearestInt());

    auto mudButtonLocalY = resoButtonLocalBottom * 1.35f;
    mudButton.setBounds (juce::Rectangle<float> ((float) mudButton.getWidth(), (float) mudButton.getHeight())
        .withPosition (buttonsArea.getX() + middleButtonsGroup, buttonsArea.getY() + mudButtonLocalY)
        .toNearestInt());

    auto mudButtonLocalBottom = mudButtonLocalY + mudButton.getHeight();
    mudFreqSlider.setBounds (juce::Rectangle<float> (mudButton.getWidth() * 1.5f, (float) mudButton.getHeight())
        .withPosition (buttonsArea.getX() + resoFreqSliderLocalX, buttonsArea.getY() + mudButtonLocalBottom * 0.95f)
        .toNearestInt());

    auto compBypassButtonLocalY = mudButtonLocalBottom * 1.18f;
    compBypassButton.setBounds (juce::Rectangle<float> ((float) compBypassButton.getWidth(), (float) compBypassButton.getHeight())
        .withPosition (buttonsArea.getX() + middleButtonsGroup, buttonsArea.getY() + compBypassButtonLocalY)
        .toNearestInt());

    auto compBypassButtonLocalBottom = compBypassButtonLocalY + compBypassButton.getHeight();
    compThresholdSlider.setBounds (juce::Rectangle<float> ((float) compThresholdSlider.getWidth(), (float) compThresholdSlider.getHeight())
        .withPosition (buttonsArea.getX() + resoFreqSliderLocalX, buttonsArea.getY() + compBypassButtonLocalBottom * 0.965f)
        .toNearestInt());

    compRatioBox.setBounds (juce::Rectangle<float> ((float) lowCutSlopeBox.getWidth(), (float) lowCutSlopeBox.getHeight())
        .withPosition (buttonsArea.getX() + middleButtonsGroup * 1.5f, cutFiltersArea.getY() + highCutSlopeBoxLocalY)
        .toNearestInt());

//    SHELF FILTERS
    auto highShelfGainKnobLocalX = (shelfFiltersArea.getWidth() - highShelfGainKnob.getWidth()) * 0.5f;
    highShelfGainKnob.setBounds (juce::Rectangle<float> ((float) highShelfGainKnob.getWidth(), (float) highShelfGainKnob.getHeight())
        .withPosition (shelfFiltersArea.getX() + highShelfGainKnobLocalX, shelfFiltersArea.getY() + leftMargin * 1.5f)
        .toNearestInt());

    auto presenceLocalY = highShelfGainKnob.getHeight() * 1.2f;
    auto presence1LocalX = highShelfGainKnobLocalX * 0.52f;
    presence1.setBounds (juce::Rectangle<float> (presenceButtonsSize, presenceButtonsSize)
        .withPosition (shelfFiltersArea.getX() + presence1LocalX, shelfFiltersArea.getY() + presenceLocalY)
        .toNearestInt());

    auto presence2LocalX = (presence1LocalX + presenceButtonsSize) + leftMargin;
    presence2.setBounds (juce::Rectangle<float> (presenceButtonsSize, presenceButtonsSize)
        .withPosition (shelfFiltersArea.getX() + presence2LocalX, shelfFiltersArea.getY() + presenceLocalY)
        .toNearestInt());

    auto presence3LocalX = (presence2LocalX + presenceButtonsSize) + leftMargin;
    presence3.setBounds (juce::Rectangle<float> (presenceButtonsSize, presenceButtonsSize)
        .withPosition (shelfFiltersArea.getX() + presence3LocalX, shelfFiltersArea.getY() + presenceLocalY)
        .toNearestInt());

    lowShelfGainKnob.setBounds (juce::Rectangle<float> ((float) highShelfGainKnob.getWidth(), (float) highShelfGainKnob.getHeight())
        .withPosition (shelfFiltersArea.getX() + highShelfGainKnobLocalX, cutFiltersArea.getY() + highCutFreqKnobLocalY)
        .toNearestInt());

    auto depthLocalY = lowShelfGainKnob.getHeight() * 2.59f;
    auto depth1LocalX = highShelfGainKnobLocalX * 0.52f;
    depth1.setBounds (juce::Rectangle<float> (presenceButtonsSize, presenceButtonsSize)
        .withPosition (shelfFiltersArea.getX() + depth1LocalX, shelfFiltersArea.getY() + depthLocalY)
        .toNearestInt());

    auto depth2LocalX = (depth1LocalX + presenceButtonsSize) + leftMargin;
    depth2.setBounds (juce::Rectangle<float> (presenceButtonsSize, presenceButtonsSize)
        .withPosition (shelfFiltersArea.getX() + depth2LocalX, shelfFiltersArea.getY() + depthLocalY)
        .toNearestInt());

    auto depth3LocalX = (depth2LocalX + presenceButtonsSize) + leftMargin;
    depth3.setBounds (juce::Rectangle<float> (presenceButtonsSize, presenceButtonsSize)
        .withPosition (shelfFiltersArea.getX() + depth3LocalX, shelfFiltersArea.getY() + depthLocalY)
        .toNearestInt());
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
