#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarToolsAudioProcessorEditor::GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setWantsKeyboardFocus(true);

//==============================================================================
    setLookAndFeel(&mainLF);

    // addAndMakeVisible(headerPanel);
    addAndMakeVisible(mainPanel);
    addAndMakeVisible(footerPanel);

    // setResizable(false, true);
    // setResizeLimits(minUIWidth, minUIHeight, 2 * minUIWidth, 2 * minUIHeight);
    // getConstrainer()->setFixedAspectRatio (960.0 / 780.0);
    setSize (800, 640);

    startTimerHz(30);
}

GuitarToolsAudioProcessorEditor::~GuitarToolsAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void GuitarToolsAudioProcessorEditor::paint (juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFHeight = boundsF.getHeight();
    const auto boundsFWidth = boundsF.getWidth();

    if (metalBackground != nullptr)
        metalBackground->drawWithin (g, boundsF, juce::RectanglePlacement::centred, 1.0f);

    g.setGradientFill (juce::ColourGradient::vertical (GTColors::mainPanelTop, 0.0f, GTColors::mainPanelBottom.brighter(0.1f), boundsFHeight));
    g.fillRect (boundsF);

    const auto borderThickness = boundsFWidth * 0.003f;
    g.setColour (juce::Colours::black);
    g.drawRect (boundsF, borderThickness);

    g.fillAll(juce::Colours::black.withAlpha(0.15f));

    // DIVIDERS
    const auto sideSectionWidth = boundsFWidth * 0.317f;
    const auto dividerThickness = boundsFWidth * 0.004f;
    const auto cornerSize = dividerThickness * 0.5f;

    // VERTICAL DIVIDERS
    const auto gapOffset = (boundsFWidth - sideSectionWidth * 3.f) * 0.025f;
    const auto verticalDividerBounds = boundsF.withWidth (dividerThickness).withBottom ((float) mainPanel.getBottom());

    for (const auto& dividerCentreX : { sideSectionWidth + gapOffset, boundsFWidth - sideSectionWidth - gapOffset })
    {
        const auto dividerRect = verticalDividerBounds.withCentre ({ dividerCentreX, verticalDividerBounds.getCentreY() });
        g.setGradientFill (juce::ColourGradient::vertical (GTColors::dividerLinesStart, dividerRect.getY(), GTColors::dividerLinesEnd, dividerRect.getBottom()));
        g.fillRoundedRectangle (dividerRect, cornerSize);
    }

    // HORIZONTAL DIVIDERS
    const auto sectionTitleH = mainPanel.getHeight() * 0.1f;
    const auto outlineThickness = dividerThickness * 0.125f;
    const auto outlineExpansion = outlineThickness * 0.5f;
    const auto cutFiltersArea = boundsF.withWidth (sideSectionWidth);

    for (const auto& sectionArea : { cutFiltersArea,
                                     boundsF.withSizeKeepingCentre (boundsFWidth - sideSectionWidth * 2.f, boundsFHeight),
                                     cutFiltersArea.withRightX (boundsFWidth) })
    {
        const auto dividerRect = sectionArea.withSizeKeepingCentre (sectionArea.getWidth() * 0.9f, dividerThickness).withY (sectionTitleH - 2 * dividerThickness);
        g.setGradientFill (juce::ColourGradient::horizontal (GTColors::dividerLinesEnd, dividerRect.getX(), GTColors::dividerLinesEnd, dividerRect.getRight()));
        g.fillRoundedRectangle (dividerRect, cornerSize);
        g.setColour (GTColors::dividerLinesEnd.brighter (0.1f));
        g.drawRoundedRectangle (dividerRect.expanded (outlineExpansion), cornerSize + outlineExpansion, outlineThickness);
    }
}

void GuitarToolsAudioProcessorEditor::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFHeight= boundsF.getHeight();
    // const float headerH = boundsFHeight * JUCE_LIVE_CONSTANT(0.05f);
    const float footH = boundsFHeight * 0.11f;

    // headerPanel.setBounds(boundsF.withHeight(headerH).toNearestInt());
    footerPanel.setBounds(boundsF.withHeight(footH).withBottomY(boundsFHeight).toNearestInt());
    mainPanel.setBounds(boundsF.withBottom(boundsFHeight - footH).toNearestInt());
}

//==============================================================================
//==============================================================================

void GuitarToolsAudioProcessorEditor::timerCallback()
{
// === Clipping light logic ===
    bool inClipping = audioProcessor.clipFlagIn.exchange(false);
    bool outClipping = audioProcessor.clipFlagOut.exchange(false);
    if (inClipping)
        footerPanel.inputGainSlider.setClippingStatus(true);
    if (outClipping)
        footerPanel.outputGainSlider.setClippingStatus(true);
    
    if (inClipping == true || outClipping == true)
        clipLightHoldCounter = 15; // clipping stays lit for 15 ticks

    if (clipLightHoldCounter > 0)
    {
        --clipLightHoldCounter;
    }
    else
    {
        footerPanel.inputGainSlider.setClippingStatus(false);
        footerPanel.outputGainSlider.setClippingStatus(false);
    }
}

//==============================================================================
void GuitarToolsAudioProcessorEditor::paintOverChildren (juce::Graphics& g)
{
   #if JUCE_DEBUG
    if (drawDraft > 0.0f && draft != nullptr)
        draft->drawWithin (g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, drawDraft);
   #endif
}

bool GuitarToolsAudioProcessorEditor::keyPressed (const juce::KeyPress& key)
{
   #if JUCE_DEBUG
    if (juce::CharacterFunctions::toLowerCase (key.getTextCharacter()) == 'd')
    {
        drawDraft = drawDraft < 1.0f ? drawDraft + 0.25f : 0.0f;
        repaint();
        return true;
    }
   #endif

    return juce::AudioProcessorEditor::keyPressed (key);
}
