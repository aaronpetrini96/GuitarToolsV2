#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>

#include "../../Utils/GTLook.h"

namespace Colors
{
    namespace Knob
    {
        const juce::Colour trackActive {juce::Colours::red.darker(0.2f)};
        const juce::Colour label {juce::Colours::white};
        const juce::Colour textBoxBackground {80, 80, 80};
        const juce::Colour value {240, 240, 240};
        const juce::Colour caret {255, 255, 255};
    }
}

class RotaryKnobLabel : public juce::Label
{
public:
    RotaryKnobLabel() : juce::Label() {}

    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override {}

    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
    {
        return createIgnoredAccessibilityHandler (*this);
    }

    juce::TextEditor* createEditorComponent() override
    {
        auto* ed = new juce::TextEditor (getName());
        ed->applyFontToAllText (getLookAndFeel().getLabelFont (*this));
        copyAllExplicitColoursTo (*ed);

        ed->setBorder (juce::BorderSize<int>());
        ed->setIndents (2, 1);
        ed->setJustification (juce::Justification::centredTop);
        ed->setPopupMenuEnabled (false);
        ed->setInputRestrictions (8);

        return ed;
    }
};

class RotaryKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RotaryKnobLookAndFeel()
    {
        setColour (juce::Label::textColourId, Colors::Knob::label);
        setColour (juce::Slider::textBoxTextColourId, Colors::Knob::label);
        setColour (juce::Slider::rotarySliderFillColourId, Colors::Knob::trackActive);
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::CaretComponent::caretColourId, Colors::Knob::caret);
    }

    static RotaryKnobLookAndFeel* get()
    {
        static RotaryKnobLookAndFeel instance;
        return &instance;
    }

    juce::Font getLabelFont ([[maybe_unused]] juce::Label& label) override
    {
        return GTFonts::ibmPlexMonoMedium (16.0f);
    }

    juce::Label* createSliderTextBox (juce::Slider& slider) override
    {
        auto l = new RotaryKnobLabel();
        l->setJustificationType (juce::Justification::centred);
        l->setKeyboardType (juce::TextInputTarget::decimalKeyboard);
        l->setColour (juce::Label::textColourId, slider.findColour (juce::Slider::textBoxTextColourId));
        l->setColour (juce::TextEditor::textColourId, Colors::Knob::value);
        l->setColour (juce::TextEditor::highlightedTextColourId, Colors::Knob::value);
        l->setColour (juce::TextEditor::highlightColourId, slider.findColour (juce::Slider::rotarySliderFillColourId));
        l->setColour (juce::TextEditor::backgroundColourId, Colors::Knob::textBoxBackground);

        return l;
    }

    void drawTextEditorOutline (juce::Graphics&, int, int, juce::TextEditor&) override {}

    void fillTextEditorBackground (juce::Graphics& g, [[maybe_unused]] int width, [[maybe_unused]] int height, juce::TextEditor& textEditor) override
    {
        g.setColour (Colors::Knob::textBoxBackground);
        g.fillRoundedRectangle (textEditor.getLocalBounds().reduced (4, 0).toFloat(), 4.f);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnobLookAndFeel)
};

//==============================================================================
/*
*/
class GainSlider  : public juce::Component, public juce::LookAndFeel_V4
{
public:
    GainSlider(const juce::String& text,
                     juce::AudioProcessorValueTreeState& apvts,
                     const juce::ParameterID& parameterID,
                     const juce::String& suffix,
                     const juce::Slider::TextEntryBoxPosition textBoxPosition);
    ~GainSlider() override;

    void paint (juce::Graphics&) override;
    void resized() override;
        
    juce::Slider slider;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

private:
//    const float pi = juce::MathConstants<float>::pi;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainSlider)
};
