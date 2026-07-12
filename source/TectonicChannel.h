#pragma once
#include <JuceHeader.h>

class TectonicChannel : public juce::Component
{
public:
    TectonicChannel (juce::AudioProcessorValueTreeState& vts, int channelIndex, bool isSynthChannel);
    ~TectonicChannel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void setFocusState (bool shouldBeFocused);

private:
    void updateBindings();

    juce::AudioProcessorValueTreeState& apvts;
    int index;
    bool isSynth;
    bool isFocused = false;

    juce::Label display;
    juce::Slider knobs[3];
    juce::TextButton buttonTop;
    juce::TextButton buttonBottom;

    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicChannel)
};
