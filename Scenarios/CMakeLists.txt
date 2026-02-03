# Scenario selection and configuration

# Check if LIBRARY and SCENARIO parameters are set
if(NOT LIBRARY OR NOT SCENARIO)
    message(FATAL_ERROR "LIBRARY and SCENARIO parameters must be specified!")
endif()

# Define valid library options
set(VALID_LIBRARIES "ArduinoJson" "EmbeddedProto" "NanoPB" "Baseline")

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

# Set ARM-specific compiler flags for scenario code
target_compile_options(scenario_lib PRIVATE
)

# Add definitions for the selected scenario
target_compile_definitions(scenario_lib PRIVATE
    SCENARIO_LIBRARY="${LIBRARY}"
    SCENARIO_NAME="${SCENARIO}"
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