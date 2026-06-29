#pragma once

#include <JuceHeader.h>

namespace AppTheme
{
    struct Theme
    {
        juce::Colour background;
        juce::Colour border;
        juce::Colour textDim;
        juce::Colour slotOutline;
    };

    inline Theme get (int index)
    {
        switch (index)
        {
            case 0: // Navy Cyber
                return {
                    juce::Colour::fromString ("#FF0A0B10"), // background (obsidian deep navy)
                    juce::Colour::fromString ("#FF00D2FF"), // border (electric blue)
                    juce::Colour::fromString ("#FF888888"), // textDim
                    juce::Colour::fromString ("#FF181C24")  // slotOutline
                };
            case 1: // Skyline Eurorack (Beige Faceplate)
                return {
                    juce::Colour::fromString ("#FFE8E4DB"), // background (classic warm eurorack beige)
                    juce::Colour::fromString ("#FF55555C"), // border (aluminum anthracite grey)
                    juce::Colour::fromString ("#FF4A4B50"), // textDim (high contrast dark grey for beige)
                    juce::Colour::fromString ("#FFC8C3BC")  // slotOutline
                };
            case 2: // Monochrome Minimal
                return {
                    juce::Colour::fromString ("#FF1E2127"),
                    juce::Colour::fromString ("#FFFFB300"),
                    juce::Colour::fromString ("#FF9E9E9E"),
                    juce::Colour::fromString ("#FF303030")
                };
            case 3: // Matrix Terminal
                return {
                    juce::Colour::fromString ("#FF030803"),
                    juce::Colour::fromString ("#FF33FF33"),
                    juce::Colour::fromString ("#FF558855"),
                    juce::Colour::fromString ("#FF112211")
                };
            default:
                return {
                    juce::Colour::fromString ("#FFE5E0D8"),
                    juce::Colour::fromString ("#FF55555C"),
                    juce::Colour::fromString ("#FF3A3A3D"),
                    juce::Colour::fromString ("#FFC4BCB4")
                };
        }
    }
}