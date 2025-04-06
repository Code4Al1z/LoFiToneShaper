cmake_minimum_required(VERSION 3.24)
project(LoFiToneShaper VERSION 0.0.1)

# JUCE module paths (ensure JUCE is correctly included)
add_subdirectory(modules/JUCE)

# Define the plugin target
juce_add_plugin(LoFiToneShaper
    COMPANY_NAME "Trailblaiz"         # Replace with your company name
    PLUGIN_MANUFACTURER_CODE JucePlugin_ManufacturerCode
    PLUGIN_CODE JucePlugin_Code
    VERSION "${PROJECT_VERSION}"
    DESCRIPTION "In short, the Lo-Fi Tone Shaper is like a toolbox of effects specifically chosen and designed to help you easily and creatively make your music sound like it has that beloved lo-fi character, with a special knob to help keep the important parts sounding clear."
    # You can add any other properties you need here, e.g., JUCE settings for your plugin

    # Include your plugin source code
    FORMATS AU VST3 AAX Standalone
    PRODUCT_NAME "LoFiToneShaper"
)

# Add your source files
target_sources(LoFiToneShaper PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    # Include any other files you want to compile, such as your custom DSP, or other source files
)

# If you need additional JUCE modules for DSP, GUI, etc., you can add them here:
target_link_libraries(LoFiToneShaper PRIVATE
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_audio_basics
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_gui_basics
    juce::juce_core
    juce::juce_data_structures
    juce::juce_audio_utils
    juce::juce_audio_devices
    # Add other JUCE modules you might need like juce_audio_basics, juce_gui_basics, etc.
)

# Set the standard C++ version (make sure to use at least C++17 for JUCE projects)
target_compile_features(LoFiToneShaper PUBLIC cxx_std_17)


# Include JUCE modules you need
juce_generate_juce_header(LoFiToneShaper)

# Specify the location of the plugin's resources (icons, etc.)
target_include_directories(LoFiToneShaper PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
)

# You might also want to specify custom build settings here, like optimization or debug flags.
