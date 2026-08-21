//
// Created by Aaron Petrini on 20/08/2026.
//
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace  GTColors
{
    inline juce::Colour controlBackground { juce::Colour(100, 100, 110).darker(0.5f) };
    inline juce::Colour footerBackground {juce::Colour(15, 15, 15).withAlpha(0.8f)};

    inline juce::Colour dividerLines {15, 15, 15};
    inline juce::Colour mainPanelTop {58, 60,65};
    inline juce::Colour mainPanelBottom {36, 39,43};


    inline juce::Colour accentActive { 0xFF40C4FF };
    inline juce::Colour transparent { juce::Colours::transparentBlack };
    inline juce::Colour clipWarning { juce::Colours::red.withAlpha(0.9f) };
    inline juce::Colour levelOk { juce::Colours::green.withAlpha(0.75f) };
    inline juce::Colour sliderThumb { juce::Colours::red.withAlpha(0.8f) };
    inline juce::Colour sliderTrack { juce::Colours::grey.withAlpha(0.3f) };
}

namespace GTFonts
{
    
}