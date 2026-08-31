#include "MainPanel.h"
#include "GUI/LAF/LookAndFeel.h"

MainPanel::MainPanel (GuitarToolsAudioProcessor& processor)
    : audioProcessor (processor)
{
//   GROUP TITLES
    for (auto* sectionTitleLabel : { &cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel})
    {
        sectionTitleLabel->setJustificationType (juce::Justification::centred);
        sectionTitleLabel->setColour (juce::Label::textColourId, GTColors::sectionTitleText);
        sectionTitleLabel->setInterceptsMouseClicks(false, false);
        sectionTitleLabel->setMinimumHorizontalScale(1.f);
        addAndMakeVisible (*sectionTitleLabel);
    }


//   CUT FILTERS
    addAndMakeVisible (lowCutFreqKnob);
    addAndMakeVisible (highCutFreqKnob);
    addAndMakeVisible (lowCutSlopeButtons);
    addAndMakeVisible (highCutSlopeButtons);

    //    SHELF FILTERS
    addAndMakeVisible(highShelfGainKnob);
    addAndMakeVisible(lowShelfGainKnob);
    addAndMakeVisible (presenceFreqButtons);
    addAndMakeVisible (depthFreqButtons);

//  MAGIC  BUTTONS
    addAndMakeVisible (resoButton);
    addChildComponent(resoFreqFader);
    resoButton.setOnStateChange ([this] { resoFreqFader.setVisible (resoButton.getToggleState()); });
    addAndMakeVisible (mudButton);
    addChildComponent(mudFreqFader);
    mudButton.setOnStateChange ([this] { mudFreqFader.setVisible (mudButton.getToggleState()); });

    addAndMakeVisible (compBypassButton);
    addChildComponent(compThresholdFader);
    compBypassButton.setOnStateChange ([this]
                                       {
                                           const auto state = compBypassButton.getToggleState();
                                           compThresholdFader.setVisible (state);
                                       });


    for (auto* l : { &resoBypassLabel, &compBypassLabel, &mudBypassLabel, &slope1Label})
    {
        l->setColour (juce::Label::textColourId, GTColors::buttonsNameText);
        l->setJustificationType (juce::Justification::centred);
        l->setInterceptsMouseClicks(false, false);
        l->setMinimumHorizontalScale(1.f);
        addAndMakeVisible (*l);
    }
    slope1Label.setColour(juce::Label::ColourIds::textColourId, GTColors::sectionTitleText);
}


void MainPanel::paint (juce::Graphics& g)
{
}

void MainPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

     // SECTIONS
    const auto sideSectionWidth = boundsFWidth * 0.317f;
    cutFiltersArea = boundsF.withSize (sideSectionWidth, boundsFHeight);
    shelfFiltersArea = cutFiltersArea.withRightX(boundsFWidth);
    buttonsArea = boundsF.withSizeKeepingCentre (boundsFWidth - sideSectionWidth * 2.f, boundsFHeight);

    // SECTION TITLES
    sectionTitleH = boundsFHeight * 0.1f;
    for (const auto l : {&cutFiltersGroupLabel, &magicButtonsGroupLabel, &shelfFiltersGroupLabel})
       l->setFont (GTFonts::futuraBold(sectionTitleH * 0.3f));
    cutFiltersGroupLabel    .setBounds (cutFiltersArea.withHeight (sectionTitleH).toNearestInt());
    magicButtonsGroupLabel  .setBounds (buttonsArea.withHeight (sectionTitleH).toNearestInt());
    shelfFiltersGroupLabel  .setBounds (shelfFiltersArea.withHeight (sectionTitleH).toNearestInt());

