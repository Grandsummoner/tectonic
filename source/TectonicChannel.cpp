#include "TectonicChannel.h"

TectonicChannel::TectonicChannel (TectonicAudioProcessor& p, int channelIndex, bool isSynthChannel)
    : processor (p), index (channelIndex), isSynth (isSynthChannel)
{
    display.setFont (juce::Font ("Courier New", 28.0f, juce::Font::bold));
    display.setJustificationType (juce::Justification::centred);
    display.setColour (juce::Label::textColourId, isSynth ? juce::Colours::limegreen : juce::Colours::red);
    display.setColour (juce::Label::backgroundColourId, juce::Colours::black);
    addAndMakeVisible (display);

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
                display.setText (juce::String (processor.drumChannels[index - 2].currentSampleIndex + 1) + ".", juce::dontSendNotification);
            }
        }
    };

    addAndMakeVisible (buttonBottom);
    buttonBottom.setColour (juce::TextButton::buttonColourId, juce::Colours::black);
    buttonBottom.setColour (juce::TextButton::textColourOffId, juce::Colours::white);

    buttonBottom.onClick = [this]() {
        if (!isFocused)
        {
            isMuted = !isMuted;
            buttonBottom.setColour (juce::TextButton::buttonColourId, isMuted ? juce::Colours::darkred : juce::Colours::black);
            
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

    updateBindings();
}

TectonicChannel::~TectonicChannel() {}

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
                if (chan->getFocusState()) anyChannelFocused = true;
        }
    }

    if (anyChannelFocused && !isFocused)
        g.fillAll (juce::Colours::black.withAlpha (0.25f));

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
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (processor.apvts, prefix + juce::String (chNumber) + "_steps", knobs[0]));
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (processor.apvts, prefix + juce::String (chNumber) + "_triggers", knobs[1]));
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (processor.apvts, prefix + juce::String (chNumber) + "_offset", knobs[2]));
        
        buttonTop.setButtonText ("RND");
        buttonBottom.setButtonText ("FIL");
        
        if (!isSynth)
            display.setText (juce::String (processor.drumChannels[index - 2].currentSampleIndex + 1) + ".", juce::dontSendNotification);
        else
            display.setText ("E.", juce::dontSendNotification);
    }
    else
    {
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (processor.apvts, prefix + juce::String (chNumber) + "_param1", knobs[0]));
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (processor.apvts, prefix + juce::String (chNumber) + "_param2", knobs[1]));
        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (processor.apvts, prefix + juce::String (chNumber) + "_param3", knobs[2]));
        
        buttonTop.setButtonText ("TRG");
        buttonBottom.setButtonText ("MUT");
        display.setText ("8.", juce::dontSendNotification);
    }
}
