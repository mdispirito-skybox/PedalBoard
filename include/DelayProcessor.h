#pragma once
#include <atomic>
#include <vector>
#include <array>
#include "juce_audio_basics/juce_audio_basics.h"

class DelayProcessor {
public:
    DelayProcessor() = default;
    ~DelayProcessor() = default;

    void prepare(double sampleRate);
    void process(juce::AudioBuffer<float>& buffer);

    // Parameters
    void setTime(float newTime)     { time.store(newTime); }
    void setFeedback(float newFb)   { feedback.store(newFb); }
    void setMix(float newMix)       { mix.store(newMix); }
    void setBypassed(bool b)        { isBypassed.store(b); }

private:
    std::atomic<float> time       { 0.3f };
    std::atomic<float> feedback   { 0.4f };
    std::atomic<float> mix        { 0.3f };
    std::atomic<bool>  isBypassed { true };

    double fs = 44100.0;

    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;
    std::array<juce::IIRFilter, 2> feedbackFilters;

    float getInterpolatedSample(const juce::AudioBuffer<float>& buffer, int channel, float readPosition);
};