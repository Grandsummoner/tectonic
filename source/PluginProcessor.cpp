// Helper struct to represent a compiled memory resource
struct BinarySampleResource
{
    const char* dataPtr;
    const int dataSize;
};

// Helper function to read a compiled binary resource and push it into a channel's pool
void loadBinarySampleToChannel (TectonicAudioProcessor::DrumChannel& channel, 
                                juce::AudioFormatManager& formatManager,
                                const BinarySampleResource& resource)
{
    auto inputStream = std::make_unique<juce::MemoryInputStream> (resource.dataPtr, resource.dataSize, false);
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (std::move (inputStream)));

    if (reader != nullptr)
    {
        juce::AudioSampleBuffer newBuffer;
        newBuffer.setSize (reader->numChannels, (int)reader->lengthInSamples);
        reader->read (&newBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
        
        // Push the buffer into this channel's dynamic pool
        channel.samplePool.push_back (std::move (newBuffer));
    }
}

TectonicAudioProcessor::TectonicAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if JucePlugin_IsSynth
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      #else
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      #endif
                     #endif
                       ),
       apvts (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    formatManager.registerBasicFormats();

    // Grouping sample compiled data variables into clean arrays for loop mapping [1.2.1]
    const BinarySampleResource drum1_resources[] = {
        { BinaryData::D1_Kick_1_wav, BinaryData::D1_Kick_1_wavSize },
        { BinaryData::D1_Kick_2_wav, BinaryData::D1_Kick_2_wavSize },
        { BinaryData::D1_Kick_3_wav, BinaryData::D1_Kick_3_wavSize },
        { BinaryData::D1_Kick_4_wav, BinaryData::D1_Kick_4_wavSize }
    };

    const BinarySampleResource drum2_resources[] = {
        { BinaryData::D2_Snare_1_wav, BinaryData::D2_Snare_1_wavSize },
        { BinaryData::D2_Snare_2_wav, BinaryData::D2_Snare_2_wavSize },
        { BinaryData::D2_Snare_3_wav, BinaryData::D2_Snare_3_wavSize },
        { BinaryData::D2_Snare_4_wav, BinaryData::D2_Snare_4_wavSize }
    };

    const BinarySampleResource drum3_resources[] = {
        { BinaryData::D3_OHH_1_wav, BinaryData::D3_OHH_1_wavSize },
        { BinaryData::D3_OHH_2_wav, BinaryData::D3_OHH_2_wavSize },
        { BinaryData::D3_OHH_3_wav, BinaryData::D3_OHH_3_wavSize },
        { BinaryData::D3_OHH_4_wav, BinaryData::D3_OHH_4_wavSize }
    };

    const BinarySampleResource drum4_resources[] = {
        { BinaryData::D4_CHH_1_wav, BinaryData::D4_CHH_1_wavSize },
        { BinaryData::D4_CHH_2_wav, BinaryData::D4_CHH_2_wavSize },
        { BinaryData::D4_CHH_3_wav, BinaryData::D4_CHH_3_wavSize },
        { BinaryData::D4_CHH_4_wav, BinaryData::D4_CHH_4_wavSize }
    };

    const BinarySampleResource drum5_resources[] = {
        { BinaryData::D5_Clap_1_wav, BinaryData::D5_Clap_1_wavSize },
        { BinaryData::D5_Clap_2_wav, BinaryData::D5_Clap_2_wavSize },
        { BinaryData::D5_Clap_3_wav, BinaryData::D5_Clap_3_wavSize },
        { BinaryData::D5_Clap_4_wav, BinaryData::D5_Clap_4_wavSize }
    };

    const BinarySampleResource drum6_resources[] = {
        { BinaryData::D6_Bell_1_wav, BinaryData::D6_Bell_1_wavSize },
        { BinaryData::D6_Bell_2_wav, BinaryData::D6_Bell_2_wavSize },
        { BinaryData::D6_Bell_3_wav, BinaryData::D6_Bell_3_wavSize },
        { BinaryData::D6_Bell_4_wav, BinaryData::D6_Bell_4_wavSize }
    };

    // Load each resource array into its corresponding drum channel slot
    for (auto& res : drum1_resources) loadBinarySampleToChannel (drumChannels[0], formatManager, res);
    for (auto& res : drum2_resources) loadBinarySampleToChannel (drumChannels[1], formatManager, res);
    for (auto& res : drum3_resources) loadBinarySampleToChannel (drumChannels[2], formatManager, res);
    for (auto& res : drum4_resources) loadBinarySampleToChannel (drumChannels[3], formatManager, res);
    for (auto& res : drum5_resources) loadBinarySampleToChannel (drumChannels[4], formatManager, res);
    for (auto& res : drum6_resources) loadBinarySampleToChannel (drumChannels[5], formatManager, res);
}
