cmake_minimum_required(VERSION 3.22)

project(tectonic VERSION 1.0.0 LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable JUCE CMake API
add_subdirectory(JUCE)

# Add compiled binary assets (your SVG background and renamed drum samples)
juce_add_binary_data(TectonicAssets
    SOURCES
        assets/images/tectonic_panel.svg
        
        assets/samples/D1_Kick_1.wav
        assets/samples/D1_Kick_2.wav
        assets/samples/D1_Kick_3.wav
        assets/samples/D1_Kick_4.wav
        
        assets/samples/D2_Snare_1.wav
        assets/samples/D2_Snare_2.wav
        assets/samples/D2_Snare_3.wav
        assets/samples/D2_Snare_4.wav
        
        assets/samples/D3_OHH_1.wav
        assets/samples/D3_OHH_2.wav
        assets/samples/D3_OHH_3.wav
        assets/samples/D3_OHH_4.wav
        
        assets/samples/D4_CHH_1.wav
        assets/samples/D4_CHH_2.wav
        assets/samples/D4_CHH_3.wav
        assets/samples/D4_CHH_4.wav
        
        assets/samples/D5_Clap_1.wav
        assets/samples/D5_Clap_2.wav
        assets/samples/D5_Clap_3.wav
        assets/samples/D5_Clap_4.wav
        
        assets/samples/D6_Bell_1.wav
        assets/samples/D6_Bell_2.wav
        assets/samples/D6_Bell_3.wav
        assets/samples/D6_Bell_4.wav
)

# Declare the audio plugin (Updated to Navy Audio)
juce_add_plugin(tectonic
    COMPANY_NAME "Navy Audio"
    PLUGIN_NAME "Tectonic"
    PRODUCT_NAME "Tectonic"
    FORMATS VST3 AU Standalone
    IS_SYNTH TRUE                          
    NEEDS_MIDI_INPUT TRUE                  
    NEEDS_MIDI_OUTPUT TRUE                 
    COPY_PLUGIN_AFTER_BUILD TRUE           
)

# Generate standard JuceHeader.h for your source files
juce_generate_juce_header(tectonic)

# Compile source files (including TectonicChannel files)
target_sources(tectonic PRIVATE
    source/PluginProcessor.cpp
    source/PluginProcessor.h
    source/PluginEditor.cpp
    source/PluginEditor.h
    source/TectonicChannel.cpp
    source/TectonicChannel.h
)

# Compile options & optimizations
target_compile_definitions(tectonic
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link modules and the compiled assets
target_link_libraries(tectonic
    PRIVATE
        TectonicAssets
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_extra
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_events
        juce::juce_core
)
