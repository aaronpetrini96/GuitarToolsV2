#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarToolsAudioProcessorEditor::GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setWantsKeyboardFocus(true);

//==============================================================================
    setLookAndFeel(&mainLF);

    addAndMakeVisible(headerPanel);
    addAndMakeVisible(mainPanel);
    addAndMakeVisible(footerPanel);

    setResizable(false, true);
    setResizeLimits(minUIWidth, minUIHeight, 2 * minUIWidth, 2 * minUIHeight);
    getConstrainer()->setFixedAspectRatio (960.0 / 780.0);
    setSize (800, 650);

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

    g.setGradientFill (juce::ColourGradient::vertical (GTColors::mainPanelTop, boundsFHeight * 0.075f, GTColors::mainPanelBottom, boundsFHeight - boundsFHeight * 0.15f));
    g.fillRect (boundsF);
    g.fillAll(juce::Colours::black.withAlpha(0.35f));

    g.setColour(GTColors::dividerLines);
    const auto lineThickness = boundsFWidth * 0.003f;
    g.drawHorizontalLine(headerPanel.getBottom(), 0.0f, boundsFWidth);
    g.drawLine (0.f, boundsFHeight * 0.05f, boundsFWidth, boundsFHeight * 0.05f, lineThickness);
    g.drawLine (0.f, boundsFHeight * 0.85f, boundsFWidth, boundsFHeight * 0.85f, lineThickness);
    const auto vertLineTop = boundsFHeight * 0.05f;
    const auto vertLineBottom = boundsFHeight * 0.85f;
    const auto firstVertLineX = boundsFWidth * 0.315f;
    const auto secondVertLineX = boundsFWidth - firstVertLineX;
    g.fillRect (firstVertLineX, vertLineTop, lineThickness, vertLineBottom - vertLineTop);
    g.fillRect (secondVertLineX, vertLineTop, lineThickness, vertLineBottom - vertLineTop);
}

void GuitarToolsAudioProcessorEditor::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight= boundsF.getHeight();



    headerPanel.setBounds(boundsF.withHeight(boundsFHeight * headerHeightFraction).toNearestInt());
    footerPanel.setBounds(boundsF.withHeight(boundsFHeight * footerHeightFraction).withBottomY(boundsF.getBottom()).toNearestInt());
    mainPanel.setBounds(boundsF.withSizeKeepingCentre(boundsFWidth, boundsFHeight * (1.f - headerHeightFraction - footerHeightFraction)).toNearestInt());
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
    {
        isClippingLightOn = true;
        clipLightHoldCounter = 15; // light stays on for 15 ticks
        clipPopScale = 1.3f;       // pop to 1.3x size when clipping
    }
    
    if (clipLightHoldCounter > 0)
    {
        --clipLightHoldCounter;
        
        // Gradually shrink pop effect back to normal
        clipPopScale -= 0.02f;
        if (clipPopScale < 1.0f)
            clipPopScale = 1.0f;
    }
    else
    {
        footerPanel.inputGainSlider.setClippingStatus(false);
        footerPanel.outputGainSlider.setClippingStatus(false);
        isClippingLightOn = false;
        clipPopScale = 1.0f;
    }
    
    repaint(); // triggers paint() to draw updated light
    
}

void GuitarToolsAudioProcessorEditor::clipLight(juce::Graphics& g)
{
    juce::Colour activeColor = juce::Colours::red.withAlpha(0.9f);
    juce::Colour offColor = juce::Colours::green.withAlpha(0.75f);
    juce::Colour currentColor = isClippingLightOn ? activeColor : offColor;

    g.setColour(currentColor);
    g.setFont(juce::Font(juce::FontOptions(16.f * clipPopScale, juce::Font::FontStyleFlags::bold)));
    g.drawFittedText("CLIP", getWidth() * 0.3, getHeight() * 0.937, 200, 20, juce::Justification::centred, 1);
}

//==============================================================================
void GuitarToolsAudioProcessorEditor::paintOverChildren (juce::Graphics& g)
{


   #if JUCE_DEBUG
    if (drawDraft <= 0.0f || draft == nullptr)
        return;

    draft->drawWithin (g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, drawDraft);
   #endif
    g.fillAll(juce::Colours::black.withAlpha(0.35f));
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
