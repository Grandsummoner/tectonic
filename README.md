# Navy Arp

Navy Arp is a generative MIDI arpeggiator and algorithmic step sequencer built with the JUCE 7/8 framework in modern C++20. Drawing inspiration from classic hardware modular setups, the plugin acts as a sound-generating synth (instrument) designed to load stably across mainstream Windows DAWs.

---

## Key Features

### 1. Decoupled Morph Crossfader & Scene Focus
* **Audio Engine**: The lock-free audio thread interpolates directly between private scene snapshots (`sceneA` and `sceneB`) in real-time based on the physical position of the morph crossfader.
* **Editing Lens**: The screen sliders and knobs serve as the active editing lens for the selected scene anchor. 
* **Focus Selection**: A long press (1000ms) on **A** or **B** swaps the active editing focus atomically and snaps the screen sliders to the selected scene's static values without affecting the ongoing crossfaded audio.

### 2. Symmetrical Tactile Modifiers & Instant Latching
* **Utility & Dice Grids**: Features unified dark-charcoal 2x2 grids for both modifiers (`Save`, `Recall`, `Copy`, `Init`) and dicing engines (`Melo`, `Arti`, `Time`, `Navy`).
* **Instant Saving & Copying**: Saving presets is instant. The `Copy` modifier handles both Scene copying (`A` to `B` / `B` to `A`) and tactile Source-to-Destination preset slot copying.
* **3-Blink Flashing Feedback**: Triggers rapid, hardware-style 3-blink flashing sequences (Save/Init blinks red-orange, Recall blinks cyan) for positive action confirmation.
* **Deep Scene Clears**: Triggering `Init` on an active scene resets its parameters and instantly snaps your GUI sliders and dials back to their flat defaults.

### 3. High-Fidelity Skyline Eurorack Aesthetics
* **Beige Color Palette**: Modeled after clean Eurorack aluminum modular faceplates, featuring a warm beige background with high-contrast anthracite borders.
* **Tactile Pointers**: Rotary knobs are styled with dark faces and vibrant red-orange dial value rings. 
* **Legible Textboxes**: Rotary textboxes automatically adapt to use bold black text on clean light-grey backgrounds, ensuring clear contrast.
* **OLED Step-Level Monitor**: A wide centerpiece OLED viewport that draws 8 tall, vertical segmented LED level columns displaying step fader probabilities and a real-time playhead play tracker.

---

## Directory Structure

```text
navy-arp/
├── modules/
│   └── clap-juce-extensions/          # CLAP wrapper target modules
├── source/
│   ├── AppTheme.h                     # Centralized color palettes and theme indexes
│   ├── OledDisplay.h                  # OLED centerpiece declarations
│   ├── OledDisplay.cpp                # Segmented fader LED ladders & live metadata bar
│   ├── ChromaCapsLookAndFeel.h        # LookAndFeel declarations
│   ├── ChromaCapsLookAndFeel.cpp      # Slider tracks, cap indicators, & toggles
│   ├── PluginProcessor.h              # Audio engine & scene registers
│   ├── PluginProcessor.cpp            # Generative arpeggiator clock & MIDI process
│   ├── PluginEditor.h                 # Parent UI declarations & attachments
│   └── PluginEditor.cpp               # Layout boundaries, listeners, & timers
├── CMakeLists.txt                     # Core build script
└── README.md                          # Documentation
