//
// Created by Aaron Petrini on 17/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../../Utils/AALook.h"
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
    };

    AAListOptionsPopup()
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
            auto* row = rows.add (new juce::TextButton (item.text));
            row->setEnabled (item.isEnabled);
            row->setToggleState (item.isSelected, juce::dontSendNotification);
            row->onClick = [this, onSelected = item.onSelected]
            {
                setVisible (false);

                if (onSelected != nullptr)
                    onSelected();
            };
            addAndMakeVisible (row);
        }

        resized();
    }

    int getNumRows() const { return rows.size(); }

    void paint (juce::Graphics& g) override
    {
        listOptionsLAF.drawListOptionsBackground (g, getLocalBounds().toFloat(), rows.size(), drawRowDividers);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto rowHeight = boundsF.getHeight() / (float) juce::jmax (1, rows.size());

        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
            rows[rowIndex]->setBounds (boundsF.withHeight (rowHeight).withY (rowHeight * (float) rowIndex).toNearestInt());
    }

    bool drawRowDividers {false};

private:
    ListOptionsPopupLAF listOptionsLAF;
    juce::OwnedArray<juce::TextButton> rows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAListOptionsPopup)
};
