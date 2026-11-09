cmake_minimum_required(VERSION 3.22)
project(RottingBass VERSION 0.2.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
 set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
if(DEFINED JUCE_DIR)
 find_package(JUCE 8.0.6 CONFIG REQUIRED)
elseif(DEFINED JUCE_SOURCE_DIR)
 add_subdirectory("${JUCE_SOURCE_DIR}" JUCE)
else()
 FetchContent_Declare(JUCE URL https://github.com/juce-framework/JUCE/archive/refs/tags/8.0.6.tar.gz URL_HASH SHA256=95752f8e8b37d802b4c10182bc757de7d88f0c8899e879afa8798589be306a11)
 FetchContent_MakeAvailable(JUCE)
endif()
juce_add_plugin(RottingBass
 COMPANY_NAME Rottingpitstufio
 BUNDLE_ID studio.rottingpitstufio.rottingbass
 PLUGIN_MANUFACTURER_CODE Rtpt
 PLUGIN_CODE RtBs
 FORMATS VST3
 PRODUCT_NAME RottingBass
 IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE
 COPY_PLUGIN_AFTER_BUILD FALSE VST3_CATEGORIES Fx)
target_sources(RottingBass PRIVATE Source/Plugin.cpp)
target_compile_definitions(RottingBass PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(RottingBass PRIVATE juce::juce_audio_processors juce::juce_gui_basics juce::juce_dsp PUBLIC juce::juce_recommended_config_flags)
option(RB_BUILD_TESTS "Build processor smoke test" OFF)
if(RB_BUILD_TESTS)
 juce_add_console_app(RottingBassSmoke PRODUCT_NAME RottingBassSmoke)
 target_sources(RottingBassSmoke PRIVATE tests/Smoke.cpp)
 target_include_directories(RottingBassSmoke PRIVATE $<TARGET_PROPERTY:RottingBass,INCLUDE_DIRECTORIES>)
 target_compile_definitions(RottingBassSmoke PRIVATE $<FILTER:$<TARGET_PROPERTY:RottingBass,COMPILE_DEFINITIONS>,EXCLUDE,^JUCE_STANDALONE_APPLICATION=>)
 target_link_libraries(RottingBassSmoke PRIVATE RottingBass)
endif()

if(RB_BUILD_TESTS)
 juce_add_console_app(RottingBassHostTest PRODUCT_NAME RottingBassHostTest)
 target_sources(RottingBassHostTest PRIVATE tests/HostSmoke.cpp)
 target_compile_definitions(RottingBassHostTest PRIVATE JUCE_PLUGINHOST_VST3=1 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
 target_link_libraries(RottingBassHostTest PRIVATE juce::juce_audio_processors juce::juce_gui_basics)
endif()
