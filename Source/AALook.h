//
// Created by Aaron Petrini on 20/08/2026.
//
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "BinaryData.h"

namespace  GTColors //project specific
{
    inline juce::Colour transparent { juce::Colours::transparentBlack };

    inline juce::Colour controlBackground { 50, 50, 55 };
    inline juce::Colour superDarkBackground {15, 15, 15};
    inline juce::Colour almostWhite {190, 190, 190};
    inline juce::Colour sectionTitleText {215, 215, 215};
    inline juce::Colour darkGrey {36, 36, 36};
    inline juce::Colour activeNeonBlue { 64, 196, 255 };

    inline juce::Colour dividerLinesEnd{12, 12, 12};
    inline juce::Colour dividerLinesStart {5, 5, 5};

    inline juce::Colour mainPanelTop {58, 60,65};
    inline juce::Colour mainPanelBottom {42, 45,49};

    inline juce::Colour headerPanelStart {12, 12, 12};
    inline juce::Colour headerPanelEnd {24, 27, 45};

    inline juce::Colour popupDarkBackground {20, 24, 34};

    inline juce::Colour fadersPlateGradientStart {juce::Colours::black.withAlpha (0.175f)};
    inline juce::Colour fadersPlateGradientEnd {juce::Colours::black.withAlpha (0.225f)};

    inline juce::Colour iconSVGColor {100, 100, 100};
    inline juce::Colour iconNormalColor {200, 200, 200};
    inline juce::Colour iconOverColor {120, 120, 120};

    inline juce::Colour presetBox {35, 42, 60};
    inline juce::Colour presetPopupHighlight {30, 50, 80};
    inline juce::Colour tabButtonBackground {17, 19, 31};
    inline juce::Colour presetBlue {45, 57, 92};
    inline juce::Colour outlinePopup {73, 83, 102};

    inline juce::Colour infoPanelTop {36, 45, 81};
    inline juce::Colour infoPanelBottom {26, 33, 60};
    inline juce::Colour infoPanelSecondaryText {140, 152, 185};
    inline juce::Colour infoPanelDivider {juce::Colour (190, 200, 225).withAlpha (0.27f)};
}

namespace AAUITools
{
    inline void setIconButtonImages (juce::DrawableButton& button, const void* iconData, const int iconDataSize)
    {
        if (const auto icon = juce::Drawable::createFromImageData (iconData, (size_t) iconDataSize))
        {
            const auto iconNormal = icon->createCopy();
            const auto iconOver = icon->createCopy();
            const auto iconActive = icon->createCopy();
            iconNormal->replaceColour (GTColors::iconSVGColor, GTColors::iconNormalColor);
            iconOver->replaceColour (GTColors::iconSVGColor, GTColors::iconOverColor);
            iconActive->replaceColour (GTColors::iconSVGColor, GTColors::activeNeonBlue);
            button.setImages (iconNormal.get(), iconOver.get(), iconActive.get(), nullptr, iconActive.get());
            button.setColour(juce::DrawableButton::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
            button.setColour(juce::DrawableButton::ColourIds::backgroundOnColourId, juce::Colours::transparentBlack);
        }
    }

    inline void drawRoundedDivider (juce::Graphics& g, const juce::Rectangle<float> dividerBounds)
    {
        const auto dividerThickness = juce::jmin (dividerBounds.getWidth(), dividerBounds.getHeight());
        const auto cornerSize = dividerThickness * 0.5f;
        const auto outlineThickness = dividerThickness * 0.125f;
        const auto outlineExpansion = outlineThickness * 0.5f;

        // g.setColour (GTColors::dividerLinesStart);
        g.setColour (juce::Colours::black);
        g.fillRoundedRectangle (dividerBounds, cornerSize);
        g.setColour (juce::Colours::black.brighter (0.1f));
        g.drawRoundedRectangle (dividerBounds.expanded (outlineExpansion), cornerSize + outlineExpansion, outlineThickness);
    }
}

namespace AAFonts
{
    inline juce::Font ibmPlexSansCondensedSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::IBMPlexSansCondensedSemiBold_ttf, BinaryData::IBMPlexSansCondensedSemiBold_ttfSize);
        return juce::Font {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))}.withExtraKerningFactor (0.08f);
    }

    inline juce::Font ibmPlexSansMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::IBMPlexSansMedium_ttf, BinaryData::IBMPlexSansMedium_ttfSize);
        return juce::Font {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))}.withExtraKerningFactor (0.04f);
    }

    inline juce::Font ibmPlexMonoMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::IBMPlexMonoMedium_ttf, BinaryData::IBMPlexMonoMedium_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font interMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptMedium_ttf, BinaryData::Inter_18ptMedium_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font interSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptSemiBold_ttf, BinaryData::Inter_18ptSemiBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font interBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptBold_ttf, BinaryData::Inter_18ptBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font montserratMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font futuraBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (BinaryData::Futura_Bold_otf, BinaryData::Futura_Bold_otfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

}
