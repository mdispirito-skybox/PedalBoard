#include "MainComponent.h"

MainComponent::MainComponent()
{
    gainLabel.setText("Gain", juce::dontSendNotification);
    toneLabel.setText("Tone", juce::dontSendNotification);
    volumeLabel.setText("Volume", juce::dontSendNotification);

    addAndMakeVisible(gainLabel);
    addAndMakeVisible(toneLabel);
    addAndMakeVisible(volumeLabel);

    // Sliders
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

    setSize(400, 200);
    setAudioChannels(1, 2); // mono in, stereo out
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    amp.prepare(sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    amp.process(*bufferToFill.buffer);
}

void MainComponent::releaseResources() {}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(20);
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
