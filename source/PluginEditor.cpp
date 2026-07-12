#include "PluginProcessor.h"
#include "PluginEditor.h"

TectonicAudioProcessorEditor::TectonicAudioProcessorEditor (TectonicAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set your default window size (matching the SVG's aspect ratio)
    setSize (800, 600);

    // 1. Load the compiled SVG data from your BinaryData
    auto xml = juce::XmlDocument::parse (BinaryData::tectonic_panel_svg);
    
    if (xml != nullptr)
    {
        // 2. Parse the XML structure into a Drawable vector object
        backgroundSvg = juce::Drawable::createFromSVG (*xml);
    }
}

TectonicAudioProcessorEditor::~TectonicAudioProcessorEditor()
{
}

void TectonicAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Clear the background first
    g.fillAll (juce::Colours::darkgrey);

    // 3. Draw the vector panel. It automatically scales to fit whatever 
    // bounds are set, keeping your vector art razor-sharp at any zoom level!
    if (backgroundSvg != nullptr)
    {
        backgroundSvg->drawWithin (g, getLocalBounds().toFloat(), 
                                   juce::RectanglePlacement::stretchToFit, 1.0f);
    }
}

void TectonicAudioProcessorEditor::resized()
{
    // This is where you will set the positions of your dynamic 
    // knobs, buttons, and jacks relative to the window size.
}
