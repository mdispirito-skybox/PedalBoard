#pragma once
#include "juce_gui_basics/juce_gui_basics.h"

class PedalComponent : public juce::Component
{
public:
    PedalComponent(juce::String name, juce::Colour color) 
        : pedalName(name), pedalColor(color) {}

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        
        // 1. Body
        g.setColour(pedalColor);
        g.fillRoundedRectangle(bounds, 12.0f);
        
        // 2. Bevel
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, 12.0f, 2.0f);
        
        // 3. Gradient Texture
        juce::ColourGradient grad(juce::Colours::white.withAlpha(0.15f), 0, 0,
                                  juce::Colours::black.withAlpha(0.1f), 0, (float)getHeight(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(bounds, 12.0f);

        // 4. NAME LABEL (Moved Down)
        // Position: Just above the footswitch area (approx 70% down the pedal)
        auto textArea = bounds.removeFromBottom(100).removeFromTop(40); 
        
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.setFont(juce::Font(22.0f, juce::Font::bold)); // Bigger, bolder font
        g.drawText(pedalName, textArea, juce::Justification::centred, true);
    }

private:
    juce::String pedalName;
    juce::Colour pedalColor;
};