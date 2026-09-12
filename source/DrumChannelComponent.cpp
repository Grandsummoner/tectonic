#include "DrumChannelComponent.h"

DrumChannelComponent::DrumChannelComponent(TectonicAudioProcessor& p, int idx)
    : processor(p), drumIndex(idx)
{
    juce::String prefix = "drum" + juce::String(drumIndex + 1);

    pitchSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    pitchSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(pitchSlider);
    pitchAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_pitch", pitchSlider);

    decaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(decaySlider);
    decayAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_decay", decaySlider);

    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(driveSlider);
    driveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_overdrive", driveSlider);

    stepsSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    stepsSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(stepsSlider);
    stepsAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_steps", stepsSlider);

    triggersSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    triggersSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(triggersSlider);
    triggersAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_triggers", triggersSlider);

    offsetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    offsetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(offsetSlider);
    offsetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_offset", offsetSlider);

    muteButton.setButtonText("MUTE");
    muteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF333333));
    addAndMakeVisible(muteButton);
    muteButton.onClick = [this]() {
        processor.drumChannels[drumIndex].isMuted.store(
            !processor.drumChannels[drumIndex].isMuted.load());
    };
}

DrumChannelComponent::~DrumChannelComponent() {}

void DrumChannelComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1A1A1A));
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText("DRUM " + juce::String(drumIndex + 1),
               getLocalBounds().removeFromTop(25),
               juce::Justification::centred);
}

void DrumChannelComponent::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    bounds.removeFromTop(30);

    pitchSlider.setBounds(bounds.removeFromTop(40));
    decaySlider.setBounds(bounds.removeFromTop(40));
    driveSlider.setBounds(bounds.removeFromTop(40));
    stepsSlider.setBounds(bounds.removeFromTop(40));
    triggersSlider.setBounds(bounds.removeFromTop(40));
    offsetSlider.setBounds(bounds.removeFromTop(40));
    muteButton.setBounds(bounds.removeFromTop(30));
}
