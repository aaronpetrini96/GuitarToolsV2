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
    }

    void updateItems()
    {
        juce::Array<AAListOptionsPopup::Item> settingsOptions;
        settingsOptions.add ({showTooltips ? "HIDE TOOLTIPS" : "SHOW TOOLTIPS", [this] { toggleTooltips(); }});
        settingsOptions.add (resizeUI.getMakeDefaultOption());
        settingsOptions.add ({"GET THE MANUAL", [] { openUserManual(); }});
        settingsOptions.add ({"REPORT A BUG", [] { sendMail ("?subject=BugReport_GuitarTools2&body=Please include DAW used and the action that caused the problem. Thanks!"); }});
        settingsOptions.add ({"CONTACT ME", [] { sendMail ({}); }});
        settingsOptions.add ({"ABOUT...", [this] { if (onAboutClicked != nullptr) onAboutClicked(); }});
        setItems (settingsOptions);
    }

    std::function<void()> onTooltipsToggled;
    std::function<void()> onAboutClicked;

private:
    void toggleTooltips()
    {
        showTooltips = ! showTooltips;

        if (onTooltipsToggled != nullptr)
            onTooltipsToggled();
    }

    static void openUserManual()
    {
        const auto manualFile = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("GuitarTools2_User_Manual.pdf");

        if (manualFile.replaceWithData (BinaryData::guitarTools2UserManual_pdf, BinaryData::guitarTools2UserManual_pdfSize))
            manualFile.startAsProcess();
    }

    static void sendMail (const juce::String& mailQuery)
    {
        juce::URL ("mailto:aaron.petrini96@gmail.com" + mailQuery).launchInDefaultBrowser();
    }

    AAResizeUI& resizeUI;
    bool& showTooltips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsOptionsPanel)
};
