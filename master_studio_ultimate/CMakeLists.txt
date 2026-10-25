cmake_minimum_required(VERSION 3.22)

project(MasterStudioUltimate
    VERSION 0.1.0
    LANGUAGES CXX
)

# ------------------------------------------------------------
# Configuration de JUCE
# ------------------------------------------------------------

if(NOT DEFINED JUCE_DIR OR NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "JUCE introuvable. Verifie le parametre JUCE_DIR."
    )
endif()

add_subdirectory("${JUCE_DIR}" JUCE)

# ------------------------------------------------------------
# Création du plugin VST3
# Compatible avec les DAW prenant en charge VST3
# ------------------------------------------------------------

juce_add_plugin(MasterStudioUltimate
    COMPANY_NAME "Master Studio"
    BUNDLE_ID "com.masterstudio.ultimate"
    PLUGIN_MANUFACTURER_CODE MStu
    PLUGIN_CODE MsU1
    FORMATS VST3
    PRODUCT_NAME "Master Studio Ultimate"
    DESCRIPTION "Mixing and mastering audio plugin prototype"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

# ------------------------------------------------------------
# Fichiers source
# ------------------------------------------------------------

target_sources(MasterStudioUltimate
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

# ------------------------------------------------------------
# Standard C++
# ------------------------------------------------------------

target_compile_features(MasterStudioUltimate
    PRIVATE
        cxx_std_17
)

# ------------------------------------------------------------
# Définitions JUCE
# ------------------------------------------------------------

target_compile_definitions(MasterStudioUltimate
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# ------------------------------------------------------------
# Modules JUCE nécessaires
# ------------------------------------------------------------

target_link_libraries(MasterStudioUltimate
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

