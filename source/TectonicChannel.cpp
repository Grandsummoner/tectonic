#include "TectonicChannel.h"

TectonicChannel::TectonicChannel (TectonicAudioProcessor& p, int channelIndex, bool isSynthChannel)
    : processor (p), index (channelIndex), isSynth (isSynthChannel)
{
    display.setFont (juce::Font ("Courier New", 28.0f, juce::Font::bold));
    display.setJustificationType (juce::Justification::centered);
    display.setColour (juce::Label::textColourId, isSynth ? juce::Colours::limegreen : juce::Colours::red);
    display.setColour (juce::Label::backgroundColourId, juce::Colours::black);
    addAndMakeVisible (display);

    // Make display forward clicks down to the TectonicChannel component
    display.setInterceptsMouseClicks (false, false);

    for (int i = 0; i < 3; ++i)
    {
        auto& knob = knobs[i];
        knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        knob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        knob.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::black);
        knob.setColour (juce::Slider::thumbColourId, juce::Colours::whitesmoke);
        addAndMakeVisible (knob);
    }

    addAndMakeVisible (buttonTop);
    buttonTop.setColour (juce::TextButton::buttonColourId, juce::Colours::black);
    buttonTop.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    
    // Top button triggers single-shot (unfocused) or randomizes samples (focused) [1]
    buttonTop.onClick = [this]() {
        if (!isFocused)
        {
            // Unfocused: Single Shot trigger
            if (!isSynth)
                processor.drumChannels[index - 2].trigger();
        }
        else
        {
            // Focused: Randomize active sample within channel pool
            if (!isSynth)
            {
                processor.drumChannels[index - 2].selectRandomSample();
                display.setText (juce::String (processor.drumChannels[index - 2].currentSampleIndex + 1) + ".", juce::dontSendNotification);
            }
        }
    };

    addAndMakeVisible (buttonBottom);
    buttonBottom.setColour (juce::TextButton::buttonColourId, juce::Colours::black);
    buttonBottom.setColour (juce::TextButton::textColourOffId, juce::Colours::white);

    // Mute/Unmute state updates standard performance states
    buttonBottom.onClick = [this]() {
        if (!isFocused)
        {
            // Unfocused: Toggle Mute
            isMuted = !isMuted;
            buttonBottom.setColour (juce::TextButton::buttonColourId, isMuted ? juce::Colours::darkred : juce::Colours::black);
            
            if (isSynth)
                processor.synthChannels[index].isMuted.store (isMuted);
            else
                processor.drumChannels[index - 2].isMuted.store (isMuted);
        }
    };

    // Momentary trigger tracking for inverse fills (only active when focused) [1]
    buttonBottom.onStateChange = [this]() {
        if (isFocused && !isSynth)
        {
            // Set Inverted Fill active on the DSP thread while button is pressed down [1]
            processor.drumChannels[index - 2].isFillActive.store (buttonBottom.isDown());
        }
    };

    updateBindings();
}

TectonicChannel::~TectonicChannel() {}

void TectonicChannel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto width = bounds.getWidth();

    // Dim column slightly if another channel has active focus [1.2.1]
    bool anyChannelFocused = false;
    if (auto* parent = getParentComponent())
    {
        for (int i = 0; i < parent->getNumChildComponents(); ++i)
        {
            if (auto* chan = dynamic_cast<TectonicChannel*> (parent->getChildComponent (i)))
                if (chan->getFocusState()) anyChannelFocused = true;
        }
    }

    if (anyChannelFocused && !isFocused)
        g.fillAll (juce::Colours::black.withAlpha (0.25f)); // Subtle dim shadow

    g.setColour (isFocused ? juce::Colours::cyan : (isSynth ? juce::Colours::limegreen : juce::Colours::red));
    g.fillEllipse (width / 2.0f - 4.0f, 106.0f, 8.0f, 8.0f);

    g.setColour (juce::Colours::black);
    g.fillEllipse (width / 2.0f - 16.0f, 144.0f, 32.0f, 32.0f);
    g.setColour (juce::Colours::grey);
    g.fillEllipse (width / 2.0f - 10.0f, 150.0f, 20.0f, 20.0f);
}

void TectonicChannel::resized()
{
    auto xOffset = getLocalBounds().getWidth() / 2;

    display.setBounds      (xOffset - 20, 210, 40, 40);
    knobs[0].setBounds     (xOffset - 22, 308, 44, 44);
    knobs[1].setBounds     (xOffset - 22, 363, 44, 44);
    knobs[2].setBounds     (xOffset - 22, 418, 44, 44);
    buttonTop.setBounds    (xOffset - 18, 482, 36, 36);
    buttonBottom.setBounds (xOffset - 18, 532, 36, 36);
}

void TectonicChannel::mouseDown (const juce::MouseEvent& event)
{
    // If user clicked inside the 7-segment display boundaries, fire the focus event
    if (display.getBounds().contains (event.getPosition()))
    {
        if (onFocusRequested != nullptr)
            onFocusRequested (index);
    }
}

void TectonicChannel::setFocusState (bool shouldBeFocused)
{
    isFocused = shouldBeFocused;
    updateBindings();
    
    // Reset button states for focus change
    buttonBottom.setColour (juce::TextButton::buttonColourId, isMuted ? juce::Colours::darkred : juce::Colours::black);
    
    repaint();
}

void TectonicChannel::updateBindings()
{
    attachments.clear();

    juce::String prefix = isSynth ? "synth" : "drum";
    int chNumber = isSynth ? (index + 1) : (index - 1);

    if (isFocused)
    {
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_steps", knobs[0]));
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_triggers", knobs[1]));
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_offset", knobs[2]));
        
        buttonTop.setButtonText ("RND");
        buttonBottom.setButtonText ("FIL");
        
        if (!isSynth)
            display.setText (juce::String (processor.drumChannels[index - 2].currentSampleIndex + 1) + ".", juce::dontSendNotification);
        else
            display.setText ("E.", juce::dontSendNotification);
    }
    else
    {
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_param1", knobs[0]));
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_param2", knobs[1]));
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_param3", knobs[2]));
        
        buttonTop.setButtonText ("TRG");
        buttonBottom.setButtonText ("MUT");
        display.setText ("8.", juce::dontSendNotification);
    }
}
