cmake_minimum_required(VERSION 3.22)

project(GuitarTools VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(JUCE)

# Auto-add: any .cpp/.h dropped under Source/ is picked up
# on the next configure, no need to edit this file. CONFIGURE_DEPENDS makes the
# build system re-run the glob whenever it re-checks CMake's inputs.
file(GLOB_RECURSE GUITARTOOLS_SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h")
file(GLOB_RECURSE GUITARTOOLS_RESOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/*")
list(FILTER GUITARTOOLS_RESOURCE_FILES EXCLUDE REGEX "/\\.DS_Store$")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    PREFIX "Source"
    FILES ${GUITARTOOLS_SOURCE_FILES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    PREFIX "Resources"
    FILES ${GUITARTOOLS_RESOURCE_FILES})

# Icons/fonts are embedded into the binary so the plugin is self-contained and
# works on any machine, regardless of whether the source tree is present.
juce_add_binary_data(GuitarToolsData SOURCES ${GUITARTOOLS_RESOURCE_FILES})

juce_add_plugin(GuitarTools
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "Aaron Petrini"
    COMPANY_WEBSITE "www.AaronPetrini.com"
    BUNDLE_ID com.AaronPetrini.GuitarTools2
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Uwz1
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "GuitarTools2")

juce_generate_juce_header(GuitarTools)

target_sources(GuitarTools
    PRIVATE
        ${GUITARTOOLS_SOURCE_FILES}
        ${GUITARTOOLS_RESOURCE_FILES})

target_include_directories(GuitarTools
    PRIVATE
        Source)

target_compile_definitions(GuitarTools
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(GuitarTools
    PRIVATE
        GuitarToolsData
        juce::juce_dsp
        juce::juce_gui_extra
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
