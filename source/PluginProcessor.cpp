TectonicAudioProcessor::TectonicAudioProcessor()
{
    // Register WAV format reader
    formatManager.registerBasicFormats();

    // 1. Create an input stream reading directly from compiled memory
    auto inputStream = std::make_unique<juce::MemoryInputStream>(
        BinaryData::Drum1_Kick_808_wav, 
        BinaryData::Drum1_Kick_808_wavSize, 
        false
    );

    // 2. Create a format reader for the stream
    std::unique_ptr<juce::AudioFormatReader> reader(
        formatManager.createReaderFor(std::move(inputStream))
    );

    if (reader != nullptr)
    {
        // 3. Size the buffer to match the sample
        kickSampleBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
        
        // 4. Read the raw audio data into our buffer
        reader->read(&kickSampleBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
    }
}