//    CUT FILTERS
    const auto topKnobsCY = boundsFHeight * 0.275f;
    const auto bottomKnobsCY= topKnobsCY * 2.625f;
    const auto knobSize = cutFiltersArea.getWidth() * 0.8F;
    const auto knobBounds= boundsF.withSize (knobSize, knobSize);
    lowCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    highCutFreqKnob.setBounds (knobBounds.withCentre ({cutFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());

    const auto slopeButtonsWidth = knobSize * 0.75f;
    const auto slopeButtonsHeight = slopeButtonsWidth / lowCutSlopeButtons.getImageAspectRatio();
    const auto slopeButtonsBounds = cutFiltersArea.withSizeKeepingCentre (slopeButtonsWidth, slopeButtonsHeight);
    const auto slopeButtonsTopCY = boundsFHeight * 0.495f;
    const auto slopeButtonsBottomCY = slopeButtonsTopCY + bottomKnobsCY - topKnobsCY;

    lowCutSlopeButtons.setBounds (slopeButtonsBounds.withY (slopeButtonsTopCY - slopeButtonsHeight * 0.5f).toNearestInt());
    highCutSlopeButtons.setBounds (slopeButtonsBounds.withY (slopeButtonsBottomCY - slopeButtonsHeight * 0.5f).toNearestInt());

    slope1Label.setFont(GTFonts::jetBrainsMonoMedium(sectionTitleH * 0.25f));
    slope1Label.setBounds(slopeButtonsBounds.withY(slopeButtonsTopCY * JUCE_LIVE_CONSTANT(1.f)).toNearestInt());

    //SHELF FILTERS knobs
    highShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), topKnobsCY}).toNearestInt());
    lowShelfGainKnob.setBounds (knobBounds.withCentre ({shelfFiltersArea.getCentreX(), bottomKnobsCY}).toNearestInt());
    const auto radioButtonsWidth = knobSize * 0.85f;
    const auto radioButtonBounds = shelfFiltersArea.withSize (radioButtonsWidth, radioButtonsWidth / 3.66f).withCentre ({shelfFiltersArea.getCentreX(), boundsFHeight * 0.495f});
    const auto rBtnGap = radioButtonBounds.getY() - (topKnobsCY + knobSize * 0.5f);
    presenceFreqButtons.setBounds (radioButtonBounds.toNearestInt());
    depthFreqButtons.setBounds(radioButtonBounds.withY(bottomKnobsCY + knobSize * 0.5f + rBtnGap).toNearestInt());

// MAGIC BUTTONS
    const auto magicButtonSize = knobSize * 0.325f;
    const auto magicButtonBounds = buttonsArea.withSizeKeepingCentre (magicButtonSize, magicButtonSize).withY (topKnobsCY - magicButtonSize * 0.5f);
    const auto gap = (buttonsArea.getWidth() - magicButtonSize * 3.f) * 0.125f;
    mudButton.setBounds (magicButtonBounds.toNearestInt());
    resoButton.setBounds (magicButtonBounds.withRightX(magicButtonBounds.getX() - gap).toNearestInt());
    compBypassButton.setBounds (magicButtonBounds.withX(magicButtonBounds.getRight() + gap).toNearestInt());

    const auto faderH= boundsFHeight * 0.575F;
    const auto faderBounds = buttonsArea.withSizeKeepingCentre (buttonsArea.getWidth() * 0.26f, faderH).withBottomY (highCutSlopeButtons.getBounds().toFloat().getBottom());

    mudFreqFader.setBounds (faderBounds.toNearestInt());
    resoFreqFader.setBounds (faderBounds.withCentre({resoButton.getBounds().toFloat().getCentreX(), faderBounds.getCentreY()}).toNearestInt());
    compThresholdFader.setBounds (faderBounds.withCentre({compBypassButton.getBounds().toFloat().getCentreX(), faderBounds.getCentreY()}).toNearestInt());

    for (const auto l : {&resoBypassLabel, &compBypassLabel, &mudBypassLabel})
        l->setFont(GTFonts::jetBrainsMonoMedium(sectionTitleH * 0.32f));
    
    const auto mudLabelBounds = magicButtonBounds.withWidth(buttonsArea.getWidth() * 0.25f).withY(boundsFHeight * 0.11f);
    mudBypassLabel.setBounds(mudLabelBounds.toNearestInt());
    resoBypassLabel.setBounds(mudLabelBounds.withX(resoButton.getBounds().toFloat().getCentreX() - mudLabelBounds.getWidth() * 0.5f).toNearestInt());
    compBypassLabel.setBounds(mudLabelBounds.withX(compBypassButton.getBounds().toFloat().getCentreX() - mudLabelBounds.getWidth() * 0.5f).toNearestInt());

}
