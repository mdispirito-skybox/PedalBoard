#include "GuitarRigEngine.h"

void GuitarRigEngine::prepare(double sampleRate, int samplesPerBlock, int numChannels) {
    fuzz.prepare(sampleRate);
    amp.prepare(sampleRate);
    cab.prepare(sampleRate, samplesPerBlock, numChannels);
    chorus.prepare(sampleRate);
}

void GuitarRigEngine::process(juce::AudioBuffer<float>& buffer) {
    if (isMuted.load()) {
        buffer.clear();
        return;
    }

    // --- SIGNAL CHAIN ---
    fuzz.process(buffer);
    amp.process(buffer);
    cab.process(buffer);
    chorus.process(buffer);

}