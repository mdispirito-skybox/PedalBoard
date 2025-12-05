#pragma once
#include "AmpSimProcessor.h"
#include "CabSimProcessor.h"
#include "juce_audio_basics/juce_audio_basics.h"

class GuitarRigEngine {
public:
    GuitarRigEngine() = default;
    ~GuitarRigEngine() = default;

    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void process(juce::AudioBuffer<float>& buffer);

    // --- Amp Controls ---
    void setAmpGain(float g)    { amp.setGain(g); }
    void setAmpBass(float b)    { amp.setBass(b); }
    void setAmpTreble(float t)  { amp.setTreble(t); }
    void setAmpVolume(float v)  { amp.setVolume(v); }

    // --- Cab Controls ---
    void loadCabIR(juce::AudioBuffer<float>&& ir, double rate) {
        cab.loadImpulseResponse(std::move(ir), rate);
    }
    void setCabBypass(bool bypassed) { cab.setBypassed(bypassed); }

    // --- Global Controls ---
    void setMuted(bool shouldMute) { isMuted.store(shouldMute); }

private:
    // The Signal Chain Components
    AmpSimProcessor amp;
    CabSimProcessor cab;

    std::atomic<bool> isMuted { true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarRigEngine)
};