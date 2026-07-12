#include "PluginProcessor.h"
#include "PluginEditor.h"

TectonicAudioProcessorEditor::TectonicAudioProcessorEditor (TectonicAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 600);

    for (int i = 0; i < 8; ++i)
    {
        bool isSynth = (i < 2);
        channels[i] = std::make_unique<TectonicChannel> (audioProcessor, i, isSynth);
        addAndMakeVisible (*channels[i]);

        channels[i]->onFocusRequested = [this] (int clickedIndex) {
            int currentlyFocused = getFocusedChannel();
            
            for (int c = 0; c < 8; ++c)
            {
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
    juce::ColourGradient grad (juce::Colour (0xFFF2F2F2), 0.0f, 0.0f,
                               juce::Colour (0xFFD5D5D5), 0.0f, static_cast<float> (getHeight()), false);
    g.setGradientFill (grad);
    g.fillAll();

    g.setColour (juce::Colours::black);
    g.drawLine (40.0f, 40.0f, 270.0f, 40.0f, 2.0f);
    g.drawLine (530.0f, 40.0f, 760.0f, 40.0f, 2.0f);

    g.setFont (juce::Font ("Helvetica Neue", 34.0f, juce::Font::bold));
    g.drawText ("TECTONIC", 0, 15, getWidth(), 50, juce::Justification::centred);

    g.setFont (juce::Font ("Helvetica Neue", 11.0f, juce::Font::bold));
    juce::String labels[] = { "SYNTH 1", "SYNTH 2", "DRUM 1", "DRUM 2", "DRUM 3", "DRUM 4", "DRUM 5", "DRUM 6" };
    
    for (int i = 0; i < 8; ++i)
    {
        int colWidth = getWidth() / 8;
        int xCenter = (i * colWidth) + (colWidth / 2);
        
        g.drawText (labels[i], xCenter - 50, 70, 100, 15, juce::Justification::centred);

        if (i < 7)
        {
            int lineX = (i + 1) * colWidth;
            g.setColour (juce::Colours::black.withAlpha (0.4f));
            g.drawLine (static_cast<float> (lineX), 95.0f, static_cast<float> (lineX), 530.0f, 0.75f);
            g.setColour (juce::Colours::black); 
        }
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
