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
    inline juce::String getLoudnessMatchName() {return "Loudness Match";}
}

namespace ParamTooltips
{
//    ========== CUT FILTERS ===========

    inline juce::String getLowCutKnobTooltip() { return "Adjust how much low end you want to remove"; }
    inline juce::String getHighCutKnobTooltip() { return "Adjust how much high end you want to remove"; }
    inline juce::String getLowCutSlopeTooltip() { return "Select low cut intensity"; }
    inline juce::String getHighCutSlopeTooltip() { return "Select high cut intensity"; }

//    ========== SHELF FILTERS ===========
    inline juce::String getDepthKnobTooltip() { return "Add or take away low end weight"; }
    inline juce::String getDepthFreqTooltip() { return "Choose where that weight sits"; }
    inline juce::String getPresenceKnobTooltip() { return "Add or take away top end bite"; }
    inline juce::String getPresenceFreqTooltip() { return "Choose where that bite sits"; }

//    ========== MAGIC BUTTONS ===========
    inline juce::String getResoButtonTooltip() { return "Kill the harsh peak in your tone"; }
    inline juce::String getResoFaderTooltip() { return "Sweep until the harshness is gone"; }
    inline juce::String getMudButtonTooltip() { return "Clear the mud out of your tone"; }
    inline juce::String getMudFaderTooltip() { return "Sweep until the mud is gone"; }
    inline juce::String getCompButtonTooltip() { return "Tighten up a loose, flappy low end"; }
    inline juce::String getThresholdFaderTooltip() { return "How tight your low end gets"; }

//    ========== LEVELS ===========
    inline juce::String getInputGainKnobTooltip() { return "How hard you hit the plugin"; }
    inline juce::String getOutputGainKnobTooltip() { return "Your level on the way out"; }
    inline juce::String getLevelMatchButtonTooltip() { return "Keep the output as loud as the input"; }

//    ========== HEADER ===========
    inline juce::String getPresetBoxTooltip() { return "Browse and load presets"; }
    inline juce::String getSavePresetTooltip() { return "Save your current sound as a preset"; }
    inline juce::String getSettingsTooltip() { return "Open the settings"; }
    inline juce::String getInfoTooltip() { return "Version and contact info"; }
    inline juce::String getPluginBypassTooltip() { return "Turn the whole plugin on or off"; }

//    ========== GUI ===========
    inline juce::String getResizeUITooltip() { return "Make the window bigger or smaller"; }
}

inline juce::String getComponentTooltip (juce::String& paramIDName)
{

}

std::map<juce::String, juce::String> parametersTooltips
{
    {ParamIDs::getLowCutFreqName(), },
};