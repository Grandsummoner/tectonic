#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
 * Manages preset saving/loading for Tectonic.
 * Stores presets as XML in Application support directory.
 */
class PresetManager
{
public:
    explicit PresetManager(TectonicAudioProcessor& processor);
    ~PresetManager() = default;

    void savePreset(const juce::String& presetName);
    void loadPreset(int presetIndex);
    void deletePreset(int presetIndex);
    juce::StringArray getPresetList() const { return presetNames; }

private:
    TectonicAudioProcessor& processor;
    juce::StringArray presetNames;
    juce::File presetsDirectory;

    void scanPresetsDirectory();
    juce::File getPresetFile(const juce::String& presetName) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
