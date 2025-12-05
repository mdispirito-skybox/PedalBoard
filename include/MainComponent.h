#pragma once
#include "GuitarRigEngine.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"

class LevelMeter : public juce::Component {
public:
    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::black);
        int height = (int)((float)getHeight() * level);
        if (level > 0.9f) {
            g.setColour(juce::Colours::red);
        } else {
            g.setColour(juce::Colours::green);
        }
        
        g.fillRect(0, getHeight() - height, getWidth(), height);
    }

    void setLevel(float newLevel) {
        if (newLevel > level) {
            level = newLevel; 
        } else {
            level *= 0.9f;
        } 
        repaint();
    }

private:
    float level = 0.0f;
};
class MainComponent : public juce::AudioAppComponent,
                      public juce::Slider::Listener,
                      public juce::Button::Listener,
                      public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void timerCallback() override;

private:
    GuitarRigEngine rigEngine;

    // --- Amp UI Elements ---
    juce::Slider gainSlider, bassSlider, trebleSlider, volumeSlider;
    juce::Label gainLabel, bassLabel, trebleLabel, volumeLabel;
    juce::TextButton loadIRButton;
    juce::ToggleButton cabToggle;
    juce::ComboBox irSelector;
    juce::TextButton openButton;

    // --- Fuzz UI ---
    juce::Slider fuzzSustainSlider, fuzzToneSlider, fuzzVolumeSlider;
    juce::Label  fuzzLabel;
    juce::Label fuzzSustainLabel, fuzzToneLabel, fuzzVolumeLabel;
    juce::ToggleButton fuzzBypassButton;




    // --- General UI Elements ---
    juce::TextButton settingsButton;
    juce::ToggleButton fileInputToggle;
    juce::ToggleButton muteButton;
    
    LevelMeter inputMeter, outputMeter;
    std::atomic<float> currentInputLevel { 0.0f };
    std::atomic<float> currentOutputLevel { 0.0f };

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    void openFile();
    void loadIR();
    void loadEmbeddedIR(int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};