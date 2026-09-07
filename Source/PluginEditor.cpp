#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarToolsAudioProcessorEditor::GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setWantsKeyboardFocus(true);
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(mainPanel);
    // addAndMakeVisible(footerPanel);

    setResizable (true, true);
    setResizeLimits (defaultUIWidth / 2, defaultUIHeight / 2, defaultUIWidth * 2, defaultUIHeight * 2);
    getConstrainer()->setFixedAspectRatio ((double) defaultUIWidth / (double) defaultUIHeight);
    setSize (defaultUIWidth, defaultUIHeight);
}

GuitarToolsAudioProcessorEditor::~GuitarToolsAudioProcessorEditor()
{
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
    g.fillAll(juce::Colours::black.withAlpha(0.15f));
}

void GuitarToolsAudioProcessorEditor::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFHeight= boundsF.getHeight();
    const float footH = boundsFHeight * 0.0f;
    const float headerH = boundsFHeight * 0.075f;
    // const float headerH = boundsFHeight * JUCE_LIVE_CONSTANT(0.07f);

    headerPanel.setBounds(boundsF.withHeight(headerH).toNearestInt());
    // footerPanel.setBounds(boundsF.withHeight(footH).withY(boundsFHeight - footH).toNearestInt());
    const auto mainPanelBounds = boundsF.withHeight(boundsFHeight - footH - headerH).withY(headerH);
    mainPanel.sectionTitleH = mainPanelBounds.getHeight() * 0.075f;
    mainPanel.setBounds(mainPanelBounds.toNearestInt());
}