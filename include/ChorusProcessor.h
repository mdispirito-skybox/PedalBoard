#pragma once
#include <atomic>
#include <vector>
#include <cmath>
#include "juce_audio_basics/juce_audio_basics.h"

class ChorusProcessor {
public:
    ChorusProcessor() = default;
    ~ChorusProcessor() = default;

    void prepare(double sampleRate);
    void process(juce::AudioBuffer<float>& buffer);

    // Parameters
    void setRate(float newRate)   { rate.store(newRate); }
    void setDepth(float newDepth) { depth.store(newDepth); }
    void setBypassed(bool b)      { isBypassed.store(b); }

private:
    std::atomic<float> rate       { 0.5f };
    std::atomic<float> depth      { 0.5f };
    std::atomic<bool>  isBypassed { true };

    double fs = 44100.0;

    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;
    float lfoPhase = 0.0f;

    float getInterpolatedSample(const juce::AudioBuffer<float>& buffer, int channel, float readPosition);
};