class TectonicAudioProcessor : public juce::AudioProcessor
{
public:
    TectonicAudioProcessor();
    // ... other standard processor functions ...

private:
    juce::AudioFormatManager formatManager;
    
    // An AudioSampleBuffer to hold the loaded sound in RAM
    juce::AudioSampleBuffer kickSampleBuffer; 
};
