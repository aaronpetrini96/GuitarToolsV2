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
    SettingsOptionsPanel (AAResizeUI& resizeUIToUse, bool& showTooltipsToUse, juce::RangedAudioParameter& autoGainParamToUse, juce::RangedAudioParameter& lowTuningParamToUse)
        : AAListOptionsPopup ("SETTINGS"), resizeUI (resizeUIToUse), showTooltips (showTooltipsToUse), autoGainParam (autoGainParamToUse), lowTuningParam (lowTuningParamToUse)
    {
        listOptionsLAF.leftAlignText = true;
    }

    void updateItems()
    {
        juce::Array<AAListOptionsPopup::Item> settingsOptions;
        settingsOptions.add ({"Show Tooltips", [this] { toggleTooltips(); }, true, showTooltips, true});
        settingsOptions.add ({"Match Loudness", [this] { toggleBoolParameter (autoGainParam); }, true, autoGainParam.getValue() >= 0.5f, true, ParamHelpers::getTooltipForParam (ParamIDs::getLoudnessMatchActiveName())});
        settingsOptions.add ({"Low Tuning Mode", [this] { toggleBoolParameter (lowTuningParam); }, true, lowTuningParam.getValue() >= 0.5f, true, ParamHelpers::getTooltipForParam (ParamIDs::getLowTuningActiveName())});
        settingsOptions.add ({"Set UI Size As Default", [this] { resizeUI.makeCurrentSizeDefault(); }, ! resizeUI.isCurrentScaleDefault(), false, false, {},
                              juce::Drawable::createFromImageData (BinaryData::resizeLines_svg, BinaryData::resizeLines_svgSize)});
        settingsOptions.add ({"Info", [this] { if (onAboutClicked != nullptr) onAboutClicked(); }, true, false, false, {},
                              juce::Drawable::createFromImageData (BinaryData::infoLetterIcon_svg, BinaryData::infoLetterIcon_svgSize)});
        settingsOptions.add ({"Get In Touch", [] { sendMail ({}); }, true, false, false, {},
                              juce::Drawable::createFromImageData (BinaryData::atIcon_svg, BinaryData::atIcon_svgSize)});
        // settingsOptions.add ({"Report a Bug", [] { sendMail ("?subject=BugReport_GuitarTools2&body=Please include DAW used and the action that caused the problem. Thanks!"); }});
        // settingsOptions.add ({"Get the Manual", [] { openUserManual(); }, true, false, false, {},
        //                       juce::Drawable::createFromImageData (BinaryData::questionMarkIcon_svg, BinaryData::questionMarkIcon_svgSize)});
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

    // static void openUserManual()
    // {
    //     const auto manualFile = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("GuitarTools2_User_Manual.pdf");
    //
    //     if (manualFile.replaceWithData (BinaryData::guitarTools2UserManual_pdf, BinaryData::guitarTools2UserManual_pdfSize))
    //         manualFile.startAsProcess();
    // }

    static void sendMail (const juce::String& mailQuery)
    {
        juce::URL ("mailto:aaron.petrini96@gmail.com" + mailQuery).launchInDefaultBrowser();
    }

    AAResizeUI& resizeUI;
    bool& showTooltips;
    juce::RangedAudioParameter& autoGainParam;
    juce::RangedAudioParameter& lowTuningParam;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsOptionsPanel)
};
