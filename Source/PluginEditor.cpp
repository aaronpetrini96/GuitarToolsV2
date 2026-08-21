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
    setSize (640, 520);
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
    if (metalBackground != nullptr)
        metalBackground->drawWithin (g, boundsF, juce::RectanglePlacement::stretchToFit, 1.f);
}

void GuitarToolsAudioProcessorEditor::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight= boundsF.getHeight();

    headerPanel.setBounds(boundsF.withHeight(boundsFHeight * 0.0375f).toNearestInt());
    footerPanel.setBounds(boundsF.withHeight(boundsFHeight * 0.15f).withBottomY(boundsF.getBottom()).toNearestInt());

    auto mainPanelBounds = boundsF;
    mainPanelBounds.setY(headerPanel.getBottom());
    mainPanelBounds.setBottom(footerPanel.getY());
    mainPanel.setBounds(mainPanelBounds.toNearestInt());
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
    g.setColour(GTColors::dividerLines);
    g.drawHorizontalLine(headerPanel.getBottom(), 0.0f, static_cast<float>(getWidth()));
    g.drawHorizontalLine(footerPanel.getY(), 0.0f, static_cast<float>(getWidth()));

   #if JUCE_DEBUG
    if (drawDraft <= 0.0f || draft == nullptr)
        return;

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
