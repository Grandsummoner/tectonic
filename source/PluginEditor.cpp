#include "PluginProcessor.h"
#include "PluginEditor.h"

TectonicAudioProcessorEditor::TectonicAudioProcessorEditor (TectonicAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // 1. First, instantiate all child channel components
    for (int i = 0; i < 8; ++i)
    {
        bool isSynth = (i < 2);
        channels[i] = std::make_unique<TectonicChannel> (audioProcessor, i, isSynth);
        addAndMakeVisible (*channels[i]);

        channels[i]->onFocusRequested = [this] (int clickedIndex) {
            int currentlyFocused = getFocusedChannel();
            
            for (int c = 0; c < 8; ++c)
            {
                if (channels[c] != nullptr)
                {
                    if (c == clickedIndex && currentlyFocused != clickedIndex)
                        channels[c]->setFocusState (true);
                    else
                        channels[c]->setFocusState (false);
                }
            }
            repaint();
        };
    }

    // 2. ONLY call setSize at the very end when all components are ready
    setSize (800, 600);
}

TectonicAudioProcessorEditor::~TectonicAudioProcessorEditor() {}

void TectonicAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Brushed Aluminum Linear Gradient precisely matching the SVG structure
    juce::ColourGradient grad (juce::Colour (0xFFF2F2F2), 0.0f, 0.0f,
                               juce::Colour (0xFFD5D5D5), 0.0f, static_cast<float> (getHeight()), false);
    
    // Add the intermediate gradient stops from SVG definition
    grad.addColour (0.3, juce::Colour (0xFFEAEAEA));
    grad.addColour (0.7, juce::Colour (0xFFE1E1E1));
    
    g.setGradientFill (grad);
    g.fillAll();

    // Top Branding Horizontal Black Lines
    g.setColour (juce::Colours::black);
    g.drawLine (40.0f, 40.0f, 270.0f, 40.0f, 2.0f);
    g.drawLine (530.0f, 40.0f, 760.0f, 40.0f, 2.0f);

    // TECTONIC Title Branding
    g.setFont (juce::Font ("Helvetica Neue", 34.0f, juce::Font::bold));
    g.drawText ("TECTONIC", 0, 15, getWidth(), 50, juce::Justification::centred);

    // Channel Headers and Thin Separator Lines
    g.setFont (juce::Font ("Helvetica Neue", 11.0f, juce::Font::bold));
    juce::String labels[] = { "SYNTH 1", "SYNTH 2", "DRUM 1", "DRUM 2", "DRUM 3", "DRUM 4", "DRUM 5", "DRUM 6" };
    
    for (int i = 0; i < 8; ++i)
    {
        int colWidth = getWidth() / 8;
        int xCenter = (i * colWidth) + (colWidth / 2);
        
        // Draw centered column text labels at baseline height
        g.drawText (labels[i], xCenter - 50, 70, 100, 15, juce::Justification::centred);

        // Thin Vertical Dividers (Opacity 0.6) from Y = 95 to Y = 530
        if (i < 7)
        {
            int lineX = (i + 1) * colWidth;
            g.setColour (juce::Colours::black.withAlpha (0.6f));
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
        // Safe check to avoid dereferencing uninitialized channels
        if (channels[i] != nullptr)
        {
            channels[i]->setBounds (i * colWidth, 0, colWidth, bounds.getHeight());
        }
    }
}

int TectonicAudioProcessorEditor::getFocusedChannel() const
{
    for (int i = 0; i < 8; ++i)
    {
        if (channels[i] != nullptr && channels[i]->getFocusState())
        {
            return i;
        }
    }
            
    return -1;
}
