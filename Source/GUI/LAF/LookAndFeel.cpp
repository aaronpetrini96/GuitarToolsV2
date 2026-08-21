#include "LookAndFeel.h"

const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::LatoMedium_ttf, BinaryData::LatoMedium_ttfSize);

juce::Font Fonts::getFont(float height)
{
    return juce::Font(typeface).withHeight(height);
}
/* For juce 8 onwards
 juce::Font Fonts::getFont(float height)
 {
     return juce::FontOptions(typeface).withMetricsKind(juce::TypefaceMetricsKind::legacy).withHeight(height);
 }
 
 */

RotaryKnobLookAndFeel::RotaryKnobLookAndFeel()
{
    setColour(juce::Label::textColourId, Colors::Knob::label);
    setColour(juce::Slider::textBoxTextColourId, Colors::Knob::label);
    setColour(juce::Slider::rotarySliderFillColourId, Colors::Knob::trackActive);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::CaretComponent::caretColourId, Colors::Knob::caret);
}

juce::Font RotaryKnobLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

void RotaryKnobLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, width).toFloat();
    auto knobRect = bounds.reduced(10.f, 10.f);
    
    auto path = juce::Path();
    path.addEllipse(knobRect);
    dropShadow.drawForPath(g, path);
    
    g.setColour(Colors::Knob::outline);
    g.fillEllipse(knobRect);
    
    auto innerRect = knobRect.reduced(2.f, 2.f);
    auto gradient = juce::ColourGradient(Colors::Knob::gradientTop, 0.f, innerRect.getY(),
                                         Colors::Knob::gradientBottom, 0.f, innerRect.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(innerRect);
    
    auto center = bounds.getCentre();
    auto radius = bounds.getWidth()/2.f;
    auto lineWidth = 3.f;
    auto arcRadius = radius - lineWidth/2.f;
    
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.f, rotaryStartAngle, rotaryEndAngle, true);
    auto strokeType = juce::PathStrokeType(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.setColour(Colors::Knob::trackBackground);
    g.strokePath(backgroundArc, strokeType);
    
    auto dialRadius = innerRect.getHeight() / 2.f - lineWidth;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    juce::Point<float> dialStart (center.x + 10.f * std::sin(toAngle), center.y - 10 * std::cos(toAngle));
    juce::Point<float> dialEnd(center.x + dialRadius * std::sin(toAngle),
                               center.y - dialRadius*std::cos(toAngle));
    juce::Path dialPath; //creating path for line between dial start and end points
    dialPath.startNewSubPath(dialStart);
    dialPath.lineTo(dialEnd);
    g.setColour(Colors::Knob::dial);
    g.strokePath(dialPath, strokeType);
    
    //Adding the fill as the knob moves
    if (slider.isEnabled())
    {
        float fromAngle = rotaryStartAngle;
        if (slider.getProperties()["drawFromMiddle"])
            fromAngle += (rotaryEndAngle - rotaryStartAngle)/2.0f;
        juce::Path valueArc;
        valueArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.f, fromAngle, toAngle,true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(valueArc, strokeType);
    }
}

void RotaryKnobLookAndFeel::fillTextEditorBackground(juce::Graphics& g,[[maybe_unused]] int width, [[maybe_unused]] int height, juce::TextEditor& textEditor)
{
    g.setColour(Colors::Knob::textBoxBackground);
    g.fillRoundedRectangle(textEditor.getLocalBounds().reduced(4, 0).toFloat(), 4.f);
}

class RotaryKnobLabel : public juce::Label
{
public:
    RotaryKnobLabel() : juce::Label() {}
    
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override {}
    
    std::unique_ptr<juce::AccessibilityHandler>createAccessibilityHandler() override
    {
        return createIgnoredAccessibilityHandler(*this);
    }
    
