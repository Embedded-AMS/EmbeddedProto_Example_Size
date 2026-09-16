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

# Scenario selection and configuration

# Check if LIBRARY and SCENARIO parameters are set
if(NOT LIBRARY OR NOT SCENARIO)
    message(FATAL_ERROR "LIBRARY and SCENARIO parameters must be specified!")
endif()

# Define valid library options
set(VALID_LIBRARIES "ArduinoJson" "EmbeddedProto" "NanoPB" "FlatBuffers" "Baseline")

# Validate LIBRARY parameter
list(FIND VALID_LIBRARIES ${LIBRARY} LIBRARY_INDEX)
if(LIBRARY_INDEX EQUAL -1)
    message(FATAL_ERROR "Invalid LIBRARY: ${LIBRARY}. Valid options are: ${VALID_LIBRARIES}")
endif()

# Construct the path to the scenario file
set(SCENARIO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY}/${SCENARIO}")

# Check if the scenario file exists (try .cpp first, then .c)
set(SCENARIO_SOURCE "")
if(EXISTS "${SCENARIO_PATH}.cpp")
    set(SCENARIO_SOURCE "${SCENARIO_PATH}.cpp")
elseif(EXISTS "${SCENARIO_PATH}.c")
    set(SCENARIO_SOURCE "${SCENARIO_PATH}.c")
else()
    message(FATAL_ERROR "Scenario file not found: ${SCENARIO_PATH}.cpp or ${SCENARIO_PATH}.c")
endif()

# Create scenario library target
add_library(scenario_lib STATIC ${SCENARIO_SOURCE})

# Code is generated at configure time, so just link against generated_code library
if(LIBRARY STREQUAL "EmbeddedProto")
    # Link against the generated EmbeddedProto headers
    target_link_libraries(scenario_lib PUBLIC generated_code)
elseif(LIBRARY STREQUAL "NanoPB")
    # Link against the generated NanoPB code and NanoPB runtime
    target_link_libraries(scenario_lib PUBLIC generated_code)
elseif(LIBRARY STREQUAL "FlatBuffers")
    # Link against the flatcc runtime, the generated headers come with it
    target_link_libraries(scenario_lib PUBLIC generated_code)
endif()

# Set include directories for the scenario
target_include_directories(scenario_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Generated
    ${CMAKE_CURRENT_SOURCE_DIR}/../Firmware/Nucleo-F446RE/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Firmware/Nucleo-F446RE/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Firmware/Nucleo-F446RE/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Firmware/Nucleo-F446RE/Drivers/CMSIS/Include
)

# Configure benchmark mode compile definitions for EmbeddedProto runtime.
# Map benchmark mode to EP_SERIALIZATION_MODE build flag:
#   normal  -> (nothing defined)
#   partial -> PARTIAL_SERIALIZATION_ENABLED
if(LIBRARY STREQUAL "EmbeddedProto")
    if(BENCHMARK_MODE STREQUAL "partial")
        target_compile_definitions(scenario_lib PRIVATE
            PARTIAL_SERIALIZATION_ENABLED
        )
    endif()
endif()

# For EmbeddedProto scenarios, add the header-only EmbeddedProto source directory
if(LIBRARY STREQUAL "EmbeddedProto")
    target_include_directories(scenario_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../External/EmbeddedProto/src)

elseif(LIBRARY STREQUAL "ArduinoJson")
    # For ArduinoJson scenarios, add the ArduinoJson include directory
    target_include_directories(scenario_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../External/ArduinoJson/src)
endif()

# Set ARM-specific compiler flags for scenario code
target_compile_options(scenario_lib PRIVATE
)

# Add definitions for the selected scenario
target_compile_definitions(scenario_lib PRIVATE
    SCENARIO_LIBRARY="${LIBRARY}"
    SCENARIO_NAME="${SCENARIO}"
    BENCHMARK_MODE="${BENCHMARK_MODE}"
    STM32F446xx
)

# Set output directory
set_target_properties(scenario_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Display configuration summary
message(STATUS "Building scenario:")
message(STATUS "  Library: ${LIBRARY}")
message(STATUS "  Scenario: ${SCENARIO}")
message(STATUS "  Source file: ${SCENARIO_SOURCE}")