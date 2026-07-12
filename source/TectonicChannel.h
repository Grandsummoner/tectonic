#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

// Beautiful, custom vector LookAndFeel for Tectonic
class TectonicLookAndFeel : public juce::LookAndFeel_V4
{
public:
    TectonicLookAndFeel()
    {
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::black);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportion, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (3.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toX = bounds.getCentreX();
        auto toY = bounds.getCentreY();
        auto ux = toX - radius;
        auto uy = toY - radius;
        auto diameter = radius * 2.0f;

        // Draw inner dial body (Dark Matte Circle)
        g.setColour (juce::Colour (0xFF1A1A1A));
        g.fillEllipse (ux, uy, diameter, diameter);

        // Outer sleek outline ring
        g.setColour (juce::Colour (0xFF333333));
        g.drawEllipse (ux, uy, diameter, diameter, 1.0f);

        // Calculate current pointer angle
        auto angle = rotaryStartAngle + sliderPosProportion * (rotaryEndAngle - rotaryStartAngle);
        
        // Draw elegant glowing active track arc
        juce::Path arcPath;
        arcPath.addCentredArc (toX, toY, radius - 2.5f, radius - 2.5f, 0.0f, rotaryStartAngle, angle, true);
        g.setColour (slider.findColour (juce::Slider::thumbColourId));
        g.strokePath (arcPath, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Draw minimalist pointer line
        juce::Path pointer;
        pointer.startNewSubPath (toX, toY);
        pointer.lineTo (toX + std::sin (angle) * (radius - 1.0f), toY - std::cos (angle) * (radius - 1.0f));
        g.setColour (slider.findColour (juce::Slider::thumbColourId).withAlpha (0.9f));
        g.strokePath (pointer, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        
        g.setColour (isButtonDown ? backgroundColour.brighter (0.1f) : backgroundColour);
        g.fillRoundedRectangle (bounds, 4.0f);

        g.setColour (button.findColour (juce::TextButton::textColourOffId).withAlpha (0.2f));
        g.drawRoundedRectangle (bounds, 4.0f, 1.0f);
    }

    // Force button text to render without margins to prevent "T..." truncation
    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool /*isMouseOverButton*/, bool /*isButtonDown*/) override
    {
        g.setFont (juce::Font ("Helvetica Neue", 10.0f, juce::Font::bold));
        g.setColour (button.findColour (juce::TextButton::textColourOffId));
        g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred);
    }
};

class TectonicChannel : public juce::Component
{
public:
    TectonicChannel (TectonicAudioProcessor& p, int channelIndex, bool isSynthChannel);
    ~TectonicChannel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void setFocusState (bool shouldBeFocused);
    bool getFocusState() const { return isFocused; }

    std::function<void(int)> onFocusRequested;

private:
    void mouseDown (const juce::MouseEvent& event) override;

    TectonicAudioProcessor& processor;
    int index;
    bool isSynth;
    bool isFocused = false;
    bool isMuted = false;

    juce::Label display;
    
    juce::Slider knobs[3];     
    juce::Slider seqKnobs[3];  
    
    juce::TextButton buttonTop;
    juce::TextButton buttonBottom;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knobAttachments[3];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> seqKnobAttachments[3];

    // Shared custom look and feel asset
    TectonicLookAndFeel customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TectonicChannel)
};
