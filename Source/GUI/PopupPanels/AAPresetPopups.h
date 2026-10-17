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
    enum class Action { save, rename, remove };

    AAPresetPopup()
    {
        setAlwaysOnTop (true);
        setLookAndFeel (&presetPopupLAF);

        titleLabel.setJustificationType (juce::Justification::centred);
        titleLabel.setColour (juce::Label::textColourId, GTColors::sectionTitleText);
        titleLabel.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (titleLabel);

        nameEditor.setJustification (juce::Justification::centred);
        nameEditor.setInputRestrictions (24);
        nameEditor.onTextChange = [this]
        {
            confirmButton.setEnabled (juce::File::createLegalFileName (nameEditor.getText().trim()).isNotEmpty() && ! (isNameReserved != nullptr && isNameReserved (nameEditor.getText())));
            isConfirmingOverwrite = false;
            confirmButton.setButtonText (getConfirmText());
        };
        addChildComponent (nameEditor);

        messageLabel.setJustificationType (juce::Justification::centred);
        messageLabel.setColour (juce::Label::textColourId, GTColors::almostWhite);
        messageLabel.setInterceptsMouseClicks (false, false);
        addChildComponent (messageLabel);

        confirmButton.onClick = [this]
        {
            const auto isRemove = action == Action::remove;
            const auto presetName = isRemove ? preparedPresetName : nameEditor.getText();

            if (! isRemove && ! isConfirmingOverwrite && presetExists != nullptr && presetExists (presetName))
            {
                isConfirmingOverwrite = true;
                confirmButton.setButtonText ("Overwrite");
                return;
            }

            if (onConfirm != nullptr)
                onConfirm (action, presetName);
        };
        cancelButton.onClick = [this] { setVisible (false); };

        for (auto* button : {&confirmButton, &cancelButton})
            addAndMakeVisible (button);
    }

    ~AAPresetPopup() override
    {
        setLookAndFeel (nullptr);
    }

    void prepare (const Action newAction, const juce::String& presetName)
    {
        action = newAction;
        preparedPresetName = presetName;
        isConfirmingOverwrite = false;
        confirmButton.setButtonText (getConfirmText());

        const auto isRemove = action == Action::remove;
        titleLabel.setText (isRemove ? "DELETE PRESET" : action == Action::save ? "SAVE PRESET" : "RENAME PRESET", juce::dontSendNotification);
        nameEditor.setVisible (! isRemove);
        messageLabel.setVisible (isRemove);

        if (isRemove)
        {
            messageLabel.setText ("Delete \"" + presetName + "\"?", juce::dontSendNotification);
            confirmButton.setEnabled (true);
            return;
        }

        nameEditor.setText (presetName, juce::dontSendNotification);
        confirmButton.setEnabled (juce::File::createLegalFileName (presetName.trim()).isNotEmpty() && ! (isNameReserved != nullptr && isNameReserved (presetName)));
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
        const auto fontHeight = lineHeight * GTFontHeightPOH;

        titleLabel.setFont (GTFonts::montserratMedium (fontHeight));
        titleLabel.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.1f).toNearestInt());

        nameEditor.applyFontToAllText (GTFonts::montserratMedium (fontHeight));
        nameEditor.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.4f).toNearestInt());

        messageLabel.setFont (GTFonts::montserratMedium (fontHeight));
        messageLabel.setBounds (boundsF.withSizeKeepingCentre (boundsFWidth * 0.9f, lineHeight).withY (boundsFHeight * 0.4f).toNearestInt());

        const auto buttonsBounds = boundsF.withSizeKeepingCentre (boundsFWidth * 0.8f, lineHeight).withY (boundsFHeight * 0.7f);
        confirmButton.setBounds (buttonsBounds.withWidth (buttonsBounds.getWidth() * 0.45f).toNearestInt());
        cancelButton.setBounds (confirmButton.getBounds().toFloat().withRightX (buttonsBounds.getRight()).toNearestInt());
    }

    std::function<void (Action action, const juce::String& presetName)> onConfirm;
    std::function<bool (const juce::String& presetName)> presetExists;
    std::function<bool (const juce::String& presetName)> isNameReserved;
    float numRows {5.f};

private:
    juce::String getConfirmText() const
    {
        return action == Action::save ? "Save" : action == Action::rename ? "Rename" : "Delete";
    }

    Action action {Action::save};
    PresetPopupLAF presetPopupLAF;
    juce::Label titleLabel {"titleLabel", ""};
    juce::Label messageLabel {"messageLabel", ""};
    juce::TextEditor nameEditor;
    juce::TextButton confirmButton, cancelButton {"Cancel"};
    juce::String preparedPresetName;
    bool isConfirmingOverwrite {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPresetPopup)
};
