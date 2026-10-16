#pragma once

#include "../../GTLook.h"
#include <functional>

class InfoPanel : public juce::Component
{
public:
    InfoPanel()
    {
        jassert(apLogo.isValid() && gtLogo.isValid());
        setInterceptsMouseClicks (true, true);
        setAlwaysOnTop(true);

        if (const auto closeIcon = juce::Drawable::createFromImageData(BinaryData::closeXIcon_svg, BinaryData::closeXIcon_svgSize))
        {
            addAndMakeVisible (closeButton);
            const auto iconNormal = closeIcon->createCopy();
            const auto iconActive = closeIcon->createCopy();
            const auto iconOver = closeIcon->createCopy();
            iconOver->replaceColour (GTColors::iconSVGColor, GTColors::iconOverColor);
            iconActive->replaceColour (GTColors::iconSVGColor, GTColors::activeNeonBlue);
            closeButton.setImages(iconNormal.get(), iconOver.get(),iconOver.get(), nullptr, iconActive.get());
        }
        closeButton.onClick = [this] { setVisible (false); };
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();

        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImage(back, boundsF, juce::RectanglePlacement::stretchToFit);
        g.fillAll(juce::Colours::black.withAlpha(0.15f));

        g.setOpacity(0.85f);
        const auto logoAPSize = boundsFHeight * 0.4f;
        const auto apLogoBounds = boundsF.withSizeKeepingCentre(logoAPSize, logoAPSize).withY(boundsFHeight * 0.055f);
        g.drawImage(apLogo, apLogoBounds, juce::RectanglePlacement::fillDestination);

        const auto gtLogoHeight = boundsFHeight * 0.125f;
        const auto gtLogoWidth = gtLogoHeight * (static_cast<float>(gtLogo.getWidth())/ static_cast<float>(gtLogo.getHeight()));
        const auto gtLogoBounds = boundsF.withSizeKeepingCentre(gtLogoWidth, gtLogoHeight).withY(apLogoBounds.getBottom());
        g.drawImage(gtLogo, gtLogoBounds, juce::RectanglePlacement::fillDestination);

        const auto dividerWidth = boundsFWidth * 0.46f;
        const auto dividerThickness = boundsFHeight * 0.003f;
        const auto dividerBounds = boundsF.withSizeKeepingCentre(dividerWidth, dividerThickness).withY(gtLogoBounds.getBottom() + boundsFHeight * 0.075f);
        const auto dividerEdgeColour = GTColors::infoPanelDivider.withAlpha(0.0f);
        juce::ColourGradient dividerGradient (dividerEdgeColour, dividerBounds.getX(), 0.f, dividerEdgeColour, dividerBounds.getRight(), 0.f, false);
        dividerGradient.addColour(0.5, GTColors::infoPanelDivider);
        g.setGradientFill(dividerGradient);
        g.fillRect(dividerBounds);

        const auto versionTextHeight = boundsFHeight * 0.055f;
        const auto copyrightTextHeight = versionTextHeight * 0.8f;
        const auto versionBounds = gtLogoBounds.withSizeKeepingCentre(gtLogoWidth, versionTextHeight).withY(dividerBounds.getBottom() + boundsFHeight * 0.055f);
        const auto copyrightBounds = versionBounds.withHeight(copyrightTextHeight).withY(versionBounds.getBottom() + boundsFHeight * 0.035f);

        g.setFont(GTFonts::montserratMedium(versionTextHeight).withExtraKerningFactor(0.025f));
        g.setColour(GTColors::almostWhite);
        g.drawText(juce::String ("Version ") + JucePlugin_VersionString, versionBounds, juce::Justification::centred);
        g.setFont(GTFonts::montserratMedium(copyrightTextHeight).withExtraKerningFactor(0.025f));
        g.setColour(GTColors::infoPanelSecondaryText);
        g.drawText(juce::String(juce::CharPointer_UTF8("Copyright \xc2\xa9 APAudio 2026")), copyrightBounds, juce::Justification::centred);
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
    juce::Image apLogo {juce::ImageCache::getFromMemory(BinaryData::logoAP_png, BinaryData::logoAP_pngSize)};
    juce::Image gtLogo {juce::ImageCache::getFromMemory(BinaryData::GTLogoV2Shadow_png, BinaryData::GTLogoV2Shadow_pngSize)};
    juce::Image back {juce::ImageCache::getFromMemory(BinaryData::infoBlueBack_png, BinaryData::infoBlueBack_pngSize)};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPanel)
};