#pragma once

#include <JuceHeader.h>

class TectonicAudioProcessor  : public juce::AudioProcessor
{
public:
    TectonicAudioProcessor();
    ~TectonicAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // Generates a Euclidean pattern using an optimized division algorithm
    static std::vector<bool> generateEuclideanPattern (int steps, int triggers, int offset);

    juce::AudioProcessorValueTreeState apvts;

private:
    double currentSampleRate = 44100.0;
    
    // Track the last processed 16th-note step across process blocks
    int lastTotal16thStep = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicAudioProcessor)
};
