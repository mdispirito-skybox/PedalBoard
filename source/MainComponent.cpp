#include "MainComponent.h"
#include "BinaryData.h"

MainComponent::MainComponent() {
setLookAndFeel(&styleSheet);

    // 1. Backgrounds
    addAndMakeVisible(fuzzPedal);
    addAndMakeVisible(chorusPedal);
    addAndMakeVisible(delayPedal);

    // 2. Setup Controls
    setupAmp();
    setupFuzz();
    setupChorus();
    setupDelay();

    // 3. Top Bar UI
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    
    addAndMakeVisible(openButton);
    openButton.setButtonText("OPEN FILE");
    openButton.addListener(this);
    
    addAndMakeVisible(fileInputToggle);
    fileInputToggle.setButtonText("PLAY FILE");
    fileInputToggle.setClickingTogglesState(true); 
    fileInputToggle.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green.darker(0.5f)); 
    fileInputToggle.addListener(this);
    
    addAndMakeVisible(muteButton);
    muteButton.setButtonText("MUTE");
    muteButton.setClickingTogglesState(true); 
    muteButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red.darker(0.2f)); 
    muteButton.setToggleState(true, juce::dontSendNotification);
    muteButton.addListener(this);
    
    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("SETTINGS");
    settingsButton.addListener(this);

    formatManager.registerBasicFormats();
    
    setSize(1100, 600);
    setAudioChannels(1, 2);
    startTimerHz(30);
}

MainComponent::~MainComponent(){
    setLookAndFeel(nullptr);
    stopTimer();
    shutdownAudio();
}

void MainComponent::setupAmp() {
    auto setupKnob = [this](juce::Slider& s, juce::Label& l, juce::String text, float def) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); 
        s.addListener(this);
        s.setValue(def);
        addAndMakeVisible(s);

        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(14.0f, juce::Font::bold));
        
        // NEW: Force these specific labels to be BLACK
        l.setColour(juce::Label::textColourId, juce::Colours::black); 
        
        addAndMakeVisible(l);
    };

    gainSlider.setRange(0.0, 10.0, 0.01);
    setupKnob(gainSlider, gainLabel, "GAIN", 1.0f);

    bassSlider.setRange(0.0, 1.0, 0.001);
    setupKnob(bassSlider, bassLabel, "BASS", 0.5f);

    trebleSlider.setRange(0.0, 1.0, 0.001);
    setupKnob(trebleSlider, trebleLabel, "TREBLE", 0.5f);

    volumeSlider.setRange(0.0, 1.0, 0.001);
    setupKnob(volumeSlider, volumeLabel, "VOLUME", 0.4f);

    addAndMakeVisible(loadIRButton); loadIRButton.setButtonText("Load IR..."); loadIRButton.addListener(this);
    addAndMakeVisible(cabToggle); cabToggle.setButtonText("Cab Sim"); cabToggle.setClickingTogglesState(true); cabToggle.setToggleState(true, juce::dontSendNotification); cabToggle.addListener(this);
    addAndMakeVisible(irSelector); irSelector.addItem("ORANGE 4x12", 1); irSelector.addItem("VOX AC30", 2); irSelector.onChange = [this] { loadEmbeddedIR(irSelector.getSelectedId()); }; irSelector.setSelectedId(2);
}

void MainComponent::setupFuzz() {
    fuzzBypassButton.setButtonText("Enable"); fuzzBypassButton.setClickingTogglesState(true); fuzzBypassButton.addListener(this); addAndMakeVisible(fuzzBypassButton);
    
    auto setupKnob = [this](juce::Slider& s, juce::Label& l, juce::String text) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.addListener(this);
        s.setValue(0.5);
        addAndMakeVisible(s);
        
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(13.0f, juce::Font::bold));
        addAndMakeVisible(l);
    };

    fuzzSustainSlider.setRange(0.0, 1.0, 0.01); setupKnob(fuzzSustainSlider, fuzzSustainLabel, "SUSTAIN");
    fuzzToneSlider.setRange(0.0, 1.0, 0.01);    setupKnob(fuzzToneSlider, fuzzToneLabel, "TONE");
    fuzzVolumeSlider.setRange(0.0, 1.0, 0.01);  setupKnob(fuzzVolumeSlider, fuzzVolumeLabel, "LEVEL");
}

void MainComponent::setupChorus() {
    chorusBypassButton.setButtonText("Enable"); chorusBypassButton.setClickingTogglesState(true); chorusBypassButton.addListener(this); addAndMakeVisible(chorusBypassButton);
    
    auto setupKnob = [this](juce::Slider& s, juce::Label& l, juce::String text, float def) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.addListener(this);
        s.setValue(def);
        addAndMakeVisible(s);
        
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(13.0f, juce::Font::bold));
        addAndMakeVisible(l);
    };

    chorusRateSlider.setRange(0.0, 1.0, 0.01);  setupKnob(chorusRateSlider, chorusRateLabel, "RATE", 0.3f);
    chorusDepthSlider.setRange(0.0, 1.0, 0.01); setupKnob(chorusDepthSlider, chorusDepthLabel, "DEPTH", 0.5f);
}

