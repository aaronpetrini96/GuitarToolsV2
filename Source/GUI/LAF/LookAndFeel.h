#pragma once

#include <JuceHeader.h>

namespace Colors
{
    const juce::Colour background {245, 240, 235};
    const juce::Colour header {40, 40, 40};
    namespace Knob
    {
//        const juce::Colour trackBackground {205, 200, 195};
        const juce::Colour trackBackground {juce::Colours::white.withAlpha(0.5f)};
//        const juce::Colour trackActive {177, 101, 135};
        const juce::Colour trackActive {juce::Colours::red.darker(0.2f)}; //rojo
//        const juce::Colour trackActive {juce::Colour(242, 130, 177)}; //rosa
        const juce::Colour outline {255, 250, 245};
        const juce::Colour gradientTop {250, 245, 240};
        const juce::Colour gradientBottom {240, 235, 230};
//        const juce::Colour dial {100, 100, 100};
        const juce::Colour dial {juce::Colours::black};
//        const juce::Colour dropShadow {195, 190, 185};
        const juce::Colour dropShadow {15, 15, 15};
//        const juce::Colour label {80, 80, 80};
        const juce::Colour label {juce::Colours::white};
        const juce::Colour textBoxBackground {80, 80, 80};
        const juce::Colour value {240, 240, 240};
        const juce::Colour caret {255, 255, 255};
    }
    
    namespace Group
    {
//        const juce::Colour label {160,155, 150};
//        const juce::Colour label {0, 0, 0};
        const juce::Colour label {juce::Colours::white};
//        const juce::Colour outline {235, 230, 255};
        const juce::Colour outline {juce::Colours::white.withAlpha(0.5f)};
    }

    namespace Button
    {
        const juce::Colour text {80, 80, 80};
        const juce::Colour textToggled {40, 40, 40};
        const juce::Colour background {245, 240, 235};
        const juce::Colour backgroundToggled {255, 250, 245};
        const juce::Colour outline {235, 230, 255};
    }

    namespace LevelMeter
    {
        const juce::Colour background {245, 240, 235};
        const juce::Colour tickLine {200, 200, 200};
        const juce::Colour tickLabel {80, 80, 80};
        const juce::Colour tooLoud {226, 74, 81};
        const juce::Colour levelOK {65, 206, 88};
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
    Fonts() = delete; //can;t create intances of this classs
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
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;
    
    juce::Font getLabelFont (juce::Label&) override;
    
    juce::Label* createSliderTextBox (juce::Slider&) override;
    
    void drawTextEditorOutline(juce::Graphics&, int, int, juce::TextEditor&) override {}
    void fillTextEditorBackground(juce::Graphics&, int width, int height, juce::TextEditor&) override;
    
private:
    juce::DropShadow dropShadow {Colors::Knob::dropShadow, 6, {0,3}};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnobLookAndFeel)
};

class MainLookAndFeel : public juce::LookAndFeel_V4 //HANDLES everything but knobs
{
public:
    MainLookAndFeel();
    
    juce::Font getLabelFont(juce::Label&) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLookAndFeel)
};

class ButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ButtonLookAndFeel ();
    
    static ButtonLookAndFeel* get()
    {
        static ButtonLookAndFeel instance;
        return &instance;
    }
    
    virtual void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
    
    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            // Draw text only, no tick box
            auto fontSize = juce::jmin (15.0f, button.getHeight() * 0.75f);
            g.setFont (fontSize);

            g.setColour (button.findColour (juce::ToggleButton::textColourId));
            g.drawFittedText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
        }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonLookAndFeel)
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
