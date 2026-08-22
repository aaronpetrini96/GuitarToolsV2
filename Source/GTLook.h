//
// Created by Aaron Petrini on 20/08/2026.
//
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

namespace  GTColors
{
    inline juce::Colour transparent { juce::Colours::transparentBlack };
    inline juce::Colour controlBackground { juce::Colour(100, 100, 110).darker(0.5f) };
    inline juce::Colour footerBackground {juce::Colour(15, 15, 15).withAlpha(0.8f)};

    inline juce::Colour sectionTitleText {122, 122, 122};

    inline juce::Colour dividerLines {19, 19, 19};
    inline juce::Colour mainPanelTop {58, 60,65};
    inline juce::Colour mainPanelBottom {36, 39,43};


    inline juce::Colour accentActive { juce::Colour(64, 196, 255) };

    inline juce::Colour clipWarning { juce::Colours::red.withAlpha(0.9f) };
    inline juce::Colour levelOk { juce::Colours::green.withAlpha(0.75f) };
    inline juce::Colour sliderThumb { juce::Colours::red.withAlpha(0.8f) };
    inline juce::Colour sliderTrack { juce::Colours::grey.withAlpha(0.3f) };
}

namespace GTFonts
{
    inline juce::Font chivoRegular (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::ChivoRegular_ttf, BinaryData::ChivoRegular_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font chivoBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::ChivoBold_ttf, BinaryData::ChivoBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font jetBrainsMonoMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::JetBrainsMonoMedium_ttf, BinaryData::JetBrainsMonoMedium_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }
}