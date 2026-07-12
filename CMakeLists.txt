cmake_minimum_required(VERSION 3.15)

# Added 'C' alongside 'CXX' to match Navy Arp compiler setup
project(tectonic VERSION 1.0.0 LANGUAGES C CXX)

# 1. Include your local JUCE directory
add_subdirectory(JUCE)

# 2. Add and define the Tectonic plugin targets (STRICTLY VST3 ONLY)
juce_add_plugin(tectonic
    COMPANY_NAME "Navy Audio"
    BUNDLE_ID "com.navyaudio.tectonic"       
    PLUGIN_MANUFACTURER_CODE "Navy"         
    PLUGIN_CODE "Tect"                      
    FORMATS VST3                            # Strictly VST3 format only
    PRODUCT_NAME "Tectonic"
    PLUGIN_NAME "Tectonic"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE                  
    NEEDS_MIDI_OUTPUT TRUE                 
    COPY_PLUGIN_AFTER_BUILD FALSE           # Disable default multi-folder copy step
)

# 3. Mandatory generator macro call to create JuceHeader.h for your target
juce_generate_juce_header(tectonic)

# 4. Custom post-build step to copy ONLY the single flat VST3 file to your custom folder
add_custom_command(TARGET tectonic_VST3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "D:/Music - Software/VST"
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:tectonic_VST3>"
    "D:/Music - Software/VST/Tectonic.vst3"
)

# 5. Source Files mapping
target_sources(tectonic PRIVATE
    source/PluginProcessor.cpp
    source/PluginProcessor.h
    source/PluginEditor.cpp
    source/PluginEditor.h
    source/TectonicChannel.cpp
    source/TectonicChannel.h
)

# 6. Global compiler definitions
target_compile_definitions(tectonic
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1 # Added to bypass parameter automation ID conflict checks
)

# Add and define binary data target to compile our renamed drum samples
juce_add_binary_data(TectonicAssets
    SOURCES
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

# 7. Library dependencies
target_link_libraries(tectonic
    PRIVATE
        TectonicAssets       
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_extra
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_events
        juce::juce_data_structures
        juce::juce_core
)
