//
// Created by Aaron Petrini on 27/08/2026.
//

#pragma once
#include <juce_core/juce_core.h>
#include <array>
#include <map>

namespace ParamIDs
{
    inline juce::String getLowCutFreqName() { return "LowCut Freq"; }
    inline juce::String getHighCutFreqName() { return "HighCut Freq"; }
    inline juce::String getLowCutSlopeName() { return "LowCut Slope"; }
    inline juce::String getHighCutSlopeName() { return "HighCut Slope"; }
    inline juce::String getResoFreqName() { return "Reso Freq"; }
    inline juce::String getResoActiveName() { return "Reso Active"; }
    inline juce::String getMudAmountName() { return "Mud Amount"; }
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
    inline juce::String getResoExtremeActiveName() { return "Reso Mode Active"; }
    inline juce::String getMudExtremeActiveName() { return "Mud Mode Active"; }
    inline juce::String getCompExtremeActiveName() { return "Comp Mode Active"; }
    inline juce::String getLowTuningActiveName() { return "Low Tuning Active"; }
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
    inline constexpr float lowCutRangeEnd {1000.f};
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

    inline constexpr float mudFreq {250.f};
    inline constexpr float lowTuningMudFreq {180.f};
    inline constexpr float mudAmount {33.f};
    inline constexpr float mudMaxCutDb {-12.f};
    inline constexpr bool mudActive {false};
    inline constexpr bool mudExtremeActive {false};
    inline constexpr float mudQ {0.7f};
    inline constexpr float mudExtremeQ {0.4f};

    inline constexpr float compAmount {50.f};
    inline constexpr float amountIntervalValue {0.1f};
    inline constexpr bool compActive {false};
    inline constexpr bool compExtremeActive {false};
    inline constexpr bool lowTuningActive {false};

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

}

namespace ParamTooltips
{
//    ========== HEADER ===========
    inline juce::String getPresetBoxTooltip() { return "Browse and load your presets"; }
    inline juce::String getSavePresetTooltip() { return "Save your current sound as a preset"; }
    inline juce::String getSettingsTooltip() { return "Open settings panel"; }
    inline juce::String getPreviousPresetTooltip() { return "Load previous preset"; }
    inline juce::String getNextPresetTooltip() { return "Load next preset"; }
    inline juce::String getUndoTooltip() { return "Undo last change"; }
    inline juce::String getRedoTooltip() { return "Redo last change"; }

//    ========== SETTINGS MENU ===========
    inline juce::String getShowTooltipsTooltip() { return "Show control descriptions at the bottom"; }
    inline juce::String getDefaultUISizeTooltip() { return "Save current UI size as the new default for the plugin"; }
    inline juce::String getInfoTooltip() { return "View plugin information"; }
    inline juce::String getContactTooltip() { return "Send an email for feedback or support"; }

//    ========== GUI ===========
    inline juce::String getResizeUITooltip() { return "Change the window size"; }

    inline const std::map<juce::String, juce::String> parameterTooltips
    {
        {ParamIDs::getLowCutFreqName(),             "Set how much low end is removed"},
        {ParamIDs::getHighCutFreqName(),            "Set how much high end is removed"},
        {ParamIDs::getLowCutSlopeName(),            "Select how fast the level drops past the cutoff"},
        {ParamIDs::getHighCutSlopeName(),           "Select how fast the level drops past the cutoff"},
        {ParamIDs::getDepthGainName(),              "Boost or reduce bass in your tone"},
        {ParamIDs::getDepthModeName(),              "Select depth mode"},
        {ParamIDs::getPresenceGainName(),           "Boost or reduce bite in your tone"},
        {ParamIDs::getPresenceModeName(),           "Select presence mode"},
        {ParamIDs::getResoActiveName(),             "Reduce high-mid harshness, use the fader to find it"},
        {ParamIDs::getResoFreqName(),               "Reduce high-mid harshness, use the fader to find it"},
        {ParamIDs::getMudActiveName(),              "Reduce boomy low-mid buildup, use the fader to set the amount"},
        {ParamIDs::getMudAmountName(),              "Reduce boomy low-mid buildup, use the fader to set the amount"},
        {ParamIDs::getCompActiveName(),             "Make your low end tight and even, use the fader to set the amount"},
        {ParamIDs::getCompAmountName(),             "Make your low end tight and even, use the fader to set the amount"},
        {ParamIDs::getResoExtremeActiveName(),      "Increase resonance reduction"},
        {ParamIDs::getMudExtremeActiveName(),       "Widen reduction range"},
        {ParamIDs::getCompExtremeActiveName(),      "Increase tightening strength"},
        {ParamIDs::getLowTuningActiveName(),        "Adapt processing for low tunings"},
        {ParamIDs::getInputGainName(),              "Set the level going into the plugin"},
        {ParamIDs::getOutputGainName(),             "Set the level coming out of the plugin"},
        {ParamIDs::getLoudnessMatchActiveName(),    "Keep the output level equal to the input"},
        {ParamIDs::getPluginBypassActiveName(),     "Turn GuitarTools ON/OFF"},
    };
}

namespace ParamHelpers
{
    inline auto getStringFromValue (const int numDecimals)
    {
        return [numDecimals] (float value, int)
        {
            return numDecimals > 0 ? juce::String (value, numDecimals) : juce::String (juce::roundToInt (value));
        };
    }

    inline juce::String getTooltipForParam (const juce::String& paramIDName)
    {
        const auto tooltip = ParamTooltips::parameterTooltips.find (paramIDName);
        return tooltip != ParamTooltips::parameterTooltips.end() ? tooltip->second : juce::String{};
    }
}
