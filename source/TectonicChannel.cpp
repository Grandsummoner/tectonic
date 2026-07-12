#include "TectonicChannel.h"

TectonicChannel::TectonicChannel (TectonicAudioProcessor& p, int channelIndex, bool isSynthChannel)
    : processor (p), index (channelIndex), isSynth (isSynthChannel)
{
    // Apply custom styling instantly to all sub-components
    setLookAndFeel (&customLookAndFeel);

    display.setFont (juce::Font ("Courier New", 28.0f, juce::Font::bold));
    display.setJustificationType (juce::Justification::centred);
    display.setColour (juce::Label::textColourId, isSynth ? juce::Colours::limegreen : juce::Colours::red);
    display.setColour (juce::Label::backgroundColourId, juce::Colours::black);
    addAndMakeVisible (display);

    display.setInterceptsMouseClicks (false, false);

    // Apply glowing colored indicators matching channel modes
    juce::Colour activeColor = isSynth ? juce::Colours::limegreen : juce::Colours::red;

    for (int i = 0; i < 3; ++i)
    {
        knobs[i].setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        knobs[i].setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        knobs[i].setColour (juce::Slider::thumbColourId, juce::Colours::whitesmoke);
        addAndMakeVisible (knobs[i]);

        seqKnobs[i].setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        seqKnobs[i].setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        seqKnobs[i].setColour (juce::Slider::thumbColourId, juce::Colours::cyan);
        addAndMakeVisible (seqKnobs[i]);
    }

    addAndMakeVisible (buttonTop);
    buttonTop.setColour (juce::TextButton::buttonColourId, juce::Colour (0xFF1A1A1A));
    buttonTop.setColour (juce::TextButton::textColourOffId, juce::Colours::whitesmoke);
    
    buttonTop.onClick = [this]() {
        if (!isFocused)
        {
            if (!isSynth)
                processor.drumChannels[index - 2].trigger();
        }
        else
        {
            if (!isSynth)
            {
                processor.drumChannels[index - 2].selectRandomSample();
                display.setText (juce::String (processor.drumChannels[index - 2].currentSampleIndex.load() + 1) + ".", juce::dontSendNotification);
            }
        }
    };

    addAndMakeVisible (buttonBottom);
    buttonBottom.setColour (juce::TextButton::buttonColourId, juce::Colour (0xFF1A1A1A));
    buttonBottom.setColour (juce::TextButton::textColourOffId, juce::Colours::whitesmoke);

    buttonBottom.onClick = [this]() {
        if (!isFocused)
        {
            isMuted = !isMuted;
            buttonBottom.setColour (juce::TextButton::buttonColourId, isMuted ? juce::Colours::darkred : juce::Colour (0xFF1A1A1A));
            buttonBottom.setColour (juce::TextButton::textColourOffId, isMuted ? juce::Colours::white : juce::Colours::whitesmoke);
            
            if (isSynth)
                processor.synthChannels[index].isMuted.store (isMuted);
            else
                processor.drumChannels[index - 2].isMuted.store (isMuted);
        }
    };

    buttonBottom.onStateChange = [this]() {
        if (isFocused && !isSynth)
        {
            processor.drumChannels[index - 2].isFillActive.store (buttonBottom.isDown());
        }
    };

    juce::String prefix = isSynth ? "synth" : "drum";
    int chNumber = isSynth ? (index + 1) : (index - 1);

    for (int i = 0; i < 3; ++i)
    {
        knobAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            processor.apvts, prefix + juce::String (chNumber) + "_param" + juce::String (i + 1), knobs[i]
        );
    }

    seqKnobAttachments[0] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.apvts, prefix + juce::String (chNumber) + "_steps", seqKnobs[0]
    );
    seqKnobAttachments[1] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.apvts, prefix + juce::String (chNumber) + "_triggers", seqKnobs[1]
    );
    seqKnobAttachments[2] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.apvts, prefix + juce::String (chNumber) + "_offset", seqKnobs[2]
    );

    setFocusState (false);
}

