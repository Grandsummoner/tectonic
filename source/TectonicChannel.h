#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class TectonicChannel : public juce::Component
{
public:
    TectonicChannel (TectonicAudioProcessor& p, int channelIndex, bool isSynthChannel);
    ~TectonicChannel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void setFocusState (bool shouldBeFocused);
    bool getFocusState() const { return isFocused; }

    // Callback to request focus from parent editor
    std::function<void(int)> onFocusRequested;

private:
    void mouseDown (const juce::MouseEvent& event) override;

    TectonicAudioProcessor& processor;
    int index;
    bool isSynth;
    bool isFocused = false;
    bool isMuted = false;

    juce::Label display;
    
    // Two separate sets of sliders to eliminate real-time re-binding crashes
    juce::Slider knobs[3];     // Sound Sliders (Tuning, Decay, Overdrive)
    juce::Slider seqKnobs[3];  // Sequencer Sliders (Steps, Triggers, Offset)
    
    juce::TextButton buttonTop;
    juce::TextButton buttonBottom;

    // Fixed, permanent attachments initialized once in constructor [1.2.1]
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knobAttachments[3];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> seqKnobAttachments[3];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicChannel)
};
