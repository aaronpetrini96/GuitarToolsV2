//
// Created by Aaron Petrini on 17/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../LAF/PresetBrowserLAF.h"
#include "../../Utils/PresetManager.h"

class AAPresetBrowser : public juce::Component
{
public:
    explicit AAPresetBrowser (PresetManager& manager)
        : presetManager (manager)
    {
        setAlwaysOnTop (true);
        setLookAndFeel (PresetBrowserLAF::get());

        viewport.setViewedComponent (&rowsHolder, false);
        viewport.setScrollBarsShown (false, false);
        addAndMakeVisible (viewport);

        defaultTabButton.setToggleState (true, juce::dontSendNotification);
        defaultTabButton.setConnectedEdges (juce::Button::ConnectedOnRight);
        userTabButton.setConnectedEdges (juce::Button::ConnectedOnLeft);
        defaultTabButton.onClick = [this] { setPresetType (PresetManager::PresetType::factory); };
        userTabButton.onClick = [this] { setPresetType (PresetManager::PresetType::user); };

        for (auto* tabButton : {&defaultTabButton, &userTabButton})
            addAndMakeVisible (tabButton);
    }

    ~AAPresetBrowser() override
    {
        setLookAndFeel (nullptr);
    }

    void refresh()
    {
        const auto hasUserPresets = ! presetManager.getPresetFiles (PresetManager::PresetType::user).isEmpty();
        userTabButton.setEnabled (hasUserPresets);

        if (! hasUserPresets && selectedPresetType == PresetManager::PresetType::user)
        {
            setPresetType (PresetManager::PresetType::factory);
            return;
        }

        presetFiles = presetManager.getPresetFiles (selectedPresetType);
        const auto currentPresetName = presetManager.getCurrentPresetName();
        const auto isUserPresets = selectedPresetType == PresetManager::PresetType::user;

        rows.clear();
        deleteButtons.clear();

        for (const auto& presetFile : presetFiles)
        {
            auto* row = rows.add (new juce::TextButton (presetFile.getFileNameWithoutExtension()));
            row->setToggleState (presetFile.getFileNameWithoutExtension() == currentPresetName, juce::dontSendNotification);
            row->onClick = [this, presetFile]
            {
                presetManager.loadPresetFile (presetFile);
                setVisible (false);
            };
            rowsHolder.addAndMakeVisible (row);

            if (! isUserPresets)
                continue;

            auto* deleteButton = deleteButtons.add (new juce::TextButton ("x"));
            deleteButton->onClick = [this, presetFile] { if (onDeleteRequested != nullptr) onDeleteRequested (presetFile); };
            rowsHolder.addAndMakeVisible (deleteButton);
        }

        resized();

        if (onRowsChanged != nullptr)
            onRowsChanged();
    }

    void showUserPresets()
    {
        setPresetType (PresetManager::PresetType::user);
    }

    float getNumRows() const { return (float) rows.size() + tabHeightRowRatio; }

    void paint (juce::Graphics& g) override
    {
        if (auto* presetBrowserLAF = dynamic_cast<PresetBrowserLAF*> (&getLookAndFeel()))
            presetBrowserLAF->drawMenuListBackground (g, getLocalBounds().toFloat());
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto rowHeight = boundsF.getHeight() / getNumRows();
        const auto tabHeight = rowHeight * tabHeightRowRatio;
        const auto tabWidth = boundsF.getWidth() * 0.5f;

        defaultTabButton.setBounds (boundsF.withSize (tabWidth, tabHeight).toNearestInt());
        userTabButton.setBounds (boundsF.withHeight (tabHeight).withLeft (tabWidth).toNearestInt());

        const auto listBounds = boundsF.withTop (tabHeight);
        viewport.setBounds (listBounds.toNearestInt());

        const auto rowWidth = boundsF.getWidth();
        rowsHolder.setSize (juce::roundToInt (rowWidth), juce::roundToInt (rowHeight * (float) rows.size()));

        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
        {
            const auto rowBounds = juce::Rectangle<float> (0.0f, rowHeight * (float) rowIndex, rowWidth, rowHeight);
            rows[rowIndex]->setBounds (rowBounds.toNearestInt());

            if (rowIndex < deleteButtons.size())
                deleteButtons[rowIndex]->setBounds (rowBounds.withWidth (rowHeight).withRightX (rowWidth).toNearestInt());
        }
    }


    float tabHeightRowRatio {1.1f};
    std::function<void (const juce::File& presetFile)> onDeleteRequested;
    std::function<void()> onRowsChanged;

private:
    void setPresetType (const PresetManager::PresetType presetType)
    {
        selectedPresetType = presetType;
        defaultTabButton.setToggleState (presetType == PresetManager::PresetType::factory, juce::dontSendNotification);
        userTabButton.setToggleState (presetType == PresetManager::PresetType::user, juce::dontSendNotification);
        refresh();
    }

    PresetManager& presetManager;
    PresetManager::PresetType selectedPresetType {PresetManager::PresetType::factory};
    juce::Array<juce::File> presetFiles;

    juce::TextButton defaultTabButton {"Factory"}, userTabButton {"User Presets"};
    juce::Component rowsHolder;
    juce::OwnedArray<juce::TextButton> rows, deleteButtons;
    juce::Viewport viewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPresetBrowser)
};
