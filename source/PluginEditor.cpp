#include "PluginProcessor.h"
#include "PluginEditor.h"

TectonicAudioProcessorEditor::TectonicAudioProcessorEditor(TectonicAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
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
    g.fillAll(juce::Colour(0xFF0A0A0A));
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font("Arial", 28.0f, juce::Font::bold));
    g.drawText("TECTONIC", getLocalBounds().removeFromTop(50),
               juce::Justification::centred);
}

void TectonicAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().removeFromTop(600);
    int colWidth = bounds.getWidth() / TectonicAudioProcessor::NUM_DRUMS;

    for (int i = 0; i < TectonicAudioProcessor::NUM_DRUMS; ++i)
    {
        drumChannels[i]->setBounds(i * colWidth, 50, colWidth, 550);
    }
}
