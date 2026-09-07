//
// Created by Aaron Petrini on 03/09/2026.
//

#pragma once

#include "../../Utils/GTLook.h"

class PresetBrowserLAF : public juce::LookAndFeel_V4
{
public:
    PresetBrowserLAF()
    {
        setColour (juce::ComboBox::backgroundColourId, juce::Colours::lightgrey.withAlpha (0.75f));
        setColour (juce::ComboBox::textColourId, juce::Colours::white);
        setColour (juce::ComboBox::outlineColourId, juce::Colour (138, 138, 138));
        setColour (juce::PopupMenu::backgroundColourId, GTColors::footerBackground);
        setColour (juce::PopupMenu::textColourId, GTColors::clearGrey);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, GTColors::controlBackground);
        setColour (juce::PopupMenu::highlightedTextColourId, GTColors::accentActive);
    }

    static PresetBrowserLAF* get()
    {
        static PresetBrowserLAF instance;
        return &instance;
    }

    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box) override
    {
        const auto cornerSize = 3.0f;
        const juce::Rectangle<int> boxBounds (0, 0, width, height);

        g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

        g.setColour (box.findColour (juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle (boxBounds.toFloat().reduced (1.f, 1.f), cornerSize, 2.f);

        const auto arrowWidth = (float) height * 0.2f;
        const auto arrowCentre = juce::Point<float> ((float) width - (float) height * 0.5f, (float) height * 0.5f);
        juce::Path arrow;
        arrow.startNewSubPath (arrowCentre.x - arrowWidth * 0.5f, arrowCentre.y - arrowWidth * 0.25f);
        arrow.lineTo (arrowCentre.x, arrowCentre.y + arrowWidth * 0.25f);
        arrow.lineTo (arrowCentre.x + arrowWidth * 0.5f, arrowCentre.y - arrowWidth * 0.25f);
        g.setColour (box.findColour (juce::ComboBox::arrowColourId));
        g.strokePath (arrow, juce::PathStrokeType ((float) height * 0.07f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto colour = backgroundColour;

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            colour = colour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = 3.0f;

        g.setColour (colour);
        g.fillRoundedRectangle (boundsF, cornerSize);

        g.setColour (button.findColour (juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle (boundsF.reduced (1.0f), cornerSize, 2.0f);
    }

    juce::Font getComboBoxFont (juce::ComboBox& box) override
    {
        return GTFonts::ibmPlexSansMedium ((float) box.getHeight() * 0.6f);
    }

    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override
    {
        return GTFonts::ibmPlexSansMedium ((float) buttonHeight * 0.55f);
    }

    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds (0, 0, box.getWidth() - box.getHeight(), box.getHeight());
        label.setFont (getComboBoxFont (box));
        label.setJustificationType (juce::Justification::centred);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserLAF)
};
