#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

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
    
    namespace Group
    {
        const juce::Colour label {juce::Colours::white};
        const juce::Colour outline {juce::Colours::white.withAlpha(0.5f)};
    }

    namespace OptionBox
    {
    const juce::Colour backgroundColour {juce::Colours::lightgrey.withAlpha(0.75f)};
    const juce::Colour textColour {juce::Colours::white};
    const juce::Colour outlineColour {juce::Colour(138, 138, 138)};
    }
}

class Fonts
{
public:
    Fonts() = delete;
    static juce::Font getFont(float height = 16.f);
    
private:
    static const juce::Typeface::Ptr typeface;
};

class RotaryKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RotaryKnobLookAndFeel();
    
    static RotaryKnobLookAndFeel* get()
    {
        static RotaryKnobLookAndFeel instance;
        return &instance;
    }
    
    juce::Font getLabelFont (juce::Label&) override;
    
    juce::Label* createSliderTextBox (juce::Slider&) override;
    
    void drawTextEditorOutline(juce::Graphics&, int, int, juce::TextEditor&) override {}
    void fillTextEditorBackground(juce::Graphics&, int width, int height, juce::TextEditor&) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnobLookAndFeel)
};

class MainLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MainLookAndFeel();
    
    juce::Font getLabelFont(juce::Label&) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLookAndFeel)
};

class ComboBoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ComboBoxLookAndFeel ();
    static ComboBoxLookAndFeel* get()
    {
        static ComboBoxLookAndFeel instance;
        return &instance;
    }
    
    virtual void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;
    
    virtual void positionComboBoxText (juce::ComboBox&, juce::Label&) override;
    
    juce::Font getLabelFont (juce::Label&) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxLookAndFeel)
};
