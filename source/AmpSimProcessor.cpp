#include "AmpSimProcessor.h"

void AmpSimProcessor::prepare(double sampleRate)
{
    fs = sampleRate > 0.0 ? sampleRate : 44100.0;
    for (auto& filter : toneFilters) {
        filter.reset();
        filter.setCoefficients(juce::IIRCoefficients::makeHighShelf(fs, 1200.0, 0.707, tone.load()));
    }
}

void AmpSimProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    const float g   = gain.load();
    const float t   = tone.load();
    const float vol = volume.load();

    // Update tone filter coefficients once per block (cheap)
    auto newCoefficients = juce::IIRCoefficients::makeHighShelf(fs, 1200.0, 0.707, t);

    for (int ch = 0; ch < numChannels; ++ch) {
        toneFilters[ch].setCoefficients(newCoefficients);

        auto* data = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i) {
            float x = data[i] * g;       // preamp gain
            x = std::tanh(x);            // soft clipping (tube-like)
            data[i] = x * vol;           // output volume
        }

        toneFilters[ch].processSamples(data, numSamples);
    }
}
