#pragma once

#include "../../GTLook.h"
#include <functional>

class InfoPopupPanel : public juce::Component
{
public:
    InfoPopupPanel()
    {
        jassert(infoPanelImage.isValid());
        setInterceptsMouseClicks (true, true);
        setAlwaysOnTop(true);

        addAndMakeVisible (closeButton);
        AAUITools::setIconButtonImages (closeButton, BinaryData::closeXIcon_svg, BinaryData::closeXIcon_svgSize, GTColors::iconSVGColor, GTColors::iconNormalColor, GTColors::iconOverColor, GTColors::activeNeonBlue);
        closeButton.onClick = [this] { setVisible (false); };
    }

    void paint (juce::Graphics& g) override
    {
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImage(infoPanelImage, getLocalBounds().toFloat(), juce::RectanglePlacement::fillDestination);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();
        const auto closeButtonSize = boundsFHeight * 0.0625f;
        closeButton.setBounds (boundsF.withSize (closeButtonSize, closeButtonSize).withRightX(boundsFWidth - closeButtonSize * 0.5f).withY(closeButtonSize * 0.5f).toNearestInt());
    }

private:
    juce::DrawableButton closeButton {"closeButton", juce::DrawableButton::ImageFitted};
    juce::Image infoPanelImage {juce::ImageCache::getFromMemory(BinaryData::infoPanel_png, BinaryData::infoPanel_pngSize)};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPopupPanel)
};