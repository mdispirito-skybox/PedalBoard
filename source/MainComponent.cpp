#include "MainComponent.h"
#include "BinaryData.h"

MainComponent::MainComponent() {
    gainLabel.setText("Gain", juce::dontSendNotification);
    bassLabel.setText("Bass", juce::dontSendNotification);
    trebleLabel.setText("Treble", juce::dontSendNotification);
    volumeLabel.setText("Volume", juce::dontSendNotification);

    addAndMakeVisible(gainLabel);
    addAndMakeVisible(bassLabel);
    addAndMakeVisible(trebleLabel);
    addAndMakeVisible(volumeLabel);

    // Amp Sliders
    gainSlider.setRange(0.0, 10.0, 0.01);
    gainSlider.addListener(this);
    gainSlider.setValue(1.0);
    addAndMakeVisible(gainSlider);

    bassSlider.setRange(0.0, 1.0, 0.001);
    bassSlider.addListener(this);
    bassSlider.setValue(0.5);
    addAndMakeVisible(bassSlider);

    trebleSlider.setRange(0.0, 1.0, 0.001);
    trebleSlider.addListener(this);
    trebleSlider.setValue(0.5);
    addAndMakeVisible(trebleSlider);

    volumeSlider.setRange(0.0, 1.0, 0.001);
    volumeSlider.addListener(this);
    volumeSlider.setValue(0.4);
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

    setupFuzz();
    setupChorus();

    // --- Meters ---
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);

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
    muteButton.setToggleState(true, juce::dontSendNotification);
    muteButton.addListener(this);

    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("Audio Settings...");
    settingsButton.addListener(this);

    formatManager.registerBasicFormats();

    setSize(500, 700);
    setAudioChannels(1, 2);
    startTimerHz(30);
}

MainComponent::~MainComponent(){
    stopTimer();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    rigEngine.prepare(sampleRate, samplesPerBlockExpected, 2);
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
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

    currentInputLevel.store(bufferToFill.buffer->getMagnitude(bufferToFill.startSample, bufferToFill.numSamples));
    rigEngine.process(*bufferToFill.buffer);
    currentOutputLevel.store(bufferToFill.buffer->getMagnitude(bufferToFill.startSample, bufferToFill.numSamples));
}

void MainComponent::releaseResources() {
  transportSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey); 
}

void MainComponent::timerCallback() {
    inputMeter.setLevel(currentInputLevel.load());
    outputMeter.setLevel(currentOutputLevel.load());
}

