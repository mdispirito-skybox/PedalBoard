#pragma once
#include "juce_gui_basics/juce_gui_basics.h"

class PedalComponent : public juce::Component
{
public:
    PedalComponent(juce::String name, juce::Colour color) 
        : pedalName(name), pedalColor(color) {}

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // 1. Body Color
        g.setColour(pedalColor);
        g.fillRoundedRectangle(bounds, 12.0f);
        
        // 2. Subtle Bevel / Edge
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, 12.0f, 2.0f);

        // 3. Name Label (Top)
        // Draw centered at the top
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(18.0f, juce::Font::bold));
        g.drawText(pedalName, 0, 15, getWidth(), 20, juce::Justification::centred);
        
        // 4. Subtle Texture Gradient (Top to Bottom)
        // Makes it look metallic rather than flat plastic
        juce::ColourGradient grad(juce::Colours::white.withAlpha(0.15f), 0, 0,
                                  juce::Colours::black.withAlpha(0.1f), 0, (float)getHeight(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(bounds, 12.0f);
    }

private:
    juce::String pedalName;
    juce::Colour pedalColor;
};