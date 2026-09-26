#pragma once

#include "../../AALook.h"
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

        g.fillAll(juce::Colours::black);
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImage(back, boundsF, juce::RectanglePlacement::stretchToFit);

        // g.setGradientFill(juce::ColourGradient::vertical(GTColors::infoPanelTop, 0.f, GTColors::infoPanelBottom, boundsFHeight));
        // g.fillRect(boundsF);
        // g.setColour (GTColors::presetBox);
        // g.drawRoundedRectangle (boundsF.reduced (borderThickness * 0.5f), cardCorners - borderThickness * 0.5f, borderThickness);

        const auto logoAPSize = boundsFHeight * 0.4f;
        const auto apLogoBounds = boundsF.withSizeKeepingCentre(logoAPSize, logoAPSize).withY(boundsFHeight * 0.055f);

        g.drawImage(apLogo, apLogoBounds, juce::RectanglePlacement::fillDestination, false);

        const auto gtLogoHeight = boundsFHeight * 0.125f;
        const auto gtLogoWidth = gtLogoHeight * (static_cast<float>(gtLogo.getWidth())/ static_cast<float>(gtLogo.getHeight()));
        const auto gtLogoBounds = boundsF.withSizeKeepingCentre(gtLogoWidth, gtLogoHeight).withY(apLogoBounds.getBottom());
        g.drawImage(gtLogo, gtLogoBounds, juce::RectanglePlacement::fillDestination, false);

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

        g.setFont(AAFonts::interMedium(versionTextHeight).withExtraKerningFactor(0.025f));
        g.setColour(GTColors::sectionTitleText);
        g.drawText(juce::String("Version 1.00"), versionBounds, juce::RectanglePlacement::centred);
        g.setFont(AAFonts::interMedium(copyrightTextHeight).withExtraKerningFactor(0.025f));
        g.setColour(GTColors::infoPanelSecondaryText);
        g.drawText(juce::String(juce::CharPointer_UTF8("Copyright \xc2\xa9 APAudio 2026")), copyrightBounds, juce::RectanglePlacement::centred);
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
    juce::Image gtLogo {juce::ImageCache::getFromMemory(BinaryData::guitarTools2Logo2_png, BinaryData::guitarTools2Logo2_pngSize)};
    juce::Image back {juce::ImageCache::getFromMemory(BinaryData::infoBlueBack_png, BinaryData::infoBlueBack_pngSize)};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPanel)
};