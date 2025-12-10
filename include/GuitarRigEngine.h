#pragma once
#include "AmpSimProcessor.h"
#include "CabSimProcessor.h"
#include "FuzzProcessor.h"
#include "ChorusProcessor.h"
#include "DelayProcessor.h"
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
    void loadCabIR(juce::AudioBuffer<float>&& ir, double rate) { cab.loadImpulseResponse(std::move(ir), rate); }
    void setCabBypass(bool bypassed) { cab.setBypassed(bypassed); }

    // --- Fuzz Controls ---
    void setFuzzSustain(float s) { fuzz.setSustain(s); }
    void setFuzzTone(float t)    { fuzz.setTone(t); }
    void setFuzzVolume(float v)  { fuzz.setVolume(v); }
    void setFuzzBypass(bool b)   { fuzz.setBypassed(b); }

    // --- Chorus Controls ---
    void setChorusRate(float r)  { chorus.setRate(r); }
    void setChorusDepth(float d) { chorus.setDepth(d); }
    void setChorusBypass(bool b) { chorus.setBypassed(b); }

    // --- Delay Controls ---
    void setDelayTime(float t)     { delay.setTime(t); }
    void setDelayFeedback(float f) { delay.setFeedback(f); }
    void setDelayMix(float m)      { delay.setMix(m); }
    void setDelayBypass(bool b)    { delay.setBypassed(b); }

    // --- Global Controls ---
    void setMuted(bool shouldMute) { isMuted.store(shouldMute); }

private:
    AmpSimProcessor amp;
    CabSimProcessor cab;

    FuzzProcessor fuzz;
    ChorusProcessor chorus;
    DelayProcessor delay;

    std::atomic<bool> isMuted { true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarRigEngine)
};