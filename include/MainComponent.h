#pragma once
#include "GuitarRigEngine.h"
#include "StyleSheet.h"
#include "PedalComponent.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"

class LevelMeter : public juce::Component {
public:
    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::black.brighter(0.1f));
        int width = (int)((float)getWidth() * level);
        if (level > 0.9f) g.setColour(juce::Colours::red);
        else g.setColour(juce::Colours::green);
        g.fillRect(0, 0, width, getHeight());
        g.setColour(juce::Colours::grey.darker());
        g.drawRect(getLocalBounds());
    }
    void setLevel(float newLevel) {
        if (newLevel > level) level = newLevel; else level *= 0.9f; 
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
    CustomLookAndFeel styleSheet;

    // Pedal Backgrounds
    PedalComponent fuzzPedal   { "WOOLLY\nMAMMOTH", juce::Colours::darkred.darker(0.1f) };
    PedalComponent chorusPedal { "SEA\nHORSE",      juce::Colours::skyblue.darker(0.5f) };
    PedalComponent delayPedal  { "GECKO\nECHO",     juce::Colours::olive.darker(0.1f) };

    // --- Amp UI ---
    juce::Slider gainSlider, bassSlider, trebleSlider, volumeSlider;
    juce::Label gainLabel, bassLabel, trebleLabel, volumeLabel;
    juce::TextButton loadIRButton;
    juce::ToggleButton cabToggle;
    juce::ComboBox irSelector;
    
    // --- Top Bar Controls ---
    // CHANGED: All buttons are now TextButtons for clear labels
    juce::TextButton openButton;
    juce::TextButton settingsButton;
    juce::TextButton fileInputToggle; // Changed from ToggleButton
    juce::TextButton muteButton;      // Changed from ToggleButton

    // --- Fuzz UI ---
    juce::Slider fuzzSustainSlider, fuzzToneSlider, fuzzVolumeSlider;
    juce::Label  fuzzLabel;
    juce::Label fuzzSustainLabel, fuzzToneLabel, fuzzVolumeLabel;
    juce::ToggleButton fuzzBypassButton;

    // --- Chorus UI ---
    juce::Label chorusLabel, chorusRateLabel, chorusDepthLabel; 
    juce::Slider chorusRateSlider, chorusDepthSlider;
    juce::ToggleButton chorusBypassButton;

    // --- Delay UI ---
    juce::Label delayLabel, delayTimeLabel, delayFeedbackLabel, delayMixLabel;
    juce::Slider delayTimeSlider, delayFeedbackSlider, delayMixSlider;
    juce::ToggleButton delayBypassButton;

    // --- Meters ---
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

    void setupChorus();
    void setupFuzz();
    void setupDelay();
    void setupAmp(); 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};