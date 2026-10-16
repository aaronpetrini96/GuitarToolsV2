//
// Created by Aaron Petrini on 18/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../LAF/PresetPopupLAF.h"

class AAPresetPopup : public juce::Component
{
public:
    enum class Action { save, remove };

    explicit AAPresetPopup (Action popupAction)
        : action (popupAction)
    {
        setAlwaysOnTop (true);
        setLookAndFeel (&presetPopupLAF);

        titleLabel.setText (action == Action::save ? "SAVE PRESET" : "DELETE PRESET", juce::dontSendNotification);
        titleLabel.setJustificationType (juce::Justification::centred);
        titleLabel.setColour (juce::Label::textColourId, GTColors::sectionTitleText);
        titleLabel.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (titleLabel);

        if (action == Action::save)
        {
            nameEditor.setJustification (juce::Justification::centred);
            nameEditor.setInputRestrictions (24);
            nameEditor.onTextChange = [this]
            {
                confirmButton.setEnabled (juce::File::createLegalFileName (nameEditor.getText().trim()).isNotEmpty() && ! (isNameReserved != nullptr && isNameReserved (nameEditor.getText())));
                isConfirmingOverwrite = false;
                confirmButton.setButtonText ("SAVE");
            };
            addAndMakeVisible (nameEditor);
        }
        else
        {
            messageLabel.setJustificationType (juce::Justification::centred);
            messageLabel.setColour (juce::Label::textColourId, GTColors::almostWhite);
            messageLabel.setInterceptsMouseClicks (false, false);
            addAndMakeVisible (messageLabel);
        }

        confirmButton.setButtonText (action == Action::save ? "SAVE" : "DELETE");
        confirmButton.onClick = [this]
        {
            const auto presetName = action == Action::save ? nameEditor.getText() : preparedPresetName;

            if (! isConfirmingOverwrite && presetExists != nullptr && presetExists (presetName))
            {
                isConfirmingOverwrite = true;
                confirmButton.setButtonText ("OVERWRITE");
                return;
            }

            if (onConfirm != nullptr)
                onConfirm (presetName);
        };
        cancelButton.onClick = [this] { setVisible (false); };

        for (auto* button : {&confirmButton, &cancelButton})
            addAndMakeVisible (button);
    }

    ~AAPresetPopup() override
    {
        setLookAndFeel (nullptr);
    }

    void prepare (const juce::String& presetName)
    {
        preparedPresetName = presetName;

        if (action == Action::remove)
        {
            messageLabel.setText ("DELETE \"" + presetName.toUpperCase() + "\"?", juce::dontSendNotification);
            return;
        }

        nameEditor.setText (presetName, juce::dontSendNotification);
        confirmButton.setEnabled (juce::File::createLegalFileName (presetName.trim()).isNotEmpty() && ! (isNameReserved != nullptr && isNameReserved (presetName)));
        isConfirmingOverwrite = false;
        confirmButton.setButtonText ("SAVE");
        nameEditor.grabKeyboardFocus();
        nameEditor.selectAll();
    }

    void paint (juce::Graphics& g) override
    {
        presetPopupLAF.drawPopupBackground (g, getLocalBounds().toFloat(), numRows);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();
        const auto lineHeight = boundsFHeight * 0.2f;
        const auto fontHeight = lineHeight * presetPopupLAF.fontPOH;

        titleLabel.setFont (GTFonts::interMedium (fontHeight));
        titleLabel.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.1f).toNearestInt());

        nameEditor.applyFontToAllText (GTFonts::montserratMedium (fontHeight * 0.95f));
        nameEditor.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.4f).toNearestInt());

        messageLabel.setFont (GTFonts::interMedium (fontHeight));
        messageLabel.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.9f, lineHeight).withY (boundsFHeight * 0.4f).toNearestInt());

        const auto buttonsBounds = boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.7f);
        confirmButton.setBounds (buttonsBounds.withWidth (buttonsBounds.getWidth() * 0.45f).toNearestInt());
        cancelButton.setBounds (confirmButton.getBounds().toFloat().withRightX (buttonsBounds.getRight()).toNearestInt());
    }

    std::function<void (const juce::String& presetName)> onConfirm;
    std::function<bool (const juce::String& presetName)> presetExists;
    std::function<bool (const juce::String& presetName)> isNameReserved;
    float numRows {5.f};

private:
    const Action action;
    PresetPopupLAF presetPopupLAF;
    juce::Label titleLabel {"titleLabel", ""};
    juce::Label messageLabel {"messageLabel", ""};
    juce::TextEditor nameEditor;
    juce::TextButton confirmButton, cancelButton {"CANCEL"};
    juce::String preparedPresetName;
    bool isConfirmingOverwrite {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPresetPopup)
};
