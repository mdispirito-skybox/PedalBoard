cmake_minimum_required(VERSION 3.15)
project(GuitarAmpSim VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/JUCE JUCE)

juce_add_gui_app(GuitarAmpSim
    PRODUCT_NAME "Big Mike's PedalBoard"
    VERSION "0.1.0"
    ICON_BIG ""
    ICON_SMALL ""
)

juce_add_binary_data(GuitarAmpSimAssets
    SOURCES
        Resources/ORANGE.wav
        Resources/VOX.wav
)

target_sources(GuitarAmpSim PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/AmpSimProcessor.cpp
    Source/CabSimProcessor.cpp
    Source/GuitarRigEngine.cpp
    Source/FuzzProcessor.cpp
    Source/ChorusProcessor.cpp
)

target_include_directories(GuitarAmpSim PRIVATE 
    include
    libs/ASIOSDK/common)

target_compile_definitions(GuitarAmpSim PRIVATE
    JUCE_ASIO=1
)

target_link_libraries(GuitarAmpSim PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_dsp
    GuitarAmpSimAssets
)
