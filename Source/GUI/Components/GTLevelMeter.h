#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../AALook.h"

class GTLevelMeter : public juce::Component, private juce::Timer
{
public:
    GTLevelMeter (std::atomic<float>& leftLevel, std::atomic<float>& rightLevel)
        : leftLevelSource (leftLevel), rightLevelSource (rightLevel)
    {
        startTimerHz (refreshRateHz);
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto barWidth = boundsFWidth * 0.49f;
        const auto cornerSize = barWidth * 0.12f;
        const auto slotInset = barWidth * 0.1f;
        const auto segmentPitch = (boundsFHeight - slotInset * 2.f) / static_cast<float> (numberOfSegments);
        const auto segmentLineThickness = segmentPitch * 0.3f;

        const auto leftBarBounds = boundsF.withWidth (barWidth);
        const auto rightBarBounds = leftBarBounds.withRightX (boundsF.getRight());

        for (const auto& [barBounds, levelDb] : { std::pair { leftBarBounds, leftLevelDb }, std::pair { rightBarBounds, rightLevelDb } })
        {
            g.setColour (GTColors::superDarkBackground);
            g.fillRoundedRectangle (barBounds, cornerSize);

            const auto slotBounds = barBounds.reduced (slotInset);
            g.setColour (GTColors::meterUnlit);
            g.fillRect (slotBounds);

            const auto dbToY = [&slotBounds] (float db) { return juce::jmap (juce::jlimit (minimumDb, maximumDb, db), minimumDb, maximumDb, slotBounds.getBottom(), slotBounds.getY()); };
            const auto levelY = dbToY (levelDb);

            for (const auto& [zoneBottomDb, zoneTopDb, zoneColour] : { std::tuple { minimumDb, yellowStartDb, GTColors::meterGreen },
                                                                        std::tuple { yellowStartDb, redStartDb, GTColors::meterYellow },
                                                                        std::tuple { redStartDb, maximumDb, GTColors::meterRed } })
            {
                g.setColour (zoneColour);
                g.fillRect (slotBounds.withTop (juce::jmax (levelY, dbToY (zoneTopDb))).withBottom (dbToY (zoneBottomDb)));
            }

            g.setColour (GTColors::meterSegmentLine);
            for (int segment {1}; segment < numberOfSegments; ++segment)
            {
                const auto segmentLineY = slotBounds.getY() + segmentPitch * static_cast<float> (segment);
                g.fillRect (slotBounds.withY (segmentLineY - segmentLineThickness * 0.5f).withHeight (segmentLineThickness));
            }
        }
    }

private:
    void timerCallback() override
    {
        const auto fallDb = fallRateDbPerSecond / static_cast<float> (refreshRateHz);
        const auto smoothingCoefficient = 1.f - std::exp (-1.f / (static_cast<float> (refreshRateHz) * smoothingTimeSeconds));
        auto needsRepaint = false;

        const auto updateLevel = [&] (std::atomic<float>& levelSource, float& targetDb, float& levelDb)
        {
            const auto peakDb = juce::Decibels::gainToDecibels (levelSource.exchange (0.f), minimumDb);
            targetDb = juce::jmax (peakDb, targetDb - fallDb);

            const auto previousLevelDb = levelDb;
            levelDb += (targetDb - levelDb) * smoothingCoefficient;
            needsRepaint = needsRepaint || std::abs (levelDb - previousLevelDb) > repaintThresholdDb;
        };

        updateLevel (leftLevelSource, leftTargetDb, leftLevelDb);
        updateLevel (rightLevelSource, rightTargetDb, rightLevelDb);

        if (needsRepaint)
            repaint();
    }

    static constexpr int refreshRateHz {60};
    static constexpr int numberOfSegments {10};
    static constexpr float minimumDb {-60.f};
    static constexpr float maximumDb {0.f};
    static constexpr float yellowStartDb {-18.f};
    static constexpr float redStartDb {-6.f};
    static constexpr float fallRateDbPerSecond {24.f};
    static constexpr float smoothingTimeSeconds {0.04f};
    static constexpr float repaintThresholdDb {0.01f};

    std::atomic<float>& leftLevelSource;
    std::atomic<float>& rightLevelSource;
    float leftTargetDb {minimumDb}, rightTargetDb {minimumDb};
    float leftLevelDb {minimumDb}, rightLevelDb {minimumDb};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GTLevelMeter)
};
