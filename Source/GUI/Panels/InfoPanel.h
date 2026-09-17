#pragma once

#include "../../Utils/GTLook.h"
#include <functional>

class InfoPanel : public juce::Component
{
public:
    InfoPanel()
    {
        jassert(apLogo.isValid() && gtLogo.isValid());
        setInterceptsMouseClicks (true, true);
        setAlwaysOnTop(true);


        for (auto* link : { &manualLink, &emailLink, &reportBugLink })
        {
            addAndMakeVisible (*link);
            link->setColour (juce::HyperlinkButton::textColourId, GTColors::almostWhite.withAlpha (0.9f));
        }

        manualLink.onClick = []
        {
            const auto manualFile = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("GuitarTools2_User_Manual.pdf");
            if (manualFile.replaceWithData (BinaryData::guitarTools2UserManual_pdf, BinaryData::guitarTools2UserManual_pdfSize))
                manualFile.startAsProcess();
        };
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
        const auto boundsFWidth = boundsF.getWidth();
        const auto cardCorners = boundsFHeight * 0.05f;
        const auto borderThickness = boundsFHeight * 0.006f;
        g.setGradientFill(juce::ColourGradient::vertical(GTColors::presetBox.darker(0.9f), 0.f, GTColors::presetBox.darker(0.8f), boundsFHeight));
        g.fillRect(boundsF);
        g.setColour (GTColors::dividerLinesEnd);
        g.drawRoundedRectangle (boundsF.reduced (borderThickness * 0.5f), cardCorners - borderThickness * 0.5f, borderThickness);

        const auto logoAPSize = boundsFWidth * 0.33f;
        apLogoBounds = boundsF.withSizeKeepingCentre(logoAPSize, logoAPSize).withY(boundsFHeight * 0.055f);
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImage(apLogo, apLogoBounds, juce::RectanglePlacement::fillDestination, false);
        const auto gtLogoHeight = boundsFHeight * 0.1f;
        const auto gtLogoWidth = gtLogoHeight * (static_cast<float>(gtLogo.getWidth())/ static_cast<float>(gtLogo.getHeight()));
        const auto gtLogoBounds = boundsF.withSizeKeepingCentre(gtLogoWidth, gtLogoHeight).withY(apLogoBounds.getBottom());
        g.drawImage(gtLogo, gtLogoBounds, juce::RectanglePlacement::fillDestination, false);

        const auto textHeight = boundsFHeight * 0.05f;
        g.setFont(GTFonts::ibmPlexMonoMedium(textHeight * 0.775f).withExtraKerningFactor(0.01f));
        g.setColour(GTColors::almostWhite.withAlpha(0.8f));
        g.drawFittedText("GuitarTools2 v1.01. Copyright © APAudio 2026.", gtLogoBounds.withSizeKeepingCentre(gtLogoWidth, textHeight).withBottomY(boundsFHeight - textHeight * 0.5f).toNearestInt(),juce::Justification::centredBottom, 1);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();

        const auto linkHeight = boundsFHeight * 0.08f;
        const auto linkBounds = boundsF.withSize(boundsFWidth, linkHeight).withCentre({boundsF.getCentreX(), boundsFHeight * 0.625f});

        for (auto* l : { &manualLink, &emailLink, &reportBugLink })
            l->setFont(GTFonts::interSemiBold (linkHeight * 0.9).withExtraKerningFactor(0.015f), false);

        manualLink.setBounds (linkBounds.toNearestInt());
        emailLink.setBounds (linkBounds.translated (0.0f, linkHeight).toNearestInt());
        reportBugLink.setBounds(linkBounds.translated (0.0f, linkHeight * 2.0f).toNearestInt());
        // portfolioLink.setBounds (linkBounds.translated (0.0f, linkHeight * 3.0f).toNearestInt());

    }

private:

    juce::Rectangle<float> apLogoBounds{};
    const float apLogoPOW {0.33f};
    juce::Image apLogo {juce::ImageCache::getFromMemory(BinaryData::logoAP_png, BinaryData::logoAP_pngSize)};
    juce::Image gtLogo {juce::ImageCache::getFromMemory(BinaryData::guitarTools2Logo_png, BinaryData::guitarTools2Logo_pngSize)};
    juce::Label gTVersionLabel {"gTVersionLabel", "GuitarTools v2.0 © 2026 AP AudioTools."};
    juce::HyperlinkButton manualLink {"GET THE MANUAL", {}};
    juce::HyperlinkButton emailLink {"CONTACT ME", juce::URL {"mailto:aaron.petrini96@gmail.com"}};
    juce::HyperlinkButton reportBugLink {"REPORT BUG", juce::URL {"mailto:aaron.petrini96@gmail.com?subject=BugReport_GuitarTools2&body=Please include DAW used and the action that caused the problem. Thanks!"}};
    // juce::HyperlinkButton portfolioLink {"CHECK MY PORTFOLIO", juce::URL {"https://sites.google.com/view/aaronpetriniwebportfolio"}};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPanel)
};