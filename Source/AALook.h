//
// Created by Aaron Petrini on 20/08/2026.
//
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "BinaryData.h"

namespace  GTColors //project specific
{
    inline const juce::Colour transparent { juce::Colours::transparentBlack };
    inline const juce::Colour superDarkBackground {15, 15, 15};
    inline const juce::Colour almostWhite {190, 190, 190};
    inline const juce::Colour sectionTitleText {215, 215, 215};
    inline const juce::Colour activeNeonBlue { 64, 196, 255 };
    inline const juce::Colour dividerLinesEnd{12, 12, 12};
    inline const juce::Colour dividerLinesStart {5, 5, 5};
    inline const juce::Colour headerPanelStart {12, 12, 16};
    inline const juce::Colour headerPanelEnd {22, 28, 45};
    inline const juce::Colour popupDarkBackground {30, 34, 44};
    inline const juce::Colour iconSVGColor {100, 100, 100};
    inline const juce::Colour iconNormalColor {200, 200, 200};
    inline const juce::Colour iconOverColor {120, 120, 120};
    inline const juce::Colour presetBox {35, 42, 60};
    inline const juce::Colour presetPopupHighlight {30, 50, 80};
    inline const juce::Colour tabButtonSelected {34, 56, 90};
    inline const juce::Colour tabButtonUnselected {21, 24, 40};
    inline const juce::Colour infoPanelSecondaryText {140, 152, 185};
    inline const juce::Colour infoPanelDivider {juce::Colour (190, 200, 225).withAlpha (0.27f)};
    inline const juce::Colour meterGreen {60, 200, 80};
    inline const juce::Colour meterYellow {235, 200, 40};
    inline const juce::Colour meterRed {235, 50, 40};
}

namespace AAUITools
{
    constexpr float outlineThicknessPOH {0.1f};
    constexpr float cornerPOH {0.0f};
    constexpr float fontHeightPOH {0.625f};

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

    inline void drawShadowAround (juce::Graphics& g, const juce::Rectangle<float> bounds, const float opacity = 0.85f, const float blurPOH = 0.03f, const float offsetYPOH = 0.01f)
    {
        const auto shadowBounds = bounds.toNearestInt();
        const auto shadow = juce::DropShadow (juce::Colours::black.withAlpha (opacity), juce::roundToInt (bounds.getHeight() * blurPOH), {0, juce::roundToInt (bounds.getHeight() * offsetYPOH)});

        const juce::Graphics::ScopedSaveState saveState (g);
        g.excludeClipRegion (shadowBounds);
        shadow.drawForRectangle (g, shadowBounds);
    }

    class PrintedText : public juce::Component
    {
    public:
        PrintedText (const juce::String& textToUse, const juce::Colour inkColorToUse, const juce::Justification justificationToUse = juce::Justification::centred)
            : text (textToUse), inkColor (inkColorToUse), justification (justificationToUse)
        {
            jassert (grainTile.isValid());
            setInterceptsMouseClicks (false, false);
        }

        void setText (const juce::String& newText)
        {
            if (text == newText)
                return;

            text = newText;
            textPathIsStale = true;
            repaint();
        }

        void setFont (const juce::Font& newFont)
        {
            if (font == newFont)
                return;

            font = newFont;
            textPathIsStale = true;
            repaint();
        }

        void setInkColor (const juce::Colour newInkColor)
        {
            if (inkColor == newInkColor)
                return;

            inkColor = newInkColor;
            repaint();
        }

        void setSideLinesVisible (bool shouldShowSideLines)
        {
            if (sideLinesVisible == shouldShowSideLines)
                return;

            sideLinesVisible = shouldShowSideLines;
            textPathIsStale = true;
            repaint();
        }

        void resized() override
        {
            textPathIsStale = true;
        }

        void paint (juce::Graphics& g) override
        {
            if (textPathIsStale)
                rebuildTextPath();

            g.setColour (inkColor);
            g.fillPath (textPath);

            const auto originInTopLevel = getTopLevelComponent()->getLocalPoint (this, juce::Point<float>());
            g.setFillType (juce::FillType (grainTile, juce::AffineTransform::translation (-originInTopLevel)));
            g.fillPath (textPath);
        }

    private:
        void rebuildTextPath()
        {
            juce::GlyphArrangement glyphs;
            glyphs.addFittedText (font, text, 0.f, 0.f, static_cast<float> (getWidth()), static_cast<float> (getHeight()), justification, 1);

            textPath.clear();
            glyphs.createPath (textPath);

            if (sideLinesVisible && ! textPath.isEmpty())
            {
                const auto textBounds = textPath.getBounds();
                const auto lineThickness = font.getHeight() * 0.07f;
                const auto lineGap = font.getHeight() * 0.5f;
                const auto lineBounds = textBounds.withSizeKeepingCentre (textBounds.getWidth(), lineThickness);
                textPath.addRectangle (lineBounds.withLeft (0.f).withRight (textBounds.getX() - lineGap));
                textPath.addRectangle (lineBounds.withLeft (textBounds.getRight() + lineGap).withRight (static_cast<float> (getWidth())));
            }

            textPathIsStale = false;
        }

        juce::Image grainTile = juce::ImageCache::getFromMemory (BinaryData::inkGrainTile_png, BinaryData::inkGrainTile_pngSize);
        juce::String text;
        juce::Colour inkColor;
        const juce::Justification justification;
        juce::Font font {juce::FontOptions {}};
        juce::Path textPath;
        bool textPathIsStale {true};
        bool sideLinesVisible {false};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PrintedText)
    };
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

    inline juce::Font montserratSemiBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::MontserratSemiBold_ttf, BinaryData::MontserratSemiBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

    inline juce::Font montserratBold (const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::MontserratBold_ttf, BinaryData::MontserratBold_ttfSize);
        return {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))};
    }

}
