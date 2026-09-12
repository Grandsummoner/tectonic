#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
 * Single drum channel UI component.
 * Shows:
 * - Sample display
 * - Pattern controls (steps, triggers, offset)
 * - Sound controls (pitch, decay, overdrive)
 * - 16-step grid editor
 */
class DrumChannelComponent : public juce::Component
{
public:
    DrumChannelComponent(TectonicAudioProcessor& processor, int drumIndex);
    ~DrumChannelComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

private:
    TectonicAudioProcessor& processor;
    int drumIndex;
    bool isFocused = false;
    bool isGridMode = false;

    // UI Controls
    juce::Slider pitchSlider;
    juce::Slider decaySlider;
    juce::Slider driveSlider;
    juce::Slider stepsSlider;
    juce::Slider triggersSlider;
    juce::Slider offsetSlider;
    juce::TextButton muteButton;
    juce::TextButton randomSampleButton;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stepsAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> triggersAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> offsetAttach;

    juce::String drumNames[6] = {"KICK", "SNARE", "OH", "CH", "CLAP", "PERC"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumChannelComponent)
};
