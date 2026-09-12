//
// Created by Aaron Petrini on 07/09/2026.
//

#pragma once

#include "../../Utils/GTLook.h"

class MatchButtonLAF : public juce::LookAndFeel_V4
{
public:
    MatchButtonLAF()
    {
        setColour (juce::TextButton::textColourOffId, GTColors::almostWhite);
        setColour (juce::TextButton::textColourOnId, GTColors::activeNeonBlue);
        setColour (juce::ComboBox::outlineColourId, GTColors::transparent);
    }

    bool verticalText {true};
    float characterSpacingRatio {0.8f};

    void drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour&, bool, bool) override
    {
        if (b.isMouseOver())
        {
            const auto boundsF = b.getLocalBounds().toFloat();
            g.setColour(GTColors::darkGrey.withAlpha(0.4f));
            g.fillRoundedRectangle(boundsF.reduced(boundsF.getWidth() * 0.3f, boundsF.getHeight() * 0.055f), boundsF.getHeight() * 0.1f);
        }
    }

    juce::Font getTextButtonFont (juce::TextButton& button, int buttonHeight) override
    {
        if (! verticalText)
            return GTFonts::ibmPlexSansCondensedSemiBold ((float) buttonHeight);

        const auto characterHeight = (float) buttonHeight / (float) juce::jmax (1, button.getButtonText().length());
        return GTFonts::ibmPlexSansCondensedSemiBold (characterHeight * 0.85f).withExtraKerningFactor (0.0f);
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto buttonText = button.getButtonText();

        g.setFont (getTextButtonFont (button, button.getHeight()));
        const auto textColour = button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                                           : juce::TextButton::textColourOffId)
                                      .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

        g.setColour (shouldDrawButtonAsHighlighted ? textColour.brighter (0.4f) : textColour);

        if (! verticalText)
        {
            g.drawText (buttonText, boundsF, juce::Justification::centred, false);
            return;
        }

        const auto numCharacters = juce::jmax (1, buttonText.length());
        const auto characterHeight = boundsF.getHeight() / (float) numCharacters;
        const auto characterPitch = characterHeight * characterSpacingRatio;
        const auto stackTop = boundsF.getCentreY() - characterPitch * (float) numCharacters * 0.5f;

        for (int characterIndex = 0; characterIndex < numCharacters; ++characterIndex)
            g.drawText (buttonText.substring (characterIndex, characterIndex + 1),
                        boundsF.withHeight (characterPitch).withY (stackTop + characterPitch * (float) characterIndex),
                        juce::Justification::centred, false);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MatchButtonLAF)
};
