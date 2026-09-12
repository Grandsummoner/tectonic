#include "DrumChannelComponent.h"

DrumChannelComponent::DrumChannelComponent(TectonicAudioProcessor& p, int idx)
    : processor(p), drumIndex(idx), currentPattern(16, false)
{
    juce::String prefix = "drum" + juce::String(drumIndex + 1);

    // Sample display
    sampleDisplay.setText("S1", juce::dontSendNotification);
    sampleDisplay.setFont(juce::Font("Arial", 18.0f, juce::Font::bold));
    sampleDisplay.setColour(juce::Label::textColourId, juce::Colours::white);
    sampleDisplay.setColour(juce::Label::backgroundColourId, juce::Colour(0xFF0A0A0A));
    sampleDisplay.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sampleDisplay);

    // Sample browser buttons
    prevSampleButton.setButtonText("◀");
    prevSampleButton.setColour(juce::TextButton::buttonColourId, drumColors[drumIndex].withAlpha(0.3f));
    addAndMakeVisible(prevSampleButton);
    prevSampleButton.onClick = [this]() {
        auto currentIdx = processor.drumChannels[drumIndex].currentSampleIndex.load();
        int newIdx = (currentIdx - 1 + TectonicAudioProcessor::NUM_SAMPLES_PER_DRUM) %
                     TectonicAudioProcessor::NUM_SAMPLES_PER_DRUM;
        processor.drumChannels[drumIndex].currentSampleIndex.store(newIdx);
        sampleDisplay.setText("S" + juce::String(newIdx + 1), juce::dontSendNotification);
    };

    nextSampleButton.setButtonText("▶");
    nextSampleButton.setColour(juce::TextButton::buttonColourId, drumColors[drumIndex].withAlpha(0.3f));
    addAndMakeVisible(nextSampleButton);
    nextSampleButton.onClick = [this]() {
        auto currentIdx = processor.drumChannels[drumIndex].currentSampleIndex.load();
        int newIdx = (currentIdx + 1) % TectonicAudioProcessor::NUM_SAMPLES_PER_DRUM;
        processor.drumChannels[drumIndex].currentSampleIndex.store(newIdx);
        sampleDisplay.setText("S" + juce::String(newIdx + 1), juce::dontSendNotification);
    };

    randomSampleButton.setButtonText("RND");
    randomSampleButton.setColour(juce::TextButton::buttonColourId, drumColors[drumIndex].withAlpha(0.3f));
    addAndMakeVisible(randomSampleButton);
    randomSampleButton.onClick = [this]() {
        processor.drumChannels[drumIndex].selectRandomSample();
        int idx = processor.drumChannels[drumIndex].currentSampleIndex.load();
        sampleDisplay.setText("S" + juce::String(idx + 1), juce::dontSendNotification);
    };

    // Setup sound controls
    pitchSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    pitchSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    pitchSlider.setColour(juce::Slider::rotarySliderFillColourId, drumColors[drumIndex]);
    pitchSlider.setColour(juce::Slider::thumbColourId, drumColors[drumIndex]);
    addAndMakeVisible(pitchSlider);
    pitchAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_pitch", pitchSlider);

    decaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    decaySlider.setColour(juce::Slider::rotarySliderFillColourId, drumColors[drumIndex]);
    decaySlider.setColour(juce::Slider::thumbColourId, drumColors[drumIndex]);
    addAndMakeVisible(decaySlider);
    decayAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_decay", decaySlider);

    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setColour(juce::Slider::rotarySliderFillColourId, drumColors[drumIndex]);
    driveSlider.setColour(juce::Slider::thumbColourId, drumColors[drumIndex]);
    addAndMakeVisible(driveSlider);
    driveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_overdrive", driveSlider);

    // Setup pattern controls
    stepsSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    stepsSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    stepsSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::cyan);
    stepsSlider.setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
    addAndMakeVisible(stepsSlider);
    stepsAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_steps", stepsSlider);

    triggersSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    triggersSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    triggersSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::cyan);
    triggersSlider.setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
    addAndMakeVisible(triggersSlider);
    triggersAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_triggers", triggersSlider);

    offsetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    offsetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    offsetSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::cyan);
    offsetSlider.setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
    addAndMakeVisible(offsetSlider);
    offsetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, prefix + "_offset", offsetSlider);

    // Mute button
    muteButton.setButtonText("M");
    muteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF333333));
    addAndMakeVisible(muteButton);
    muteButton.onClick = [this]() {
        processor.drumChannels[drumIndex].isMuted.store(
            !processor.drumChannels[drumIndex].isMuted.load());
        muteButton.setColour(juce::TextButton::buttonColourId,
                              processor.drumChannels[drumIndex].isMuted.load()
                                  ? juce::Colours::darkred
                                  : juce::Colour(0xFF333333));
    };

    // Initial grid pattern
    updateGridState();
}

DrumChannelComponent::~DrumChannelComponent() {}

