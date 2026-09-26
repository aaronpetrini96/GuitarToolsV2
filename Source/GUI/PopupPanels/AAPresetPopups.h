//
// Created by Aaron Petrini on 18/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../LAF/PresetPopupLAF.h"

class AASavePresetPopup : public juce::Component
{
public:
    AASavePresetPopup()
    {
        setAlwaysOnTop (true);
        setLookAndFeel (PresetPopupLAF::get());

        titleLabel.setJustificationType (juce::Justification::centred);
        titleLabel.setColour (juce::Label::textColourId, GTColors::sectionTitleText);
        titleLabel.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (titleLabel);

        nameEditor.setJustification (juce::Justification::centred);
        nameEditor.setInputRestrictions (24);
        nameEditor.onTextChange = [this]
        {
            saveButton.setEnabled (nameEditor.getText().trim().isNotEmpty());
            isConfirmingOverwrite = false;
            saveButton.setButtonText ("SAVE");
        };
        nameEditor.setColour (juce::TextEditor::backgroundColourId, GTColors::presetBox);
        nameEditor.setColour (juce::TextEditor::textColourId, GTColors::sectionTitleText);
        addAndMakeVisible (nameEditor);

        saveButton.onClick = [this]
        {
            const auto presetName = nameEditor.getText();

            if (! isConfirmingOverwrite && presetExists != nullptr && presetExists (presetName))
            {
                isConfirmingOverwrite = true;
                saveButton.setButtonText ("OVERWRITE");
                return;
            }

            if (onConfirm != nullptr)
                onConfirm (presetName);
        };
        cancelButton.onClick = [this] { setVisible (false); };

        for (auto* button : {&saveButton, &cancelButton})
            addAndMakeVisible (button);
    }

    ~AASavePresetPopup() override
    {
        setLookAndFeel (nullptr);
    }

    void prepare (const juce::String& presetName)
    {
        nameEditor.setText (presetName, juce::dontSendNotification);
        saveButton.setEnabled (presetName.trim().isNotEmpty());
        isConfirmingOverwrite = false;
        saveButton.setButtonText ("SAVE");
        nameEditor.grabKeyboardFocus();
        nameEditor.selectAll();
    }

    void paint (juce::Graphics& g) override
    {
        if (auto* presetPopupLAF = dynamic_cast<PresetPopupLAF*> (&getLookAndFeel()))
            presetPopupLAF->drawPopupBackground (g, getLocalBounds().toFloat());
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();
        const auto lineHeight = boundsFHeight * 0.2f;
        const auto fontHeight = lineHeight * PresetPopupLAF::get()->fontPOH;

        titleLabel.setFont (AAFonts::interMedium (fontHeight));
        titleLabel.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.1f).toNearestInt());

        nameEditor.applyFontToAllText (AAFonts::montserratMedium (fontHeight));
        nameEditor.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.4f).toNearestInt());

        const auto buttonsBounds = boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.7f);
        saveButton.setBounds (buttonsBounds.withWidth (buttonsBounds.getWidth() * 0.45f).toNearestInt());
        cancelButton.setBounds (saveButton.getBounds().toFloat().withRightX (buttonsBounds.getRight()).toNearestInt());
    }

    std::function<void (const juce::String& presetName)> onConfirm;
    std::function<bool (const juce::String& presetName)> presetExists;

private:
    juce::Label titleLabel {"titleLabel", "SAVE PRESET"};
    juce::TextEditor nameEditor;
    juce::TextButton saveButton {"SAVE"}, cancelButton {"CANCEL"};
    bool isConfirmingOverwrite {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AASavePresetPopup)
};

class AADeletePresetPopup : public juce::Component
{
public:
    AADeletePresetPopup()
    {
        setAlwaysOnTop (true);
        setLookAndFeel (PresetPopupLAF::get());

        titleLabel.setJustificationType (juce::Justification::centred);
        titleLabel.setColour (juce::Label::textColourId, GTColors::sectionTitleText);
        titleLabel.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (titleLabel);

        messageLabel.setJustificationType (juce::Justification::centred);
        messageLabel.setColour (juce::Label::textColourId, GTColors::almostWhite);
        messageLabel.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (messageLabel);

        deleteButton.onClick = [this] { if (onConfirm != nullptr) onConfirm(); };
        cancelButton.onClick = [this] { setVisible (false); };

        for (auto* button : {&deleteButton, &cancelButton})
            addAndMakeVisible (button);
    }

    ~AADeletePresetPopup() override
    {
        setLookAndFeel (nullptr);
    }

    void prepare (const juce::String& presetName)
    {
        messageLabel.setText ("DELETE \"" + presetName.toUpperCase() + "\"?", juce::dontSendNotification);
    }

    void paint (juce::Graphics& g) override
    {
        if (auto* presetPopupLAF = dynamic_cast<PresetPopupLAF*> (&getLookAndFeel()))
            presetPopupLAF->drawPopupBackground (g, getLocalBounds().toFloat());
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();
        const auto lineHeight = boundsFHeight * 0.2f;
        const auto fontHeight = lineHeight * PresetPopupLAF::get()->fontPOH;

        titleLabel.setFont (AAFonts::interMedium (fontHeight));
        titleLabel.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.1f).toNearestInt());

        messageLabel.setFont (AAFonts::interMedium (fontHeight));
        messageLabel.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.9f, lineHeight).withY (boundsFHeight * 0.4f).toNearestInt());

        const auto buttonsBounds = boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.7f);
        deleteButton.setBounds (buttonsBounds.withWidth (buttonsBounds.getWidth() * 0.45f).toNearestInt());
        cancelButton.setBounds (deleteButton.getBounds().toFloat().withRightX (buttonsBounds.getRight()).toNearestInt());
    }

    std::function<void()> onConfirm;

private:
    juce::Label titleLabel {"titleLabel", "DELETE PRESET"};
    juce::Label messageLabel {"messageLabel", ""};
    juce::TextButton deleteButton {"DELETE"}, cancelButton {"CANCEL"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AADeletePresetPopup)
};
