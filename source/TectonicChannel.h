#pragma once
#include <JuceHeader.h>

class TectonicChannel : public juce::Component
{
public:
    TectonicChannel (juce::AudioProcessorValueTreeState& vts, int channelIndex, bool isSynthChannel)
        : apvts (vts), index (channelIndex), isSynth (isSynthChannel)
    {
        // 1. Configure LEDs and Jacks (We will draw these in paint, or use basic components)
        
        // 2. Configure 7-Segment Display (using a styled label)
        display.setFont (juce::Font ("Courier New", 28.0f, juce::Font::bold));
        display.setJustificationType (juce::Justification::centered);
        display.setColour (juce::Label::textColourId, isSynth ? juce::Colours::limegreen : juce::Colours::red);
        display.setColour (juce::Label::backgroundColourId, juce::Colours::black);
        addAndMakeVisible (display);
        
        // Listen to clicks on the display to toggle channel focus
        display.onTextChange = [this]() { /* Focus logic will trigger here */ };

        // 3. Configure the 3 Knobs (Rotary Sliders)
        for (int i = 0; i < 3; ++i)
        {
            auto& knob = knobs[i];
            knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            knob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            
            // Custom look: Black round knobs
            knob.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::black);
            knob.setColour (juce::Slider::thumbColourId, juce::Colours::whitesmoke);
            
            addAndMakeVisible (knob);
        }

        // 4. Configure the 2 Square Buttons
        addAndMakeVisible (buttonTop);
        buttonTop.setColour (juce::TextButton::buttonColourId, juce::Colours::black);
        buttonTop.setColour (juce::TextButton::textColourOffId, juce::Colours::white);

        addAndMakeVisible (buttonBottom);
        buttonBottom.setColour (juce::TextButton::buttonColourId, juce::Colours::black);
        buttonBottom.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
        
        // Bind default parameters to the knobs
        updateBindings();
    }

    ~TectonicChannel() override {}

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        auto width = bounds.getWidth();

        // Draw the Status LED at the top (Y = 110)
        g.setColour (isFocused ? juce::Colours::cyan : (isSynth ? juce::Colours::limegreen : juce::Colours::red));
        g.fillEllipse (width / 2 - 4, 106, 8, 8);

        // Draw the Audio Jack Outer Ring (Y = 160)
        g.setColour (juce::Colours::black);
        g.fillEllipse (width / 2 - 16, 144, 32, 32);
        g.setColour (juce::Colours::grey);
        g.fillEllipse (width / 2 - 10, 150, 20, 20); // Inner metal sleeve
    }

    void resized() override
    {
        auto xOffset = getLocalBounds().getWidth() / 2;

        // Position components vertically matching the SVG grid
        display.setBounds      (xOffset - 20, 210, 40, 40);
        
        // Tiny pinhole mode indicator circle
        // Knobs (Y = 330, 385, 440)
        knobs[0].setBounds     (xOffset - 22, 308, 44, 44);
        knobs[1].setBounds     (xOffset - 22, 363, 44, 44);
        knobs[2].setBounds     (xOffset - 22, 418, 44, 44);

        // Square Buttons (Y = 500, 550)
        buttonTop.setBounds    (xOffset - 18, 482, 36, 36);
        buttonBottom.setBounds (xOffset - 18, 532, 36, 36);
    }

    // Dynamic parameter switching logic when focus is toggled
    void setFocusState (bool shouldBeFocused)
    {
        isFocused = shouldBeFocused;
        updateBindings();
        repaint();
    }

private:
    void updateBindings()
    {
        // Reset attachments
        attachments.clear();

        juce::String prefix = isSynth ? "synth" : "drum";
        int chNumber = index + 1;

        if (isFocused)
        {
            // Focused Mode: Knobs control Euclidean Sequencer parameters
            attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_steps", knobs[0]));
            attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_triggers", knobs[1]));
            attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_offset", knobs[2]));
            
            display.setText ("E.", juce::dontSendNotification); // Show E to denote Euclidean Edit state
        }
        else
        {
            // Standard Mode: Knobs control Sound parameters (Tuning, Decay, Overdrive)
            attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_param1", knobs[0]));
            attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_param2", knobs[1]));
            attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (apvts, prefix + juce::String (chNumber) + "_param3", knobs[2]));
            
            display.setText ("8.", juce::dontSendNotification); // Default state
        }
    }

    juce::AudioProcessorValueTreeState& apvts;
    int index;
    bool isSynth;
    bool isFocused = false;

    juce::Label display;
    juce::Slider knobs[3];
    juce::TextButton buttonTop;
    juce::TextButton buttonBottom;

    // List of attachments to dynamically manage connection with DSP parameters
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicChannel)
};
