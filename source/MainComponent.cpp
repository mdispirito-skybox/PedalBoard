#include "MainComponent.h"
#include "BinaryData.h"

MainComponent::MainComponent() {
    setLookAndFeel(&styleSheet);

    addAndMakeVisible(fuzzPedal);
    addAndMakeVisible(chorusPedal);
    addAndMakeVisible(delayPedal);

    setupAmp();
    setupFuzz();
    setupChorus();
    setupDelay();

    // --- GENERAL UI ---
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    addAndMakeVisible(openButton);
    openButton.setButtonText("Open Audio...");
    openButton.addListener(this);
    addAndMakeVisible(fileInputToggle);
    fileInputToggle.setButtonText("Use File");
    fileInputToggle.addListener(this);
    addAndMakeVisible(muteButton);
    muteButton.setButtonText("MUTE");
    muteButton.setClickingTogglesState(true);
    muteButton.addListener(this);
    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("Audio Settings");
    settingsButton.addListener(this);


    formatManager.registerBasicFormats();
    setSize(1200, 600);
    setAudioChannels(1, 2);
    startTimerHz(30);
}

MainComponent::~MainComponent(){
    setLookAndFeel(nullptr);
    stopTimer();
    shutdownAudio();
}

void MainComponent::setupAmp() {
    gainSlider.setRange(0.0, 10.0, 0.01); gainSlider.addListener(this); gainSlider.setValue(1.0); 
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(gainSlider);

    bassSlider.setRange(0.0, 1.0, 0.001); bassSlider.addListener(this); bassSlider.setValue(0.5); 
    bassSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(bassSlider);

    trebleSlider.setRange(0.0, 1.0, 0.001); trebleSlider.addListener(this); trebleSlider.setValue(0.5); 
    trebleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    trebleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(trebleSlider);

    volumeSlider.setRange(0.0, 1.0, 0.001); volumeSlider.addListener(this); volumeSlider.setValue(0.4); 
    volumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(volumeSlider);

    addAndMakeVisible(loadIRButton); loadIRButton.setButtonText("Load IR..."); loadIRButton.addListener(this);
    addAndMakeVisible(cabToggle); cabToggle.setButtonText("Cab Sim"); cabToggle.setClickingTogglesState(true); cabToggle.setToggleState(true, juce::dontSendNotification); cabToggle.addListener(this);
    addAndMakeVisible(irSelector); irSelector.addItem("ORANGE 4x12", 1); irSelector.addItem("VOX AC30", 2); irSelector.onChange = [this] { loadEmbeddedIR(irSelector.getSelectedId()); }; irSelector.setSelectedId(2);
}

void MainComponent::setupFuzz() {
    fuzzBypassButton.setButtonText("Enable"); fuzzBypassButton.setClickingTogglesState(true); fuzzBypassButton.addListener(this); addAndMakeVisible(fuzzBypassButton);
    
    fuzzSustainSlider.setRange(0.0, 1.0, 0.01);
    fuzzSustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    fuzzSustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    fuzzSustainSlider.addListener(this);
    fuzzSustainSlider.setValue(0.5);
    addAndMakeVisible(fuzzSustainSlider);

    fuzzToneSlider.setRange(0.0, 1.0, 0.01);
    fuzzToneSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    fuzzToneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    fuzzToneSlider.addListener(this);
    fuzzToneSlider.setValue(0.5);
    addAndMakeVisible(fuzzToneSlider);

    fuzzVolumeSlider.setRange(0.0, 1.0, 0.01);
    fuzzVolumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    fuzzVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    fuzzVolumeSlider.addListener(this);
    fuzzVolumeSlider.setValue(0.5);
    addAndMakeVisible(fuzzVolumeSlider);
}

