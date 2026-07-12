#include "PluginProcessor.h"
#include "PluginEditor.h"

struct BinarySampleResource
{
    const char* dataPtr;
    const int dataSize;
};

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

    // Pre-cache all 48 parameters to avoid heap allocations/string hashes inside processBlock [43]
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
    // Retrieves pre-cached atomic floats in O(1) time without locks or allocations [43]
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

    if (ptr != nullptr)
        return ptr->load();

    return 0.0f;
}

juce::AudioProcessorValueTreeState::ParameterLayout TectonicAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // 1. Synthesizers (1 to 2)
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

    // 2. Drums (1 to 6)
    for (int i = 1; i <= 6; ++i)
    {
        juce::String prefix = "drum" + juce::String (i);
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param1", 1 }, prefix + " Tuning", -12.0f, 12.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param2", 1 }, prefix + " Decay", 0.01f, 2.0f, 0.5f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param3", 1 }, prefix + " Overdrive", 0.0f, 1.0f, 0.0f));

        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_steps", 1 }, prefix + " Steps", 1, 16, 16));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_triggers", 1 }, prefix + " Triggers", 1, 16, 4));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_offset", 1 }, prefix + " Offset", 0, 15, 0));
    }

    return { params.begin(), params.end() };
}

std::vector<bool> TectonicAudioProcessor::generateEuclideanPattern (int steps, int triggers, int offset)
{
    std::vector<bool> pattern (steps, false);
    if (steps <= 0 || triggers <= 0) return pattern;
    if (triggers > steps) triggers = steps;

    for (int i = 0; i < steps; ++i)
    {
        if ((i * triggers) % steps < triggers)
            pattern[i] = true;
    }

    if (offset > 0 && steps > 0)
        std::rotate (pattern.rbegin(), pattern.rbegin() + (offset % steps), pattern.rend());

    return pattern;
}

void TectonicAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    lastTotal16thStep = -1;
}

void TectonicAudioProcessor::releaseResources() {}

void TectonicAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    bool sequencerTriggeredThisBlock = false;
    double bpm = 120.0;
    double startPpq16th = 0.0;
    double ppq16thPerSample = 0.0;

    if (auto* playHead = getPlayHead())
    {
        auto positionInfo = playHead->getPosition();
        if (positionInfo.hasValue() && positionInfo->getIsPlaying())
        {
            bpm = positionInfo->getBpm().hasValue() ? *(positionInfo->getBpm()) : 120.0;
            auto ppqPosition = positionInfo->getPpqPosition().hasValue() ? *(positionInfo->getPpqPosition()) : 0.0;

            double ppqPerSample = (bpm / 60.0) / currentSampleRate;
            ppq16thPerSample = ppqPerSample * 4.0; 
            startPpq16th = ppqPosition * 4.0;
            sequencerTriggeredThisBlock = true;
        }
    }

    for (int sampleIdx = 0; sampleIdx < buffer.getNumSamples(); ++sampleIdx)
    {
        float leftMixSum = 0.0f;
        float rightMixSum = 0.0f;

        if (sequencerTriggeredThisBlock)
        {
            double currentPpq16th = startPpq16th + (sampleIdx * ppq16thPerSample);
            int currentTotalStep = static_cast<int> (std::floor (currentPpq16th));

            if (currentTotalStep != lastTotal16thStep)
            {
                lastTotal16thStep = currentTotalStep;

                // A. Synths
                for (int synthIdx = 1; synthIdx <= 2; ++synthIdx)
                {
                    int chanIdx = synthIdx - 1;
                    auto& syn = synthChannels[chanIdx];
                    if (syn.isMuted.load()) 
                        continue;

                    // Allocation-free pre-cached parameter fetches
                    int steps    = static_cast<int> (getCachedParam (chanIdx, 3));
                    int triggers = static_cast<int> (getCachedParam (chanIdx, 4));
                    int offset   = static_cast<int> (getCachedParam (chanIdx, 5));

                    if (steps > 0)
                    {
                        auto pattern = generateEuclideanPattern (steps, triggers, offset);
                        if (!pattern.empty())
                        {
                            if (pattern[currentTotalStep % steps])
                            {
                                float rootNote = getCachedParam (chanIdx, 0);
                                float density  = getCachedParam (chanIdx, 2);

                                if (juce::Random::getSystemRandom().nextFloat() <= density)
                                {
                                    int noteToPlay = static_cast<int> (rootNote);
                                    midiMessages.addEvent (juce::MidiMessage::noteOn (1, noteToPlay, 0.8f), sampleIdx);
                                    midiMessages.addEvent (juce::MidiMessage::noteOff (1, noteToPlay), sampleIdx + 2000);
                                }
                            }
                        }
                    }
                }

                // B. Drums
                for (int drumIdx = 1; drumIdx <= 6; ++drumIdx)
                {
                    int chanIdx = drumIdx + 1; // Maps Drum 1 to 6 (indices 2 to 7)
                    auto& drum = drumChannels[drumIdx - 1];
                    if (drum.isMuted.load()) 
                        continue;

                    // Allocation-free pre-cached parameter fetches
                    int steps    = static_cast<int> (getCachedParam (chanIdx, 3));
                    int triggers = static_cast<int> (getCachedParam (chanIdx, 4));
                    int offset   = static_cast<int> (getCachedParam (chanIdx, 5));

                    if (steps > 0)
                    {
                        auto pattern = generateEuclideanPattern (steps, triggers, offset);
                        if (!pattern.empty())
                        {
                            bool shouldTrigger = pattern[currentTotalStep % steps];

                            if (drum.isFillActive.load())
                                shouldTrigger = !shouldTrigger;

                            if (shouldTrigger)
                            {
                                drum.trigger();
                            }
                        }
                    }
                }
            }
        }

        for (int d = 0; d < 6; ++d)
        {
            auto& chan = drumChannels[d];
            if (!chan.isPlaying) 
                continue;

            const auto* sampleBuffer = chan.getActiveBuffer();
            if (sampleBuffer == nullptr) 
                continue;

            int numFrames = sampleBuffer->getNumSamples();
            int chanIdx = d + 2; // Maps Drum 1 to 6 (indices 2 to 7)

            // Read pre-cached real-time parameters [43]
            float tuning    = getCachedParam (chanIdx, 0);
            float decay     = getCachedParam (chanIdx, 1);
            float overdrive = getCachedParam (chanIdx, 2);

            double speedRatio = std::pow (2.0, tuning / 12.0);

            double decayTimeSamples = decay * currentSampleRate;
            float decayCoeff = (decayTimeSamples > 0.0) ? std::exp (-1.0f / static_cast<float> (decayTimeSamples)) : 0.0f;

            int idxInt = static_cast<int> (chan.readPointer);
            float fraction = static_cast<float> (chan.readPointer - idxInt);

            if (idxInt >= numFrames - 1)
            {
                chan.isPlaying = false;
                chan.readPointer = 0.0;
                chan.envLevel = 0.0f;
                continue;
            }

            int numSrcChans = sampleBuffer->getNumChannels();
            float leftSample = 0.0f;
            float rightSample = 0.0f;

            float s0_l = sampleBuffer->getSample (0, idxInt);
            float s1_l = sampleBuffer->getSample (0, idxInt + 1);
            leftSample = s0_l + fraction * (s1_l - s0_l);

            if (numSrcChans > 1)
            {
                float s0_r = sampleBuffer->getSample (1, idxInt);
                float s1_r = sampleBuffer->getSample (1, idxInt + 1);
                rightSample = s0_r + fraction * (s1_r - s0_r);
            }
            else
            {
                rightSample = leftSample;
            }

            leftSample *= chan.envLevel;
            rightSample *= chan.envLevel;

            chan.envLevel *= decayCoeff;
            
            if (chan.envLevel < 0.0005f)
            {
                chan.isPlaying = false;
                chan.envLevel = 0.0f;
            }

            if (overdrive > 0.01f)
            {
                float driveGain = 1.0f + (overdrive * 6.0f);
                
                leftSample *= driveGain;
                rightSample *= driveGain;

                auto softClip = [] (float x) {
                    if (x > 1.25f) return 1.0f;
                    if (x < -1.25f) return -1.0f;
                    return x - (x * x * x) * 0.15f;
                };

                leftSample = softClip (leftSample) / std::sqrt (driveGain);
                rightSample = softClip (rightSample) / std::sqrt (driveGain);
            }

            leftMixSum += leftSample;
            rightMixSum += rightSample;

            chan.readPointer += speedRatio;
        }

        if (buffer.getNumChannels() > 0) buffer.addSample (0, sampleIdx, leftMixSum);
        if (buffer.getNumChannels() > 1) buffer.addSample (1, sampleIdx, rightMixSum);
    }
}

bool TectonicAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* TectonicAudioProcessor::createEditor() { return new TectonicAudioProcessorEditor (*this); }

const juce::String TectonicAudioProcessor::getName() const { return JucePlugin_Name; }
bool TectonicAudioProcessor::acceptsMidi() const { return true; }
bool TectonicAudioProcessor::producesMidi() const { return true; }
bool TectonicAudioProcessor::isMidiEffect() const { return false; }
double TectonicAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int TectonicAudioProcessor::getNumPrograms() { return 1; }
int TectonicAudioProcessor::getCurrentProgram() { return 0; }
void TectonicAudioProcessor::setCurrentProgram (int index) {}
const juce::String TectonicAudioProcessor::getProgramName (int index) { return {}; }
void TectonicAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void TectonicAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void TectonicAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new TectonicAudioProcessor(); }
