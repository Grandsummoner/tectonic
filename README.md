# Tectonic - VCV Rack Drum Sequencer Plugin

A powerful drum sequencer with rhythmic pattern generation built on Euclidean rhythms, featuring 6 drum channels with built-in drum samples and intuitive step editing.

## Features

### Core Sequencing
- **Euclidean Rhythm Engine** - Classic Euclidean pattern generation with step, beat, and rotation controls
- **6 Drum Channels** - Kick, Snare, Open Hat, Closed Hat, Clap, Percussion
- **4 Sample Variants** - Each drum has 4 unique samples for variation
- **Grid-Based Step Editor** - Visual 16-step sequencer with per-track focus mode
- **Real-time Pattern Control** - Modify sequences on the fly while playing

### Drum Channel Controls
Each drum channel includes:
- **Sample Selection** - Random or manual selection from 4 samples
- **Pitch Shift** - ±12 semitones for tuning
- **Decay** - Control sample release time (0.01s - 2s)
- **Overdrive** - Add saturation and character
- **Pattern Parameters**:
  - Steps: 1-16 pattern length
  - Triggers: Number of hits per pattern
  - Offset: Rotation of the pattern

### UI Highlights
- **Dual Mode Display** - Default view shows drum sounds, click to focus for pattern editing
- **Visual Feedback** - LED indicators show active samples and pattern state
- **Responsive Layout** - 8 columns (2 synth + 6 drums) with touch-friendly controls
- **Modern Aesthetic** - Clean dark interface with color-coded channels

## Build Instructions

### Requirements
- CMake 3.15+
- C++17 compiler
- JUCE 7.0+ (included as submodule)

### Building

```bash
git clone --recursive https://github.com/Grandsummoner/tectonic.git
cd tectonic
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

The VST3 plugin will be built to `build/tectonic_artefacts/Release/VST3/`

## Usage

1. Load Tectonic in your DAW
2. Use the 16-step grid to create drum patterns
3. Adjust pattern parameters (steps, triggers, offset) to evolve the rhythm
4. Modify drum sounds with tuning, decay, and overdrive controls
5. Click the display area to focus on individual channels for detailed editing

## Roadmap

- [ ] MIDI note input (drums follow external sequencing)
- [ ] Pattern presets/saving
- [ ] More drum samples (jazz kit, acoustic, electronic)
- [ ] FX rack (reverb, compression, EQ per channel)
- [ ] Swing/humanization controls
- [ ] Arpeggiator for synth channels

## License

GPL v3.0
