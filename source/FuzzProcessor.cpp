#include "FuzzProcessor.h"

void FuzzProcessor::prepare(double sampleRate) {
    fs = sampleRate > 0.0 ? sampleRate : 44100.0;

    for (int i = 0; i < 2; ++i) {
        lowPassFilters[i].reset();
        highPassFilters[i].reset();

        lowPassFilters[i].setCoefficients(juce::IIRCoefficients::makeLowPass(fs, 400.0f));
        highPassFilters[i].setCoefficients(juce::IIRCoefficients::makeHighPass(fs, 1200.0f));
    }
}

void FuzzProcessor::process(juce::AudioBuffer<float>& buffer) {
    if (isBypassed.load()) return; // Pass audio through untouched

    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    float s = sustain.load();
    float drive = 1.0f + (std::pow(s, 2.0f) * 100.0f);

    // Hard clipping limits signal to +/- 1.0, so we just need simple attenuation.
    float vol = std::pow(volume.load(), 2.0f);

    float t = tone.load();

    for (int ch = 0; ch < numChannels; ++ch) {
        auto* data = buffer.getWritePointer(ch);
        
        for (int i = 0; i < numSamples; ++i) {
            float x = data[i] * drive;
            data[i] = std::tanh(x) * 0.95f; 
        }

        juce::AudioBuffer<float> bassPath(1, numSamples);
        bassPath.copyFrom(0, 0, data, numSamples);
        
        juce::AudioBuffer<float> trebPath(1, numSamples);
        trebPath.copyFrom(0, 0, data, numSamples);

        lowPassFilters[ch].processSamples(bassPath.getWritePointer(0), numSamples);
        highPassFilters[ch].processSamples(trebPath.getWritePointer(0), numSamples);

        auto* b = bassPath.getReadPointer(0);
        auto* tr = trebPath.getReadPointer(0);

        for (int i = 0; i < numSamples; ++i) {
            float bassSample = b[i] * (1.0f - t);
            float trebSample = tr[i] * t;
            
            data[i] = (bassSample + trebSample) * vol;
        }
    }
}