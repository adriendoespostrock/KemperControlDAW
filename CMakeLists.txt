cmake_minimum_required(VERSION 3.22)
project(KemperControl VERSION 0.3.6)

set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/JUCE" CACHE PATH "Path to JUCE source")
if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "Set JUCE_DIR to your JUCE source directory")
endif()
add_subdirectory("${JUCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/JUCE")

function(configure_kemper_target target)
    juce_generate_juce_header(${target})
    target_sources(${target} PRIVATE
        Source/PluginProcessor.cpp Source/PluginProcessor.h
        Source/PluginEditor.cpp Source/PluginEditor.h
        Source/DirectMidiOutput.h Source/MidiPacketQueue.h)
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_compile_definitions(${target} PRIVATE
        JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
    target_link_libraries(${target} PRIVATE juce::juce_audio_utils
        juce::juce_recommended_config_flags juce::juce_recommended_warning_flags
        juce::juce_recommended_lto_flags)
endfunction()

# Live: a silent instrument with a real stereo output bus and MIDI output.
# Keep the original VST3 identity and parameter IDs.
juce_add_plugin(KemperControl
    COMPANY_NAME "Adrien Deurveilher"
    PRODUCT_NAME "Kemper Control"
    BUNDLE_ID "com.adrien.kempercontrol"
    PLUGIN_MANUFACTURER_CODE Adr1 PLUGIN_CODE Kmp1
    FORMATS VST3 Standalone
    IS_SYNTH TRUE IS_MIDI_EFFECT FALSE
    NEEDS_MIDI_INPUT TRUE NEEDS_MIDI_OUTPUT TRUE
    VST3_CATEGORIES Instrument
    COPY_PLUGIN_AFTER_BUILD TRUE)
configure_kemper_target(KemperControl)

# Logic: a separate MIDI FX build, without audio buses.
if(APPLE)
    juce_add_plugin(KemperControlLogic
        COMPANY_NAME "Adrien Deurveilher"
        PRODUCT_NAME "Kemper Control"
        BUNDLE_ID "com.adrien.kempercontrol.logic"
        PLUGIN_MANUFACTURER_CODE Adr1 PLUGIN_CODE Kmp1
        FORMATS AU
        IS_SYNTH FALSE IS_MIDI_EFFECT TRUE
        NEEDS_MIDI_INPUT TRUE NEEDS_MIDI_OUTPUT TRUE
        AU_MAIN_TYPE kAudioUnitType_MIDIProcessor
        COPY_PLUGIN_AFTER_BUILD TRUE)
    configure_kemper_target(KemperControlLogic)
endif()