TectonicChannel::~TectonicChannel() 
{
    setLookAndFeel (nullptr); // Avoid dangling LookAndFeel pointer crashes on teardown
}

void TectonicChannel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto width = bounds.getWidth();

    bool anyChannelFocused = false;
    if (auto* parent = getParentComponent())
    {
        for (int i = 0; i < parent->getNumChildComponents(); ++i)
        {
            if (auto* chan = dynamic_cast<TectonicChannel*> (parent->getChildComponent (i)))
                if (chan->getFocusState()) 
                    anyChannelFocused = true;
        }
    }

    // Dynamic background dimming
    if (anyChannelFocused && !isFocused)
        g.fillAll (juce::Colours::black.withAlpha (0.15f));

    // Glow ring behind status LED
    juce::Colour activeColor = isFocused ? juce::Colours::cyan : (isSynth ? juce::Colours::limegreen : juce::Colours::red);
    g.setColour (activeColor.withAlpha (0.15f));
    g.fillEllipse (width / 2.0f - 7.0f, 103.0f, 14.0f, 14.0f);

    // Dynamic status indicator LED
    g.setColour (activeColor);
    g.fillEllipse (width / 2.0f - 4.0f, 106.0f, 8.0f, 8.0f);

    // Decorative Hardware style screw head
    g.setColour (juce::Colours::black);
    g.fillEllipse (width / 2.0f - 16.0f, 144.0f, 32.0f, 32.0f);
    g.setColour (juce::Colours::grey.withAlpha (0.5f));
    g.fillEllipse (width / 2.0f - 10.0f, 150.0f, 20.0f, 20.0f);
    g.setColour (juce::Colour (0xFF1A1A1A));
    g.drawEllipse (width / 2.0f - 10.0f, 150.0f, 20.0f, 20.0f, 1.0f);
    g.drawLine (width / 2.0f - 6.0f, 160.0f, width / 2.0f + 6.0f, 160.0f, 1.5f); // Horizontal screw slot
}

void TectonicChannel::resized()
{
    auto xOffset = getLocalBounds().getWidth() / 2;

    display.setBounds (xOffset - 20, 210, 40, 40);
    
    for (int i = 0; i < 3; ++i)
    {
        int y = 308 + (i * 55);
        knobs[i].setBounds    (xOffset - 22, y, 44, 44);
        seqKnobs[i].setBounds (xOffset - 22, y, 44, 44);
    }

    buttonTop.setBounds    (xOffset - 18, 482, 36, 36);
    buttonBottom.setBounds (xOffset - 18, 532, 36, 36);
}

void TectonicChannel::mouseDown (const juce::MouseEvent& event)
{
    if (display.getBounds().contains (event.getPosition()))
    {
        if (onFocusRequested != nullptr)
            onFocusRequested (index);
    }
}

void TectonicChannel::setFocusState (bool shouldBeFocused)
{
    isFocused = shouldBeFocused;
    
    for (int i = 0; i < 3; ++i)
    {
        knobs[i].setVisible (!isFocused);
        seqKnobs[i].setVisible (isFocused);
    }

    if (isFocused)
    {
        buttonTop.setButtonText ("RND");
        buttonBottom.setButtonText ("FIL");
        
        if (!isSynth)
            display.setText (juce::String (processor.drumChannels[index - 2].currentSampleIndex.load() + 1) + ".", juce::dontSendNotification);
        else
            display.setText ("E.", juce::dontSendNotification);
    }
    else
    {
        buttonTop.setButtonText ("TRG");
        buttonBottom.setButtonText ("MUT");
        display.setText ("8.", juce::dontSendNotification);
    }

    buttonBottom.setColour (juce::TextButton::buttonColourId, isMuted ? juce::Colours::darkred : juce::Colour (0xFF1A1A1A));
    buttonBottom.setColour (juce::TextButton::textColourOffId, isMuted ? juce::Colours::white : juce::Colours::whitesmoke);
    repaint();
}
