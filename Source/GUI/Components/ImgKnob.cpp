#include "ImgKnob.h"
#include "GTLook.h"
#include "GUI/LAF/LookAndFeel.h"

ImgKnob::ImgKnob (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const juce::String& suffix, bool drawFromMiddle)
    : attachment (apvts, parameterID.getParamID(), *this),
      unit (suffix),
      drawFromMiddle (drawFromMiddle)
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
    const juce::PathStrokeType stroke (knobRadius * 0.075f, juce::PathStrokeType::mitered, juce::PathStrokeType::butt);
    g.setColour(GTColors::dividerLines);

    const auto r = knobRadius * 1.5f;

    if (drawFromMiddle)
        paintFillStroke (g, centre, r, stroke, angle);
    else
        paintLightDots (g, centre, r, knobRadius, angle);

    g.setColour (GTColors::accentActive);

    const juce::Point<float> tickDirection (std::sin (angle), -std::cos (angle));
    juce::Path tick;
    tick.startNewSubPath (centre + tickDirection * (knobRadius * 0.4f));
    tick.lineTo (centre + tickDirection * (knobRadius * 0.65f));
    g.strokePath (tick, stroke);

    const auto text = juce::String (juce::roundToInt (getValue())) + unit;
    g.setColour (juce::Colours::white);
    g.setFont (Fonts::getFont (boundsF.getWidth() * 0.0975f));
    g.drawText (text, boundsF.translated (0.0f, boundsF.getHeight() * JUCE_LIVE_CONSTANT (0.0f)).toNearestInt(), juce::Justification::centredBottom, true);
}

void ImgKnob::paintFillStroke (juce::Graphics& g, juce::Point<float> centre, float r, const juce::PathStrokeType& stroke, float angle)
{
    const auto rotaryParams = getRotaryParameters();
    juce::Path backgroundArc;
    backgroundArc.addCentredArc (centre.x, centre.y,
                                  r, r, 0.0f,
                                   rotaryParams.startAngleRadians, rotaryParams.endAngleRadians, true);
    g.strokePath (backgroundArc, stroke);

    g.setColour (GTColors::accentActive);

    const auto minStubAngle = 0.05f;
    const auto midAngle = (rotaryParams.startAngleRadians + rotaryParams.endAngleRadians) * 0.5f;
    const auto valueArcStart = drawFromMiddle ? juce::jmin (angle, midAngle) - minStubAngle : rotaryParams.startAngleRadians;
    const auto valueArcEnd = drawFromMiddle ? juce::jmax (angle, midAngle) + minStubAngle : juce::jmax (angle, rotaryParams.startAngleRadians + minStubAngle);
    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y,
                             r, r,
                             0.0f, valueArcStart, valueArcEnd, true);
    g.strokePath (valueArc, stroke);
}

void ImgKnob::paintLightDots (juce::Graphics& g, juce::Point<float> centre, float r, float knobRadius, float angle)
{
    const auto rotaryParams = getRotaryParameters();
    const auto numberOfDots = 11;
    const auto dotRadius = knobRadius * 0.05f;
    const auto minStubAngle = 0.05f;
    const auto midAngle = (rotaryParams.startAngleRadians + rotaryParams.endAngleRadians) * 0.5f;
    const auto litFromAngle = drawFromMiddle ? juce::jmin (angle, midAngle) - minStubAngle : rotaryParams.startAngleRadians;
    const auto litToAngle = drawFromMiddle ? juce::jmax (angle, midAngle) + minStubAngle : angle;

    for (auto dotIndex = 0; dotIndex < numberOfDots; ++dotIndex)
    {
        const auto dotProportion = static_cast<float> (dotIndex) / static_cast<float> (numberOfDots - 1);
        const auto dotAngle = rotaryParams.startAngleRadians + dotProportion * (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
        const juce::Point<float> dotDirection (std::sin (dotAngle), -std::cos (dotAngle));
        const auto dotCentre = centre + dotDirection * r * 0.77f;

        const auto isLit = dotAngle >= litFromAngle && dotAngle <= litToAngle;
        const auto litColour = GTColors::accentActive.withBrightness (juce::jmap (dotProportion, 0.7f, 1.0f));
        g.setColour (isLit ? litColour : GTColors::dividerLines);
        g.fillEllipse (dotCentre.x - dotRadius, dotCentre.y - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
    }
}
