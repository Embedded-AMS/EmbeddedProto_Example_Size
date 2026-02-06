cmake_minimum_required(VERSION 3.10)

# ARM toolchain configuration - must be set BEFORE project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# Prevent CMake from testing the compilers (cross-compilation)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Project configuration
project(EmbeddedProto_Example_Size CXX C ASM)

# Set default build type to Release, but allow Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Optimization flags for cross-compilation (not provided by CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_DEBUG "-Og -g -DDEBUG" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g -DDEBUG" CACHE STRING "" FORCE)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Cortex-M4 specific flags for STM32F446RE
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPU_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPU_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${CPU_FLAGS}")

# Command line parameters for scenario selection
set(LIBRARY "" CACHE STRING "Library subfolder to select scenario from (ArduinoJson, EmbeddedProto, NanoPB)")
set(SCENARIO "" CACHE STRING "Specific scenario file to build (without extension)")

# Validate parameters
if(NOT LIBRARY OR NOT SCENARIO)
    message(WARNING "LIBRARY and SCENARIO parameters must be specified for scenario selection")
    message(WARNING "Usage: cmake -DLIBRARY=<library> -DSCENARIO=<scenario> ..")
endif()

# Add subdirectories
add_subdirectory(Generated)
add_subdirectory(Scenarios)
add_subdirectory(Firmware/Nucleo-F446RE)