void MainComponent::setupDelay() {
    delayBypassButton.setButtonText("Enable"); delayBypassButton.setClickingTogglesState(true); delayBypassButton.addListener(this); addAndMakeVisible(delayBypassButton);
    
    auto setupKnob = [this](juce::Slider& s, juce::Label& l, juce::String text, float def) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.addListener(this);
        s.setValue(def);
        addAndMakeVisible(s);
        
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(13.0f, juce::Font::bold));
        addAndMakeVisible(l);
    };

    delayTimeSlider.setRange(0.0, 1.0, 0.01);     setupKnob(delayTimeSlider, delayTimeLabel, "TIME", 0.3f);
    delayFeedbackSlider.setRange(0.0, 1.0, 0.01); setupKnob(delayFeedbackSlider, delayFeedbackLabel, "REPEATS", 0.4f);
    delayMixSlider.setRange(0.0, 1.0, 0.01);      setupKnob(delayMixSlider, delayMixLabel, "MIX", 0.3f);
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black.brighter(0.12f)); 

    auto area = getLocalBounds();
    auto topBar = area.removeFromTop(80); 
    auto floor = area.removeFromBottom(520).reduced(20); 
    
    int pedalW = 150;
    int pedalH = 360; 
    int gap = 15;
    
    floor.removeFromLeft(pedalW + gap);
    floor.removeFromLeft(pedalW + gap);
    floor.removeFromLeft(pedalW + gap);
    
    auto ampArea = floor.withSizeKeepingCentre(floor.getWidth(), pedalH); 

    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(ampArea.toFloat(), 5.0f);
    g.setColour(juce::Colours::white.withAlpha(0.2f)); 
    g.drawRoundedRectangle(ampArea.toFloat().reduced(2), 5.0f, 2.0f);

    auto faceplate = ampArea.removeFromTop(ampArea.getHeight() / 2).reduced(10);
    g.setColour(juce::Colours::silver.darker(0.1f)); 
    g.fillRect(faceplate);
    
    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("MD MK-1", faceplate.removeFromTop(30), juce::Justification::centred, true);

    auto grill = ampArea.reduced(10);
    g.setColour(juce::Colours::darkgrey.darker(0.5f)); 
    g.fillRect(grill);
    
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    for (int i = 0; i < grill.getWidth(); i+=4) g.drawLine(grill.getX() + i, grill.getY(), grill.getX() + i, grill.getBottom());
    for (int i = 0; i < grill.getHeight(); i+=4) g.drawLine(grill.getX(), grill.getY() + i, grill.getRight(), grill.getY() + i);
}

