#pragma once
#include "AmpSimProcessor.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_formats/juce_audio_formats.h" // <-- 1. Include audio formats

class MainComponent : public juce::AudioAppComponent,
                      public juce::Slider::Listener,
                      public juce::Button::Listener  // <-- 2. Inherit from Button::Listener
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
    void buttonClicked(juce::Button* button) override; // <-- 3. Declare button handler

private:
    // ... existing amp, sliders, and labels
    AmpSimProcessor amp;

    juce::Slider gainSlider, toneSlider, volumeSlider;
    juce::Label gainLabel, toneLabel, volumeLabel;

    // --- 4. Add new members for file playback ---
    juce::TextButton openButton;
    juce::ToggleButton fileInputToggle;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::FileChooser> fileChooser;


    void openFile(); // <-- 5. Add a helper function to open the file

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};