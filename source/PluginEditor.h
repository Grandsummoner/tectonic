#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class TectonicAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TectonicAudioProcessorEditor (TectonicAudioProcessor&);
    ~TectonicAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TectonicAudioProcessor& audioProcessor;

    // A smart pointer to hold our parsed SVG panel
    std::unique_ptr<juce::Drawable> backgroundSvg;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicAudioProcessorEditor)
};
