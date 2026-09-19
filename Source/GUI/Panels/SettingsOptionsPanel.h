#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../Components/AAListOptionsPopup.h"
#include "../../Utils/AAResizeUI.h"

class SettingsOptionsPanel : public AAListOptionsPopup
{
public:
    SettingsOptionsPanel (AAResizeUI& resizeUIToUse, bool& showTooltipsToUse)
        : resizeUI (resizeUIToUse), showTooltips (showTooltipsToUse)
    {
        drawRowDividers = true;
        onItemClicked = [this] (const int itemID) { handleSettingsOption (itemID); };
    }

    void updateItems()
    {
        juce::Array<AAListOptionsPopup::Item> settingsOptions;
        settingsOptions.add (AAListOptionsPopup::Item {1, showTooltips ? "Hide Tooltips" : "Show Tooltips"});
        settingsOptions.add (resizeUI.getMakeDefaultOption (3));
        settingsOptions.add (AAListOptionsPopup::Item {4, "Get the Manual"});
        settingsOptions.add (AAListOptionsPopup::Item {5, "Report a Bug"});
        settingsOptions.add (AAListOptionsPopup::Item {6, "Contact Me"});
        settingsOptions.add (AAListOptionsPopup::Item {2, "About..."});
        setItems (settingsOptions);
    }

    std::function<void()> onTooltipsToggled;
    std::function<void()> onAboutClicked;

private:
    void handleSettingsOption (const int itemID)
    {
        if (itemID == 1)
        {
            showTooltips = ! showTooltips;

            if (onTooltipsToggled != nullptr)
                onTooltipsToggled();
        }
        else if (itemID == 2)
        {
            if (onAboutClicked != nullptr)
                onAboutClicked();
        }
        else if (itemID == 3)
        {
            resizeUI.makeCurrentSizeDefault();
        }
        else if (itemID == 4)
        {
            const auto manualFile = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("GuitarTools2_User_Manual.pdf");
            if (manualFile.replaceWithData (BinaryData::guitarTools2UserManual_pdf, BinaryData::guitarTools2UserManual_pdfSize))
                manualFile.startAsProcess();
        }
        else if (itemID == 5)
        {
            juce::URL ("mailto:aaron.petrini96@gmail.com?subject=BugReport_GuitarTools2&body=Please include DAW used and the action that caused the problem. Thanks!").launchInDefaultBrowser();
        }
        else if (itemID == 6)
        {
            juce::URL ("mailto:aaron.petrini96@gmail.com").launchInDefaultBrowser();
        }
    }

    AAResizeUI& resizeUI;
    bool& showTooltips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsOptionsPanel)
};
