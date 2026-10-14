//
// Created by Aaron Petrini on 27/08/2026.
//

#pragma once
#include <juce_core/juce_core.h>
#include <array>

namespace ParamIDs
{
    inline juce::String getLowCutFreqName() { return "LowCut Freq"; }
    inline juce::String getHighCutFreqName() { return "HighCut Freq"; }
    inline juce::String getLowCutSlopeName() { return "LowCut Slope"; }
    inline juce::String getHighCutSlopeName() { return "HighCut Slope"; }
    inline juce::String getResoFreqName() { return "Reso Freq"; }
    inline juce::String getResoActiveName() { return "Reso Active"; }
    inline juce::String getMudFreqName() { return "Mud Freq"; }
    inline juce::String getMudActiveName() { return "Mud Active"; }
    inline juce::String getPresenceModeName() { return "Presence Mode"; }
    inline juce::String getPresenceGainName() { return "Presence Gain"; }
    inline juce::String getDepthModeName() { return "Depth Mode"; }
    inline juce::String getDepthGainName() { return "Depth Gain"; }
    inline juce::String getPluginBypassActiveName() { return "Plugin Bypass Active"; }
    inline juce::String getInputGainName() { return "Input Gain"; }
    inline juce::String getOutputGainName() { return "Output Gain"; }
    inline juce::String getCompAmountName() { return "Comp Amount"; }
    inline juce::String getCompActiveName() { return "Comp Active"; }
    inline juce::String getLoudnessMatchActiveName() {return "Loudness Match Active";}
    inline juce::String getGainLimitActiveName() { return "Gain Limit Active"; }
    inline juce::String getResoExtremeActiveName() { return "Reso Mode Active"; }
    inline juce::String getMudExtremeActiveName() { return "Mud Mode Active"; }
    inline juce::String getCompExtremeActiveName() { return "Comp Mode Active"; }
}

namespace DefaultValues
{
    // GENERAL
    inline constexpr bool pluginBypassActive {false};
    inline constexpr double parameterSmoothingTime {0.05};
    inline constexpr float frequencyIntervalValue {0.01f};
    inline constexpr float decibelIntervalValue {0.001f};
    inline constexpr float gainRangeStart {-12.f};
    inline constexpr float gainRangeEnd {12.f};

//    ========== TONE RANGE ===========
    inline constexpr float lowCutFreq {20.f};
    inline constexpr float lowCutRangeStart {20.f};
    inline constexpr float lowCutRangeEnd {2500.f};
    inline constexpr float highCutFreq {20000.f};
    inline constexpr float highCutRangeStart {1000.f};
    inline constexpr float highCutRangeEnd {20000.f};
    inline constexpr int lowCutSlope {0};
    inline constexpr int highCutSlope {0};

//    ========== CLEAN-UP ===========
    inline constexpr float resoFreq {3850.f};
    inline constexpr float resoRangeStart {2000.f};
    inline constexpr float resoRangeEnd {9000.f};
    inline constexpr bool resoActive {false};
    inline constexpr bool resoExtremeActive {false};
    inline constexpr float resoQ {7.f};
    inline constexpr float resoGainDb {-6.f};
    inline constexpr float resoExtremeQ {10.f};
    inline constexpr float resoExtremeGainDb {-12.f};

    inline constexpr float mudFreq {120.f};
    inline constexpr float mudRangeStart {60.f};
    inline constexpr float mudRangeEnd {300.f};
    inline constexpr bool mudActive {false};
    inline constexpr bool mudExtremeActive {false};
    inline constexpr float mudQ {0.7f};
    inline constexpr float mudGainDb {-4.f};
    inline constexpr float mudExtremeQ {0.7f};
    inline constexpr float mudExtremeGainDb {-8.f};

    inline constexpr float compAmount {50.f};
    inline constexpr float compAmountRangeStart {0.f};
    inline constexpr float compAmountRangeEnd {100.f};
    inline constexpr float compAmountIntervalValue {0.1f};
    inline constexpr bool compActive {false};
    inline constexpr bool compExtremeActive {false};

//    ========== COLOR ===========
    inline constexpr int presenceMode {1};
    inline constexpr float presenceGain {0.f};
    inline constexpr std::array<float, 4> presenceFreqByMode {4500.f, 6250.f, 8000.f, 12000.f};
    inline constexpr std::array<float, 4> presenceResonanceByMode {0.5f, 0.4f, 0.25f, 0.1f};

