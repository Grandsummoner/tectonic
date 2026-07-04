#pragma once

#include <JuceHeader.h>

namespace AppTheme
{
    struct Theme
    {
        juce::Colour backgroundStart; // Start color of your faceplate background gradient
        juce::Colour backgroundEnd;   // End color of your faceplate background gradient
        juce::Colour border;
        juce::Colour textDim;
        juce::Colour slotOutline;
        juce::Colour knobFillLeft;     // Custom left knob indicator color (Amber/Gold or Red-Orange)
        juce::Colour knobFillRight;    // Custom right knob indicator color
        juce::Colour faderCap;         // Custom deep Navy Blue fader cap color
        juce::Colour crossfaderTrackA; // Accent color for Scene A (Left of crossfader)
        juce::Colour crossfaderTrackB; // Accent color for Scene B (Right of crossfader)
    };

    inline Theme get (int index)
    {
        switch (index)
        {
            case 0: // Navy Cyber (Stunning Midnight-to-Obsidian Gradient)
                return {
                    juce::Colour::fromString ("#FF0D1E36"), // backgroundStart
                    juce::Colour::fromString ("#FF030508"), // backgroundEnd
                    juce::Colour::fromString ("#FF5D8AA8"), // border
                    juce::Colour::fromString ("#FF888888"), // textDim
                    juce::Colour::fromString ("#FF181C24"), // slotOutline
                    juce::Colour::fromString ("#FFFFB300"), // knobFillLeft
                    juce::Colour::fromString ("#FFFFB300"), // knobFillRight
                    juce::Colour::fromString ("#FF0A1D33"), // faderCap
                    juce::Colour::fromString ("#FFFFB300"), // crossfaderTrackA
                    juce::Colour::fromString ("#FF00D2FF")  // crossfaderTrackB
                };
            case 1: // Skyline Eurorack (Warm Flat Beige Faceplate)
                return {
                    juce::Colour::fromString ("#FFE8E4DB"), // backgroundStart
                    juce::Colour::fromString ("#FFE8E4DB"), // backgroundEnd
                    juce::Colour::fromString ("#FF55555C"), // border
                    juce::Colour::fromString ("#FF4A4B50"), // textDim
                    juce::Colour::fromString ("#FFC8C3BC"), // slotOutline
                    juce::Colour::fromString ("#FFFF5533"), // knobFillLeft
                    juce::Colour::fromString ("#FFFF5533"), // knobFillRight
                    juce::Colour::fromString ("#FF1B2838"), // faderCap
                    juce::Colour::fromString ("#FFFF5533"), // crossfaderTrackA
                    juce::Colour::fromString ("#FF48929B")  // crossfaderTrackB
                };
            case 2: // Monochrome Minimal (Dark Slate Gradient)
                return {
                    juce::Colour::fromString ("#FF23272F"), // backgroundStart
                    juce::Colour::fromString ("#FF121417"), // backgroundEnd
                    juce::Colour::fromString ("#FFFFB300"), // border
                    juce::Colour::fromString ("#FF9E9E9E"), // textDim
                    juce::Colour::fromString ("#FF303030"), // slotOutline
                    juce::Colour::fromString ("#FFFFB300"), // knobFillLeft
                    juce::Colour::fromString ("#FFFFB300"), // knobFillRight
                    juce::Colour::fromString ("#FF0A1D33"), // faderCap
                    juce::Colour::fromString ("#FFFFB300"), // crossfaderTrackA
                    juce::Colour::fromString ("#FFFFB300")  // crossfaderTrackB
                };
            case 3: // Matrix Terminal (Futuristic Green-Black Terminal)
                return {
                    juce::Colour::fromString ("#FF030B03"), // backgroundStart
                    juce::Colour::fromString ("#FF000000"), // backgroundEnd
                    juce::Colour::fromString ("#FF33FF33"), // border
                    juce::Colour::fromString ("#FF558855"), // textDim
                    juce::Colour::fromString ("#FF112211"), // slotOutline
                    juce::Colour::fromString ("#FF33FF33"), // knobFillLeft
                    juce::Colour::fromString ("#FF33FF33"), // knobFillRight
                    juce::Colour::fromString ("#FF051505"), // faderCap
                    juce::Colour::fromString ("#FF33FF33"), // crossfaderTrackA
                    juce::Colour::fromString ("#FF33FF33")  // crossfaderTrackB
                };
            default:
                return {
                    juce::Colour::fromString ("#FFE5E0D8"), // backgroundStart
                    juce::Colour::fromString ("#FFE5E0D8"), // backgroundEnd
                    juce::Colour::fromString ("#FF55555C"), // border
                    juce::Colour::fromString ("#FF3A3A3D"), // textDim
                    juce::Colour::fromString ("#FFC4BCB4"), // slotOutline
                    juce::Colour::fromString ("#FFFF5533"), // knobFillLeft
                    juce::Colour::fromString ("#FFFF5533"), // knobFillRight
                    juce::Colour::fromString ("#FF1B2838"), // faderCap
                    juce::Colour::fromString ("#FFFF5533"), // crossfaderTrackA
                    juce::Colour::fromString ("#FF48929B")  // crossfaderTrackB
                };
        }
    }
}