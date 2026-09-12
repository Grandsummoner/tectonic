#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DrumChannelComponent.h"

/**
 * Main editor UI for Tectonic sequencer.
 * Displays 6 drum channels in a grid layout with pattern editing.
 */
class TectonicAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit TectonicAudioProcessorEditor(TectonicAudioProcessor&);
    ~TectonicAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TectonicAudioProcessor& processor;
    std::array<std::unique_ptr<DrumChannelComponent>, TectonicAudioProcessor::NUM_DRUMS> drumChannels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TectonicAudioProcessorEditor)
};
