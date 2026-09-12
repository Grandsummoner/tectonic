#include "DrumChannelComponent.h"

DrumChannelComponent::DrumChannelComponent(TectonicAudioProcessor& p, int idx)
    : processor(p), drumIndex(idx)
{
    juce::String prefix = "drum" + juce::String(drumIndex + 1);

    // Setup sound controls
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

    // Setup pattern controls
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

    // Buttons
    muteButton.setButtonText("M");
    muteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF1A1A1A));
    addAndMakeVisible(muteButton);
    muteButton.onClick = [this]() {
        processor.drumChannels[drumIndex].isMuted.store(
            !processor.drumChannels[drumIndex].isMuted.load());
    };

    randomSampleButton.setButtonText("R");
    randomSampleButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF1A1A1A));
    addAndMakeVisible(randomSampleButton);
    randomSampleButton.onClick = [this]() {
        processor.drumChannels[drumIndex].selectRandomSample();
    };
}

DrumChannelComponent::~DrumChannelComponent() {}

void DrumChannelComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Channel background
    g.fillAll(juce::Colour(0xFF1A1A1A));

    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(bounds, 1);

    // Channel label
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
    g.drawText(drumNames[drumIndex], bounds.removeFromTop(30), juce::Justification::centred);
}

void DrumChannelComponent::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    bounds.removeFromTop(40); // Skip header

    // Sound controls
    pitchSlider.setBounds(bounds.removeFromTop(50));
    decaySlider.setBounds(bounds.removeFromTop(50));
    driveSlider.setBounds(bounds.removeFromTop(50));

    // Pattern controls
    stepsSlider.setBounds(bounds.removeFromTop(50));
    triggersSlider.setBounds(bounds.removeFromTop(50));
    offsetSlider.setBounds(bounds.removeFromTop(50));

    // Buttons
    auto buttonRow = bounds.removeFromTop(40);
    muteButton.setBounds(buttonRow.removeFromLeft(buttonRow.getWidth() / 2).reduced(3));
    randomSampleButton.setBounds(buttonRow.reduced(3));
}

void DrumChannelComponent::mouseDown(const juce::MouseEvent& event)
{
    isGridMode = !isGridMode;
    repaint();
}
