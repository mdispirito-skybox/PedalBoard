#pragma once
#include <atomic>
#include <array>
#include "juce_audio_basics/juce_audio_basics.h"

class AmpSimProcessor {
public:
    AmpSimProcessor() = default;
    ~AmpSimProcessor() = default;

    void prepare(double sampleRate);

    void setGain(float newGain) noexcept   { gain.store(newGain); }
    void setTone(float newTone) noexcept   { tone.store(newTone); }
    void setVolume(float newVol) noexcept  { volume.store(newVol); }
    void setPresence(float newPres) noexcept { presence.store(newPres); }

    void process(juce::AudioBuffer<float>& buffer);

private:
    std::atomic<float> gain   { 2.0f };
    std::atomic<float> tone   { 0.5f };
    std::atomic<float> volume { 0.8f };
    std::atomic<float> presence { 0.5f };

    double fs = 44100.0;
    std::array<juce::IIRFilter, 2> toneFilters;
    std::array<juce::IIRFilter, 2> presenceFilters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpSimProcessor)
};
