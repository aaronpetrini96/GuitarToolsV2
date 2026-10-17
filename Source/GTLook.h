//
// Created by Aaron Petrini on 20/08/2026.
//
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "BinaryData.h"
#include "Utils/AAUITools.h"

constexpr float GTOutlineThicknessPOH {0.05f};
constexpr float GTCornerPOH {0.025f};
constexpr float GTFontHeightPOH {0.62f};
constexpr float GTPopupTitleRowRatio {1.05f};

namespace  GTColors //project specific
{
    inline const juce::Colour transparent { juce::Colours::transparentBlack };
    inline const juce::Colour superDarkBackground {15, 15, 15};
    inline const juce::Colour almostWhite {190, 190, 190};
    inline const juce::Colour sectionTitleText {215, 215, 215};
    inline const juce::Colour activeNeonBlue { 64, 196, 255 };
    inline const juce::Colour almostBlack {5, 5, 5};
    inline const juce::Colour popupBackground {5, 7, 14};
    // inline const juce::Colour blueGradientStart {9, 12, 25};
    // inline const juce::Colour blueGradientEnd {22, 28, 60};
    inline const juce::Colour popupDarkBackground {20, 28, 50};
    // inline const juce::Colour popupBlueHighlight {30, 50, 80};
    inline const juce::Colour iconSVGColor {100, 100, 100};
    inline const juce::Colour iconNormalColor {200, 200, 200};
    inline const juce::Colour iconOverColor {120, 120, 120};
    inline const juce::Colour presetBoxBackground {35, 42, 60};
    inline const juce::Colour presetBoxOutlineColor {70, 85, 115};
    inline const juce::Colour popupOutlineColor {40, 50, 70};

    inline const juce::Colour infoPanelSecondaryText {140, 152, 185};
    inline const juce::Colour infoPanelDivider {juce::Colour (190, 200, 225).withAlpha (0.27f)};
    inline const juce::Colour meterGreen {60, 200, 80};
    inline const juce::Colour meterYellow {235, 200, 40};
    inline const juce::Colour meterRed {235, 50, 40};
}

inline const juce::Colour GTPopupBackgroundColor {GTColors::popupBackground};
inline const juce::Colour GTPopupOutlineColor {GTColors::popupOutlineColor};
inline const juce::Colour GTPopupTextColor {GTColors::sectionTitleText};
inline const juce::Colour GTPopupSelectedTextColor {GTColors::activeNeonBlue};

namespace GTFonts
{
    inline juce::Font ibmPlexSansCondensedSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::IBMPlexSansCondensedSemiBold_ttf, BinaryData::IBMPlexSansCondensedSemiBold_ttfSize);
        return AAUITools::makeFont (typeface, fontHeight, 0.08f);
    }

    inline juce::Font ibmPlexSansMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::IBMPlexSansMedium_ttf, BinaryData::IBMPlexSansMedium_ttfSize);
        return AAUITools::makeFont (typeface, fontHeight, 0.04f);
    }

    inline juce::Font interMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptMedium_ttf, BinaryData::Inter_18ptMedium_ttfSize);
        return AAUITools::makeFont (typeface, fontHeight);
    }

    inline juce::Font interSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptSemiBold_ttf, BinaryData::Inter_18ptSemiBold_ttfSize);
        return AAUITools::makeFont (typeface, fontHeight);
    }

    inline juce::Font interBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptBold_ttf, BinaryData::Inter_18ptBold_ttfSize);
        return AAUITools::makeFont (typeface, fontHeight);
    }

    inline juce::Font montserratMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize);
        return AAUITools::makeFont (typeface, fontHeight);
    }

    inline juce::Font montserratSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::MontserratSemiBold_ttf, BinaryData::MontserratSemiBold_ttfSize);
        return AAUITools::makeFont (typeface, fontHeight);
    }

    inline juce::Font montserratBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::MontserratBold_ttf, BinaryData::MontserratBold_ttfSize);
        return AAUITools::makeFont (typeface, fontHeight);
    }

}
