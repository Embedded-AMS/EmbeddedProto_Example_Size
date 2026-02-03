cmake_minimum_required(VERSION 3.10)

# Project configuration
project(EmbeddedProto_Example_Size CXX C)

# Set default build type to Release, but allow Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ARM toolchain configuration
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Command line parameters for scenario selection
set(LIBRARY "" CACHE STRING "Library subfolder to select scenario from (ArduinoJson, EmbeddedProto, NanoPB)")
set(SCENARIO "" CACHE STRING "Specific scenario file to build (without extension)")

# Validate parameters
if(NOT LIBRARY OR NOT SCENARIO)
    message(WARNING "LIBRARY and SCENARIO parameters must be specified for scenario selection")
    message(WARNING "Usage: cmake -DLIBRARY=<library> -DSCENARIO=<scenario> ..")
endif()

# Add subdirectories
add_subdirectory(Firmware/Nucleo-F446RE)
add_subdirectory(Scenarios)
add_subdirectory(Generated)