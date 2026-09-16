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

        # Field options kept outside the shared .proto (callbackStorage, maxLength) live in
        # an optional <name>.options.json next to it.
        set(OPTIONS_JSON "${CMAKE_SOURCE_DIR}/Protofiles/${PROTO_NAME}.options.json")
        set(EAMS_OPTIONS "")
        if(EXISTS "${OPTIONS_JSON}")
            set(EAMS_OPTIONS "--eams_opt=options_file=${OPTIONS_JSON}")
        endif()

        message(STATUS "Generating EmbeddedProto code for ${PROTO_NAME}")
        execute_process(
            COMMAND ${PROTOC}
                --plugin=protoc-gen-eams=${PROTOC_GEN_EAMS}
                ${EAMS_OPTIONS}
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

        if(NOT EXISTS "${OUTPUT_H}" OR "${PROTO_FILE}" IS_NEWER_THAN "${OUTPUT_H}" OR "${OPTIONS_FILE}" IS_NEWER_THAN "${OUTPUT_H}")
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

        # Leave out nanopb's error strings, as its documentation recommends for a small
        # target; Embedded Proto has none either. PB_NO_ERRMSG changes the stream structs,
        # so it must reach every file which includes a nanopb header, hence PUBLIC.
        target_compile_definitions(generated_code PUBLIC PB_NO_ERRMSG)

        # Add NanoPB runtime source files
        set(NANOPB_RUNTIME_SOURCES
            "${NANOPB_DIR}/pb_common.c"
            "${NANOPB_DIR}/pb_encode.c"
            "${NANOPB_DIR}/pb_decode.c"
        )
        target_sources(generated_code PRIVATE ${NANOPB_RUNTIME_SOURCES})
    endif()
    
elseif(LIBRARY STREQUAL "FlatBuffers")
    # FlatBuffers through flatcc, the C implementation, which is the one that fits a
    # microcontroller. The flatcc compiler is built once for the host from the submodule.
    set(FLATCC_DIR "${CMAKE_SOURCE_DIR}/External/flatcc")
    set(FLATCC_HOST_BUILD "${CMAKE_SOURCE_DIR}/build/flatcc-host")
    set(FLATCC_BIN "${FLATCC_DIR}/bin/flatcc")

    if(NOT EXISTS "${FLATCC_BIN}")
        message(STATUS "Building the flatcc compiler for the host")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -S ${FLATCC_DIR} -B ${FLATCC_HOST_BUILD}
                -DFLATCC_TEST=OFF -DFLATCC_CXX_TEST=OFF -DFLATCC_INSTALL=OFF
                -DFLATCC_REFLECTION=OFF -DCMAKE_BUILD_TYPE=Release
            RESULT_VARIABLE FLATCC_CONFIG_RESULT
            OUTPUT_QUIET
        )
        if(NOT FLATCC_CONFIG_RESULT EQUAL 0)
            message(FATAL_ERROR "Configuring the flatcc host build failed")
        endif()
        execute_process(
            COMMAND ${CMAKE_COMMAND} --build ${FLATCC_HOST_BUILD} --parallel
            RESULT_VARIABLE FLATCC_BUILD_RESULT
            OUTPUT_QUIET
        )
        if(NOT FLATCC_BUILD_RESULT EQUAL 0 OR NOT EXISTS "${FLATCC_BIN}")
            message(FATAL_ERROR "Building the flatcc compiler failed")
        endif()
    endif()

    # Generate reader, builder and verifier headers for every schema at configure time.
    file(GLOB FBS_FILES "${CMAKE_SOURCE_DIR}/Protofiles/*.fbs")
    foreach(FBS_FILE ${FBS_FILES})
        get_filename_component(FBS_NAME ${FBS_FILE} NAME_WE)
        message(STATUS "Generating FlatBuffers code for ${FBS_NAME}")
        execute_process(
            COMMAND ${FLATCC_BIN} -a -I ${CMAKE_SOURCE_DIR}/Protofiles -o ${CMAKE_CURRENT_SOURCE_DIR} ${FBS_FILE}
            RESULT_VARIABLE FLATCC_RESULT
            ERROR_VARIABLE FLATCC_ERROR
        )
        if(NOT FLATCC_RESULT EQUAL 0)
            message(FATAL_ERROR "FlatBuffers code generation failed: ${FLATCC_ERROR}")
        endif()
    endforeach()

    # The runtime the scenarios need: builder and emitter to write, verifier to check a
    # received buffer. JSON support is left out. Allocation goes through the counting
    # allocator so the RAM a build takes can be reported.
    add_library(generated_code STATIC
        "${FLATCC_DIR}/src/runtime/builder.c"
        "${FLATCC_DIR}/src/runtime/emitter.c"
        "${FLATCC_DIR}/src/runtime/refmap.c"
        "${FLATCC_DIR}/src/runtime/verifier.c"
        "${CMAKE_SOURCE_DIR}/Scenarios/FlatBuffers/counting_alloc.c"
    )
    target_include_directories(generated_code PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(generated_code PUBLIC ${FLATCC_DIR}/include)
    target_compile_options(generated_code PUBLIC -include ${CMAKE_SOURCE_DIR}/Scenarios/FlatBuffers/counting_alloc.h)

else()
    # For ArduinoJson and Baseline, no code generation is needed
    message(STATUS "No code generation required for ${LIBRARY}")
endif()
