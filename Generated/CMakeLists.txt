# Generated code configuration

# Find all header and source files in the Generated directory
file(GLOB GENERATED_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB GENERATED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Create a library target for generated code if there are any files
if(GENERATED_HEADERS OR GENERATED_SOURCES)
    add_library(generated_code STATIC ${GENERATED_HEADERS} ${GENERATED_SOURCES})

    # Set include directories
    target_include_directories(generated_code PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Set ARM-specific compiler flags for generated code
    target_compile_options(generated_code PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
    )

    # Set output directory
    set_target_properties(generated_code PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )

    # Display what generated files were found
    if(GENERATED_HEADERS)
        message(STATUS "Found generated headers: ${GENERATED_HEADERS}")
    endif()
    if(GENERATED_SOURCES)
        message(STATUS "Found generated sources: ${GENERATED_SOURCES}")
    endif()
else()
    # Create an interface library if no generated files exist yet
    add_library(generated_code INTERFACE)
    message(STATUS "No generated code files found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# Add a custom target for code generation (placeholder)
add_custom_target(generate_code
    COMMAND echo "Code generation command would go here"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating code..."
)