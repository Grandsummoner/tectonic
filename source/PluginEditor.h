#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TectonicChannel.h" // Include our channel class

class TectonicAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TectonicAudioProcessorEditor (TectonicAudioProcessor&);
    ~TectonicAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TectonicAudioProcessor& audioProcessor;
    std::unique_ptr<juce::Drawable> backgroundSvg;

    // Our array of 8 modular channels
    std::array<std::unique_ptr<TectonicChannel>, 8> channels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicAudioProcessorEditor)
};
