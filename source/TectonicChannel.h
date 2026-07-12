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

    // Callback to request focus from parent editor when 7-segment label is clicked
    std::function<void(int)> onFocusRequested;

private:
    void updateBindings();
    void mouseDown (const juce::MouseEvent& event) override;

    TectonicAudioProcessor& processor;
    int index;
    bool isSynth;
    bool isFocused = false;
    bool isMuted = false;

    juce::Label display;
    juce::Slider knobs[3];
    juce::TextButton buttonTop;
    juce::TextButton buttonBottom;

    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicChannel)
};
