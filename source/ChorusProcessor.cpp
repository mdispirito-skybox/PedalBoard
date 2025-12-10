#include "ChorusProcessor.h"

void ChorusProcessor::prepare(double sampleRate) {
    fs = sampleRate;
    delayBuffer.setSize(2, (int)sampleRate + 1);
    delayBuffer.clear();
    writePosition = 0;
    lfoPhase = 0.0f;
}

void ChorusProcessor::process(juce::AudioBuffer<float>& buffer) {
   if (isBypassed.load()) return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const int bufferLength = delayBuffer.getNumSamples();

    float lfoSpeed = 0.1f + (rate.load() * 2.9f); 
    float depthMs = 1.0f + (depth.load() * 3.0f);
    float depthSamples = depthMs * (fs / 1000.0f);
    float centerDelay = depthSamples + 20.0f; 

    float mixAmount = 0.5f; 

    for (int i = 0; i < numSamples; ++i) {
        float lfo = std::sin(lfoPhase);

        lfoPhase += (2.0f * juce::MathConstants<float>::pi * lfoSpeed) / fs;
        if (lfoPhase >= 2.0f * juce::MathConstants<float>::pi) {
            lfoPhase -= 2.0f * juce::MathConstants<float>::pi;
        }

        for (int ch = 0; ch < numChannels; ++ch) {
            auto* channelData = buffer.getWritePointer(ch);
            auto* delayData = delayBuffer.getWritePointer(ch);

            float drySample = channelData[i];
            delayData[writePosition] = drySample;

            float delayTimeInSamples = centerDelay + (lfo * depthSamples);
            float readPos = (float)writePosition - delayTimeInSamples;

            while (readPos < 0.0f) readPos += bufferLength;
            while (readPos >= bufferLength) readPos -= bufferLength;

            float wetSample = getInterpolatedSample(delayBuffer, ch, readPos);
            channelData[i] = (drySample * (1.0f - mixAmount)) + (wetSample * mixAmount);
        }
        
        writePosition++;
        if (writePosition >= bufferLength) writePosition = 0;
    }
}

float ChorusProcessor::getInterpolatedSample(const juce::AudioBuffer<float>& buffer, int channel, float readPosition) {
    int index1 = (int)readPosition;
    int index2 = index1 + 1;
    float frac = readPosition - (float)index1;

    if (index2 >= buffer.getNumSamples()) index2 = 0;

    auto* data = buffer.getReadPointer(channel);
    float sample1 = data[index1];
    float sample2 = data[index2];

    return sample1 + frac * (sample2 - sample1);
}