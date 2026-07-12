#pragma once

#include <JuceHeader.h>
#include <vector>
#include <array>

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

    // Static layout helper function to generate our parameter map
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // Generates a Euclidean pattern using an optimized division algorithm
    static std::vector<bool> generateEuclideanPattern (int steps, int triggers, int offset);

    // Dynamic drum channel structure containing DSP state
    struct DrumChannel
    {
        std::vector<juce::AudioSampleBuffer> samplePool;
        int currentSampleIndex = 0;
        
        double readPointer = 0.0;
        bool isPlaying = false;
        
        float envLevel = 0.0f;

        void trigger()
        {
            if (samplePool.empty()) 
                return;

            readPointer = 0.0;
            isPlaying = true;
            envLevel = 1.0f; // Reset envelope back to full volume
        }

        void selectRandomSample()
        {
            if (samplePool.empty()) 
                return;

            auto& random = juce::Random::getSystemRandom();
            currentSampleIndex = random.nextInt (static_cast<int> (samplePool.size()));
        }

        const juce::AudioSampleBuffer* getActiveBuffer() const
        {
            if (samplePool.empty() || currentSampleIndex < 0 || currentSampleIndex >= samplePool.size())
                return nullptr;

            return &samplePool[currentSampleIndex];
        }
    };

    // Array of 6 independent drum channels
    std::array<DrumChannel, 6> drumChannels;

    // ValueTree containing all parameter data
    juce::AudioProcessorValueTreeState apvts;

private:
    double currentSampleRate = 44100.0;
    
    // Track the last processed 16th-note step across process blocks
    int lastTotal16thStep = -1;

    // Format manager to read WAV sample data
    juce::AudioFormatManager formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicAudioProcessor)
};
