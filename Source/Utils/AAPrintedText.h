#pragma once

#include "../AALook.h"

class AAPrintedText : public juce::Component
{
public:
    AAPrintedText (const juce::String& textToUse, const juce::Colour inkColorToUse, const juce::Justification justificationToUse = juce::Justification::centred)
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
    const juce::Colour inkColor;
    const juce::Justification justification;
    juce::Font font {juce::FontOptions {}};
    juce::Path textPath;
    bool textPathIsStale {true};
    bool sideLinesVisible {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPrintedText)
};
