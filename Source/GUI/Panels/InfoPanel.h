#pragma once

#include "../../Utils/GTLook.h"
#include <functional>

class InfoPanel : public juce::Component
{
public:
    InfoPanel()
    {
        setInterceptsMouseClicks (true, true);
        setAlwaysOnTop(true);

        addAndMakeVisible (closeButton);
        UITools::setIconButtonImages (closeButton, BinaryData::closeXIcon_svg, BinaryData::closeXIcon_svgSize);

        closeButton.onClick = [this] { setVisible (false); };
    }

    std::function<void()> onHidden;

    void visibilityChanged() override
    {
        if (! isVisible() && onHidden != nullptr)
            onHidden();
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto cardCorners = boundsFHeight * 0.05f;
        const auto borderThickness = boundsFHeight * 0.005f;

        g.setGradientFill(juce::ColourGradient::vertical(GTColors::presetBox.darker(0.9f), 0.f, GTColors::presetBox.darker(0.6f), boundsFHeight));
        g.fillRect(boundsF);
        g.setColour (GTColors::dividerLinesEnd.withAlpha(0.9f));
        g.drawRoundedRectangle (boundsF.reduced (borderThickness * 0.5f), cardCorners - borderThickness * 0.5f, borderThickness);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto closeButtonSize = boundsF.getHeight() * 0.06f;
        closeButton.setBounds (boundsF.withSize (closeButtonSize, closeButtonSize).withCentre({boundsF.getWidth() - closeButtonSize * 0.775f, closeButtonSize * 0.775f}).toNearestInt());
    }

private:
    juce::DrawableButton closeButton {"Close", juce::DrawableButton::ImageFitted};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPanel)
};