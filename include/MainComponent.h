#pragma once
#include "AmpSimProcessor.h"
#include "CabSimProcessor.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::Slider::Listener,
                      public juce::Button::Listener
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

private:
    AmpSimProcessor amp;
    CabSimProcessor cab;

    // --- Amp Parameters ---
    juce::Slider gainSlider, bassSlider, trebleSlider, volumeSlider;
    juce::Label gainLabel, bassLabel, trebleLabel, volumeLabel;
    
    // --- Cab Parameters ---
    juce::TextButton loadIRButton;
    juce::ToggleButton cabToggle;
    juce::ComboBox irSelector;

    // --- General UI  ---
    juce::TextButton openButton;
    juce::TextButton settingsButton;
    juce::ToggleButton fileInputToggle;
    juce::ToggleButton muteButton;

    // --- General Audio Management ---
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::FileChooser> fileChooser;
    std::atomic<bool> isMuted { false };

    void openFile();
    void loadIR();
    void loadEmbeddedIR(int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};