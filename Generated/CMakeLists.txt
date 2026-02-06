# Generated code configuration

# Find protoc compiler
find_program(PROTOC protoc)
if(NOT PROTOC)
    message(FATAL_ERROR "protoc not found! Please install protobuf compiler.")
endif()
    
# Find all proto files
file(GLOB PROTO_FILES "${CMAKE_SOURCE_DIR}/Protofiles/*.proto")

# Set up code generation based on the selected library
if(LIBRARY STREQUAL "EmbeddedProto")
    # EmbeddedProto code generation
    set(EMBEDDEDPROTO_DIR "${CMAKE_SOURCE_DIR}/External/EmbeddedProto")
    set(PROTOC_GEN_EAMS "${EMBEDDEDPROTO_DIR}/protoc-gen-eams")
    
    # Check if the generator exists
    if(NOT EXISTS "${PROTOC_GEN_EAMS}")
        message(FATAL_ERROR "EmbeddedProto generator not found at ${PROTOC_GEN_EAMS}")
    endif()
    
    # Make the generator executable
    execute_process(COMMAND chmod +x "${PROTOC_GEN_EAMS}")
    
    # Generate code for each proto file at configure time
    foreach(PROTO_FILE ${PROTO_FILES})
        get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
        set(OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_NAME}.h")
        
        if(NOT EXISTS "${OUTPUT_FILE}" OR "${PROTO_FILE}" IS_NEWER_THAN "${OUTPUT_FILE}")
            message(STATUS "Generating EmbeddedProto code for ${PROTO_NAME}")
            execute_process(
                COMMAND ${PROTOC}
                    --plugin=protoc-gen-eams=${PROTOC_GEN_EAMS}
                    --eams_out=${CMAKE_CURRENT_SOURCE_DIR}
                    --proto_path=${CMAKE_SOURCE_DIR}/Protofiles
                    ${PROTO_FILE}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Protofiles
                RESULT_VARIABLE PROTOC_RESULT
                ERROR_VARIABLE PROTOC_ERROR
            )
            if(NOT PROTOC_RESULT EQUAL 0)
                message(FATAL_ERROR "EmbeddedProto code generation failed: ${PROTOC_ERROR}")
            endif()
        endif()

        list(APPEND GENERATED_HEADERS "${OUTPUT_FILE}")
    endforeach()

    # Create a library target for generated code if there are any files
    if(GENERATED_HEADERS)
        # EmbeddedProto only generates header files, so create an INTERFACE library
        add_library(generated_code INTERFACE)
        target_include_directories(generated_code INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    
elseif(LIBRARY STREQUAL "NanoPB")
    # NanoPB code generation
    set(NANOPB_DIR "${CMAKE_SOURCE_DIR}/External/Nanopb")
    set(NANOPB_VENV_DIR "${NANOPB_DIR}/venv")
    set(NANOPB_VENV_PYTHON "${NANOPB_VENV_DIR}/bin/python3")
    set(PROTOC_GEN_NANOPB "${NANOPB_DIR}/generator/protoc-gen-nanopb")
    
    # Check if venv exists, if not create it and install dependencies
    if(NOT EXISTS "${NANOPB_VENV_PYTHON}")
        message(STATUS "Creating Python venv for NanoPB at ${NANOPB_VENV_DIR}")
        execute_process(
            COMMAND python3 -m venv "${NANOPB_VENV_DIR}"
            RESULT_VARIABLE VENV_RESULT
            ERROR_VARIABLE VENV_ERROR
        )
        if(NOT VENV_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to create venv: ${VENV_ERROR}")
        endif()
        
        message(STATUS "Installing NanoPB Python dependencies...")
        execute_process(
            COMMAND "${NANOPB_VENV_PYTHON}" -m pip install --upgrade pip protobuf grpcio-tools
            RESULT_VARIABLE PIP_RESULT
            ERROR_VARIABLE PIP_ERROR
        )
        if(NOT PIP_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to install dependencies: ${PIP_ERROR}")
        endif()
    endif()
    
    # Check if the generator exists
    if(NOT EXISTS "${PROTOC_GEN_NANOPB}")
        message(FATAL_ERROR "NanoPB generator not found at ${PROTOC_GEN_NANOPB}")
    endif()
    
    # Make the generator executable
    execute_process(COMMAND chmod +x "${PROTOC_GEN_NANOPB}")
    
    # Create a wrapper script that uses the venv Python with proper PYTHONPATH
    set(NANOPB_WRAPPER "${CMAKE_BINARY_DIR}/protoc-gen-nanopb-wrapper")
    file(WRITE "${NANOPB_WRAPPER}" "#!/bin/bash\nexport PYTHONPATH=\"${NANOPB_DIR}/generator\"\nexec \"${NANOPB_VENV_PYTHON}\" \"${PROTOC_GEN_NANOPB}\" \"$@\"\n")
    execute_process(COMMAND chmod +x "${NANOPB_WRAPPER}")
    
    # Generate code for each proto file at configure time
    foreach(PROTO_FILE ${PROTO_FILES})
        get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
        set(OUTPUT_H "${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_NAME}.pb.h")
        set(OUTPUT_C "${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_NAME}.pb.c")

        # Check if options file exists for this proto file
        set(OPTIONS_FILE "${CMAKE_SOURCE_DIR}/Protofiles/${PROTO_NAME}.options")
        set(NANOPB_OPTIONS "")
        if(EXISTS "${OPTIONS_FILE}")
            set(NANOPB_OPTIONS "--nanopb_opt=${OPTIONS_FILE}")
        endif()

        if(NOT EXISTS "${OUTPUT_H}" OR "${PROTO_FILE}" IS_NEWER_THAN "${OUTPUT_H}")
            message(STATUS "Generating NanoPB code for ${PROTO_NAME}")
            execute_process(
                COMMAND ${PROTOC}
                    --plugin=protoc-gen-nanopb=${NANOPB_WRAPPER}
                    --nanopb_out=${CMAKE_CURRENT_SOURCE_DIR}
                    --proto_path=${CMAKE_SOURCE_DIR}/Protofiles
                    ${NANOPB_OPTIONS}
                    ${PROTO_NAME}.proto
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Protofiles
                RESULT_VARIABLE PROTOC_RESULT
                ERROR_VARIABLE PROTOC_ERROR
            )
            if(NOT PROTOC_RESULT EQUAL 0)
                message(FATAL_ERROR "NanoPB code generation failed: ${PROTOC_ERROR}")
            endif()
        endif()

        list(APPEND GENERATED_SOURCES "${OUTPUT_H}" "${OUTPUT_C}")
    endforeach()
    
    # Create a library target for generated code if there are any files
    if(GENERATED_SOURCES)
        add_library(generated_code STATIC ${GENERATED_SOURCES})
        target_include_directories(generated_code PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
        target_include_directories(generated_code PUBLIC ${NANOPB_DIR})

        # Add NanoPB runtime source files
        set(NANOPB_RUNTIME_SOURCES
            "${NANOPB_DIR}/pb_common.c"
            "${NANOPB_DIR}/pb_encode.c"
            "${NANOPB_DIR}/pb_decode.c"
        )
        target_sources(generated_code PRIVATE ${NANOPB_RUNTIME_SOURCES})
    endif()
    
else()
    # For ArduinoJson and Baseline, no code generation is needed
    message(STATUS "No code generation required for ${LIBRARY}")
endif()