void MainComponent::setupChorus() {
    chorusBypassButton.setButtonText("Enable");
    chorusBypassButton.setClickingTogglesState(true);
    chorusBypassButton.addListener(this);
    addAndMakeVisible(chorusBypassButton);
    
    chorusRateSlider.setRange(0.0, 1.0, 0.01);
    chorusRateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    chorusRateSlider.addListener(this); chorusRateSlider.setValue(0.3);
    addAndMakeVisible(chorusRateSlider);

    chorusDepthSlider.setRange(0.0, 1.0, 0.01);
    chorusDepthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    chorusDepthSlider.addListener(this);
    chorusDepthSlider.setValue(0.5);
    addAndMakeVisible(chorusDepthSlider);
}

void MainComponent::setupDelay() {
    delayBypassButton.setButtonText("Enable");
    delayBypassButton.setClickingTogglesState(true);
    delayBypassButton.addListener(this);
    addAndMakeVisible(delayBypassButton);
    
    delayTimeSlider.setRange(0.0, 1.0, 0.01);
    delayTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    delayTimeSlider.addListener(this); delayTimeSlider.setValue(0.3);
    addAndMakeVisible(delayTimeSlider);

    delayFeedbackSlider.setRange(0.0, 1.0, 0.01);
    delayFeedbackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayFeedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    delayFeedbackSlider.addListener(this); delayFeedbackSlider.setValue(0.4);
    addAndMakeVisible(delayFeedbackSlider);

    delayMixSlider.setRange(0.0, 1.0, 0.01);
    delayMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    delayMixSlider.addListener(this);
    delayMixSlider.setValue(0.3);
    addAndMakeVisible(delayMixSlider);
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black.brighter(0.12f)); 

    auto area = getLocalBounds();
    auto topBar = area.removeFromTop(80); // MATCH RESIZED HEIGHT
    auto floor = area.removeFromBottom(520).reduced(20); // Floor takes rest
    
    int pedalW = 150;
    int gap = 15;
    floor.removeFromLeft(pedalW + gap);
    floor.removeFromLeft(pedalW + gap);
    floor.removeFromLeft(pedalW + gap);
    
    auto ampArea = floor; 
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(ampArea.toFloat(), 5.0f);
    g.setColour(juce::Colours::white.withAlpha(0.2f)); 
    g.drawRoundedRectangle(ampArea.toFloat().reduced(2), 5.0f, 2.0f);

    auto faceplate = ampArea.removeFromTop(ampArea.getHeight() / 2).reduced(10);
    g.setColour(juce::Colours::gold.darker(0.1f)); 
    g.fillRect(faceplate);
    
    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("BRITISH 30", faceplate.removeFromTop(30), juce::Justification::centred, true);

    auto grill = ampArea.reduced(10);
    g.setColour(juce::Colours::darkgrey.darker(0.5f)); 
    g.fillRect(grill);
    
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    for (int i = 0; i < grill.getWidth(); i+=4) g.drawLine(grill.getX() + i, grill.getY(), grill.getX() + i, grill.getBottom());
    for (int i = 0; i < grill.getHeight(); i+=4) g.drawLine(grill.getX(), grill.getY() + i, grill.getRight(), grill.getY() + i);
}

