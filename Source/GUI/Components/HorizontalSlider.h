/*
  ==============================================================================

    HorizontalSlider.h
    Created: 22 Apr 2025 6:19:46pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../LAF/LookAndFeelHelpers.h"
#include "../LAF/LookAndFeel.h"
//==============================================================================
/*
*/
class HorizontalSlider  : public juce::Component, public juce::LookAndFeel_V4
{
public:
    HorizontalSlider(const juce::String& text,
                     juce::AudioProcessorValueTreeState& apvts,
                     const juce::ParameterID& parameterID,
                     const juce::String& suffix,
                     bool drawFromMiddle = false);
    ~HorizontalSlider() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void drawLinearSliderOutline (juce::Graphics& g, int, int, int, int,
                                  const juce::Slider::SliderStyle, juce::Slider& slider) override;
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float minSliderPos, float maxSliderPos,
                                             const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    void drawLinearSliderBackground (juce::Graphics& g, int x, int y, int width, int height,
                                                     float /*sliderPos*/,
                                                     float /*minSliderPos*/,
                                                     float /*maxSliderPos*/,
                                     const juce::Slider::SliderStyle /*style*/, juce::Slider& slider) override;
    

    
    void drawLinearSliderThumb (juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPos, float minSliderPos, float maxSliderPos,
                                                  const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    
    
    
    
    juce::Slider slider;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    juce::Label label;


private:
    const float pi = juce::MathConstants<float>::pi;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HorizontalSlider)
};
