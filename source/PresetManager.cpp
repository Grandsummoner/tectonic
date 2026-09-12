#include "PresetManager.h"

PresetManager::PresetManager(TectonicAudioProcessor& p) : processor(p)
{
    // Get application support directory
#if JUCE_WINDOWS
    presetsDirectory = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                           .getChildFile("Tectonic")
                           .getChildFile("Presets");
#elif JUCE_MAC
    presetsDirectory = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                           .getChildFile("Tectonic")
                           .getChildFile("Presets");
#else
    presetsDirectory = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                           .getChildFile("Tectonic")
                           .getChildFile("Presets");
#endif

    if (!presetsDirectory.exists())
        presetsDirectory.createDirectory();

    scanPresetsDirectory();
}

void PresetManager::scanPresetsDirectory()
{
    presetNames.clear();

    if (!presetsDirectory.exists())
        return;

    auto files = presetsDirectory.findChildFiles(juce::File::findFiles, false, "*.tectonic");

    for (const auto& file : files)
    {
        presetNames.add(file.getFileNameWithoutExtension());
    }

    // Add default presets if directory is empty
    if (presetNames.isEmpty())
    {
        presetNames.add("Default");
        savePreset("Default");
    }
}

juce::File PresetManager::getPresetFile(const juce::String& presetName) const
{
    return presetsDirectory.getChildFile(presetName + ".tectonic");
}

void PresetManager::savePreset(const juce::String& presetName)
{
    auto presetFile = getPresetFile(presetName);

    // Get current state from processor
    juce::MemoryBlock stateData;
    processor.getStateInformation(stateData);

    // Create XML wrapper
    auto xml = std::make_unique<juce::XmlElement>("TectonicPreset");
    xml->setAttribute("name", presetName);
    xml->setAttribute("version", "1.0");
    xml->setAttribute("timestamp", juce::Time::getCurrentTime().toMilliseconds());

    // Store state as base64
    xml->setAttribute("state", stateData.toBase64Encoding());

    // Write to file
    if (xml->writeToFile(presetFile, juce::String()))
    {
        if (!presetNames.contains(presetName))
            presetNames.add(presetName);
    }
}

void PresetManager::loadPreset(int presetIndex)
{
    if (presetIndex < 0 || presetIndex >= presetNames.size())
        return;

    auto presetFile = getPresetFile(presetNames[presetIndex]);
    auto xml = std::unique_ptr<juce::XmlElement>(juce::XmlDocument::parse(presetFile));

    if (xml != nullptr && xml->hasAttribute("state"))
    {
        juce::MemoryBlock stateData;
        if (stateData.fromBase64Encoding(xml->getStringAttribute("state")))
        {
            processor.setStateInformation(stateData.getData(), stateData.getSize());
        }
    }
}

void PresetManager::deletePreset(int presetIndex)
{
    if (presetIndex < 0 || presetIndex >= presetNames.size())
        return;

    auto presetFile = getPresetFile(presetNames[presetIndex]);
    presetFile.deleteFile();
    presetNames.remove(presetIndex);
}
