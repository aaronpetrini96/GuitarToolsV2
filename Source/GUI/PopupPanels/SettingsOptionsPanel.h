#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "AAListOptionsPopup.h"
#include "../../Utils/AAResizeUI.h"
#include "../../Utils/AAParamUtils.h"

class SettingsOptionsPanel : public AAListOptionsPopup
{
public:
    SettingsOptionsPanel (AAResizeUI& resizeUIToUse, bool& showTooltipsToUse, juce::RangedAudioParameter& autoGainParamToUse, juce::RangedAudioParameter& gainLimitParamToUse)
        : AAListOptionsPopup ("SETTINGS"), resizeUI (resizeUIToUse), showTooltips (showTooltipsToUse), autoGainParam (autoGainParamToUse), gainLimitParam (gainLimitParamToUse)
    {
        listOptionsLAF.leftAlignText = true;
    }

    void updateItems()
    {
        juce::Array<AAListOptionsPopup::Item> settingsOptions;
        settingsOptions.add ({"Show Tooltips", [this] { toggleTooltips(); }, true, showTooltips, true});
        settingsOptions.add ({"Match Loudness", [this] { toggleBoolParameter (autoGainParam); }, true, autoGainParam.getValue() >= 0.5f, true, ParamHelpers::getTooltipForParam (ParamIDs::getLoudnessMatchActiveName())});
        // settingsOptions.add ({"Clip Guard",[this] { toggleBoolParameter (gainLimitParam); }, true, gainLimitParam.getValue() >= 0.5f, true, ParamHelpers::getTooltipForParam (ParamIDs::getGainLimitActiveName())});
        settingsOptions.add (resizeUI.getMakeDefaultOption());
        settingsOptions.add ({"About...", [this] { if (onAboutClicked != nullptr) onAboutClicked(); }});
        settingsOptions.add ({"Contact Me", [] { sendMail ({}); }});
        // settingsOptions.add ({"Report a Bug", [] { sendMail ("?subject=BugReport_GuitarTools2&body=Please include DAW used and the action that caused the problem. Thanks!"); }});
        settingsOptions.add ({"Get the Manual", [] { openUserManual(); }});
        setItems (settingsOptions);
    }

    std::function<void()> onTooltipsToggled;
    std::function<void()> onAboutClicked;

private:
    static void toggleBoolParameter (juce::RangedAudioParameter& boolParameter)
    {
        boolParameter.beginChangeGesture();
        boolParameter.setValueNotifyingHost (boolParameter.getValue() >= 0.5f ? 0.f : 1.f);
        boolParameter.endChangeGesture();
    }

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
    juce::RangedAudioParameter& autoGainParam;
    juce::RangedAudioParameter& gainLimitParam;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsOptionsPanel)
};
