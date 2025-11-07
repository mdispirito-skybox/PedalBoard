#include "AmpSimProcessor.h"

void AmpSimProcessor::prepare(double sampleRate)
{
    fs = sampleRate > 0.0 ? sampleRate : 44100.0;
    toneFilter.reset();
    // initialize with default tone
    toneFilter.setCoefficients(juce::IIRCoefficients::makeHighShelf(fs, 1200.0, 0.707, tone.load()));
}

void AmpSimProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    const float g   = gain.load();
    const float t   = tone.load();
    const float vol = volume.load();

    // Update tone filter coefficients once per block (cheap)
    toneFilter.setCoefficients(juce::IIRCoefficients::makeHighShelf(fs, 1200.0, 0.707, t));

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            float x = data[i] * g;       // preamp gain
            x = std::tanh(x);            // soft clipping (tube-like)
            data[i] = x * vol;           // output volume
        }

        // apply tone filter per channel
        toneFilter.processSamples(data, numSamples);
    }
}