    juce::TextEditor* createEditorComponent() override
    {
        auto* ed = new juce::TextEditor(getName());
        ed-> applyFontToAllText(getLookAndFeel().getLabelFont(*this));
        copyAllExplicitColoursTo(*ed);
        
        ed->setBorder(juce::BorderSize<int>());
        ed->setIndents(2, 1);
        ed->setJustification(juce::Justification::centredTop);
        ed->setPopupMenuEnabled(false);
        ed->setInputRestrictions(8);
        
        return ed;
    }
};

juce::Label* RotaryKnobLookAndFeel::createSliderTextBox(juce::Slider & slider)
{
    auto l = new RotaryKnobLabel();
    l->setJustificationType(juce::Justification::centred);
    l->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
    l->setColour(juce::Label::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
    l->setColour(juce::TextEditor::textColourId, Colors::Knob::value);
    l->setColour(juce::TextEditor::highlightedTextColourId, Colors::Knob::value);
    l->setColour(juce::TextEditor::highlightColourId, slider.findColour(juce::Slider::rotarySliderFillColourId));
    l->setColour(juce::TextEditor::backgroundColourId, Colors::Knob::textBoxBackground);
    
    return l;
}


MainLookAndFeel::MainLookAndFeel()
{
    setColour(juce::GroupComponent::textColourId, Colors::Group::label);
    setColour(juce::GroupComponent::outlineColourId, Colors::Group::outline);
}

juce::Font MainLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

ButtonLookAndFeel::ButtonLookAndFeel()
{
    setColour(juce::TextButton::textColourOffId, Colors::Button::text);
    setColour(juce::TextButton::textColourOnId, Colors::Button::textToggled);
    setColour(juce::TextButton::buttonColourId, Colors::Button::background);
    setColour(juce::TextButton::buttonOnColourId, juce::Colours::lightyellow);
    
}

void ButtonLookAndFeel::drawButtonBackground(juce::Graphics &g, juce::Button &button, const juce::Colour &backgroundColour,
                                             [[maybe_unused]] bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto cornersize = bounds.getHeight() * 0.25f;
    auto buttonRect = bounds.reduced(1.f, 1.f).withTrimmedBottom(1.f);


    if (shouldDrawButtonAsDown)
        buttonRect.translate(0.f, 1.f);

    
    // Background color 
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(buttonRect, cornersize);
    
    // Button border
    g.setColour(juce::Colour(169, 105, 134));
    g.drawRoundedRectangle(buttonRect, cornersize, 2.f);
}

void ButtonLookAndFeel::drawButtonText(juce::Graphics &g, juce::TextButton &button, [[maybe_unused]] bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto buttonRect = bounds.reduced(1.f, 1.f).withTrimmedBottom(1.f);
    
    // Ignore hover effect; only account for "down" state
    if (shouldDrawButtonAsDown)
        buttonRect.translate(0.f, 1.f);
    
    // Set text color based on toggle state
    if (button.getToggleState())
    {
        g.setColour(button.findColour(juce::TextButton::textColourOnId));
    }
    else
    {
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
    }
    
    g.setFont(Fonts::getFont());
    g.drawText(button.getButtonText(), buttonRect, juce::Justification::centred);
}

ComboBoxLookAndFeel::ComboBoxLookAndFeel()
{
//    setColour(juce::ComboBox::ColourIds::arrowColourId, juce::Colours::lightgrey);
    setColour(juce::ComboBox::ColourIds::backgroundColourId, Colors::OptionBox::backgroundColour);
    setColour(juce::ComboBox::ColourIds::textColourId, Colors::OptionBox::textColour);
    setColour(juce::ComboBox::outlineColourId, Colors::OptionBox::outlineColour);
}

void ComboBoxLookAndFeel::drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &box)
{
    auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    juce::Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
//    g.setColour (juce::Colour(138, 138, 138));
    g.drawRoundedRectangle (boxBounds.toFloat().reduced (1.f, 1.f), cornerSize, 2.f);

    
}

juce::Font ComboBoxLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

void ComboBoxLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (0, 0,
                     box.getWidth(),
                     box.getHeight() - 3);

    label.setFont (getComboBoxFont (box));
    label.setJustificationType(juce::Justification::centred);
}


