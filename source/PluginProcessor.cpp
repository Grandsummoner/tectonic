#include "PluginProcessor.h"
#include "PluginEditor.h"

struct BinarySampleResource { const char* dataPtr; const int dataSize; };

void loadBinarySampleToChannel (TectonicAudioProcessor::DrumChannel& channel, juce::AudioFormatManager& formatManager, const BinarySampleResource& resource)
{
    auto inputStream = std::make_unique<juce::MemoryInputStream> (resource.dataPtr, resource.dataSize, false);
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (std::move (inputStream)));
    if (reader != nullptr)
    {
        juce::AudioSampleBuffer newBuffer;
        newBuffer.setSize (reader->numChannels, (int)reader->lengthInSamples);
        reader->read (&newBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
        channel.samplePool.push_back (std::move (newBuffer));
    }
}

TectonicAudioProcessor::TectonicAudioProcessor()
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
{
    formatManager.registerBasicFormats();

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

    for (auto& res : drum1_resources) loadBinarySampleToChannel (drumChannels[0], formatManager, res);
    for (auto& res : drum2_resources) loadBinarySampleToChannel (drumChannels[1], formatManager, res);
    for (auto& res : drum3_resources) loadBinarySampleToChannel (drumChannels[2], formatManager, res);
    for (auto& res : drum4_resources) loadBinarySampleToChannel (drumChannels[3], formatManager, res);
    for (auto& res : drum5_resources) loadBinarySampleToChannel (drumChannels[4], formatManager, res);
    for (auto& res : drum6_resources) loadBinarySampleToChannel (drumChannels[5], formatManager, res);

    for (int i = 0; i < 8; ++i)
    {
        bool isSynth = (i < 2);
        juce::String prefix = isSynth ? "synth" : "drum";
        int chNumber = isSynth ? (i + 1) : (i - 1);
        auto& cp = cachedParams[i];
        cp.param1   = apvts.getRawParameterValue (prefix + juce::String (chNumber) + "_param1");
        cp.param2   = apvts.getRawParameterValue (prefix + juce::String (chNumber) + "_param2");
        cp.param3   = apvts.getRawParameterValue (prefix + juce::String (chNumber) + "_param3");
        cp.steps    = apvts.getRawParameterValue (prefix + juce::String (chNumber) + "_steps");
        cp.triggers = apvts.getRawParameterValue (prefix + juce::String (chNumber) + "_triggers");
        cp.offset   = apvts.getRawParameterValue (prefix + juce::String (chNumber) + "_offset");
    }
}

TectonicAudioProcessor::~TectonicAudioProcessor() {}

float TectonicAudioProcessor::getCachedParam (int channelIndex, int paramType) const
{
    auto& cp = cachedParams[channelIndex];
    std::atomic<float>* ptr = nullptr;
    switch (paramType)
    {
        case 0: ptr = cp.param1;   break;
        case 1: ptr = cp.param2;   break;
        case 2: ptr = cp.param3;   break;
        case 3: ptr = cp.steps;    break;
        case 4: ptr = cp.triggers; break;
        case 5: ptr = cp.offset;   break;
    }
    if (ptr != nullptr) return ptr->load();
    return 0.0f;
}

juce::AudioProcessorValueTreeState::ParameterLayout TectonicAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    for (int i = 1; i <= 2; ++i)
    {
        juce::String prefix = "synth" + juce::String (i);
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param1", 1 }, prefix + " Root Note", 0.0f, 127.0f, 60.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param2", 1 }, prefix + " Scale", 0.0f, 7.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param3", 1 }, prefix + " Note Density", 0.0f, 1.0f, 0.5f));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_steps", 1 }, prefix + " Steps", 1, 16, 16));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_triggers", 1 }, prefix + " Triggers", 1, 16, 4));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_offset", 1 }, prefix + " Offset", 0, 15, 0));
    }
    for (int i = 1; i <= 6; ++i)
    {
        juce::String prefix = "drum" + juce::String (i);
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param1", 1 }, prefix + " Tuning", -12.0f, 12.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param2", 1 }, prefix + " Decay", 0.01f, 2.0f, 0.5f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param3", 1 }, prefix + " Overdrive", 0.0f, 1.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_steps", 1 }, prefix + " Steps", 1, 16, 16));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_triggers", 1 }, prefix + " Triggers", 1, 16,
