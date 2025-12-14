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
        
        // Body
        g.setColour(pedalColor);
        g.fillRoundedRectangle(bounds, 12.0f);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, 12.0f, 2.0f);
        
        // Gradient
        juce::ColourGradient grad(juce::Colours::white.withAlpha(0.15f), 0, 0,
                                  juce::Colours::black.withAlpha(0.1f), 0, (float)getHeight(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(bounds, 12.0f);

        // NAME LABEL
        // Moved up to Y approx -120 from bottom to avoid switch overlap
        auto textArea = bounds.removeFromBottom(120).removeFromTop(40); 
        
        g.setColour(juce::Colours::white.withAlpha(0.95f));
        g.setFont(juce::Font(20.0f, juce::Font::bold));
        g.drawText(pedalName, textArea, juce::Justification::centred, true);
    }

private:
    juce::String pedalName;
    juce::Colour pedalColor;
};