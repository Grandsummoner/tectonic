#include "PluginProcessor.h"
#include "PluginEditor.h"

TectonicAudioProcessorEditor::TectonicAudioProcessorEditor (TectonicAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 600);

    auto xml = juce::XmlDocument::parse (BinaryData::tectonic_panel_svg);
    if (xml != nullptr)
        backgroundSvg = juce::Drawable::createFromSVG (*xml);

    for (int i = 0; i < 8; ++i)
    {
        bool isSynth = (i < 2);
        channels[i] = std::make_unique<TectonicChannel> (audioProcessor, i, isSynth);
        addAndMakeVisible (*channels[i]);

        // Focus selection coordinator callback [1]
        channels[i]->onFocusRequested = [this] (int clickedIndex) {
            int currentlyFocused = getFocusedChannel();
            
            for (int c = 0; c < 8; ++c)
            {
                // Toggle off focus if clicked a second time, or switch focus [1]
                if (c == clickedIndex && currentlyFocused != clickedIndex)
                    channels[c]->setFocusState (true);
                else
                    channels[c]->setFocusState (false);
            }
            repaint();
        };
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
    auto bounds = getLocalBounds();
    int colWidth = bounds.getWidth() / 8;

    for (int i = 0; i < 8; ++i)
    {
        channels[i]->setBounds (i * colWidth, 0, colWidth, bounds.getHeight());
    }
}

int TectonicAudioProcessorEditor::getFocusedChannel() const
{
    for (int i = 0; i < 8; ++i)
        if (channels[i]->getFocusState()) 
            return i;
            
    return -1;
}
