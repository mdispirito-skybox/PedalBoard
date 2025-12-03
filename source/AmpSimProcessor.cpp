#include "AmpSimProcessor.h"
#include <cmath>

void AmpSimProcessor::prepare(double sampleRate) {
    fs = sampleRate > 0.0 ? sampleRate : 44100.0;

    for (int i = 0; i < 2; ++i) {
        preFilters[i].reset();
        bassFilters[i].reset();
        trebleFilters[i].reset();

        preFilters[i].setCoefficients(juce::IIRCoefficients::makeHighPass(fs, 300.0f));
    }

}

void AmpSimProcessor::process(juce::AudioBuffer<float>& buffer) {
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    const float g   = gain.load() * 10.0f; 
    const float b   = bass.load();
    const float tr  = treble.load();
    const float vol = volume.load();

    float bassGain = 0.5f + (b * 2.0f);
    float trebGain = 0.5f + (tr * 2.0f);

    auto bassCoeffs = juce::IIRCoefficients::makeLowShelf(fs, 150.0f, 0.707f, bassGain);
    auto trebCoeffs = juce::IIRCoefficients::makeHighShelf(fs, 3500.0f, 0.707f, trebGain);

    for (int ch = 0; ch < numChannels; ++ch) {
        bassFilters[ch].setCoefficients(bassCoeffs);
        trebleFilters[ch].setCoefficients(trebCoeffs);
        auto* data = buffer.getWritePointer(ch);

        preFilters[ch].processSamples(data, numSamples);

        for (int i = 0; i < numSamples; ++i) {
            float x = data[i] * g;

            if (x > 0.0f) {
                x = std::tanh(x); 
            } else {
                x = std::tanh(x * 0.8f) / 0.8f; 
            }

            data[i] = x * vol;
        }
        
        bassFilters[ch].processSamples(data, numSamples);
        trebleFilters[ch].processSamples(data, numSamples);
    }
}
