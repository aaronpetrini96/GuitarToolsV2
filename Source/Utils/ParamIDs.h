//
// Created by Aaron Petrini on 27/08/2026.
//

#pragma once
#include <juce_core/juce_core.h>


namespace ParamIDs
{

    // === 1176 ===
    inline juce::String getLowCutFreqName() { return "LowCut Freq"; }
    inline juce::String getHighCutFreqName() { return "HighCut Freq"; }
    inline juce::String getLowCutSlopeName() { return "LowCut Slope"; }
    inline juce::String getHighCutSlopeName() { return "HighCut Slope"; }
    inline juce::String getResoFreqName() { return "Reso Freq"; }
    inline juce::String getResoBypassName() { return "Reso Bypass"; }
    inline juce::String getMudFreqName() { return "Mud Freq"; }
    inline juce::String getMudBypassName() { return "Mud Bypass"; }
    inline juce::String getPresenceFreqName() { return "Presence Freq"; }
    inline juce::String getPresenceGainName() { return "Presence Gain"; }
    inline juce::String getDepthFreqName() { return "Depth Freq"; }
    inline juce::String getDepthGainName() { return "Depth Gain"; }
    inline juce::String getPluginBypassName() { return "Plugin Bypass"; }
    inline juce::String getCompGainInName() { return "Comp Gain In"; }
    inline juce::String getCompGainOutName() { return "Comp Gain Out"; }
    inline juce::String getThresholdName() { return "Threshold"; }
    inline juce::String getAttackName() { return "Attack"; }
    inline juce::String getReleaseName() { return "Release"; }
    inline juce::String getRatioName() { return "Ratio"; }
    inline juce::String getCompBypassName() { return "Comp Bypass"; }
//    inline juce::String getCompRangeStartName() { return "Comp Range Start"; }
//    inline juce::String getCompRangeEndName() { return "Comp Range End"; }
}
