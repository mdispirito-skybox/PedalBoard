#include "FuzzProcessor.h"

void FuzzProcessor::prepare(double sampleRate) {
    fs = sampleRate > 0.0 ? sampleRate : 44100.0;

    for (int i = 0; i < 2; ++i) {
        lowPassFilters[i].reset();
        highPassFilters[i].reset();

        lowPassFilters[i].setCoefficients(juce::IIRCoefficients::makeLowPass(fs, 400.0f));
        highPassFilters[i].setCoefficients(juce::IIRCoefficients::makeHighPass(fs, 1200.0f));
    }
    toneScratchBuffer.setSize(2, 8192);
    toneScratchBuffer.clear();
}

void FuzzProcessor::process(juce::AudioBuffer<float>& buffer) {
if (isBypassed.load()) return; // Pass audio through untouched
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    float s = sustain.load();
    float drive = 1.0f + (std::pow(s, 2.0f) * 100.0f);
    float vol = std::pow(volume.load(), 2.0f); 
    float t = tone.load();

    for (int ch = 0; ch < numChannels; ++ch) {
        auto* data = buffer.getWritePointer(ch);
        
        for (int i = 0; i < numSamples; ++i) {
            float x = data[i] * drive;
            data[i] = std::tanh(x) * 0.95f; 
        }

        toneScratchBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
        lowPassFilters[ch].processSamples(buffer.getWritePointer(ch), numSamples);
        highPassFilters[ch].processSamples(toneScratchBuffer.getWritePointer(ch), numSamples);

        auto* bassData = buffer.getReadPointer(ch);
        auto* trebData = toneScratchBuffer.getReadPointer(ch);
        auto* outData  = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i) {
            float bassSample = bassData[i] * (1.0f - t);
            float trebSample = trebData[i] * t;
            outData[i] = (bassSample + trebSample) * vol;
        }
    }
}