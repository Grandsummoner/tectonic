#include "PluginProcessor.h"
#include "PluginEditor.h"

TectonicAudioProcessorEditor::TectonicAudioProcessorEditor (TectonicAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Fix layout scale (matching SVG coordinate ratio)
    setSize (800, 600);

    // Load background SVG
    auto xml = juce::XmlDocument::parse (BinaryData::tectonic_panel_svg);
    if (xml != nullptr)
        backgroundSvg = juce::Drawable::createFromSVG (*xml);

    // Initialize our 8 channels
    for (int i = 0; i < 8; ++i)
    {
        // First 2 are Synths, remaining 6 are Drums
        bool isSynth = (i < 2);
        
        channels[i] = std::make_unique<TectonicChannel> (audioProcessor.apvts, i, isSynth);
        addAndMakeVisible (*channels[i]);
    }
}

TectonicAudioProcessorEditor::~TectonicAudioProcessorEditor() {}

void TectonicAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);

    if (backgroundSvg != nullptr)
    {
        backgroundSvg->drawWithin (g, getLocalBounds().toFloat(), 
                                   juce::RectanglePlacement::stretchToFit, 1.0f);
    }
}

void TectonicAudioProcessorEditor::resized()
{
    // Divide window into 8 equal columns [1.2.5]
    auto bounds = getLocalBounds();
    int colWidth = bounds.getWidth() / 8;

    for (int i = 0; i < 8; ++i)
    {
        // Position each channel exactly within its 100px boundary
        channels[i]->setBounds (i * colWidth, 0, colWidth, bounds.getHeight());
    }
}
