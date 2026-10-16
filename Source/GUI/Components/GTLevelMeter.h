#pragma once

#include "../../GTLook.h"

class GTLevelMeter : public juce::Component, private juce::Timer
{
public:
    enum class ScalePlacement { none, left, right, centre };

    GTLevelMeter (std::atomic<float>& leftLevel, std::atomic<float>& rightLevel, ScalePlacement scalePlacementToUse = ScalePlacement::none)
        : leftLevelSource (leftLevel), rightLevelSource (rightLevel), scalePlacement (scalePlacementToUse)
    {
        smoothingCoefficient = 1.f - std::exp (-1.f / (static_cast<float> (refreshRateHz) * smoothingTimeSeconds));
        startTimerHz (refreshRateHz);
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto lightGap = boundsF.getHeight() * 0.012f;
        const auto lightRowGap = lightGap * 3.f;
        lightDiameter = boundsF.getHeight() * 0.1f;
        const auto lightCentreDistance = lightDiameter + lightGap;

        const auto lightRowCentreDistance = lightDiameter + lightRowGap;
        const auto scaleFont = GTFonts::ibmPlexSansMedium ( boundsF.getHeight() * 0.095f);
        const auto scaleLabelsGap = lightGap * 3.5f;
        auto scaleLabelsWidth = 0.f;
        for (const auto& label : scaleLabels)
            scaleLabelsWidth = juce::jmax (scaleLabelsWidth, juce::GlyphArrangement::getStringWidth (scaleFont, label));
        const auto scaleWidth = scalePlacement != ScalePlacement::none ? scaleLabelsWidth + scaleLabelsGap : 0.f;
        const auto lightsWidth = lightCentreDistance + lightDiameter;
        const auto groupX = boundsF.getCentreX() - (scaleWidth + lightsWidth) * 0.5f;
        const auto lightsX = scalePlacement == ScalePlacement::left ? groupX + scaleWidth : groupX;
        const auto leftCentreX = lightsX + lightDiameter * 0.5f;
        const auto rightCentreX = leftCentreX + lightCentreDistance + (scalePlacement == ScalePlacement::centre ? scaleWidth : 0.f);

        for (const auto isGlowPass : { true, false })
        {
            for (size_t index {0}; index < numberOfLights; ++index)
            {
                const auto centreY = boundsF.getCentreY() + lightRowCentreDistance * (static_cast<float> (numberOfLights - 1) * 0.5f - static_cast<float> (index));
                paintLight (g, {leftCentreX, centreY}, index, leftLevelDb, leftClipped, isGlowPass);
                paintLight (g, {rightCentreX, centreY}, index, rightLevelDb, rightClipped, isGlowPass);
            }
        }

        if (scalePlacement != ScalePlacement::none)
        {
            const auto scaleSpan = lightRowCentreDistance * static_cast<float> (numberOfLights - 1);
            const auto scaleTopY = boundsF.getCentreY() - scaleSpan * 0.5f;
            const auto scaleLabelsX = scalePlacement == ScalePlacement::centre ? boundsF.getCentreX() - scaleLabelsWidth * 0.5f
                                    : scalePlacement == ScalePlacement::right  ? lightsX + lightsWidth + scaleLabelsGap
                                                                               : lightsX - scaleLabelsGap - scaleLabelsWidth;
            const auto scaleLabelsArea = boundsF.withX (scaleLabelsX).withWidth (scaleLabelsWidth);

            g.setColour (GTColors::almostWhite.withAlpha (0.85f));
            g.setFont (scaleFont);

            for (int labelIndex {0}; labelIndex < scaleLabels.size(); ++labelIndex)
            {
                const auto labelCentreY = scaleTopY + scaleSpan * static_cast<float> (labelIndex) / static_cast<float> (scaleLabels.size() - 1);
                g.drawText (scaleLabels[labelIndex], scaleLabelsArea.withHeight (lightDiameter).withCentre ({scaleLabelsArea.getCentreX(), labelCentreY}), juce::Justification::centred);
            }
        }
    }

