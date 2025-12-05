#pragma once
#include <atomic>
#include <array>
#include <cmath>
#include "juce_audio_basics/juce_audio_basics.h"

class FuzzProcessor {
public:
    FuzzProcessor() = default;
    ~FuzzProcessor() = default;

    void prepare(double sampleRate);
    void process(juce::AudioBuffer<float>& buffer);

    // Parameters
    void setSustain(float newSustain) { sustain.store(newSustain); }
    void setTone(float newTone)       { tone.store(newTone); }
    void setVolume(float newVol)      { volume.store(newVol); }
    void setBypassed(bool newBypassed){ isBypassed.store(newBypassed); }

private:
    std::atomic<float> sustain    { 0.5f };
    std::atomic<float> tone       { 0.5f };
    std::atomic<float> volume     { 0.5f };
    std::atomic<bool>  isBypassed { true };

    double fs = 44100.0;

    std::array<juce::IIRFilter, 2> lowPassFilters;
    std::array<juce::IIRFilter, 2> highPassFilters;
};