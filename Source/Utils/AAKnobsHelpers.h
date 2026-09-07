//
// Created by Aaron Petrini on 05/09/2026.
//

//This header should provide access to helpers and utilities shared by all AAKnobs
//It should not store anything that is not used by only one custom component

#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_dsp/juce_dsp.h"
#include "juce_core/juce_core.h"

namespace AAKnobsHelpers
{
    struct AAKnobValueLabel : juce::Label
    {
        juce::TextEditor* createEditorComponent() override
        {
            auto* editor = juce::Label::createEditorComponent();

            editor->setBorder (juce::BorderSize<int>());
            editor->setIndents (getBorderSize().getLeft(), getBorderSize().getTop());
            editor->setJustification (getJustificationType());
            editor->setPopupMenuEnabled (false);

            return editor;
        }
    };
}
