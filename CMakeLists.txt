cmake_minimum_required(VERSION 3.15)
project(GuitarAmpSim VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point this to your JUCE folder
add_subdirectory(libs/JUCE)

juce_add_executable(GuitarAmpSim
    PRODUCT_NAME "Big Mike's PedalBoard"
    VERSION "0.1.0"
    ICON_BIG ""
    ICON_SMALL ""
    COMPANY_NAME "I need to think of a company name later..."
)

target_sources(GuitarAmpSim PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/AmpSimProcessor.cpp
)

target_include_directories(GuitarAmpSim PRIVATE Source)

target_link_libraries(GuitarAmpSim PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_gui_extra
)
