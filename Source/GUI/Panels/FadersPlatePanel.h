//
// Created by Aaron Petrini on 05/09/2026.
//

#pragma once

#include "../../Utils/AALook.h"
#include "../../Utils/AAParamUtils.h"
#include "PluginProcessor.h"

#include "GUI/Components/AAFaderSSL.h"
#include "GUI/Components/AAImgSwitch.h"

class FadersPlatePanel : public juce::Component
{
public:
    explicit FadersPlatePanel (GuitarToolsAudioProcessor& processor)
        : resoFreqFader (*processor.treeState.getParameter (ParamIDs::getResoFreqName()), "Hz"),
          mudFreqFader (*processor.treeState.getParameter (ParamIDs::getMudFreqName()), "Hz"),
          compAmountFader (*processor.treeState.getParameter (ParamIDs::getCompAmountName()), "%"),
          resoModeSwitch (*processor.treeState.getParameter (ParamIDs::getResoExtremeActiveName())),
          compModeSwitch (*processor.treeState.getParameter (ParamIDs::getCompExtremeActiveName())),
          mudModeSwitch (*processor.treeState.getParameter (ParamIDs::getMudExtremeActiveName()))
    {
        for (auto* fader : {&resoFreqFader, &mudFreqFader, &compAmountFader})
        {
            addAndMakeVisible (*fader);
            fader->setTooltip (ParamHelpers::getTooltipForParam (fader->getComponentID()));
            fader->setUndoManager (&processor.undoManager);
        }

        for (auto* label : {&resoModeLabel, &mudModeLabel, &compModeLabel})
        {
            label->setJustificationType (juce::Justification::centredTop);
            label->setColour (juce::Label::textColourId, GTColors::almostWhite.withAlpha (0.9f));
            label->setInterceptsMouseClicks (false, false);
            label->setMinimumHorizontalScale (1.0f);
            addAndMakeVisible (label);
        }

        for (auto* modeSwitch : {&resoModeSwitch, &mudModeSwitch, &compModeSwitch})
        {
            addAndMakeVisible (*modeSwitch);
            modeSwitch->setTooltip (ParamHelpers::getTooltipForParam (modeSwitch->getComponentID()));
            modeSwitch->setUndoManager (processor.undoManager);
        }

    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto plateHeight= boundsF.getHeight();
        const auto plateArea = boundsF.withSizeKeepingCentre(boundsF.proportionOfWidth(0.9f), plateHeight);
        const auto plateCorners      = plateHeight * 0.07f;
        const auto borderThickness  = plateHeight * 0.01f;

        g.setGradientFill(juce::ColourGradient::vertical(GTColors::fadersPlateGradientStart, GTColors::fadersPlateGradientEnd, plateArea));
        g.fillRoundedRectangle (plateArea, plateCorners);
        g.setColour(GTColors::dividerLinesEnd);
        g.drawRoundedRectangle(plateArea.reduced(borderThickness * 0.5f), plateCorners - borderThickness * 0.5f, borderThickness);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();
        const auto faderWidth = boundsFWidth * 0.3f;
        const auto edgeOffset = boundsFHeight * 0.03f;

        const auto switchLabelHeight = boundsFHeight * 0.042f;
        const auto switchLabelBounds = boundsF.withSizeKeepingCentre (faderWidth, switchLabelHeight).withBottomY(boundsFHeight - edgeOffset);
        for (auto* label : {&resoModeLabel, &mudModeLabel, &compModeLabel})
            label->setFont (AAFonts::ibmPlexSansMedium (switchLabelHeight * 0.975f));

        mudModeLabel.setBounds (switchLabelBounds.toNearestInt());
        resoModeLabel.setBounds (switchLabelBounds.translated (-faderSpacing, 0.0f).toNearestInt());
        compModeLabel.setBounds (switchLabelBounds.translated (faderSpacing, 0.0f).toNearestInt());

        const auto switchHeight = boundsFHeight * 0.092f;
        const auto switchBounds = boundsF.withSizeKeepingCentre(switchHeight * 2.f, switchHeight).withBottomY(switchLabelBounds.getY());
        mudModeSwitch.setBounds (switchBounds.toNearestInt());
        resoModeSwitch.setBounds (switchBounds.translated (-faderSpacing, 0.0f).toNearestInt());
        compModeSwitch.setBounds (switchBounds.translated (faderSpacing, 0.0f).toNearestInt());

        const auto faderBounds = boundsF.withSizeKeepingCentre (faderWidth, (boundsFHeight - switchHeight - switchLabelHeight) * 0.92f).withY(boundsF.getY() + edgeOffset);
        mudFreqFader.setBounds (faderBounds.toNearestInt());
        resoFreqFader.setBounds (faderBounds.translated(-faderSpacing, 0.f).toNearestInt());
        compAmountFader.setBounds (faderBounds.translated(faderSpacing, 0.f).toNearestInt());
    }

    void setFaderSpacing (const float newFaderSpacing)
    {
        if (juce::approximatelyEqual(faderSpacing, newFaderSpacing))
            return;
        faderSpacing = newFaderSpacing;
        resized();
    }

    AAVerticalSSLFader resoFreqFader, mudFreqFader, compAmountFader;
    AAImgSwitch resoModeSwitch, compModeSwitch, mudModeSwitch;
private:
    float faderSpacing {0.f};
    juce::Label resoModeLabel {"resoModeLabel", "MODE"};
    juce::Label mudModeLabel {"mudModeLabel", "MODE"};
    juce::Label compModeLabel {"compModeLabel", "MODE"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FadersPlatePanel)
};
