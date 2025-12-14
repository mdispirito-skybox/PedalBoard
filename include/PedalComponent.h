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
        
        // 1. Body
        g.setColour(pedalColor);
        g.fillRoundedRectangle(bounds, 12.0f);
        
        // 2. Bevel
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, 12.0f, 2.0f);
        
        // 3. Gradient
        juce::ColourGradient grad(juce::Colours::white.withAlpha(0.15f), 0, 0,
                                  juce::Colours::black.withAlpha(0.1f), 0, (float)getHeight(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(bounds, 12.0f);

        // 4. NAME LABEL (Updated for 2 lines)
        // Moved up to clear switch
        auto textArea = bounds.removeFromBottom(130).removeFromTop(50); 
        
        g.setColour(juce::Colours::white.withAlpha(0.95f));
        g.setFont(juce::Font(22.0f, juce::Font::bold));
        
        // CHANGED: drawFittedText allows wrapping on newlines
        g.drawFittedText(pedalName, textArea.toNearestInt(), juce::Justification::centred, 2);
    }

private:
    juce::String pedalName;
    juce::Colour pedalColor;
};