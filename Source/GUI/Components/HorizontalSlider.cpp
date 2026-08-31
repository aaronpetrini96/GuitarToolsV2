/*
  ==============================================================================

    HorizontalSlider.cpp
    Created: 22 Apr 2025 6:19:46pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#include "HorizontalSlider.h"
#include "GTLook.h"

//==============================================================================
HorizontalSlider::HorizontalSlider(const juce::String& text,
                                   juce::AudioProcessorValueTreeState& apvts,
                                   const juce::ParameterID& parameterID,
                                   const juce::String& suffix,
                                   bool drawFromMiddle)
: attachment(apvts, parameterID.getParamID(), slider)

{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::red.withAlpha(0.8f));
    slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::grey.withAlpha(0.3f));
    slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::ColourIds::textBoxTextColourId, GTColors::clearGrey);
    
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    if (suffix != "") {
        slider.setTextValueSuffix(suffix);
    }
    addAndMakeVisible(slider);
     
//    label.setText(text, juce::NotificationType::dontSendNotification);
//    label.setJustificationType(juce::Justification::horizontallyCentred);
//    label.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::transparentBlack);
//    label.setBorderSize(juce::BorderSize<int>{0, 0, 2, 0});
//    label.attachToComponent(&slider, false);
//    addAndMakeVisible(label);
    
    setSize(60, 170);
//    setLookAndFeel(RotaryKnobLookAndFeel::get());

    
    slider.setRotaryParameters(1.25f*pi, 2.75f*pi, true);
    slider.getProperties().set("drawFromMiddle", drawFromMiddle);

}

HorizontalSlider::~HorizontalSlider()
{
}

void HorizontalSlider::paint (juce::Graphics& g)
{

}

void HorizontalSlider::resized()
{
    slider.setBounds (getLocalBounds());
}

void HorizontalSlider::drawLinearSliderOutline (juce::Graphics& g, int, int, int, int,
                                              const juce::Slider::SliderStyle, juce::Slider& slider)
{
    if (slider.getTextBoxPosition() == juce::Slider::NoTextBox)
    {
        g.setColour (slider.findColour (juce::Slider::textBoxOutlineColourId));
        g.drawRect (0, 0, slider.getWidth(), slider.getHeight(), 1);
    }
}


void HorizontalSlider::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float minSliderPos, float maxSliderPos,
                                       const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    g.fillAll (slider.findColour (juce::Slider::backgroundColourId));

    if (style == juce::Slider::LinearBar || style == juce::Slider::LinearBarVertical)
    {
        const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
        
        

        drawLinearSliderOutline (g, x, y, width, height, style, slider);
    }
    else
    {
        drawLinearSliderBackground (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        drawLinearSliderThumb (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}



void HorizontalSlider::drawLinearSliderBackground (juce::Graphics& g, int x, int y, int width, int height,
                                                 float /*sliderPos*/,
                                                 float /*minSliderPos*/,
                                                 float /*maxSliderPos*/,
                                                 const juce::Slider::SliderStyle /*style*/, juce::Slider& slider)
{
    auto sliderRadius = (float) (getSliderThumbRadius (slider) - 2);
    auto trackColour = slider.findColour (juce::Slider::trackColourId);
    auto gradCol1 = trackColour.overlaidWith (juce::Colours::black.withAlpha (slider.isEnabled() ? 0.25f : 0.13f));
    auto gradCol2 = trackColour.overlaidWith (juce::Colour (0x14000000));

    juce::Path indent;

    if (slider.isHorizontal())
    {
        const float iy = (float) y + (float) height * 0.5f - sliderRadius * 0.5f;
        const float ih = sliderRadius;

        g.setGradientFill (juce::ColourGradient::vertical (gradCol1, iy, gradCol2, iy + ih));

        indent.addRoundedRectangle ((float) x - sliderRadius * 0.5f, iy,
                                    (float) width + sliderRadius, ih,
                                    5.0f);
    }
    else
    {
        const float ix = (float) x + (float) width * 0.5f - sliderRadius * 0.5f;
        const float iw = sliderRadius;

        g.setGradientFill (juce::ColourGradient::horizontal (gradCol1, ix, gradCol2, ix + iw));

        indent.addRoundedRectangle (ix, (float) y - sliderRadius * 0.5f,
                                    iw, (float) height + sliderRadius,
                                    5.0f);
    }

    g.fillPath (indent);

    g.setColour (juce::Colour (0x4c000000));
    g.strokePath (indent, juce::PathStrokeType (0.5f));
}



void HorizontalSlider::drawLinearSliderThumb (juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float minSliderPos, float maxSliderPos,
                                            const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto sliderRadius = (float) (getSliderThumbRadius (slider) - 2);
    
    

    auto knobColour = LookAndFeelHelpers::createBaseColour (slider.findColour (juce::Slider::thumbColourId),
                                                                    slider.hasKeyboardFocus (false) && slider.isEnabled(),
                                                                    slider.isMouseOverOrDragging() && slider.isEnabled(),
                                                                    slider.isMouseButtonDown() && slider.isEnabled());

    const float outlineThickness = slider.isEnabled() ? 0.8f : 0.3f;

    if (style == juce::Slider::LinearHorizontal || style == juce::Slider::LinearVertical)
    {
        float kx, ky;

        if (style == juce::Slider::LinearVertical)
        {
            kx = (float) x + (float) width * 0.5f;
            ky = sliderPos;
        }
        else
        {
            kx = sliderPos;
            ky = (float) y + (float) height * 0.5f;
        }

        drawGlassSphere (g,
                         kx - sliderRadius,
                         ky - sliderRadius,
                         sliderRadius * 2.0f,
                         knobColour, outlineThickness);
    }
  
    
}




