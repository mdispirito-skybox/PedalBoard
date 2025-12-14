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

        // Knob Body (Matte Black)
        g.setColour(juce::Colours::black.brighter(0.1f));
        g.fillEllipse(rx, ry, rw, rw);
        
        // Metallic Rim
        g.setColour(juce::Colours::grey);
        g.drawEllipse(rx, ry, rw, rw, 2.0f);

        // Pointer (White High-Vis Line)
        juce::Path p;
        auto pointerLength = radius * 0.7f;
        auto pointerThickness = 3.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }

    // --- 2. METAL FOOTSWITCHES + LEDs ---
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        bool isOn = button.getToggleState();

        // A. THE LED (Top Center)
        float ledSize = 12.0f;
        float ledX = bounds.getCentreX() - (ledSize / 2.0f);
        float ledY = bounds.getY() + 2.0f;

        // Color Logic: Bright Red if ON, Dim Dark Red if OFF
        juce::Colour ledCol = isOn ? juce::Colours::red : juce::Colours::darkred.darker(0.5f);
        
        g.setColour(ledCol);
        g.fillEllipse(ledX, ledY, ledSize, ledSize);
        
        // Glow Effect (if ON)
        if (isOn) {
            g.setColour(ledCol.withAlpha(0.4f));
            g.fillEllipse(ledX - 4, ledY - 4, ledSize + 8, ledSize + 8);
            // White hot center
            g.setColour(juce::Colours::white.withAlpha(0.8f));
            g.fillEllipse(ledX + 3, ledY + 3, 4, 4);
        }

        // B. THE METAL SWITCH (Below LED)
        float switchSize = 35.0f;
        auto switchRect = bounds.withSizeKeepingCentre(switchSize, switchSize).translated(0, 10);

        // Silver Body
        g.setColour(juce::Colours::silver.darker(0.1f));
        g.fillEllipse(switchRect);
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawEllipse(switchRect, 1.0f);

        // Inner Plunger (Chrome Gradient)
        auto innerRect = switchRect.reduced(3);
        juce::ColourGradient grad(juce::Colours::white, innerRect.getCentreX(), innerRect.getY(),
                                  juce::Colours::grey, innerRect.getCentreX(), innerRect.getBottom(), false);
        g.setGradientFill(grad);
        g.fillEllipse(innerRect);
    }
};