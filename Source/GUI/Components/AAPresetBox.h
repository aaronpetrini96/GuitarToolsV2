//
// Created by Aaron Petrini on 19/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "BinaryData.h"
#include "../../AALook.h"
#include "../../Utils/PresetManager.h"

class PresetBoxLAF : public juce::LookAndFeel_V4
{
public:
    PresetBoxLAF() = default;

    static PresetBoxLAF* get()
    {
        static PresetBoxLAF instance;
        return &instance;
    }

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& , bool , bool ) override
    {
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();

        g.setColour (GTColors::sectionTitleText);
        g.setFont (AAFonts::montserratMedium (boundsF.getHeight() * 0.65f));
        g.drawFittedText (button.getButtonText(), boundsF.toNearestInt(), juce::Justification::centred, 1);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBoxLAF)
};

class AAPresetBox : public juce::Component
{
public:
    explicit AAPresetBox (PresetManager& presetManagerToUse) : presetManager (presetManagerToUse)
    {
        nameButton.setLookAndFeel (PresetBoxLAF::get());
        nameButton.setColour(juce::TextButton::ColourIds::buttonColourId, GTColors::transparent);
        nameButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, GTColors::transparent);
        nameButton.onClick = [this] { if (onNameClicked != nullptr) onNameClicked(); };
        addAndMakeVisible (nameButton);

        AAUITools::setIconButtonImages (leftArrowButton, BinaryData::leftChevronArrow_svg, BinaryData::leftChevronArrow_svgSize);
        AAUITools::setIconButtonImages (rightArrowButton, BinaryData::rightChevronArrow_svg, BinaryData::rightChevronArrow_svgSize);
        leftArrowButton.onClick = [this] { presetManager.loadAdjacentPreset (-1); };
        rightArrowButton.onClick = [this] { presetManager.loadAdjacentPreset (1); };
        addAndMakeVisible (leftArrowButton);
        addAndMakeVisible (rightArrowButton);
    }

    ~AAPresetBox() override
    {
        nameButton.setLookAndFeel (nullptr);
    }

    void setPresetName (const juce::String& presetName)
    {
        nameButton.setButtonText (presetName);
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        g.setColour(GTColors::presetBox);
        g.fillRoundedRectangle(boundsF.withSizeKeepingCentre(boundsF.getWidth() * 0.7f, boundsF.getHeight()), boundsF.getHeight() * 0.225f);
    }

    void resized() override
    {
        const auto b = getLocalBounds().toFloat();
        const auto boundsF = b.withSizeKeepingCentre(b.getWidth(), b.getHeight() * 0.88f);
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto nameButtonBounds = boundsF.withSizeKeepingCentre (boundsFWidth * 0.575f, boundsFHeight);
        nameButton.setBounds (nameButtonBounds.toNearestInt());

        const auto arrowSize = boundsFHeight * 0.7f;
        const auto arrowBounds = nameButtonBounds.withSizeKeepingCentre (arrowSize, arrowSize);
        leftArrowButton.setBounds (arrowBounds.withRightX (nameButtonBounds.getX()).toNearestInt());
        rightArrowButton.setBounds (arrowBounds.withX (nameButtonBounds.getRight()).toNearestInt());
    }

    std::function<void()> onNameClicked;

private:
    PresetManager& presetManager;
    juce::TextButton nameButton;
    juce::DrawableButton leftArrowButton {"Previous preset", juce::DrawableButton::ImageFitted};
    juce::DrawableButton rightArrowButton {"Next preset", juce::DrawableButton::ImageFitted};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPresetBox)
};
