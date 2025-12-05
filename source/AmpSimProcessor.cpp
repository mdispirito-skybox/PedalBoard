#include "AmpSimProcessor.h"
#include <cmath>

void AmpSimProcessor::prepare(double sampleRate) {
    fs = sampleRate > 0.0 ? sampleRate : 44100.0;

    for (int i = 0; i < 2; ++i) {
        preFilters[i].reset();
        bassFilters[i].reset();
        trebleFilters[i].reset();

        dcBlockerState[i] = 0.0f;
        lastInputSample[i] = 0.0f;

        preFilters[i].setCoefficients(juce::IIRCoefficients::makeHighPass(fs, 300.0f));
    }
}

void AmpSimProcessor::process(juce::AudioBuffer<float>& buffer) {
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    const float rawGain = gain.load();
    const float driveAmount = 1.0f + (std::pow(rawGain, 3.0f) * 2.5f);
    const float rawVol = volume.load();
    const float outputLevel = std::pow(rawVol, 3.0f) * 0.75f;

    const float b = bass.load();
    const float tr = treble.load();
    float bassGain = 0.5f + (b * 2.0f);
    float trebGain = 0.5f + (tr * 2.0f);

    auto bassCoeffs = juce::IIRCoefficients::makeLowShelf(fs, 150.0f, 0.707f, bassGain);
    auto trebCoeffs = juce::IIRCoefficients::makeHighShelf(fs, 3500.0f, 0.707f, trebGain);

    for (int ch = 0; ch < numChannels; ++ch) {
        auto* data = buffer.getWritePointer(ch);

        bassFilters[ch].setCoefficients(bassCoeffs);
        trebleFilters[ch].setCoefficients(trebCoeffs);

        // PRE-FILTERING (Tighten up the DI signal)
        preFilters[ch].processSamples(data, numSamples);

        // DISTORTION LOOP
        for (int i = 0; i < numSamples; ++i) {
            float x = data[i];
            x *= driveAmount;

            float bias = 0.4f; 
            float driven = x + bias;
            x = std::tanh(driven);
            x = x - std::tanh(bias);

            float r = 0.995f;
            float y = x - lastInputSample[ch] + (r * dcBlockerState[ch]);
            
            lastInputSample[ch] = x;
            dcBlockerState[ch] = y;
            
            data[i] = y;
        }
        
        bassFilters[ch].processSamples(data, numSamples);
        trebleFilters[ch].processSamples(data, numSamples);
        juce::FloatVectorOperations::multiply(data, outputLevel, numSamples);
    }
}
