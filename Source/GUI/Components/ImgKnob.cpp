#include "ImgKnob.h"
#include "GTLook.h"
#include "GUI/LAF/LookAndFeel.h"

ImgKnob::ImgKnob (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const juce::String& suffix)
    : attachment (apvts, parameterID.getParamID(), *this),
      unit (suffix)
{
    setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    setMouseDragSensitivity(400);

    float pi = juce::MathConstants<float>::pi;
    setRotaryParameters (1.25f * pi, 2.75f * pi, true);
}

void ImgKnob::paint (juce::Graphics& g)
{
    if (!knobImage.isValid())
        return;

    const auto boundsF = getLocalBounds().toFloat();
    g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);

    // knob.png already contains the knob face and its shadow.
    g.drawImage (knobImage, boundsF, juce::RectanglePlacement::centred, false);


    const auto knobRadius = boundsF.getWidth() * 0.25f;
    const auto centre = boundsF.getCentre();

    const auto rotaryParams = getRotaryParameters();
    const auto sliderPos = static_cast<float> (valueToProportionOfLength (getValue()));
    const auto angle = rotaryParams.startAngleRadians + sliderPos * (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
    const juce::PathStrokeType stroke (knobRadius * 0.07f, juce::PathStrokeType::mitered, juce::PathStrokeType::butt);
    g.setColour (GTColors::accentActive);

    if (sliderPos > 0.001f)
    {
        const auto r = knobRadius * 1.2f;
        juce::Path valueArc;
        valueArc.addCentredArc (centre.x, centre.y,
                                 r, r,
                                 0.0f, rotaryParams.startAngleRadians, angle, true);
        g.strokePath (valueArc, stroke);
    }

    const juce::Point<float> tickDirection (std::sin (angle), -std::cos (angle));
    juce::Path tick;
    tick.startNewSubPath (centre + tickDirection * (knobRadius * 0.4f));
    tick.lineTo (centre + tickDirection * (knobRadius * 0.65f));
    g.strokePath (tick, stroke);

    const auto text = juce::String (juce::roundToInt (getValue())) + unit;
    g.setColour (juce::Colours::white);
    g.setFont (Fonts::getFont (boundsF.getWidth() * 0.0975f));
    g.drawText (text, boundsF.toNearestInt(), juce::Justification::centredBottom, true);
}
