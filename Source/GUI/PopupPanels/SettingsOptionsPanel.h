#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "AAListOptionsPopup.h"
#include "../../Utils/AAResizeUI.h"

class SettingsOptionsPanel : public AAListOptionsPopup
{
public:
    SettingsOptionsPanel (AAResizeUI& resizeUIToUse, bool& showTooltipsToUse)
        : resizeUI (resizeUIToUse), showTooltips (showTooltipsToUse)
    {
        // titleLabel.setJustificationType (juce::Justification::centred);
        // titleLabel.setColour (juce::Label::textColourId, GTColors::sectionTitleText.withAlpha (0.9f));
        // titleLabel.setInterceptsMouseClicks (false, false);
        // addAndMakeVisible (titleLabel);
    }

    // void paint (juce::Graphics& g) override
    // {
    //     AAListOptionsPopup::paint (g);
    //     listOptionsLAF.drawListOptionsTitleBackground (g, getLocalBounds().toFloat(), titleBounds);
    // }

    // void resized() override
    // {
    //     const auto boundsF = getLocalBounds().toFloat();
    //     const auto rowHeight = boundsF.getHeight() / (float) (getNumRows() + 1);
    //
    //     titleBounds = boundsF.withHeight (rowHeight);
    //     titleLabel.setFont (AAFonts::ibmPlexSansCondensedSemiBold (rowHeight * 0.7f).withExtraKerningFactor (0.12f));
    //     titleLabel.setBounds (titleBounds.toNearestInt());
    //
    //     auto rowIndex = 1;
    //
    //     for (auto* row : getChildren())
    //         if (row != &titleLabel)
    //             row->setBounds (boundsF.withHeight (rowHeight).withY (rowHeight * (float) rowIndex++).toNearestInt());
    // }

    void updateItems()
    {
        juce::Array<AAListOptionsPopup::Item> settingsOptions;
        settingsOptions.add ({showTooltips ? "Hide Tooltips" : "Show Tooltips", [this] { toggleTooltips(); }});
        settingsOptions.add (resizeUI.getMakeDefaultOption());
        settingsOptions.add ({"Get the Manual", [] { openUserManual(); }});
        settingsOptions.add ({"Report a Bug", [] { sendMail ("?subject=BugReport_GuitarTools2&body=Please include DAW used and the action that caused the problem. Thanks!"); }});
        settingsOptions.add ({"Contact Me", [] { sendMail ({}); }});
        settingsOptions.add ({"About...", [this] { if (onAboutClicked != nullptr) onAboutClicked(); }});
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
    // juce::Label titleLabel {"titleLabel", "SETTINGS"};
    // juce::Rectangle<float> titleBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsOptionsPanel)
};
