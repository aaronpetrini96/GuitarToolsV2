//
// Created by Aaron Petrini on 03/09/2026.
//

#pragma once
#include "../../Utils/GTLook.h"

class PresetBrowserLAF : public juce::LookAndFeel_V4
{
public:
    PresetBrowserLAF()
    {
        setColour (juce::PopupMenu::backgroundColourId, GTColors::transparent);
        setColour (juce::PopupMenu::textColourId, GTColors::iconOverColor);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, GTColors::presetPopupHighlight.withAlpha(0.7f));
    }

    static PresetBrowserLAF* get()
    {
        static PresetBrowserLAF instance;
        return &instance;
    }

    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox&) override
    {
        const juce::Rectangle<int> boxBounds (0, 0, width, height);
        const auto boundsF = boxBounds.toFloat();
        const auto cornerSize = boundsF.getHeight() * 0.2f;
        g.setGradientFill (juce::ColourGradient::horizontal (GTColors::presetBox, 0.0f, GTColors::presetBox.darker (0.1f), boundsF.getWidth()));
        g.fillRoundedRectangle (boundsF, cornerSize);
    }

    int getMenuWindowFlags() override
    {
        return 0;
    }

    void drawPopupMenuBackgroundWithOptions (juce::Graphics& g, int width, int height,
                                             const juce::PopupMenu::Options& options) override
    {
        const auto boundsF = juce::Rectangle<int> (0, 0, width, height).toFloat();
        const auto cornerSize = (options.getStandardItemHeight() > 0
                                     ? options.getStandardItemHeight()
                                     : getPopupMenuFont().getHeight()) * 0.2f;

        g.setGradientFill (juce::ColourGradient::vertical (GTColors::presetBox, 0.0f, GTColors::presetBox.darker (0.1f), boundsF.getHeight()));
        g.fillRoundedRectangle (boundsF, cornerSize);
    }

    juce::PopupMenu::Options getOptionsForComboBoxPopupMenu (juce::ComboBox& box, juce::Label& label) override
    {
        const auto options = juce::LookAndFeel_V4::getOptionsForComboBoxPopupMenu (box, label);
        const auto boxBounds = box.getScreenBounds().toFloat();
        const auto popupTop = box.getParentComponent()->getScreenBounds().getBottom();

        return options.withTargetScreenArea ({ juce::roundToInt(boxBounds.getRight()), popupTop,  juce::roundToInt(boxBounds.getWidth() * 0.75f), 0 })
                      .withPreferredPopupDirection (juce::PopupMenu::Options::PopupDirection::downwards);
    }

    int getPopupMenuBorderSizeWithOptions (const juce::PopupMenu::Options&) override
    {
        return 0;
    }

    void drawPopupMenuItemWithOptions (juce::Graphics& g, const juce::Rectangle<int>& area,
                                      bool isHighlighted, const juce::PopupMenu::Item& item,
                                      const juce::PopupMenu::Options& options) override
    {

        auto itemBounds = area.toFloat();
        if (isHighlighted && item.isEnabled)
        {
            g.setColour (findColour (juce::PopupMenu::highlightedBackgroundColourId));
            g.fillRect(itemBounds);
        }

        const auto* targetComponent = options.getTargetComponent();
        const auto fontH = (targetComponent != nullptr ? targetComponent->getHeight() : itemBounds.getHeight()) * 0.675f;
        itemBounds.reduce (itemBounds.getWidth() * 0.05f, itemBounds.getHeight() * 0.04f);

        g.setColour (item.isTicked ? juce::Colours::white : GTColors::almostWhite);
        g.setFont (item.isTicked ? GTFonts::interSemiBold(fontH) : GTFonts::interMedium (fontH));
        g.drawFittedText (item.text, itemBounds.toNearestInt(), juce::Justification::centred, 1);

        if (item.subMenu != nullptr && (item.itemID == 0 || item.subMenu->getNumItems() > 0))
        {
            const auto arrowHeight = itemBounds.getHeight() * 0.35f;
            const auto arrowX = itemBounds.removeFromRight (arrowHeight).getX();
            const auto arrowCentreY = itemBounds.getCentreY();

            juce::Path arrow;
            arrow.startNewSubPath (arrowX, arrowCentreY - arrowHeight * 0.5f);
            arrow.lineTo (arrowX + arrowHeight * 0.6f, arrowCentreY);
            arrow.lineTo (arrowX, arrowCentreY + arrowHeight * 0.5f);
            g.strokePath (arrow, juce::PathStrokeType (itemBounds.getHeight() * 0.08f));
        }
    }

    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds (box.getLocalBounds());
        label.setFont (GTFonts::interMedium (box.getHeight() * 0.675f));
        label.setJustificationType (juce::Justification::centred);
    }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserLAF)
};
