#include "MainComponent.h"

MainComponent::MainComponent()
{
    gainLabel.setText("Gain", juce::dontSendNotification);
    toneLabel.setText("Tone", juce::dontSendNotification);
    volumeLabel.setText("Volume", juce::dontSendNotification);

    addAndMakeVisible(gainLabel);
    addAndMakeVisible(toneLabel);
    addAndMakeVisible(volumeLabel);

    // Amp Sliders
    gainSlider.setRange(0.0, 10.0, 0.01);
    gainSlider.setValue(2.0);
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);

    toneSlider.setRange(0.0, 1.0, 0.001);
    toneSlider.setValue(0.5);
    toneSlider.addListener(this);
    addAndMakeVisible(toneSlider);

    volumeSlider.setRange(0.0, 1.0, 0.001);
    volumeSlider.setValue(0.8);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // --- Audio Playback ---
    addAndMakeVisible(openButton);
    openButton.setButtonText("Open Audio File...");
    openButton.addListener(this);

    addAndMakeVisible(fileInputToggle);
    fileInputToggle.setButtonText("Use File Input");
    fileInputToggle.addListener(this);

    addAndMakeVisible(muteButton);
    muteButton.setButtonText("PANIC");
    muteButton.setClickingTogglesState(true);
    muteButton.setToggleState(isMuted.load(), juce::dontSendNotification);
    muteButton.addListener(this);

    formatManager.registerBasicFormats();

    setSize(400, 300);
    setAudioChannels(1, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    amp.prepare(sampleRate);
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (fileInputToggle.getToggleState() && transportSource.isPlaying()) {
        transportSource.getNextAudioBlock(bufferToFill);
    } else {
        // Get the single mono input channel
        const float* inputData = bufferToFill.buffer->getReadPointer(0);

        // Get the two stereo output channels
        float* outputDataLeft = bufferToFill.buffer->getWritePointer(0);
        float* outputDataRight = bufferToFill.buffer->getWritePointer(1);

        // Copy the mono input to both left and right output channels
        for (int i = 0; i < bufferToFill.numSamples; ++i) {
            outputDataLeft[i] = inputData[i];
            outputDataRight[i] = inputData[i];
        }
    }

    amp.process(*bufferToFill.buffer);

    if (isMuted.load()) {
        bufferToFill.buffer->clear();
    }
}

void MainComponent::releaseResources() 
{
  transportSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(20);

    // --- General UI ---
    auto buttonArea = area.removeFromTop(40);
    openButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 3).reduced(5));
    fileInputToggle.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(5));
    muteButton.setBounds(buttonArea.reduced(5));
    area.removeFromTop(20);

    // --- Amp UI ---
    auto sliderHeight = 40;
    gainLabel.setBounds(area.removeFromTop(sliderHeight / 2));
    gainSlider.setBounds(area.removeFromTop(sliderHeight));
    toneLabel.setBounds(area.removeFromTop(sliderHeight / 2));
    toneSlider.setBounds(area.removeFromTop(sliderHeight));
    volumeLabel.setBounds(area.removeFromTop(sliderHeight / 2));
    volumeSlider.setBounds(area.removeFromTop(sliderHeight));
}

void MainComponent::sliderValueChanged(juce::Slider* slider) // TODO. I think it'll make more sense to change with some type of helper function. Pass in an object and then have the slider be a property of the object
{
    if (slider == &gainSlider)
        amp.setGain((float)slider->getValue());
    else if (slider == &toneSlider)
        amp.setTone((float)slider->getValue());
    else if (slider == &volumeSlider)
        amp.setVolume((float)slider->getValue());
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &openButton) {
        openFile();
    }
    else if (button == &fileInputToggle) {
        if (fileInputToggle.getToggleState()) {
            transportSource.start();
        } else {
            transportSource.stop();
        }
    } else if (button == &muteButton) {
        isMuted.store(muteButton.getToggleState());
    }
}

void MainComponent::openFile()
{
    // Stop any current playback
    transportSource.stop();

    // Reset and launch the file chooser
    fileChooser = std::make_unique<juce::FileChooser>("Select an audio file to play...",
                                                      juce::File{},
                                                      "*.wav;*.aif;*.aiff;*.flac");

    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(flags, [this](const juce::FileChooser& fc)
    {
        // This code block runs *after* the user closes the file dialog
        auto file = fc.getResult();

        if (file != juce::File{}) // Check if the user actually selected a file
        {
            auto* reader = formatManager.createReaderFor(file);

            if (reader != nullptr)
            {
                // Create a new reader source
                auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                
                // Set the transport source to use this new reader
                transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                
                // Enable looping
                transportSource.setLooping(true);
                
                // Store the reader source
                readerSource.reset(newSource.release());

                // Auto-enable and start playback
                fileInputToggle.setToggleState(true, juce::dontSendNotification);
                transportSource.start();
            }
        }
        
        // When this lambda finishes, the fileChooser unique_ptr is still held
        // by MainComponent, but the native window will be gone.
        // It will be reset the next time openFile() is called.
    });
}
