#pragma once
#include <atomic>
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_dsp/juce_dsp.h"

class CabSimProcessor {
public:
    CabSimProcessor() = default;
    ~CabSimProcessor() = default;

    void prepare(double sampleRate, int maximumBlockSize, int numChannels);
    void process(juce::AudioBuffer<float>& buffer);
    void setBypassed(bool bypass) noexcept { 
        isBypassed.store(bypass); 
    }

    void loadImpulseResponse(juce::AudioBuffer<float>&& rawIR, double sampleRate);

private:
    juce::dsp::Convolution convolution;
    std::atomic<bool> isBypassed { false };
    
    juce::AudioBuffer<float> irData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabSimProcessor)
};