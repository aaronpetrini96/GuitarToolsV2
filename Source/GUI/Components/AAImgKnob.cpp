#include "AAImgKnob.h"
#include "GTLook.h"
#include "GUI/LAF/LookAndFeel.h"

#include <cmath>

AAImgKnob::AAImgKnob (juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const juce::String& name, const juce::String& suffix, bool drawFromMiddle, bool fillFromEnd, bool useFillStroke)
    : attachment (apvts, parameterID.getParamID(), *this),
      unit (suffix),
      drawFromMiddle (drawFromMiddle),
      fillFromEnd (fillFromEnd),
      useFillStroke (useFillStroke)
{
    setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    setMouseDragSensitivity(400);

    setRotaryParameters (startAngle, endAngle, true);

    nameLabel.setText (name, juce::dontSendNotification);
    nameLabel.setJustificationType (juce::Justification::centred);
    nameLabel.setColour (juce::Label::textColourId, GTColors::clearGrey);
    nameLabel.setInterceptsMouseClicks (false, false);
    nameLabel.setBorderSize (juce::BorderSize<int> (0));
    addAndMakeVisible (nameLabel);

    valueLabel.setJustificationType (juce::Justification::centred);
    valueLabel.setColour (juce::Label::textColourId, GTColors::clearGrey);
    valueLabel.setInterceptsMouseClicks (false, false);
    valueLabel.setBorderSize (juce::BorderSize<int> (0));
    addAndMakeVisible (valueLabel);

    onValueChange = [this] { updateValueLabelText(); };
    updateValueLabelText();
}

void AAImgKnob::updateValueLabelText()
{
    valueLabel.setText (juce::String (juce::roundToInt (getValue())) + unit, juce::dontSendNotification);
}

void AAImgKnob::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    if (boundsF.isEmpty())
        return;

    const auto fontBounds = boundsF.withHeight(boundsF.getHeight() * 0.2f);
    nameLabel.setFont (GTFonts::jetBrainsMonoMedium(fontBounds.getHeight() * 0.5f).withExtraKerningFactor(0.02f));
    nameLabel.setBounds (fontBounds.toNearestInt());
    valueLabel.setFont (GTFonts::jetBrainsMonoMedium(fontBounds.getHeight() * 0.45f));
    valueLabel.setBounds (fontBounds.withBottomY(boundsF.getHeight()).toNearestInt());
}

void AAImgKnob::paint (juce::Graphics& g)
{
    if (!knobImage.isValid())
        return;

    const auto boundsF = getLocalBounds().toFloat();
    const auto imageBounds = boundsF.withSizeKeepingCentre (boundsF.getWidth() * imageScale, boundsF.getHeight() * imageScale);
    g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
    g.setOpacity(0.7f);
    g.drawImage (knobImage, imageBounds, juce::RectanglePlacement::centred, false);

    const auto knobRadius = imageBounds.getWidth() * 0.25f;
    const auto centre = imageBounds.getCentre();
    const auto sliderPos = static_cast<float> (valueToProportionOfLength (getValue()));
    const auto angle = startAngle + sliderPos * (endAngle - startAngle);
    const juce::PathStrokeType stroke (knobRadius * 0.075f, juce::PathStrokeType::mitered, juce::PathStrokeType::butt);
    const auto r = knobRadius * 1.5f;

    if (useFillStroke)
        paintFillStroke (g, centre, r, stroke, angle);
    else
        paintLightDots (g, centre, r, knobRadius, angle);

    g.setColour (GTColors::accentActive);

    const juce::Point<float> tickDirection (std::sin (angle), -std::cos (angle));
    juce::Path tick;
    tick.startNewSubPath (centre + tickDirection * (knobRadius * 0.4f));
    tick.lineTo (centre + tickDirection * (knobRadius * 0.65f));
    g.strokePath (tick, stroke);
}

void AAImgKnob::paintFillStroke (juce::Graphics& g, juce::Point<float> centre, float r, const juce::PathStrokeType& stroke, float angle)
{
    juce::Path backgroundArc;
    backgroundArc.addCentredArc (centre.x, centre.y, r, r, 0.0f, startAngle, endAngle, true);
    g.setColour (GTColors::dividerLinesStart);
    g.strokePath (backgroundArc, stroke);

    // keeps a stub as wide as the tick when the value sits on the arc's origin
    const auto minStubAngle = stroke.getStrokeThickness() * 0.5f / r;
    const auto midAngle = (startAngle + endAngle) * 0.5f;
    const auto fillFromAngle = drawFromMiddle ? juce::jmin (angle, midAngle) : (fillFromEnd ? angle : startAngle);
    const auto fillToAngle = drawFromMiddle ? juce::jmax (angle, midAngle) : (fillFromEnd ? endAngle : angle);

    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y, r, r, 0.0f, fillFromAngle - minStubAngle, fillToAngle + minStubAngle, true);
    g.setColour (GTColors::accentActive);
    g.strokePath (valueArc, stroke);
}

void AAImgKnob::paintLightDots (juce::Graphics& g, juce::Point<float> centre, float r, float knobRadius, float angle)
{
    const auto dotRadius = knobRadius * 0.05f;
    const auto midAngle = (startAngle + endAngle) * 0.5f;
    const auto litFromAngle = drawFromMiddle ? juce::jmin (angle, midAngle) : (fillFromEnd ? angle : startAngle);
    const auto litToAngle = drawFromMiddle ? juce::jmax (angle, midAngle) : (fillFromEnd ? endAngle : angle);

    for (auto dotIndex = 0; dotIndex < numberOfDots; ++dotIndex)
    {
        const auto dotProportion = static_cast<float> (dotIndex) / static_cast<float> (numberOfDots - 1);
        const auto dotAngle = startAngle + dotProportion * (endAngle - startAngle);
        const juce::Point<float> dotDirection (std::sin (dotAngle), -std::cos (dotAngle));
        const auto dotCentre = centre + dotDirection * r * 0.77f;

        const auto isLit = dotAngle >= litFromAngle && dotAngle <= litToAngle;
//        const auto litColour = GTColors::accentActive.withBrightness (fillFromEnd ? juce::jmap (dotProportion, 1.0f, 0.7f) : juce::jmap (dotProportion, 0.7f, 1.0f));
        g.setColour (isLit ? GTColors::accentActive : GTColors::dividerLinesStart);
        g.fillEllipse (dotCentre.x - dotRadius, dotCentre.y - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
    }
}
