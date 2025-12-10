#include "DelayProcessor.h"
#include <cmath>

void DelayProcessor::prepare(double sampleRate) {
    fs = sampleRate;
    
    int bufferSize = (int)(2.0 * sampleRate) + 1;
    delayBuffer.setSize(2, bufferSize);
    delayBuffer.clear();
    
    writePosition = 0;
    for (int i = 0; i < 2; ++i) {
        feedbackFilters[i].reset();
        feedbackFilters[i].setCoefficients(juce::IIRCoefficients::makeLowPass(fs, 2500.0f));
    }
}

void DelayProcessor::process(juce::AudioBuffer<float>& buffer) {
    if (isBypassed.load()) return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const int bufferLength = delayBuffer.getNumSamples();

    float delayTimeMs = 50.0f + (time.load() * 950.0f);
    float delaySamples = delayTimeMs * (fs / 1000.0f);

    float fb = feedback.load() * 0.95f;
    float wetMix = mix.load();
    float dryMix = 1.0f - wetMix;

    for (int i = 0; i < numSamples; ++i) {
        
        for (int ch = 0; ch < numChannels; ++ch) {
            auto* channelData = buffer.getWritePointer(ch);
            auto* delayData = delayBuffer.getWritePointer(ch);

            float inputSample = channelData[i];
            float readPos = (float)writePosition - delaySamples;
            
            while (readPos < 0.0f) readPos += bufferLength;
            while (readPos >= bufferLength) readPos -= bufferLength;

            float delayedSample = getInterpolatedSample(delayBuffer, ch, readPos);
            float feedbackSignal = delayedSample * fb;
          
            feedbackSignal = feedbackFilters[ch].processSingleSampleRaw(feedbackSignal);
            delayData[writePosition] = inputSample + feedbackSignal;
            channelData[i] = (inputSample * dryMix) + (delayedSample * wetMix);
        }
        writePosition++;
        if (writePosition >= bufferLength) writePosition = 0;
    }
}

float DelayProcessor::getInterpolatedSample(const juce::AudioBuffer<float>& buffer, int channel, float readPosition) {
    int index1 = (int)readPosition;
    int index2 = index1 + 1;
    float frac = readPosition - (float)index1;

    if (index2 >= buffer.getNumSamples()) index2 = 0;

    auto* data = buffer.getReadPointer(channel);
    float sample1 = data[index1];
    float sample2 = data[index2];

    return sample1 + frac * (sample2 - sample1);
}