void MainComponent::resized() {
    auto area = getLocalBounds().reduced(20);

    // --- Side Meters ---
    inputMeter.setBounds(area.removeFromLeft(20));
    area.removeFromLeft(10);
    outputMeter.setBounds(area.removeFromRight(20));
    area.removeFromRight(10);
    
    // --- Row 1: Top Controls (File/Mute) ---
    auto topArea = area.removeFromTop(40);
    openButton.setBounds(topArea.removeFromLeft(topArea.getWidth() / 3).reduced(5));
    fileInputToggle.setBounds(topArea.removeFromLeft(topArea.getWidth() / 2).reduced(5));
    muteButton.setBounds(topArea.reduced(5));
    
    // --- Row 2: Cab Controls ---
    auto cabArea = area.removeFromTop(40);
    loadIRButton.setBounds(cabArea.removeFromLeft(cabArea.getWidth() / 3).reduced(5));
    irSelector.setBounds(cabArea.removeFromLeft(cabArea.getWidth() / 2).reduced(5));
    cabToggle.setBounds(cabArea.reduced(5));

    // --- Row 3: Settings ---
    auto settingsArea = area.removeFromTop(40);
    settingsButton.setBounds(settingsArea.withWidth(140).withCentre(settingsArea.getCentre()).reduced(5));

    area.removeFromTop(20); // Spacer

    // --- Row 4: FUZZ PEDAL (Moved Down) ---
    // Header
    auto fuzzHeader = area.removeFromTop(30);
    fuzzLabel.setBounds(fuzzHeader.removeFromLeft(100));
    fuzzBypassButton.setBounds(fuzzHeader.removeFromRight(80).reduced(2));
    
    // Sliders + Labels (Giving them 100px height now)
    auto fuzzControlArea = area.removeFromTop(100);
    int knobWidth = fuzzControlArea.getWidth() / 3;

    // Col 1: Sustain
    auto f1 = fuzzControlArea.removeFromLeft(knobWidth);
    fuzzSustainLabel.setBounds(f1.removeFromTop(20));
    fuzzSustainSlider.setBounds(f1.reduced(5));

    // Col 2: Tone
    auto f2 = fuzzControlArea.removeFromLeft(knobWidth);
    fuzzToneLabel.setBounds(f2.removeFromTop(20));
    fuzzToneSlider.setBounds(f2.reduced(5));

    // Col 3: Level
    auto f3 = fuzzControlArea; // remaining
    fuzzVolumeLabel.setBounds(f3.removeFromTop(20));
    fuzzVolumeSlider.setBounds(f3.reduced(5));

    area.removeFromTop(20); // Spacer

    // --- CHORUS ROW ---
    auto chorusArea = area.removeFromTop(100);
    
    auto chHeader = chorusArea.removeFromTop(20);
    chorusLabel.setBounds(chHeader.removeFromLeft(80));
    chorusBypassButton.setBounds(chHeader.removeFromRight(60));

    // Sliders & Switch
    int chWidth = chorusArea.getWidth() / 3;
    
    // Col 1: Rate
    auto ch1 = chorusArea.removeFromLeft(chWidth);
    chorusRateLabel.setBounds(ch1.removeFromTop(20));
    chorusRateSlider.setBounds(ch1.reduced(5));

    // Col 2: Depth
    auto ch2 = chorusArea.removeFromLeft(chWidth);
    chorusDepthLabel.setBounds(ch2.removeFromTop(20));
    chorusDepthSlider.setBounds(ch2.reduced(5));

    // Col 3: Mode Switch
    auto ch3 = chorusArea;
    // Center the button in the remaining space
    chorusVibratoButton.setBounds(ch3.reduced(10, 30));

    // --- Row 5: AMP CONTROLS (Bottom) ---
    // Now we have plenty of space, so they won't disappear.
    int sliderHeight = 60; 

    gainLabel.setBounds(area.removeFromTop(20));
    gainSlider.setBounds(area.removeFromTop(sliderHeight));
    
    bassLabel.setBounds(area.removeFromTop(20));
    bassSlider.setBounds(area.removeFromTop(sliderHeight));
    
    trebleLabel.setBounds(area.removeFromTop(20));
    trebleSlider.setBounds(area.removeFromTop(sliderHeight));
    
    volumeLabel.setBounds(area.removeFromTop(20));
    volumeSlider.setBounds(area.removeFromTop(sliderHeight));
}

void MainComponent::sliderValueChanged(juce::Slider* slider) {
    if (slider == &gainSlider) {
        rigEngine.setAmpGain((float)slider->getValue());
    } else if (slider == &bassSlider) {
        rigEngine.setAmpBass((float)slider->getValue());
    } else if (slider == &trebleSlider) {
        rigEngine.setAmpTreble((float)slider->getValue());
    } else if (slider == &volumeSlider) {
        rigEngine.setAmpVolume((float)slider->getValue());
    } else if (slider == &fuzzSustainSlider) {
        rigEngine.setFuzzSustain((float)slider->getValue());
    } else if (slider == &fuzzToneSlider) {
        rigEngine.setFuzzTone((float)slider->getValue());
    } else if (slider == &fuzzVolumeSlider) {
        rigEngine.setFuzzVolume((float)slider->getValue());
    } else if (slider == &chorusRateSlider) {
        rigEngine.setChorusRate(slider->getValue());
    } else if (slider == &chorusDepthSlider) {
        rigEngine.setChorusDepth(slider->getValue());
    }
}

