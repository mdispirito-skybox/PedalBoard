#include "MainComponent.h"
#include "BinaryData.h"

MainComponent::MainComponent() {
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

    // --- Cab UI ---
    addAndMakeVisible(loadIRButton);
    loadIRButton.setButtonText("Load IR File...");
    loadIRButton.addListener(this);

    addAndMakeVisible(cabToggle);
    cabToggle.setButtonText("Cab Sim ON");
    cabToggle.setClickingTogglesState(true);
    cabToggle.setToggleState(true, juce::dontSendNotification); // Start ON by default
    cabToggle.addListener(this);

    addAndMakeVisible(irSelector);
    irSelector.addItem("ORANGE", 1);
    irSelector.addItem("VOX", 2);
    irSelector.onChange = [this] { loadEmbeddedIR(irSelector.getSelectedId()); };
    
    irSelector.setSelectedId(2);

    // --- Audio Playback ---
    addAndMakeVisible(openButton);
    openButton.setButtonText("Open Audio File...");
    openButton.addListener(this);

    addAndMakeVisible(fileInputToggle);
    fileInputToggle.setButtonText("Use File Input");
    fileInputToggle.addListener(this);

    addAndMakeVisible(muteButton);
    muteButton.setButtonText("MUTE");
    muteButton.setClickingTogglesState(true);
    muteButton.setToggleState(isMuted.load(), juce::dontSendNotification);
    muteButton.addListener(this);

    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("Audio Settings...");
    settingsButton.addListener(this);

    formatManager.registerBasicFormats();

    setSize(400, 380);
    setAudioChannels(1, 2);
}

MainComponent::~MainComponent(){
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    amp.prepare(sampleRate);
    cab.prepare(sampleRate, samplesPerBlockExpected, 2);
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    if (isMuted.load()) {
        bufferToFill.buffer->clear();
        return;
    }

    if (fileInputToggle.getToggleState() && transportSource.isPlaying()) {
        transportSource.getNextAudioBlock(bufferToFill);
    } else {
        const float* inputData = bufferToFill.buffer->getReadPointer(0);
        float* outputDataLeft = bufferToFill.buffer->getWritePointer(0);
        float* outputDataRight = bufferToFill.buffer->getWritePointer(1);

        for (int i = 0; i < bufferToFill.numSamples; ++i) {
            outputDataLeft[i] = inputData[i];
            outputDataRight[i] = inputData[i];
        }
    }

    amp.process(*bufferToFill.buffer);
    cab.process(*bufferToFill.buffer);
}

void MainComponent::releaseResources() {
  transportSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized() {
    auto area = getLocalBounds().reduced(20);
    
    // --- Row 1: File/Mute controls ---
    auto topButtonArea = area.removeFromTop(40);
    openButton.setBounds(topButtonArea.removeFromLeft(topButtonArea.getWidth() / 3).reduced(5));
    fileInputToggle.setBounds(topButtonArea.removeFromLeft(topButtonArea.getWidth() / 2).reduced(5));
    muteButton.setBounds(topButtonArea.reduced(5));
    area.removeFromTop(5);

    // --- Row 2: Cab Sim Controls ---
    auto cabArea = area.removeFromTop(40);
    loadIRButton.setBounds(cabArea.removeFromLeft(cabArea.getWidth() / 3).reduced(5));
    irSelector.setBounds(cabArea.removeFromLeft(cabArea.getWidth() / 2).reduced(5));
    cabToggle.setBounds(cabArea.reduced(5));

    // --- Row 3: Settings ---
    auto settingsArea = area.removeFromTop(30);
    settingsButton.setBounds(settingsArea.withWidth(140).withCentre(settingsArea.getCentre()));
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

void MainComponent::buttonClicked(juce::Button* button) {
if (button == &openButton) {
        openFile();
    } else if (button == &fileInputToggle) {
        if (fileInputToggle.getToggleState())
            transportSource.start();
        else
            transportSource.stop();
    } else if (button == &muteButton) {
        isMuted.store(muteButton.getToggleState());
    } else if (button == &cabToggle) {
        cab.setBypassed(!cabToggle.getToggleState());
    } else if (button == &loadIRButton) {
        loadIR();
    } else if (button == &settingsButton) {
        auto* settingsPanel = new juce::AudioDeviceSelectorComponent(deviceManager, 0, 0, 0, 2, false, false, true, false);
        settingsPanel->setSize(400, 300);

        juce::DialogWindow::LaunchOptions o;
        o.content.setOwned(settingsPanel);
        o.dialogTitle = "Audio Settings";
        o.resizable = false;
        o.dialogBackgroundColour = juce::Colours::darkgrey;
        o.launchAsync();
    }
}

void MainComponent::openFile()
{
    transportSource.stop();
    fileChooser = std::make_unique<juce::FileChooser>("Select an audio file to play...",juce::File{},"*.wav;*.aif;*.aiff;*.flac");
    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    fileChooser->launchAsync(flags, [this](const juce::FileChooser& fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
            auto* reader = formatManager.createReaderFor(file);
            if (reader != nullptr) {
                auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                transportSource.setLooping(true);
                readerSource.reset(newSource.release());
                fileInputToggle.setToggleState(true, juce::dontSendNotification);
                transportSource.start();
            }
        }
    });
}

void MainComponent::loadIR() {
    transportSource.stop(); // TODO Why do we need to stop the audio while loading a new IR?
    fileChooser = std::make_unique<juce::FileChooser>("Select an Impulse Response (IR) File...", juce::File{}, "*.wav;*.aif;*.aiff");
    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(flags, [this](const juce::FileChooser& fc) {
        auto file = fc.getResult();
        if (file == juce::File{}) {
            return;
        }

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

        if (reader != nullptr) {
            juce::AudioBuffer<float> rawIRBuffer((int)reader->numChannels, (int)reader->lengthInSamples);
            reader->read(&rawIRBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
            cab.loadImpulseResponse(std::move(rawIRBuffer), reader->sampleRate);
            juce::Logger::writeToLog("Loaded IR: " + file.getFileName());
        }
    });
}

void MainComponent::loadEmbeddedIR(int index) {
    const char* data = nullptr;
    int dataSize = 0;

    switch (index) {
        case 1:
            data = BinaryData::VOX_wav;
            dataSize = BinaryData::VOX_wavSize;
            break;
        case 2:
            data = BinaryData::ORANGE_wav;
            dataSize = BinaryData::ORANGE_wavSize;
            break;
        default:
            return;
    }

    // Standard loading logic (same as before, but from memory stream)
    auto stream = std::make_unique<juce::MemoryInputStream>(data, dataSize, false);
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::move(stream)));

    if (reader != nullptr) {
        juce::AudioBuffer<float> rawIRBuffer((int)reader->numChannels, (int)reader->lengthInSamples);
        reader->read(&rawIRBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
        cab.loadImpulseResponse(std::move(rawIRBuffer), reader->sampleRate);

    }
}
