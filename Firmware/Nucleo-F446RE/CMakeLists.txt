# Main platform code configuration for Nucleo-F446RE

# Create the main executable target
add_executable(nucleo_f446re_app)

# Set ARM-specific compiler flags
target_compile_options(nucleo_f446re_app PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -specs=nosys.specs
)

# Set ARM-specific linker flags
target_link_options(nucleo_f446re_app PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -specs=nosys.specs
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# Include directories
target_include_directories(nucleo_f446re_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Generated
)

# Add platform source files
target_sources(nucleo_f446re_app PRIVATE
    main.cpp
    platform.cpp
)

# Link with scenario library (defined in Scenarios/CMakeLists.txt)
target_link_libraries(nucleo_f446re_app PRIVATE scenario_lib)

# Link with generated code (defined in Generated/CMakeLists.txt)
target_link_libraries(nucleo_f446re_app PRIVATE generated_code)

# Set output directory
set_target_properties(nucleo_f446re_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Add custom target for flashing (placeholder)
add_custom_target(flash
    COMMAND echo "Flash command would go here"
    DEPENDS nucleo_f446re_app
)