void MainComponent::buttonClicked(juce::Button* button) {
    if (button == &fuzzBypassButton) {
        rigEngine.setFuzzBypass(!fuzzBypassButton.getToggleState());
    } else if (button == &openButton) {
        openFile();
    } else if (button == &fileInputToggle) {
        if (fileInputToggle.getToggleState())
            transportSource.start();
        else
            transportSource.stop();
    } else if (button == &muteButton) {
        rigEngine.setMuted(muteButton.getToggleState());
    } else if (button == &cabToggle) {
        rigEngine.setCabBypass(!cabToggle.getToggleState());
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
    } else if (button == &chorusBypassButton) {
        rigEngine.setChorusBypass(!chorusBypassButton.getToggleState());
    } else if (button == &chorusVibratoButton) {
        rigEngine.setChorusVibrato(chorusVibratoButton.getToggleState());
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
    transportSource.stop();
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
            rigEngine.loadCabIR(std::move(rawIRBuffer), reader->sampleRate);
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
        rigEngine.loadCabIR(std::move(rawIRBuffer), reader->sampleRate);
    }
}

void MainComponent::setupChorus() {
    chorusLabel.setText("CHORUS", juce::dontSendNotification);
    chorusLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(chorusLabel);

    chorusBypassButton.setButtonText("ON");
    chorusBypassButton.setClickingTogglesState(true);
    chorusBypassButton.setToggleState(false, juce::dontSendNotification);
    chorusBypassButton.addListener(this);
    addAndMakeVisible(chorusBypassButton);
    rigEngine.setChorusBypass(true);

    // Rate
    chorusRateLabel.setText("Rate", juce::dontSendNotification);
    chorusRateLabel.setFont(juce::Font(12.0f));
    addAndMakeVisible(chorusRateLabel);
    
    chorusRateSlider.setRange(0.0, 1.0, 0.01);
    chorusRateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    chorusRateSlider.addListener(this);
    chorusRateSlider.setValue(0.3);
    addAndMakeVisible(chorusRateSlider);

    // Depth
    chorusDepthLabel.setText("Depth", juce::dontSendNotification);
    chorusDepthLabel.setFont(juce::Font(12.0f));
    addAndMakeVisible(chorusDepthLabel);

    chorusDepthSlider.setRange(0.0, 1.0, 0.01);
    chorusDepthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    chorusDepthSlider.addListener(this);
    chorusDepthSlider.setValue(0.5);
    addAndMakeVisible(chorusDepthSlider);

    chorusVibratoButton.setButtonText("Vibrato Mode");
    chorusVibratoButton.setClickingTogglesState(true);
    chorusVibratoButton.setToggleState(false, juce::dontSendNotification);
    chorusVibratoButton.addListener(this);
    addAndMakeVisible(chorusVibratoButton);
}

void MainComponent::setupFuzz() {
    fuzzLabel.setText("B I G FUZZ", juce::dontSendNotification);
    fuzzLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(fuzzLabel);

    fuzzBypassButton.setButtonText("ON");
    fuzzBypassButton.setClickingTogglesState(true);
    fuzzBypassButton.setToggleState(false, juce::dontSendNotification);
    fuzzBypassButton.addListener(this);
    addAndMakeVisible(fuzzBypassButton);

    fuzzSustainLabel.setText("Sustain", juce::dontSendNotification);
    fuzzSustainLabel.setFont(juce::Font(12.0f));
    addAndMakeVisible(fuzzSustainLabel);

    fuzzToneLabel.setText("Tone", juce::dontSendNotification);
    fuzzToneLabel.setFont(juce::Font(12.0f));
    addAndMakeVisible(fuzzToneLabel);

    fuzzVolumeLabel.setText("Level", juce::dontSendNotification);
    fuzzVolumeLabel.setFont(juce::Font(12.0f));
    addAndMakeVisible(fuzzVolumeLabel);

    fuzzSustainSlider.setRange(0.0, 1.0, 0.01);
    fuzzSustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    fuzzSustainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    fuzzSustainSlider.addListener(this);
    fuzzSustainSlider.setValue(0.5);
    addAndMakeVisible(fuzzSustainSlider);

    fuzzToneSlider.setRange(0.0, 1.0, 0.01);
    fuzzToneSlider.addListener(this);
    fuzzToneSlider.setValue(0.5);
    addAndMakeVisible(fuzzToneSlider);

    fuzzVolumeSlider.setRange(0.0, 1.0, 0.01);
    fuzzVolumeSlider.addListener(this);
    fuzzVolumeSlider.setValue(0.5);
    addAndMakeVisible(fuzzVolumeSlider);

}
