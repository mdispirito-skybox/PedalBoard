#include "CabSimProcessor.h"
#include "juce_core/system/juce_CompilerWarnings.h"

void CabSimProcessor::prepare(double sampleRate, int maximumBlockSize, int numChannels) {
    convolution.prepare({ sampleRate, (juce::uint32)maximumBlockSize, (juce::uint32)2 }); 
    convolution.reset();
    irData.clear();
}

void CabSimProcessor::process(juce::AudioBuffer<float>& buffer) {
    if (isBypassed.load()) {
        return;
    }
    juce::dsp::AudioBlock<float> block(buffer);
  
    convolution.process(juce::dsp::ProcessContextReplacing<float>(block));
}

void CabSimProcessor::loadImpulseResponse(juce::AudioBuffer<float>&& rawIR, double sampleRate) {
    if (rawIR.getNumSamples() == 0) {
        return;
    }

    convolution.loadImpulseResponse(std::move(rawIR), sampleRate, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, juce::dsp::Convolution::Normalise::no);
}