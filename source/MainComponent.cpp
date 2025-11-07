#include "MainComponent.h"

MainComponent::MainComponent()
{
    gainLabel.setText("Gain", juce::dontSendNotification);
    toneLabel.setText("Tone", juce::dontSendNotification);
    volumeLabel.setText("Volume", juce::dontSendNotification);

    for (auto* label : { &gainLabel, &toneLabel, &volumeLabel })
        addAndMakeVisible(label);

    for (auto* slider : { &gainSlider, &toneSlider, &volumeSlider })
    {
        slider->setRange(0.0, 10.0, 0.01);
        slider->setValue(2.0);
        slider->addListener(this);
        addAndMakeVisible(slider);
    }

    setSize(400, 200);
    setAudioChannels(2, 2); // stereo input/output
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

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &gainSlider)
        amp.setGain((float)slider->getValue());
    else if (slider == &toneSlider)
        amp.setTone((float)slider->getValue());
    else if (slider == &volumeSlider)
        amp.setVolume((float)slider->getValue());
}