    void mouseDown (const juce::MouseEvent&) override
    {
        leftClipped = rightClipped = false;
        repaint();
    }

private:
    void paintLight (juce::Graphics& g, juce::Point<float> ledCentre, size_t index, float levelDb, bool isClipped, bool isGlowPass)
    {
        const auto ledDiameter = lightDiameter * 0.8f;
        const auto barPosition = juce::square (juce::jmap (levelDb, minimumDb, 0.f, 0.f, 1.f)) * static_cast<float> (numberOfLights - 1);
        const auto litAmount = index == numberOfLights - 1 ? (isClipped ? 1.f : 0.f) : juce::jlimit (0.f, 1.f, barPosition - static_cast<float> (index));
        const auto ledColour = index == numberOfLights - 1 ? GTColors::meterRed : index >= 3 ? GTColors::meterYellow : GTColors::meterGreen;
        const auto ledBounds = juce::Rectangle<float> (ledDiameter, ledDiameter).withCentre (ledCentre);

        if (isGlowPass)
        {
            if (litAmount > 0.f)
            {
                const auto glowRadius = ledDiameter * 0.9f;
                g.setGradientFill (juce::ColourGradient (ledColour.withAlpha (0.2f * litAmount), ledCentre, ledColour.withAlpha (0.f), ledCentre.translated (glowRadius, 0.f), true));
                g.fillEllipse (ledBounds.withSizeKeepingCentre (glowRadius * 2.f, glowRadius * 2.f));
            }
            return;
        }

        g.setColour (GTColors::superDarkBackground);
        g.fillEllipse (ledBounds.withSizeKeepingCentre (lightDiameter, lightDiameter));

        const auto centreColour = ledColour.withMultipliedBrightness (0.4f).interpolatedWith (ledColour.interpolatedWith (juce::Colours::white, 0.5f), litAmount);
        const auto edgeColour = ledColour.withMultipliedBrightness (0.15f).interpolatedWith (ledColour, litAmount);
        g.setGradientFill (juce::ColourGradient (centreColour, ledCentre.translated (0.f, ledDiameter * 0.3f), edgeColour, { ledCentre.x, ledBounds.getY() }, true));
        g.fillEllipse (ledBounds);

        g.setColour (juce::Colours::white.withAlpha (0.55f));
        g.fillEllipse (ledBounds.withSizeKeepingCentre (ledDiameter * 0.15f, ledDiameter * 0.12f).translated (0.f, -ledDiameter * 0.22f));
    }

    void timerCallback() override
    {
        const auto fallDb = fallRateDbPerSecond / static_cast<float> (refreshRateHz);
        auto needsRepaint = false;

        const auto updateLevel = [&] (std::atomic<float>& levelSource, float& targetDb, float& levelDb, bool& isClipped)
        {
            const auto peakDb = juce::Decibels::gainToDecibels (levelSource.exchange (0.f), minimumDb);
            targetDb = juce::jmax (peakDb, targetDb - fallDb);

            if (peakDb >= 0.f && ! isClipped)
            {
                isClipped = true;
                needsRepaint = true;
            }

            const auto previousLevelDb = levelDb;
            levelDb = juce::jmax (targetDb, levelDb + (targetDb - levelDb) * smoothingCoefficient);
            needsRepaint = needsRepaint || std::abs (levelDb - previousLevelDb) > repaintThresholdDb;

            if (isClipped && levelDb < clipReleaseDb)
            {
                isClipped = false;
                needsRepaint = true;
            }
        };

        updateLevel (leftLevelSource, leftTargetDb, leftLevelDb, leftClipped);
        updateLevel (rightLevelSource, rightTargetDb, rightLevelDb, rightClipped);

        if (needsRepaint)
            repaint();
    }

    const int refreshRateHz {30};
    const float minimumDb {-48.f};
    const float fallRateDbPerSecond {24.f};
    const float repaintThresholdDb {0.01f};
    const float clipReleaseDb {-6.f};

    const float smoothingTimeSeconds {0.04f};
    float smoothingCoefficient {0.f};

    std::atomic<float>& leftLevelSource;
    std::atomic<float>& rightLevelSource;
    float leftTargetDb {minimumDb}, rightTargetDb {minimumDb};
    float leftLevelDb {minimumDb}, rightLevelDb {minimumDb};
    bool leftClipped {false}, rightClipped {false};

    const int numberOfLights {6};
    float lightDiameter {0.f};

    const ScalePlacement scalePlacement;
    const juce::StringArray scaleLabels {"  0", " -9", "-20", "-48"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GTLevelMeter)
};
