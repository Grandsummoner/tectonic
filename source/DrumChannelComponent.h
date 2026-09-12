#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
 * Single drum channel UI component.
 * Shows:
 * - Sample display with browser
 * - 16-step grid editor
 * - Pattern controls (steps, triggers, offset)
 * - Sound controls (pitch, decay, overdrive)
 */
class DrumChannelComponent : public juce::Component
{
public:
    DrumChannelComponent(TectonicAudioProcessor& processor, int drumIndex);
    ~DrumChannelComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void updateGridState();

private:
    TectonicAudioProcessor& processor;
    int drumIndex;
    bool isFocused = false;

    // UI Controls
    juce::Slider pitchSlider;
    juce::Slider decaySlider;
    juce::Slider driveSlider;
    juce::Slider stepsSlider;
    juce::Slider triggersSlider;
    juce::Slider offsetSlider;
    juce::TextButton muteButton;
    juce::TextButton randomSampleButton;
    juce::TextButton prevSampleButton;
    juce::TextButton nextSampleButton;
    juce::Label sampleDisplay;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stepsAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> triggersAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> offsetAttach;

    // Grid state for 16-step editor
    std::vector<bool> currentPattern;
    int lastStepCount = 16;
    int lastTriggerCount = 4;
    int lastOffsetCount = 0;

    juce::String drumNames[6] = {"KICK", "SNARE", "OH", "CH", "CLAP", "PERC"};
    juce::Colour drumColors[6] = {
        juce::Colour(0xFFFF6B6B), // Red for kick
        juce::Colour(0xFF4ECDC4), // Teal for snare
        juce::Colour(0xFFFFE66D), // Yellow for open hat
        juce::Colour(0xFFA8E6CF), // Mint for closed hat
        juce::Colour(0xFFFF8B94), // Pink for clap
        juce::Colour(0xFFB19CD9)  // Purple for perc
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumChannelComponent)
};
