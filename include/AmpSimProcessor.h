#pragma once
#include <atomic>
#include "juce_audio_basics/juce_audio_basics.h"

class AmpSimProcessor
{
public:
    AmpSimProcessor() = default;
    ~AmpSimProcessor() = default;

    // Called from audio thread during prepareToPlay
    void prepare(double sampleRate);

    // Thread-safe setters (UI thread)
    void setGain(float newGain) noexcept   { gain.store(newGain); }
    void setTone(float newTone) noexcept   { tone.store(newTone); }
    void setVolume(float newVol) noexcept  { volume.store(newVol); }

    // Called from audio thread to process a buffer in-place
    void process(juce::AudioBuffer<float>& buffer);

private:
    std::atomic<float> gain   { 2.0f };
    std::atomic<float> tone   { 0.5f };
    std::atomic<float> volume { 0.8f };

    double fs = 44100.0;
    juce::IIRFilter toneFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpSimProcessor)
};
