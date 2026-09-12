#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class DrumChannelComponent : public juce::Component
{
public:
    DrumChannelComponent(TectonicAudioProcessor& processor, int drumIndex);
    ~DrumChannelComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TectonicAudioProcessor& processor;
    int drumIndex;

    juce::Slider pitchSlider;
    juce::Slider decaySlider;
    juce::Slider driveSlider;
    juce::Slider stepsSlider;
    juce::Slider triggersSlider;
    juce::Slider offsetSlider;
    juce::TextButton muteButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stepsAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> triggersAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> offsetAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumChannelComponent)
};
