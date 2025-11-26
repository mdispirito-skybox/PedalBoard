#include "AmpSimProcessor.h"
#include <cmath>

void AmpSimProcessor::prepare(double sampleRate) {
    fs = sampleRate > 0.0 ? sampleRate : 44100.0;
    for (auto& filter : toneFilters) {
        filter.reset();
        filter.setCoefficients(juce::IIRCoefficients::makeLowPass(fs, 5000.0f));
    }

    for (auto& filter : presenceFilters) {
        filter.reset();
        filter.setCoefficients(juce::IIRCoefficients::makeHighShelf(fs, 3000.0f, 0.707f, 1.0f));
    }
}

void AmpSimProcessor::process(juce::AudioBuffer<float>& buffer) {
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    const float g   = gain.load();
    const float t   = tone.load();
    const float vol = volume.load();
    const float p   = presence.load();

    float toneFreq = 500.0f + (t * 7500.0f);
    auto toneCoeffs = juce::IIRCoefficients::makeLowPass(fs, toneFreq);

    float presenceGain = 1.0f + (p * 3.0f); 
    auto presCoeffs = juce::IIRCoefficients::makeHighShelf(fs, 3000.0f, 0.707f, presenceGain);

    for (int ch = 0; ch < numChannels; ++ch) {
        toneFilters[ch].setCoefficients(toneCoeffs);
        presenceFilters[ch].setCoefficients(presCoeffs);

        auto* data = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i) {
            // --- STAGE 1: Pre-Gain ---
            float x = data[i] * g;

            // --- STAGE 2: Asymmetric Soft Clipping (Tube Emulation) ---
            if (x > 0.0f) {
                x = std::tanh(x); 
            } else {
                x = std::tanh(x * 0.8f) / 0.8f; 
            }
            // --- STAGE 3: Output Level ---
            data[i] = x * vol;
        }

        // --- STAGE 4: EQ Stack ---
        toneFilters[ch].processSamples(data, numSamples);
        presenceFilters[ch].processSamples(data, numSamples);
    }
}
