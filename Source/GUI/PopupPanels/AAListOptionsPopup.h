//
// Created by Aaron Petrini on 17/09/2026.
//

#pragma once

#include <functional>
#include "../LAF/ListOptionsPopupLAF.h"

class AAListOptionsPopup : public juce::Component
{
public:
    struct Item
    {
        juce::String text;
        std::function<void()> onSelected;
        bool isEnabled {true};
        bool isSelected {false};
        bool isCheckbox {false};
        juce::String tooltip;
        std::shared_ptr<juce::Drawable> icon;
    };

    explicit AAListOptionsPopup (const juce::String& titleToUse) : title (titleToUse)
    {
        setAlwaysOnTop (true);
        setLookAndFeel (&listOptionsLAF);
    }

    ~AAListOptionsPopup() override
    {
        setLookAndFeel (nullptr);
    }

    float getNumRows() const { return (float) rows.size() + GTPopupTitleRowRatio; }

    void paint (juce::Graphics& g) override
    {
        listOptionsLAF.drawListOptionsBackground (g, getLocalBounds().toFloat(), getNumRows(), drawRowDividers);
        listOptionsLAF.drawListOptionsTitle (g, titleBounds, title);
    }

    void paintOverChildren (juce::Graphics& g) override
    {
        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
        {
            if (const auto& icon = rowIcons[(size_t) rowIndex])
            {
                const auto rowBounds = rows[rowIndex]->getBounds().toFloat();
                const auto iconSize = rowBounds.getHeight() * 0.6f;
                const auto iconWidth = iconSize * juce::jmax (1.0f, icon->getDrawableBounds().getWidth() / icon->getDrawableBounds().getHeight());
                const auto iconBounds = rowBounds.withSizeKeepingCentre (iconWidth, iconSize).withRightX (rowBounds.getRight() - rowBounds.getHeight() * 0.45f);
                icon->drawWithin (g, iconBounds, juce::RectanglePlacement::centred, rows[rowIndex]->isEnabled() ? 1.f : 0.3f);
            }
        }

        listOptionsLAF.drawListOptionsOutline (g, getLocalBounds().toFloat());
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto rowHeight = boundsF.getHeight() / getNumRows();
        titleBounds = boundsF.withHeight (rowHeight * GTPopupTitleRowRatio);

        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
            rows[rowIndex]->setBounds (boundsF.withHeight (rowHeight).withY (titleBounds.getBottom() + rowHeight * (float) rowIndex).toNearestInt());
    }

    bool drawRowDividers {false};

protected:
    ListOptionsPopupLAF listOptionsLAF;

    void setItems (const juce::Array<Item>& newItems)
    {
        rows.clear();
        rowIcons.clear();
        for (const auto& item : newItems)
        {
            auto* row = rows.add (item.isCheckbox ? static_cast<juce::Button*> (new juce::ToggleButton (item.text)) : new juce::TextButton (item.text));
            row->setEnabled (item.isEnabled);
            row->setToggleState (item.isSelected, juce::dontSendNotification);
            row->setTooltip (item.tooltip);
            row->onClick = [this, onSelected = item.onSelected, closesPopup = ! item.isCheckbox]
            {
                if (closesPopup)
                    setVisible (false);

                if (onSelected != nullptr)
                    onSelected();
            };
            addAndMakeVisible (row);

            if (item.icon != nullptr)
                item.icon->replaceColour(GTColors::iconSVGColor, GTPopupTextColor);
            rowIcons.push_back (item.icon);
        }

        resized();
    }

private:
    juce::String title;
    juce::Rectangle<float> titleBounds;
    juce::OwnedArray<juce::Button> rows;
    std::vector<std::shared_ptr<juce::Drawable>> rowIcons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAListOptionsPopup)
};
