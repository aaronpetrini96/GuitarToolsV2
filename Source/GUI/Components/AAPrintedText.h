#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../AALook.h"

class AAPrintedText : public juce::Component
{
public:
    AAPrintedText (const juce::String& textToUse, const juce::Colour inkColorToUse, const juce::Justification justificationToUse = juce::Justification::centred)
        : text (textToUse), inkColor (inkColorToUse), justification (justificationToUse)
    {
        jassert (grainTile.isValid());
        setInterceptsMouseClicks (false, false);
    }

    void setFont (const juce::Font& newFont)
    {
        if (font == newFont)
            return;

        font = newFont;
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
        textPathIsStale = false;
    }

    juce::Image grainTile = juce::ImageCache::getFromMemory (BinaryData::inkGrainTile_png, BinaryData::inkGrainTile_pngSize);
    const juce::String text;
    const juce::Colour inkColor;
    const juce::Justification justification;
    juce::Font font {juce::FontOptions {}};
    juce::Path textPath;
    bool textPathIsStale {true};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPrintedText)
};
