//
// Created by Aaron Petrini on 03/09/2026.
//

#pragma once
#include "../../Utils/AALook.h"

class PresetBrowserLAF : public juce::LookAndFeel_V4
{
public:
    PresetBrowserLAF()
    {
        setColour (juce::ScrollBar::thumbColourId, GTColors::iconOverColor);
    }

    static PresetBrowserLAF* get()
    {
        static PresetBrowserLAF instance;
        return &instance;
    }

    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox&) override
    {
        const juce::Rectangle<int> boxBounds (0, 0, width, height);
        const auto boundsF = boxBounds.toFloat();
        const auto cornerSize = boundsF.getWidth() * 0.025f;
        const auto outlineThickness = boundsF.getWidth() * 0.001f;

        // g.setGradientFill (juce::ColourGradient::vertical (GTColors::in, 0.0f, GTColors::gradientNavy, boundsF.getHeight()));
        g.setColour(GTColors::presetBox);
        g.fillRoundedRectangle (boundsF, cornerSize);
        // g.setColour (GTColors::sectionTitleText);
        // g.drawRoundedRectangle (boundsF.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);
    }

    void drawMenuListBackground (juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        const auto cornerSize = bounds.getWidth() * 0.02f;
        const auto outlineThickness = bounds.getWidth() * 0.001f;
        g.setGradientFill (juce::ColourGradient::vertical (GTColors::gradientNavyDark, bounds.getY(), GTColors::gradientNavy, bounds.getBottom()));
        g.fillRoundedRectangle (bounds, cornerSize);

        g.setColour (GTColors::sectionTitleText);
        g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);
    }

    void drawMenuListHeader (juce::Graphics& g, juce::Rectangle<float> bounds, const juce::String& text)
    {
        g.setColour (GTColors::sectionTitleText);
        g.setFont (AAFonts::ibmPlexSansCondensedSemiBold (bounds.getHeight() * 0.55f));
        g.drawFittedText (text.toUpperCase(), bounds.reduced (bounds.getWidth() * 0.05f, 0.0f).toNearestInt(), juce::Justification::centred, 1);
    }
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& , bool , bool ) override
    {
        if (button.getConnectedEdgeFlags() == 0)
            return;

        const auto boundsF = button.getLocalBounds().toFloat();
        const auto isSelected = button.getToggleState();
        const auto underlineThickness = boundsF.getHeight() * (isSelected ? 0.06f : 0.03f);
        g.setColour (isSelected ? GTColors::activeNeonBlue : GTColors::dividerLinesEnd);
        g.fillRect (boundsF.withTop (boundsF.getBottom() - underlineThickness));
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();

        if (button.getConnectedEdgeFlags() != 0)
        {
            const auto tabTextColor = button.getToggleState() ? GTColors::sectionTitleText
                                                              : (shouldDrawButtonAsHighlighted ? GTColors::almostWhite : GTColors::iconOverColor);
            g.setColour (tabTextColor.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.4f));
            g.setFont (AAFonts::ibmPlexSansCondensedSemiBold (boundsF.getHeight() * 0.55f));
            g.drawFittedText (button.getButtonText(), boundsF.toNearestInt(), juce::Justification::centred, 1);
            return;
        }

        const auto fontHeight = boundsF.getHeight() * 0.675f;
        const auto isSelected = button.getToggleState();

        const auto textColor = isSelected ? GTColors::activeNeonBlue : GTColors::sectionTitleText;
        g.setColour (shouldDrawButtonAsHighlighted ? textColor.withBrightness(2.f) : textColor);
        g.setFont (isSelected ? AAFonts::interSemiBold (fontHeight) : AAFonts::interMedium (fontHeight));
        g.drawFittedText (button.getButtonText(), boundsF.reduced (boundsF.getWidth() * 0.05f, boundsF.getHeight() * 0.04f).toNearestInt(), juce::Justification::centred, 1);
    }

    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds (box.getLocalBounds());
        label.setFont (AAFonts::interMedium (box.getHeight() * 0.675f));
        label.setJustificationType (juce::Justification::centred);
    }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserLAF)
};
