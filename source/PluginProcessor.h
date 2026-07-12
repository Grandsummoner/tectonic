#pragma once

#include <JuceHeader.h>
#include <vector>
#include <array>
#include <atomic>

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
    static std::vector<bool> generateEuclideanPattern (int steps, int triggers, int offset);

    // Synth Channel Struct (Supports Muting)
    struct SynthChannel
    {
        std::atomic<bool> isMuted { false };
    };

    // Upgraded Drum Channel Struct (Supports Muting & Momentary Fill)
    struct DrumChannel
    {
        std::vector<juce::AudioSampleBuffer> samplePool;
        int currentSampleIndex = 0;
        
        double readPointer = 0.0;
        bool isPlaying = false;
        float envLevel = 0.0f;

        // Thread-safe performance flags
        std::atomic<bool> isMuted { false };
        std::atomic<bool> isFillActive { false };

        void trigger()
        {
            if (samplePool.empty()) 
                return;

            readPointer = 0.0;
            isPlaying = true;
            envLevel = 1.0f;
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

    std::array<SynthChannel, 2> synthChannels;
    std::array<DrumChannel, 6> drumChannels;
    juce::AudioProcessorValueTreeState apvts;

private:
    double currentSampleRate = 44100.0;
    int lastTotal16thStep = -1;
    juce::AudioFormatManager formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicAudioProcessor)
};
