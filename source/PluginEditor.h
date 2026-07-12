#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TectonicChannel.h"

class TectonicAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TectonicAudioProcessorEditor (TectonicAudioProcessor&);
    ~TectonicAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int getFocusedChannel() const;

private:
    TectonicAudioProcessor& audioProcessor;
    std::array<std::unique_ptr<TectonicChannel>, 8> channels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicAudioProcessorEditor)
};