void MainComponent::resized() {
    auto area = getLocalBounds();

    // --- TOP BAR ---
    auto topBar = area.removeFromTop(80);
    inputMeter.setBounds(topBar.removeFromLeft(120).reduced(10, 25));
    outputMeter.setBounds(topBar.removeFromRight(120).reduced(10, 25));
    topBar = topBar.reduced(10, 0); 
    
    int btnWidth = 90; 
    int btnHeight = 30;
    int gap = 20;
    int totalBtnWidth = (btnWidth * 4) + (gap * 3);
    auto centerArea = topBar.withWidth(totalBtnWidth).withCentre(topBar.getCentre());
    
    openButton.setBounds(centerArea.removeFromLeft(btnWidth).withSizeKeepingCentre(btnWidth, btnHeight));
    centerArea.removeFromLeft(gap);
    fileInputToggle.setBounds(centerArea.removeFromLeft(btnWidth).withSizeKeepingCentre(btnWidth, btnHeight));
    centerArea.removeFromLeft(gap);
    muteButton.setBounds(centerArea.removeFromLeft(btnWidth).withSizeKeepingCentre(btnWidth, btnHeight));
    centerArea.removeFromLeft(gap);
    settingsButton.setBounds(centerArea.removeFromLeft(btnWidth).withSizeKeepingCentre(btnWidth, btnHeight));

    // --- FLOORBOARD ---
    auto floor = area.reduced(20); 
    int pedalW = 150;
    int pedalH = 360;
    int gapVal = 15;
    int labelH = 20; // Height for labels

    // 1. Fuzz
    auto fuzzSlot = floor.removeFromLeft(pedalW);
    auto fuzzArea = fuzzSlot.withSizeKeepingCentre(pedalW, pedalH);
    fuzzPedal.setBounds(fuzzArea);
    
    auto fKnobs = fuzzArea.removeFromTop(200); 
    
    // Tone (Top Center)
    auto fToneArea = fKnobs.removeFromTop(80).withSizeKeepingCentre(70, 80);
    fuzzToneLabel.setBounds(fToneArea.removeFromBottom(labelH));
    fuzzToneSlider.setBounds(fToneArea);

    // Sustain (Bottom Left)
    auto fSusArea = fKnobs.removeFromLeft(75).reduced(2);
    fuzzSustainLabel.setBounds(fSusArea.removeFromBottom(labelH));
    fuzzSustainSlider.setBounds(fSusArea);

    // Volume (Bottom Right)
    auto fVolArea = fKnobs.removeFromRight(75).reduced(2);
    fuzzVolumeLabel.setBounds(fVolArea.removeFromBottom(labelH));
    fuzzVolumeSlider.setBounds(fVolArea);
    
    fuzzBypassButton.setBounds(fuzzArea.removeFromBottom(70).withSizeKeepingCentre(50, 60));

    floor.removeFromLeft(gapVal);

    // 2. Chorus
    auto chorSlot = floor.removeFromLeft(pedalW);
    auto chorArea = chorSlot.withSizeKeepingCentre(pedalW, pedalH);
    chorusPedal.setBounds(chorArea);
    
    // ADJUSTED HEIGHT: Reduced container height to pull labels up
    auto cKnobs = chorArea.removeFromTop(120).translated(0, 30); 
    
    // Rate
    auto cRateArea = cKnobs.removeFromLeft(75).reduced(2);
    chorusRateLabel.setBounds(cRateArea.removeFromBottom(labelH));
    chorusRateSlider.setBounds(cRateArea);

    // Depth
    auto cDepthArea = cKnobs.removeFromRight(75).reduced(2);
    chorusDepthLabel.setBounds(cDepthArea.removeFromBottom(labelH));
    chorusDepthSlider.setBounds(cDepthArea);
    
    chorusBypassButton.setBounds(chorArea.removeFromBottom(70).withSizeKeepingCentre(50, 60));

    floor.removeFromLeft(gapVal);

    // 3. Delay
    auto delSlot = floor.removeFromLeft(pedalW);
    auto delArea = delSlot.withSizeKeepingCentre(pedalW, pedalH);
    delayPedal.setBounds(delArea);
    
    auto dKnobs = delArea.removeFromTop(200);
    
    // Row 1 (Time / Mix)
    auto dRow1 = dKnobs.removeFromTop(80);
    
    auto dTimeArea = dRow1.removeFromLeft(75).reduced(2);
    delayTimeLabel.setBounds(dTimeArea.removeFromBottom(labelH));
    delayTimeSlider.setBounds(dTimeArea);

    auto dMixArea = dRow1.removeFromRight(75).reduced(2);
    delayMixLabel.setBounds(dMixArea.removeFromBottom(labelH));
    delayMixSlider.setBounds(dMixArea);

    // Row 2 (Feedback)
    auto dFbArea = dKnobs.removeFromTop(80).withSizeKeepingCentre(70, 80);
    delayFeedbackLabel.setBounds(dFbArea.removeFromBottom(labelH));
    delayFeedbackSlider.setBounds(dFbArea);
    
    delayBypassButton.setBounds(delArea.removeFromBottom(70).withSizeKeepingCentre(50, 60));

    floor.removeFromLeft(gapVal);

    // 4. Amp Head
    auto ampSlot = floor; 
    auto ampArea = ampSlot.withSizeKeepingCentre(ampSlot.getWidth(), pedalH);
    
    auto faceplate = ampArea.removeFromTop(ampArea.getHeight() / 2).reduced(10);
    faceplate.removeFromTop(30); 
    
    auto preamp = faceplate.removeFromLeft(300);
    int knobW = preamp.getWidth() / 4;
    
    // Gain
    auto gainArea = preamp.removeFromLeft(knobW).reduced(2);
    gainLabel.setBounds(gainArea.removeFromBottom(labelH));
    gainSlider.setBounds(gainArea);

    // Bass
    auto bassArea = preamp.removeFromLeft(knobW).reduced(2);
    bassLabel.setBounds(bassArea.removeFromBottom(labelH));
    bassSlider.setBounds(bassArea);

    // Treble
    auto trebArea = preamp.removeFromLeft(knobW).reduced(2);
    trebleLabel.setBounds(trebArea.removeFromBottom(labelH));
    trebleSlider.setBounds(trebArea);

    // Volume
    auto volArea = preamp.removeFromLeft(knobW).reduced(2);
    volumeLabel.setBounds(volArea.removeFromBottom(labelH));
    volumeSlider.setBounds(volArea);
    
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
