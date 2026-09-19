//
// Created by Aaron Petrini on 17/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../../Utils/AALook.h"

class AAListOptionsPopup : public juce::Component
{
public:
    struct Item
    {
        int itemID {0};
        juce::String text;
        bool isEnabled {true};
        bool isSelected {false};
    };

    AAListOptionsPopup()
    {
        setAlwaysOnTop (true);
    }

    void setItems (const juce::Array<Item>& newItems)
    {
        rows.clear();

        for (const auto& item : newItems)
        {
            auto* row = rows.add (new RowButton (item));
            row->onClick = [this, itemID = item.itemID] { itemClicked (itemID); };
            addAndMakeVisible (row);
        }

        resized();
    }

    int getNumRows() const { return rows.size(); }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto rowHeight = boundsF.getHeight() / (float) juce::jmax (1, rows.size());
        const auto referenceWidth = rowHeight * 4.25f;
        const auto cornerSize = referenceWidth * 0.055f;
        const auto lineThickness = referenceWidth * 0.002f;

        g.setGradientFill (juce::ColourGradient::vertical (GTColors::gradientNavyDark, 0.f, GTColors::gradientNavy, boundsF.getHeight()));
        g.fillRoundedRectangle (boundsF, cornerSize);

        if (drawRowDividers)
        {
            const auto dividerBounds = boundsF.withSizeKeepingCentre (boundsF.getWidth() * 0.9f, rowHeight * 0.06f);

            for (int rowIndex = 1; rowIndex < rows.size(); ++rowIndex)
                AAUITools::drawRoundedDivider (g, dividerBounds.withCentre ({ boundsF.getCentreX(), rowHeight * (float) rowIndex }));
        }

        g.setColour (GTColors::almostWhite);
        g.drawRoundedRectangle (boundsF.reduced (lineThickness * 0.5f), cornerSize, lineThickness);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto rowHeight = boundsF.getHeight() / (float) juce::jmax (1, rows.size());

        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
            rows[rowIndex]->setBounds (boundsF.withHeight (rowHeight).withY (rowHeight * (float) rowIndex).toNearestInt());
    }

    std::function<void (int itemID)> onItemClicked;
    bool drawRowDividers {false};

private:
    class RowButton : public juce::Button
    {
    public:
        explicit RowButton (const Item& item) : juce::Button (item.text)
        {
            setEnabled (item.isEnabled);
            setToggleState (item.isSelected, juce::dontSendNotification);
        }

        void paintButton (juce::Graphics& g, const bool shouldDrawButtonAsHighlighted, bool) override
        {
            const auto isActive = getToggleState() || (shouldDrawButtonAsHighlighted && isEnabled());

            g.setColour ((isActive ? GTColors::activeNeonBlue : GTColors::iconNormalColor).withMultipliedAlpha (isEnabled() ? 1.0f : 0.5f));

            g.setFont (AAFonts::ibmPlexSansMedium ((float) getHeight() * 0.68f));
            g.drawText (getButtonText(), getLocalBounds(), juce::Justification::centred, true);
        }
    };

    void itemClicked (const int itemID)
    {
        setVisible (false);

        if (onItemClicked != nullptr)
            onItemClicked (itemID);
    }

    juce::OwnedArray<RowButton> rows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAListOptionsPopup)
};
