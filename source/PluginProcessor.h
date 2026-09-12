#pragma once

#include <JuceHeader.h>
#include <vector>
#include <array>
#include <atomic>
#include <memory>

/**
 * Core audio processor for Tectonic drum sequencer.
 * Handles:
 * - APVTS parameter management
 * - Euclidean rhythm pattern generation
 * - Audio buffer playback and mixing
 * - MIDI/playhead synchronization
 */
class TectonicAudioProcessor : public juce::AudioProcessor
{
public:
    TectonicAudioProcessor();
    ~TectonicAudioProcessor() override;

    // Standard AudioProcessor methods
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override;
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // APVTS and parameters
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Rhythm generation
    static std::vector<bool> generateEuclideanPattern(int steps, int triggers, int offset);

    // Drum channel definitions
    static constexpr int NUM_DRUMS = 6;
    static constexpr int NUM_SAMPLES_PER_DRUM = 4;

    struct DrumSample
    {
        juce::AudioSampleBuffer buffer;
        int sampleIndex = 0;
        double readPointer = 0.0;
        bool isPlaying = false;
        float envLevel = 0.0f;
        bool isMuted = false;
    };

    struct DrumChannel
    {
        std::array<juce::AudioSampleBuffer, NUM_SAMPLES_PER_DRUM> samples;
        std::atomic<int> currentSampleIndex{0};
        std::atomic<double> readPointer{0.0};
        std::atomic<bool> isPlaying{false};
        std::atomic<float> envLevel{0.0f};
        std::atomic<bool> isMuted{false};

        void trigger()
        {
            if (samples[0].getNumSamples() == 0)
                return;
            readPointer.store(0.0);
            isPlaying.store(true);
            envLevel.store(1.0f);
        }

        void selectRandomSample()
        {
            auto& random = juce::Random::getSystemRandom();
            currentSampleIndex.store(random.nextInt(NUM_SAMPLES_PER_DRUM));
        }

        const juce::AudioSampleBuffer* getActiveBuffer() const
        {
            int idx = currentSampleIndex.load();
            if (idx < 0 || idx >= NUM_SAMPLES_PER_DRUM)
                return nullptr;
            return &samples[idx];
        }
    };

    std::array<DrumChannel, NUM_DRUMS> drumChannels;

private:
    double currentSampleRate = 44100.0;
    int lastTotal16thStep = -1;
    juce::AudioFormatManager formatManager;

    void loadDrumSamples();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TectonicAudioProcessor)
};
