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

        nameEditor.setJustification (juce::Justification::centred);
        nameEditor.setIndents(0, 0);
        nameEditor.setInputRestrictions (GTPresetNameMaxLength);
        nameEditor.setEscapeAndReturnKeysConsumed (false);
        setWantsKeyboardFocus (true);
        nameEditor.onTextChange = [this]
        {
            confirmButton.setEnabled (juce::File::createLegalFileName (nameEditor.getText().trim()).isNotEmpty() && ! (isNameReserved != nullptr && isNameReserved (nameEditor.getText())));
            isConfirmingOverwrite = false;
            confirmButton.setButtonText (getConfirmText());
        };
        addChildComponent (nameEditor);

        messageLabel.setJustificationType (juce::Justification::centred);
        messageLabel.setColour (juce::Label::textColourId, GTPopupTextColor);
        messageLabel.setInterceptsMouseClicks (false, false);
        addChildComponent (messageLabel);

        nameLabel.setJustificationType (juce::Justification::centred);
        nameLabel.setColour (juce::Label::textColourId, GTColors::almostWhiteText);
        nameLabel.setInterceptsMouseClicks (false, false);
        addChildComponent (nameLabel);

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
        title = isRemove ? "DELETE PRESET" : action == Action::save ? "SAVE PRESET" : "RENAME PRESET";
        repaint();
        nameEditor.setVisible (! isRemove);
        messageLabel.setVisible (isRemove);
        nameLabel.setVisible (! isRemove);

        if (isRemove)
        {
            messageLabel.setText ("Delete \"" + presetName + "\"?", juce::dontSendNotification);
            confirmButton.setEnabled (true);
            grabKeyboardFocus();
            return;
        }

        nameEditor.setText (presetName, juce::dontSendNotification);
        confirmButton.setEnabled (juce::File::createLegalFileName (presetName.trim()).isNotEmpty() && ! (isNameReserved != nullptr && isNameReserved (presetName)));
        nameEditor.grabKeyboardFocus();
        nameEditor.selectAll();
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * GTCornerPOH;
        const auto outlineThickness = boundsF.getHeight() / numRows * GTOutlineThicknessPOH;
        AAUITools::drawOutlinedRoundedRectangle (g, boundsF, cornerSize, outlineThickness, GTPopupBackgroundColor, GTPopupOutlineColor);

        juce::Path titleShape;
        titleShape.addRoundedRectangle (titleBounds.getX(), titleBounds.getY(), titleBounds.getWidth(), titleBounds.getHeight(), cornerSize, cornerSize,
                                        true, true, false, false);
        g.setColour (GTColors::presetBoxBackground);
        g.fillPath (titleShape);

        g.setColour (GTPopupOutlineColor);
        g.fillRect (titleBounds.withTop (titleBounds.getBottom() - outlineThickness * 0.5f));

        g.setColour (GTPopupTextColor.withBrightness (2.f));
        g.setFont (GTFonts::montserratSemiBold (titleBounds.getHeight() * GTFontHeightPOH));
        g.drawText (title, titleBounds, juce::Justification::centred, true);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();
        const auto lineHeight = boundsFHeight * 0.18f;
        const auto fontHeight = lineHeight * GTFontHeightPOH;
        titleBounds = boundsF.withHeight (boundsFHeight / numRows * GTPopupTitleRowRatio);

        const auto nameEditorWidth = boundsFWidth * 0.85f;
        const auto nameEditorHeight = boundsFHeight * 0.2f;
        const auto nameEditorBounds = boundsF.withSizeKeepingCentre (nameEditorWidth, nameEditorHeight);
        nameEditor.applyFontToAllText (GTFonts::montserratMedium (fontHeight));
        nameEditor.setBounds (nameEditorBounds.toNearestInt());

        nameLabel.setFont (GTFonts::montserratMedium (fontHeight));
        nameLabel.setBounds (nameEditorBounds.withBottomY (nameEditorBounds.getY()).toNearestInt());

        messageLabel.setFont (GTFonts::montserratMedium (fontHeight));
        messageLabel.setBounds (nameEditorBounds.toNearestInt());

        const auto buttonWidth = boundsFWidth * 0.35f;
        const auto buttonsBounds = boundsF.withSizeKeepingCentre (buttonWidth, lineHeight).withY (boundsFHeight * 0.72f);
        confirmButton.setBounds (buttonsBounds.withCentre({ boundsFWidth * 0.3f, buttonsBounds.getCentreY()}).toNearestInt());
        cancelButton.setBounds (buttonsBounds.withCentre({boundsFWidth * 0.7f, buttonsBounds.getCentreY()}).toNearestInt());
    }

    bool keyPressed (const juce::KeyPress& key) override
    {
        if (key == juce::KeyPress::returnKey)
        {
            confirmButton.triggerClick();
            return true;
        }

        if (key == juce::KeyPress::escapeKey)
        {
            cancelButton.triggerClick();
            return true;
        }

        return false;
    }

    std::function<void (Action action, const juce::String& presetName)> onConfirm;
    std::function<bool (const juce::String& presetName)> presetExists;
    std::function<bool (const juce::String& presetName)> isNameReserved;
    float numRows {6.f};

private:
    juce::String getConfirmText() const
    {
        return action == Action::save ? "Save" : action == Action::rename ? "Rename" : "Delete";
    }

    Action action {Action::save};
    PresetPopupLAF presetPopupLAF;
    juce::String title;
    juce::Rectangle<float> titleBounds;
    juce::Label messageLabel {"messageLabel", ""};
    juce::Label nameLabel {"nameLabel", "New preset name"};
    juce::TextEditor nameEditor;
    juce::TextButton confirmButton, cancelButton {"Cancel"};
    juce::String preparedPresetName;
    bool isConfirmingOverwrite {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPresetPopup)
};
