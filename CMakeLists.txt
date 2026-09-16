#
#  Copyright (C) 2020-2026 Embedded AMS B.V. - All Rights Reserved
#
#  This file is part of Embedded Proto.
#
#  Embedded Proto is open source software: you can redistribute it and/or 
#  modify it under the terms of the GNU General Public License as published 
#  by the Free Software Foundation, version 3 of the license.
#
#  Embedded Proto  is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
#
#  For commercial and closed source application please visit:
# <https://embeddedproto.com/pricing/>.
#
#  Embedded AMS B.V.
#  Info:
#    info at EmbeddedProto dot com
#
#  Postal address:
#    Atoomweg 2
#    1627 LE, Hoorn
#    the Netherlands
#

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

# C++ flags every embedded toolchain project (STM32CubeIDE, MCUXpresso, PlatformIO) enables by
# default. Without them GCC links the exception unwinder, RTTI and malloc into every C++ build,
# which nanopb (plain C) never pays for and which would skew the comparison by several kilobytes.
set(CXX_EMBEDDED_FLAGS "-fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit")

# Optimization flags for cross-compilation (not provided by CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG -ffunction-sections -fdata-sections ${CXX_EMBEDDED_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DDEBUG -fno-omit-frame-pointer" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG -fno-omit-frame-pointer -fno-inline ${CXX_EMBEDDED_FLAGS}" CACHE STRING "" FORCE)

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
set(LIBRARY "" CACHE STRING "Library subfolder to select scenario from (ArduinoJson, EmbeddedProto, NanoPB, FlatBuffers, Zcbor)")
set(SCENARIO "" CACHE STRING "Specific scenario file to build (without extension)")
set(BENCHMARK_MODE "normal" CACHE STRING "Benchmark mode: normal or partial")

# Validate parameters
if(NOT LIBRARY OR NOT SCENARIO)
    message(WARNING "LIBRARY and SCENARIO parameters must be specified for scenario selection")
    message(WARNING "Usage: cmake -DLIBRARY=<library> -DSCENARIO=<scenario> ..")
endif()

# Validate benchmark mode
if(NOT BENCHMARK_MODE STREQUAL "normal" AND NOT BENCHMARK_MODE STREQUAL "partial")
    message(FATAL_ERROR "Invalid BENCHMARK_MODE: ${BENCHMARK_MODE}. Valid options are: normal, partial")
endif()

# Add subdirectories
add_subdirectory(Generated)
add_subdirectory(Scenarios)
add_subdirectory(Firmware/Nucleo-F446RE)