void DrumChannelComponent::updateGridState()
{
    auto* stepsPtr = processor.apvts.getRawParameterValue("drum" + juce::String(drumIndex + 1) + "_steps");
    auto* triggersPtr = processor.apvts.getRawParameterValue("drum" + juce::String(drumIndex + 1) + "_triggers");
    auto* offsetPtr = processor.apvts.getRawParameterValue("drum" + juce::String(drumIndex + 1) + "_offset");

    int steps = static_cast<int>(stepsPtr->load());
    int triggers = static_cast<int>(triggersPtr->load());
    int offset = static_cast<int>(offsetPtr->load());

    if (steps != lastStepCount || triggers != lastTriggerCount || offset != lastOffsetCount)
    {
        lastStepCount = steps;
        lastTriggerCount = triggers;
        lastOffsetCount = offset;
        currentPattern = TectonicAudioProcessor::generateEuclideanPattern(steps, triggers, offset);
    }
}

void DrumChannelComponent::paint(juce::Graphics& g)
{
    updateGridState();

    auto bounds = getLocalBounds();
    auto originalBounds = bounds;

    // Channel background
    g.fillAll(juce::Colour(0xFF1A1A1A));

    // Top border accent
    g.setColour(drumColors[drumIndex]);
    g.fillRect(bounds.removeFromTop(3));

    // Channel label
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font("Arial", 12.0f, juce::Font::bold));
    auto headerBounds = bounds.removeFromTop(25);
    g.drawText(drumNames[drumIndex], headerBounds, juce::Justification::centred);

    // Sample display area
    auto sampleAreaBounds = bounds.removeFromTop(50);
    g.setColour(juce::Colour(0xFF0A0A0A));
    g.fillRoundedRectangle(sampleAreaBounds.reduced(5).toFloat(), 4.0f);

    // 16-step grid
    auto gridBounds = bounds.removeFromTop(40).reduced(5);
    int stepWidth = gridBounds.getWidth() / 16;
    int gridHeight = gridBounds.getHeight();

    for (int i = 0; i < 16; ++i)
    {
        auto stepBounds = gridBounds.withX(gridBounds.getX() + i * stepWidth).withWidth(stepWidth).reduced(2);

        if (i < currentPattern.size())
        {
            if (currentPattern[i])
            {
                // Active step
                g.setColour(drumColors[drumIndex].withAlpha(0.9f));
                g.fillRoundedRectangle(stepBounds.toFloat(), 2.0f);
                g.setColour(juce::Colours::white);
            }
            else
            {
                // Inactive step
                g.setColour(juce::Colour(0xFF333333));
                g.fillRoundedRectangle(stepBounds.toFloat(), 2.0f);
                g.setColour(drumColors[drumIndex].withAlpha(0.3f));
            }
        }
        else
        {
            g.setColour(juce::Colour(0xFF222222));
            g.fillRoundedRectangle(stepBounds.toFloat(), 2.0f);
        }

        g.drawRoundedRectangle(stepBounds.toFloat(), 2.0f, 1.0f);

        // Step number
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.setFont(juce::Font("Arial", 8.0f));
        g.drawText(juce::String(i + 1), stepBounds, juce::Justification::centred);
    }

    // Control labels
    bounds.removeFromTop(10);
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(juce::Font("Arial", 9.0f));

    auto soundLabelBounds = bounds.removeFromTop(15);
    g.drawText("SOUND", soundLabelBounds, juce::Justification::centred);

    bounds.removeFromTop(90);
    auto patternLabelBounds = bounds.removeFromTop(15);
    g.drawText("PATTERN", patternLabelBounds, juce::Justification::centred);
}

void DrumChannelComponent::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    bounds.removeFromTop(3);  // Top accent
    bounds.removeFromTop(25); // Header

    // Sample display
    auto sampleArea = bounds.removeFromTop(50);
    sampleDisplay.setBounds(sampleArea.removeFromTop(25).reduced(3));
    auto buttonArea = sampleArea;
    int btnWidth = buttonArea.getWidth() / 3;
    prevSampleButton.setBounds(buttonArea.removeFromLeft(btnWidth).reduced(2));
    randomSampleButton.setBounds(buttonArea.removeFromLeft(btnWidth).reduced(2));
    nextSampleButton.setBounds(buttonArea.reduced(2));

    bounds.removeFromTop(50); // Grid area
    bounds.removeFromTop(10); // Spacing

    // Sound control labels & sliders
    bounds.removeFromTop(15); // SOUND label
    pitchSlider.setBounds(bounds.removeFromTop(40).reduced(3));
    decaySlider.setBounds(bounds.removeFromTop(40).reduced(3));
    driveSlider.setBounds(bounds.removeFromTop(40).reduced(3));

    // Pattern control labels & sliders
    bounds.removeFromTop(15); // PATTERN label
    stepsSlider.setBounds(bounds.removeFromTop(40).reduced(3));
    triggersSlider.setBounds(bounds.removeFromTop(40).reduced(3));
    offsetSlider.setBounds(bounds.removeFromTop(40).reduced(3));

    // Mute button at bottom
    muteButton.setBounds(bounds.removeFromBottom(35).reduced(3));
}

void DrumChannelComponent::mouseDown(const juce::MouseEvent& event)
{
    // Could implement click-to-toggle grid steps here in future
}
