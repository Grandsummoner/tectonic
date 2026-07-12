#include "PluginProcessor.h"
#include "PluginEditor.h"

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
}

TectonicAudioProcessor::~TectonicAudioProcessor()
{
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

// Highly efficient division-based Euclidean pattern generator
std::vector<bool> TectonicAudioProcessor::generateEuclideanPattern (int steps, int triggers, int offset)
{
    std::vector<bool> pattern (steps, false);
    if (steps <= 0 || triggers <= 0) return pattern;
    if (triggers > steps) triggers = steps;

    for (int i = 0; i < steps; ++i)
    {
        // Spreads triggers as evenly as mathematically possible across step length [1]
        if ((i * triggers) % steps < triggers)
        {
            pattern[i] = true;
        }
    }

    // Rotate pattern by offset amount
    if (offset > 0 && steps > 0)
    {
        std::rotate (pattern.rbegin(), pattern.rbegin() + (offset % steps), pattern.rend());
    }

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

    // 1. Check if the playhead clock is active
    if (auto* playHead = getPlayHead())
    {
        auto positionInfo = playHead->getPosition();
        if (positionInfo.hasValue() && positionInfo->getIsPlaying())
        {
            auto bpm = positionInfo->getBpm().value_or (120.0);
            auto ppqPosition = positionInfo->getPpqPosition().value_or (0.0);

            // 2. Convert standard PPQ (Quarter note length) to 16th notes [1]
            double ppqPerSample = (bpm / 60.0) / currentSampleRate;
            double ppq16thPerSample = ppqPerSample * 4.0; // 4 steps per quarter note

            double startPpq16th = ppqPosition * 4.0;

            // 3. Step through the sample buffer to schedule sample-accurate events [1.2.1]
            for (int sampleIdx = 0; sampleIdx < buffer.getNumSamples(); ++sampleIdx)
            {
                double currentPpq16th = startPpq16th + (sampleIdx * ppq16thPerSample);
                int currentTotalStep = static_cast<int> (std::floor (currentPpq16th));

                // 4. Trigger transition if a new 16th-note step boundaries are crossed
                if (currentTotalStep != lastTotal16thStep)
                {
                    lastTotal16thStep = currentTotalStep;

                    // A. Process Synth 1 and Synth 2 triggers
                    for (int synthIdx = 1; synthIdx <= 2; ++synthIdx)
                    {
                        juce::String prefix = "synth" + juce::String (synthIdx);
                        
                        int steps = *apvts.getRawParameterValue (prefix + "_steps");
                        int triggers = *apvts.getRawParameterValue (prefix + "_triggers");
                        int offset = *apvts.getRawParameterValue (prefix + "_offset");

                        auto pattern = generateEuclideanPattern (steps, triggers, offset);
                        int activeStep = currentTotalStep % steps;

                        if (pattern[activeStep])
                        {
                            // Calculate generative note data based on Root, Scale and Density parameters
                            float rootNote = *apvts.getRawParameterValue (prefix + "_param1");
                            float density = *apvts.getRawParameterValue (prefix + "_param3");

                            // Simple probability filter based on Density parameter
                            if (juce::Random::getSystemRandom().nextFloat() <= density)
                            {
                                int noteToPlay = static_cast<int> (rootNote);
                                
                                // Create sample-accurate MIDI event queue outputs [1.2.1]
                                auto noteOnMsg = juce::MidiMessage::noteOn (1, noteToPlay, 0.8f);
                                midiMessages.addEvent (noteOnMsg, sampleIdx);

                                // Schedule corresponding note-off event
                                auto noteOffMsg = juce::MidiMessage::noteOff (1, noteToPlay);
                                midiMessages.addEvent (noteOffMsg, sampleIdx + 2000); // Short note-length
                            }
                        }
                    }

                    // B. Process Drum 1 to Drum 6 triggers
                    for (int drumIdx = 1; drumIdx <= 6; ++drumIdx)
                    {
                        juce::String prefix = "drum" + juce::String (drumIdx);

                        int steps = *apvts.getRawParameterValue (prefix + "_steps");
                        int triggers = *apvts.getRawParameterValue (prefix + "_triggers");
                        int offset = *apvts.getRawParameterValue (prefix + "_offset");

                        auto pattern = generateEuclideanPattern (steps, triggers, offset);
                        int activeStep = currentTotalStep % steps;

                        if (pattern[activeStep])
                        {
                            // Trigger drum sample voice inside the audio playback engine
                            // (We will write the sample voice player triggers in the next step)
                        }
                    }
                }
            }
        }
    }
}

bool TectonicAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutput() != juce::AudioChannelSet::mono()
     && layouts.getMainOutput() != juce::AudioChannelSet::stereo())
        return false;
   #if ! JucePlugin_IsSynth
    if (layouts.getMainInput() != layouts.getMainOutput())
        return false;
   #endif
    return true;
  #endif
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
