//
// Created by Aaron Petrini on 02/09/2026.
//

#pragma once

#include "../../Utils/GTLook.h"

#include <functional>
#include <memory>

class AANeveSquareButtons : public juce::Component
{
public:
    explicit AANeveSquareButtons (int numberOfButtons)
    {
        for (auto index = 0; index < numberOfButtons; ++index)
        {
            auto* button = buttons.add (new juce::DrawableButton ({}, juce::DrawableButton::ImageFitted));

            button->setImages (buttonOff.get(), buttonOff.get(), buttonOn.get(), nullptr,
                               buttonOn.get(), buttonOn.get(), buttonOn.get());
            button->setClickingTogglesState (true);
            button->setEdgeIndent (0);
            button->setColour (juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
            button->setColour (juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);
            button->onClick = [this, index]
            {
                setSelectedIndex (index);

                if (onButtonClicked != nullptr)
                    onButtonClicked (index);
            };

            addAndMakeVisible (button);
        }

        setSelectedIndex (0);
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto outerCorner = boundsFHeight * 0.13f;
        const auto wellBounds = boundsF.reduced (boundsFHeight * 0.075f, boundsFHeight * 0.11f);
        const auto wellCorner = boundsFHeight * 0.085f;

        g.setGradientFill (juce::ColourGradient::vertical (
            juce::Colour (4, 5, 6), wellBounds.getY(),
            juce::Colour (31, 33, 36), wellBounds.getBottom()));
        g.fillRoundedRectangle (wellBounds, wellCorner);

        const auto wellStroke = boundsFHeight * 0.018f;
        g.setColour (juce::Colour (2, 2, 3));
        g.drawRoundedRectangle (wellBounds.reduced (wellStroke * 0.5f), wellCorner, wellStroke);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();
        const auto buttonsArea = boundsF.withSizeKeepingCentre (boundsFWidth * 0.93f, boundsFHeight * 0.82f);
        const auto cellWidth = buttonsArea.getWidth() / (float) buttons.size();
        const auto buttonSize = juce::jmin (cellWidth, buttonsArea.getHeight());

        for (auto index = 0; index < buttons.size(); ++index)
        {
            const auto cellBounds = buttonsArea.withWidth (cellWidth)
                                                .withX (buttonsArea.getX() + cellWidth * (float) index);
            buttons[index]->setBounds (cellBounds.withSizeKeepingCentre (buttonSize, buttonSize).toNearestIntEdges());
        }
    }

    void setSelectedIndex (int selectedIndex)
    {
        for (auto index = 0; index < buttons.size(); ++index)
            buttons[index]->setToggleState (index == selectedIndex, juce::dontSendNotification);
    }

    std::function<void (int)> onButtonClicked;

private:
    std::unique_ptr<juce::Drawable> buttonOff = juce::Drawable::createFromImageData (
        BinaryData::AANeveSquareButton_OFF_png, BinaryData::AANeveSquareButton_OFF_pngSize);
    std::unique_ptr<juce::Drawable> buttonOn = juce::Drawable::createFromImageData (
        BinaryData::AANeveSquareButton_ON_png, BinaryData::AANeveSquareButton_ON_pngSize);
    juce::OwnedArray<juce::DrawableButton> buttons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AANeveSquareButtons)
};
