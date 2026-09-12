#include "PluginProcessor.h"
#include "PluginEditor.h"

TectonicAudioProcessorEditor::TectonicAudioProcessorEditor(TectonicAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    presetManager = std::make_unique<PresetManager>(processor);

    // Preset selector
    presetSelector.addItemList(presetManager->getPresetList(), 1);
    presetSelector.onChange = [this]() {
        int selectedIdx = presetSelector.getSelectedItemIndex();
        if (selectedIdx >= 0)
        {
            presetManager->loadPreset(selectedIdx);
        }
    };
    addAndMakeVisible(presetSelector);

    // Save preset button
    savePresetButton.setButtonText("SAVE");
    savePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF2ECC71));
    savePresetButton.onClick = [this]() {
        juce::AlertWindow::showAsync(
            juce::AlertWindow::showOkCancelBox(
                juce::AlertWindow::InfoIcon,
                "Save Preset",
                "Enter preset name:",
                "Save", "Cancel",
                nullptr,
                [this](int result) {
                    if (result)
                    {
                        auto name = juce::Time::getCurrentTime().toString(true, true, true, true);
                        presetManager->savePreset(name);
                        presetSelector.addItem(name, presetSelector.getNumItems() + 1);
                        presetSelector.setSelectedItemIndex(presetSelector.getNumItems() - 1);
                    }
                }),
            nullptr);
    };
    addAndMakeVisible(savePresetButton);

    // Load preset button (handled by selector)
    loadPresetButton.setButtonText("DELETE");
    loadPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFE74C3C));
    loadPresetButton.onClick = [this]() {
        int selectedIdx = presetSelector.getSelectedItemIndex();
        if (selectedIdx >= 0)
        {
            presetManager->deletePreset(selectedIdx);
            presetSelector.removeItemAt(selectedIdx);
            if (presetSelector.getNumItems() > 0)
                presetSelector.setSelectedItemIndex(0);
        }
    };
    addAndMakeVisible(loadPresetButton);

    // Preset label
    presetNameLabel.setText("Presets:", juce::dontSendNotification);
    presetNameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(presetNameLabel);

    // Drum channels
    for (int i = 0; i < TectonicAudioProcessor::NUM_DRUMS; ++i)
    {
        drumChannels[i] = std::make_unique<DrumChannelComponent>(processor, i);
        addAndMakeVisible(*drumChannels[i]);
    }

    setSize(1200, 650);
}

TectonicAudioProcessorEditor::~TectonicAudioProcessorEditor() {}

void TectonicAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Dark gradient background
    juce::ColourGradient grad(juce::Colour(0xFF0A0A0A), 0, 0,
                              juce::Colour(0xFF1A1A1A), 0, static_cast<float>(getHeight()),
                              false);
    g.setGradientFill(grad);
    g.fillAll();

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font("Arial", 28.0f, juce::Font::bold));
    g.drawText("TECTONIC", juce::Rectangle<int>(0, 5, getWidth(), 40), juce::Justification::centred);
}

void TectonicAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Top bar with title and presets
    auto topBar = bounds.removeFromTop(50);
    topBar.removeFromLeft(20);
    topBar.removeFromRight(20);

    presetNameLabel.setBounds(topBar.removeFromLeft(70));
    presetSelector.setBounds(topBar.removeFromLeft(150).reduced(2));
    savePresetButton.setBounds(topBar.removeFromLeft(60).reduced(2));
    loadPresetButton.setBounds(topBar.removeFromLeft(60).reduced(2));

    // Drum channels in grid
    auto contentBounds = bounds.reduced(10);
    int colWidth = contentBounds.getWidth() / TectonicAudioProcessor::NUM_DRUMS;

    for (int i = 0; i < TectonicAudioProcessor::NUM_DRUMS; ++i)
    {
        drumChannels[i]->setBounds(contentBounds.getX() + i * colWidth, contentBounds.getY(),
                                    colWidth, contentBounds.getHeight());
    }
}
