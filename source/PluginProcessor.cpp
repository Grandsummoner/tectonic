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
        
        // Sound controls: Root (0-127 MIDI pitch), Scale (0-7 selection), Note Density (0-1 probability)
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param1", 1 }, prefix + " Root Note", 0.0f, 127.0f, 60.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param2", 1 }, prefix + " Scale", 0.0f, 7.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param3", 1 }, prefix + " Note Density", 0.0f, 1.0f, 0.5f));

        // Euclidean controls
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_steps", 1 }, prefix + " Steps", 1, 16, 16));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_triggers", 1 }, prefix + " Triggers", 1, 16, 4));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_offset", 1 }, prefix + " Offset", 0, 15, 0));
    }

    // 2. Drums (1 to 6)
    for (int i = 1; i <= 6; ++i)
    {
        juce::String prefix = "drum" + juce::String (i);
        
        // Sound controls: Tuning (-12 to +12 semitones), Decay (10ms to 2s), Overdrive (0 to 1)
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param1", 1 }, prefix + " Tuning", -12.0f, 12.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param2", 1 }, prefix + " Decay", 0.01f, 2.0f, 0.5f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { prefix + "_param3", 1 }, prefix + " Overdrive", 0.0f, 1.0f, 0.0f));

        // Euclidean controls
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_steps", 1 }, prefix + " Steps", 1, 16, 16));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_triggers", 1 }, prefix + " Triggers", 1, 16, 4));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { prefix + "_offset", 1 }, prefix + " Offset", 0, 15, 0));
    }

    return { params.begin(), params.end() };
}

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

void TectonicAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {}
void TectonicAudioProcessor::releaseResources() {}

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

void TectonicAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
}

bool TectonicAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* TectonicAudioProcessor::createEditor() { return new TectonicAudioProcessorEditor (*this); }

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

// BinaryCreator instantiation entry point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new TectonicAudioProcessor(); }
