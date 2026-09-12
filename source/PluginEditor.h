#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DrumChannelComponent.h"
#include "PresetManager.h"

/**
 * Main editor UI for Tectonic sequencer.
 * Displays 6 drum channels in a grid layout with preset management.
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
    std::unique_ptr<PresetManager> presetManager;

    juce::TextButton savePresetButton;
    juce::TextButton loadPresetButton;
    juce::Label presetNameLabel;
    juce::ComboBox presetSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TectonicAudioProcessorEditor)
};
