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
    inline juce::Colour footerBackground {juce::Colour(15, 15, 15).withAlpha(0.9f)};

    inline juce::Colour sectionTitleText {215, 215, 215};
    inline juce::Colour buttonsNameText {200, 200, 200};
    inline juce::Colour clearGrey {180, 180, 180};
    inline juce::Colour darkGrey {90, 90, 90};

    // inline juce::Colour dividerLinesStart{12, 12, 12};
    inline juce::Colour dividerLinesEnd{12, 12, 12};
    // inline juce::Colour dividerLinesEnd {5, 5, 5};
    inline juce::Colour dividerLinesStart {5, 5, 5};
    inline juce::Colour mainPanelTop {58, 60,65};
    // inline juce::Colour mainPanelBottom {36, 39,43};
    inline juce::Colour mainPanelBottom {42, 45,49};


    // inline juce::Colour accentActive { juce::Colour(64, 196, 255) };
    inline juce::Colour accentActive { juce::Colour(95, 200, 255) };

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

    inline juce::Font futuraBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::FuturaBold_otf, BinaryData::FuturaBold_otfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font interRegular (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptRegular_ttf, BinaryData::Inter_18ptRegular_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font interMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptMedium_ttf, BinaryData::Inter_18ptMedium_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font interSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptSemiBold_ttf, BinaryData::Inter_18ptSemiBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font interBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptBold_ttf, BinaryData::Inter_18ptBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font jetBrainsMonoMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::JetBrainsMonoMedium_ttf, BinaryData::JetBrainsMonoMedium_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font robotoSerifThin (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifThin_ttf, BinaryData::RobotoSerifThin_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font robotoSerifExtraLight (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifExtraLight_ttf, BinaryData::RobotoSerifExtraLight_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font robotoSerifLight (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifLight_ttf, BinaryData::RobotoSerifLight_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font robotoSerifRegular (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifRegular_ttf, BinaryData::RobotoSerifRegular_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font robotoSerifMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifMedium_ttf, BinaryData::RobotoSerifMedium_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }

    inline juce::Font robotoSerifSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifSemiBold_ttf, BinaryData::RobotoSerifSemiBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (fontHeight)};
    }
}
