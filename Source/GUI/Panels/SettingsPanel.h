//
// Created by Aaron Petrini on 08/09/2026.
//

#pragma once

#include "../../Utils/GTLook.h"
#include <functional>

class SettingsPanel : public juce::Component
{
public:
    SettingsPanel()
    {
        setInterceptsMouseClicks (true, true);
        setAlwaysOnTop(true);

        // addAndMakeVisible (closeButton);
        // UITools::setIconButtonImages (closeButton, BinaryData::closeXIcon_svg, BinaryData::closeXIcon_svgSize);
        //
        // closeButton.onClick = [this] { setVisible (false); };

        titleLabel.setJustificationType (juce::Justification::centred);
        titleLabel.setColour (juce::Label::textColourId, GTColors::sectionTitleText);
        titleLabel.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (titleLabel);

        showTooltipsButton.setButtonText ("SHOW TOOLTIPS");
        addAndMakeVisible (showTooltipsButton);
        showTooltipsButton.onClick = [this]
        {
            if (onShowTooltipsChanged != nullptr)
                onShowTooltipsChanged (showTooltipsButton.getToggleState());
        };
    }

    std::function<void()> onHidden;
    std::function<void (bool)> onShowTooltipsChanged;

    void setShowTooltips (bool shouldShow)
    {
        showTooltipsButton.setToggleState (shouldShow, juce::dontSendNotification);
    }

    void visibilityChanged() override
    {
        if (! isVisible() && onHidden != nullptr)
            onHidden();
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();

        g.setGradientFill(juce::ColourGradient::vertical(GTColors::presetBox.darker(0.9f), 0.f, GTColors::presetBox.darker(0.8f), boundsFHeight));
        g.fillRect(boundsF);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();
        // const auto closeButtonSize = boundsFHeight * 0.9f;
        // closeButton.setBounds (boundsF.withSize (closeButtonSize, closeButtonSize).withRightX(boundsFWidth - closeButtonSize * 0.5f).withY(closeButtonSize * 0.5f).toNearestInt());
        const auto titleBounds = boundsF.withHeight (boundsFHeight * 0.12f);
        titleLabel.setFont (GTFonts::ibmPlexSansCondensedSemiBold (titleBounds.getHeight() * 0.55f).withExtraKerningFactor (0.16f));
        titleLabel.setBounds (titleBounds.toNearestInt());
        const auto settingsMargin = boundsFWidth * 0.08f;
        showTooltipsButton.setBounds (boundsF.withSize (boundsFWidth - settingsMargin * 2.f, boundsFHeight * 0.07f).withPosition (settingsMargin, boundsFHeight * 0.15f).toNearestInt());
    }

private:
    // juce::DrawableButton closeButton {"Close", juce::DrawableButton::ImageFitted};

    juce::Label titleLabel {{}, "SETTINGS"};
    juce::ToggleButton showTooltipsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsPanel)
};
