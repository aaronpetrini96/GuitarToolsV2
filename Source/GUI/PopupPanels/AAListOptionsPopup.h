//
// Created by Aaron Petrini on 17/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../../AALook.h"
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

    void setItems (const juce::Array<Item>& newItems)
    {
        rows.clear();

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
        }

        resized();
    }

    float getNumRows() const { return (float) rows.size() + titleHeightRowRatio; }

    void paint (juce::Graphics& g) override
    {
        listOptionsLAF.drawListOptionsBackground (g, getLocalBounds().toFloat(), getNumRows(), drawRowDividers);
        listOptionsLAF.drawListOptionsTitle (g, titleBounds, title);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto rowHeight = boundsF.getHeight() / getNumRows();
        titleBounds = boundsF.withHeight (rowHeight * titleHeightRowRatio);

        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
            rows[rowIndex]->setBounds (boundsF.withHeight (rowHeight).withY (titleBounds.getBottom() + rowHeight * (float) rowIndex).toNearestInt());
    }

    bool drawRowDividers {false};
    float titleHeightRowRatio {1.1f};

protected:
    ListOptionsPopupLAF listOptionsLAF;

private:
    juce::String title;
    juce::Rectangle<float> titleBounds;
    juce::OwnedArray<juce::Button> rows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAListOptionsPopup)
};