    inline constexpr int depthMode {1};
    inline constexpr float depthGain {0.f};
    inline constexpr std::array<float, 4> depthFreqByMode {50.f, 80.f, 150.f, 275.f};
    inline constexpr std::array<float, 4> depthResonanceByMode {0.3f, 0.25f, 0.2f, 0.1f};

//    ========== LEVELS ===========
    inline constexpr float inputGainValue {0.f};
    inline constexpr float outputGainValue {0.f};

    inline constexpr bool loudnessMatchActive {false};
    inline constexpr bool gainLimitActive {false};

}

namespace ParamTooltips
{
//    ========== HEADER ===========
    inline juce::String getPresetBoxTooltip() { return "Browse and load your presets"; }
    inline juce::String getSavePresetTooltip() { return "Save the current settings as a preset"; }
    inline juce::String getSettingsTooltip() { return "Open the plugin settings"; }
    inline juce::String getInfoTooltip() { return "View plugin information and contact details"; }

//    ========== GUI ===========
    inline juce::String getResizeUITooltip() { return "Change the plugin window size"; }

    inline const std::map<juce::String, juce::String> parameterTooltips
    {
        {ParamIDs::getLowCutFreqName(),             "Low cut: Adjust how much low end you want to remove"},
        {ParamIDs::getHighCutFreqName(),            "High cut: Adjust how much high end you want to remove"},
        {ParamIDs::getLowCutSlopeName(),            "Low cut slope: Choose how sharply the low end is removed"},
        {ParamIDs::getHighCutSlopeName(),           "High cut slope: Choose how sharply the high end is removed"},
        {ParamIDs::getDepthGainName(),              "Depth: Adjust how much low-end body your tone needs"},
        {ParamIDs::getDepthModeName(),              "Depth mode: Choose where the low-end body is focused"},
        {ParamIDs::getPresenceGainName(),           "Presence: Adjust the amount of presence and bite"},
        {ParamIDs::getPresenceModeName(),           "Presence mode: Choose where the presence adjustment is focused"},
        {ParamIDs::getResoActiveName(),             "Kill reso: Tame harsh resonances"},
        {ParamIDs::getResoFreqName(),               "Reso frequency: Find the harsh resonance you want to tame"},
        {ParamIDs::getMudActiveName(),              "Clear mud: Reduce muddy buildup"},
        {ParamIDs::getMudFreqName(),                "Mud frequency: Find the muddy frequency you want to reduce"},
        {ParamIDs::getCompActiveName(),             "Tame lows: Tighten the dynamics of your low end"},
        {ParamIDs::getCompAmountName(),             "Tame lows amount: Adjust how much the low end is tightened"},
        {ParamIDs::getResoExtremeActiveName(),      "Kill reso max: Tame harsh resonances even harder"},
        {ParamIDs::getMudExtremeActiveName(),       "Clear mud max: Reduce muddy buildup even harder"},
        {ParamIDs::getCompExtremeActiveName(),      "Tame lows max: Tighten your low end even harder"},
        {ParamIDs::getInputGainName(),             "Input: Adjust the level going into the plugin"},
        {ParamIDs::getOutputGainName(),             "Output: Adjust the level leaving the plugin"},
        {ParamIDs::getLoudnessMatchActiveName(),    "Match level: Match the output loudness to the input"},
        {ParamIDs::getGainLimitActiveName(),        "Clip guard: Stop the gain from pushing the output into clipping"},
        {ParamIDs::getPluginBypassActiveName(),     "Bypass: Bypass all plugin processing"},
    };
}

namespace ParamHelpers
{
    inline juce::String getTooltipForParam (const juce::String& paramIDName)
    {
        const auto tooltip = ParamTooltips::parameterTooltips.find (paramIDName);
        return tooltip != ParamTooltips::parameterTooltips.end() ? tooltip->second : juce::String{};
    }
}
