#pragma once
#include "juce_gui_basics/juce_gui_basics.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour(juce::Label::textColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::ComboBox::backgroundColourId, juce::Colours::black.brighter(0.1f));
        setColour(juce::ComboBox::outlineColourId, juce::Colours::grey);
        setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }

    // --- 1. VECTOR PEDAL KNOBS ---
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 2.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Body
        g.setColour(juce::Colours::black.brighter(0.1f));
        g.fillEllipse(rx, ry, rw, rw);
        g.setColour(juce::Colours::grey);
        g.drawEllipse(rx, ry, rw, rw, 2.0f);

        // Pointer
        juce::Path p;
        auto pointerLength = radius * 0.7f;
        auto pointerThickness = 3.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }

    // --- 2. DRAW FOOTSWITCHES (Fixed) ---
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        bool isOn = button.getToggleState();
        
        float midX = bounds.getCentreX();
        
        // Dynamic Layout Calculation
        float height = bounds.getHeight();
        float ledSize = 10.0f;
        float switchSize = juce::jmin(30.0f, height * 0.5f); // Prevent switch from getting too big
        
        // Position LED at top 20%, Switch at bottom 60%
        float ledY = bounds.getY() + (height * 0.15f);
        float switchY = bounds.getBottom() - switchSize - (height * 0.1f);

        // A. THE LED
        juce::Colour ledCol = isOn ? juce::Colours::red : juce::Colours::darkred.darker(0.5f);
        g.setColour(ledCol);
        g.fillEllipse(midX - ledSize/2, ledY, ledSize, ledSize);
        
        if (isOn) {
            g.setColour(ledCol.withAlpha(0.4f));
            g.fillEllipse(midX - ledSize, ledY - ledSize/2, ledSize*2, ledSize*2);
            g.setColour(juce::Colours::white.withAlpha(0.9f));
            g.fillEllipse(midX - 2, ledY + 2, 4, 4);
        }

        // B. THE METAL SWITCH
        auto switchRect = juce::Rectangle<float>(midX - switchSize/2, switchY, switchSize, switchSize);

        // Body
        g.setColour(juce::Colours::silver.darker(0.1f));
        g.fillEllipse(switchRect);
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawEllipse(switchRect, 1.0f);

        // Plunger
        auto innerRect = switchRect.reduced(3);
        if (shouldDrawButtonAsDown) innerRect = innerRect.reduced(2); 

        juce::ColourGradient grad(juce::Colours::white, midX, innerRect.getY(),
                                  juce::Colours::grey, midX, innerRect.getBottom(), false);
        g.setGradientFill(grad);
        g.fillEllipse(innerRect);
    }
    
    // --- 3. HORIZONTAL METERS ---
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colours::black.brighter(0.2f));
        g.fillRect(x, y, width, height);
        
        float fillWidth = sliderPos - (float)x;
        if (fillWidth > 0) {
            juce::ColourGradient grad(juce::Colours::green, (float)x, (float)y,
                                      juce::Colours::red, (float)width, (float)y, false);
            g.setGradientFill(grad);
            g.fillRect((float)x, (float)y, fillWidth, (float)height);
        }
        g.setColour(juce::Colours::grey.darker());
        g.drawRect(x, y, width, height);
    }
};