void MainComponent::resized() {
    auto area = getLocalBounds();

    // --- 1. TOP BAR (Expanded) ---
    // Increased height to 80px to fit labels and buttons nicely
    auto topBar = area.removeFromTop(80);
    
    // Meters (Far Edges)
    inputMeter.setBounds(topBar.removeFromLeft(120).reduced(10, 25));
    outputMeter.setBounds(topBar.removeFromRight(120).reduced(10, 25));
    
    // Center Controls
    // Create two groups: "Player" and "Master"
    auto centerArea = topBar.reduced(20, 5); // Padding
    
    // --- Group 1: File Player ---
    auto playerGroup = centerArea.removeFromLeft(200);
    playerLabel.setBounds(playerGroup.removeFromTop(20)); // Label on top
    // Buttons below
    int btnH = 30;
    openButton.setBounds(playerGroup.removeFromLeft(95).withHeight(btnH));
    fileInputToggle.setBounds(playerGroup.removeFromRight(95).withHeight(btnH));

    centerArea.removeFromLeft(40); // Gap

    // --- Group 2: Master ---
    auto masterGroup = centerArea.removeFromLeft(200);
    masterLabel.setBounds(masterGroup.removeFromTop(20));
    muteButton.setBounds(masterGroup.removeFromLeft(60).withHeight(btnH));
    settingsButton.setBounds(masterGroup.removeFromRight(100).withHeight(btnH));

    // --- 2. FLOORBOARD ---
    auto floor = area.reduced(20); 
    int pedalW = 150;
    int gap = 15;

    // Fuzz
    auto fuzzArea = floor.removeFromLeft(pedalW);
    fuzzPedal.setBounds(fuzzArea);
    auto fKnobs = fuzzArea.removeFromTop(200); 
    fuzzToneSlider.setBounds(fKnobs.removeFromTop(80).withSizeKeepingCentre(70, 80));
    fuzzSustainSlider.setBounds(fKnobs.removeFromLeft(75).reduced(2));
    fuzzVolumeSlider.setBounds(fKnobs.removeFromRight(75).reduced(2));
    fuzzBypassButton.setBounds(fuzzArea.removeFromBottom(70).withSizeKeepingCentre(50, 60));

    floor.removeFromLeft(gap);

    // Chorus
    auto chorArea = floor.removeFromLeft(pedalW);
    chorusPedal.setBounds(chorArea);
    auto cKnobs = chorArea.removeFromTop(160).translated(0, 20);
    chorusRateSlider.setBounds(cKnobs.removeFromLeft(75).reduced(2));
    chorusDepthSlider.setBounds(cKnobs.removeFromRight(75).reduced(2));
    chorusBypassButton.setBounds(chorArea.removeFromBottom(70).withSizeKeepingCentre(50, 60));

    floor.removeFromLeft(gap);

    // Delay
    auto delArea = floor.removeFromLeft(pedalW);
    delayPedal.setBounds(delArea);
    auto dKnobs = delArea.removeFromTop(200);
    auto dRow1 = dKnobs.removeFromTop(80);
    delayTimeSlider.setBounds(dRow1.removeFromLeft(75).reduced(2));
    delayMixSlider.setBounds(dRow1.removeFromRight(75).reduced(2));
    delayFeedbackSlider.setBounds(dKnobs.removeFromTop(80).withSizeKeepingCentre(70, 80));
    delayBypassButton.setBounds(delArea.removeFromBottom(70).withSizeKeepingCentre(50, 60));

    floor.removeFromLeft(gap);

    // Amp Head
    auto ampArea = floor;
    auto faceplate = ampArea.removeFromTop(ampArea.getHeight() / 2).reduced(10);
    faceplate.removeFromTop(30); 
    
    auto preamp = faceplate.removeFromLeft(300);
    int knobW = preamp.getWidth() / 4;
    gainSlider.setBounds(preamp.removeFromLeft(knobW));
    bassSlider.setBounds(preamp.removeFromLeft(knobW));
    trebleSlider.setBounds(preamp.removeFromLeft(knobW));
    volumeSlider.setBounds(preamp.removeFromLeft(knobW));
    
    auto cabSec = faceplate;
    irSelector.setBounds(cabSec.removeFromTop(30).reduced(5));
    loadIRButton.setBounds(cabSec.removeFromTop(30).reduced(5));
    cabToggle.setBounds(cabSec.withSizeKeepingCentre(60, 60));
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

void MainComponent::timerCallback() {
    inputMeter.setLevel(currentInputLevel.load());
    outputMeter.setLevel(currentOutputLevel.load());
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
    } else if (slider == &delayTimeSlider) {
        rigEngine.setDelayTime(slider->getValue());
    } else if (slider == &delayFeedbackSlider) {
        rigEngine.setDelayFeedback(slider->getValue());
    } else if (slider == &delayMixSlider) {
        rigEngine.setDelayMix(slider->getValue());
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
    } else if (button == &delayBypassButton) {
        rigEngine.setDelayBypass(!delayBypassButton.getToggleState());
    } 
}

void MainComponent::openFile() {
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
