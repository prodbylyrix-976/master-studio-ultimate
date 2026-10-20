cmake_minimum_required(VERSION 3.22)
project(MasterStudioUltimate VERSION 0.1.0)

# Install JUCE separately, then configure with:
# cmake -S . -B build -DJUCE_DIR=/path/to/JUCE
if(NOT DEFINED JUCE_DIR)
  message(FATAL_ERROR "Set JUCE_DIR to your local JUCE checkout (or adapt this project to your JUCE package).")
endif()
add_subdirectory(${JUCE_DIR} JUCE)

juce_add_plugin(MasterStudioUltimate
    COMPANY_NAME "Master Studio"
    PLUGIN_MANUFACTURER_CODE MStu
    PLUGIN_CODE MsU1
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Master Studio Ultimate"
    DESCRIPTION "Mixing, mastering and vocal tools prototype"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(MasterStudioUltimate)
target_sources(MasterStudioUltimate PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

target_compile_features(MasterStudioUltimate PRIVATE cxx_std_17)
target_compile_definitions(MasterStudioUltimate PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(MasterStudioUltimate PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)
