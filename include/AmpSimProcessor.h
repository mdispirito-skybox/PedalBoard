#pragma once
#include <atomic>
#include <array>
#include <cmath>
#include "juce_audio_basics/juce_audio_basics.h"

class AmpSimProcessor {
public:
    AmpSimProcessor() = default;
    ~AmpSimProcessor() = default;

    void prepare(double sampleRate);

    void setGain(float newGain) noexcept   { gain.store(newGain); }
    void setBass(float newBass) noexcept   { bass.store(newBass); }
    void setTreble(float newTreb) noexcept { treble.store(newTreb); }
    void setVolume(float newVol) noexcept  { volume.store(newVol); }

    void process(juce::AudioBuffer<float>& buffer);

private:
    std::atomic<float> gain   { 0.5f };
    std::atomic<float> bass   { 0.5f };
    std::atomic<float> treble { 0.5f };
    std::atomic<float> volume { 0.8f };

    double fs = 44100.0;
    std::array<juce::IIRFilter, 2> preFilters;
    std::array<juce::IIRFilter, 2> bassFilters;
    std::array<juce::IIRFilter, 2> trebleFilters;

    std::array<float, 2> dcBlockerState { 0.0f, 0.0f };
    std::array<float, 2> lastInputSample { 0.0f, 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpSimProcessor)
};
