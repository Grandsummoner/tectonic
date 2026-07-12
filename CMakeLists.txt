cmake_minimum_required(VERSION 3.22)

project(tectonic VERSION 1.0.0 LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable JUCE CMake API
add_subdirectory(JUCE)

# Add compiled binary assets (your SVG background)
juce_add_binary_data(TectonicAssets
    SOURCES
        assets/images/tectonic_panel.svg
)

# Declare the audio plugin
juce_add_plugin(tectonic
    COMPANY_NAME "Grandsummoner"
    PLUGIN_NAME "Tectonic"
    PRODUCT_NAME "Tectonic"
    FORMATS VST3 AU Standalone
    IS_SYNTH TRUE                          
    NEEDS_MIDI_INPUT TRUE                  
    NEEDS_MIDI_OUTPUT TRUE                 
    COPY_PLUGIN_AFTER_BUILD TRUE           
)

# Compile source files
target_sources(tectonic PRIVATE
    source/PluginProcessor.cpp
    source/PluginProcessor.h
    source/PluginEditor.cpp
    source/PluginEditor.h
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
