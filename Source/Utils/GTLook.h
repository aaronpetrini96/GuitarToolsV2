//
// Created by Aaron Petrini on 20/08/2026.
//
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "BinaryData.h"

namespace  GTColors
{
    inline juce::Colour transparent { juce::Colours::transparentBlack };

    inline juce::Colour controlBackground { 50, 50, 55 };
    inline juce::Colour superDarkBackground {15, 15, 15};
    inline juce::Colour almostWhite {190, 190, 190};
    inline juce::Colour sectionTitleText {215, 215, 215};
    inline juce::Colour darkGrey {36, 36, 36};

    inline juce::Colour dividerLinesEnd{12, 12, 12};
    inline juce::Colour dividerLinesStart {5, 5, 5};
    inline juce::Colour mainPanelTop {58, 60,65};
    inline juce::Colour mainPanelBottom {42, 45,49};
    inline juce::Colour activeNeonBlue { 64, 196, 255 };

    inline juce::Colour iconSVGColor {100, 100, 100};
    inline juce::Colour iconNormalColor {200, 200, 200};
    inline juce::Colour iconDownColor { 64, 196, 255 };
    inline juce::Colour iconOverColor {120, 120, 120};

    inline juce::Colour presetBox {40, 47, 65};
    inline juce::Colour presetPopupHighlight {42, 64, 96};
}

namespace UITools
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

    inline void drawDropShadow (juce::Graphics& g, juce::Rectangle<float> bounds, const float blurRadius,
                                const float opacity, const float cornerSize = 0.0f, juce::Point<int> offset = {})
    {
        if (bounds.isEmpty() || blurRadius <= 0.0f || opacity <= 0.0f)
            return;

        const juce::Graphics::ScopedSaveState savedState (g);

        juce::Path shape;

        if (cornerSize > 0.0f)
            shape.addRoundedRectangle (bounds, cornerSize);
        else
            shape.addRectangle (bounds);

        juce::DropShadow (juce::Colours::black.withAlpha (opacity), juce::jmax (1, juce::roundToInt (blurRadius)), offset).drawForPath (g, shape);
    }
}

namespace GTFonts
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

    inline juce::Font ibmPlexMonoRegular (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::IBMPlexMonoRegular_ttf, BinaryData::IBMPlexMonoRegular_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font ibmPlexMonoSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::IBMPlexMonoSemiBold_ttf, BinaryData::IBMPlexMonoSemiBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font ibmPlexMonoBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::IBMPlexMonoBold_ttf, BinaryData::IBMPlexMonoBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font chivoRegular (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::ChivoRegular_ttf, BinaryData::ChivoRegular_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font chivoBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::ChivoBold_ttf, BinaryData::ChivoBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font futuraBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::FuturaBold_otf, BinaryData::FuturaBold_otfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font interRegular (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::Inter_18ptRegular_ttf, BinaryData::Inter_18ptRegular_ttfSize);
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

    inline juce::Font jetBrainsMonoMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::JetBrainsMonoMedium_ttf, BinaryData::JetBrainsMonoMedium_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font robotoSerifThin (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifThin_ttf, BinaryData::RobotoSerifThin_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font robotoSerifExtraLight (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifExtraLight_ttf, BinaryData::RobotoSerifExtraLight_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font robotoSerifLight (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifLight_ttf, BinaryData::RobotoSerifLight_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font robotoSerifRegular (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifRegular_ttf, BinaryData::RobotoSerifRegular_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font robotoSerifMedium (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifMedium_ttf, BinaryData::RobotoSerifMedium_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font robotoSerifSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoSerifSemiBold_ttf, BinaryData::RobotoSerifSemiBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }
}
