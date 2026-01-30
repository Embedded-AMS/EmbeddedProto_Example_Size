################################################################################
# CMakeLists.txt for STM32F446RE EmbeddedProto Size Comparison
#
# This CMake file builds the STM32 firmware for size comparison testing.
# It supports building with different serialization libraries:
# - EmbeddedProto (EAMS)
# - NanoPB
# - ArduinoJson
#
# Usage:
#   mkdir build && cd build
#   cmake -DLIBRARY=arduinojson -DCMAKE_BUILD_TYPE=Release ..
#   make
#
# Library options: embeddedproto, nanopb, arduinojson
################################################################################

cmake_minimum_required(VERSION 3.16)

# Set the toolchain file before project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Find ARM toolchain
set(ARM_TOOLCHAIN_SEARCH_PATHS
    "/home/bart/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.0.202410170706/tools/bin"
    "/home/bart/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.12.3.rel1.linux64_1.0.100.202403111256/tools/bin"
    "/usr/bin"
    "/usr/local/bin"
)

# Find the toolchain
foreach(PATH ${ARM_TOOLCHAIN_SEARCH_PATHS})
    if(EXISTS "${PATH}/arm-none-eabi-gcc")
        set(ARM_TOOLCHAIN_PATH ${PATH})
        break()
    endif()
endforeach()

# Also try glob patterns
if(NOT ARM_TOOLCHAIN_PATH)
    file(GLOB ARM_TOOLCHAIN_GLOB_PATHS 
        "/home/*/st/stm32cubeide*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*/tools/bin"
    )
    foreach(PATH ${ARM_TOOLCHAIN_GLOB_PATHS})
        if(EXISTS "${PATH}/arm-none-eabi-gcc")
            set(ARM_TOOLCHAIN_PATH ${PATH})
            break()
        endif()
    endforeach()
endif()

if(NOT ARM_TOOLCHAIN_PATH)
    message(FATAL_ERROR "ARM toolchain not found!")
endif()

message(STATUS "ARM toolchain: ${ARM_TOOLCHAIN_PATH}")

set(CMAKE_C_COMPILER ${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${ARM_TOOLCHAIN_PATH}/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc)
set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_PATH}/arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP ${ARM_TOOLCHAIN_PATH}/arm-none-eabi-objdump)
set(CMAKE_SIZE ${ARM_TOOLCHAIN_PATH}/arm-none-eabi-size)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(EmbeddedProto_Example_Size C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Library selection (default to arduinojson for this task)
set(LIBRARY "arduinojson" CACHE STRING "Library to test: embeddedproto, nanopb, arduinojson")
set_property(CACHE LIBRARY PROPERTY STRINGS embeddedproto nanopb arduinojson)

# Scenario selection (required)
set(SCENARIO "" CACHE STRING "Scenario to test: simple_types, nested_message")
if(NOT SCENARIO)
    message(FATAL_ERROR "SCENARIO must be defined. Use -DSCENARIO=<scenario_name>")
endif()

message(STATUS "Building for library: ${LIBRARY}")
message(STATUS "Building for scenario: ${SCENARIO}")

# Base directory
set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# MCU flags for STM32F446RE (Cortex-M4 with FPU)
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Compiler flags
set(COMMON_FLAGS "${MCU_FLAGS} -Wall -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++17 -fno-exceptions -fno-rtti -fno-use-cxa-atexit")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp")

# Release/Debug specific flags
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-Og -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3 -DDEBUG")

# Linker flags
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F446RETX_FLASH.ld)
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -T${LINKER_SCRIPT} --specs=nosys.specs --specs=nano.specs -Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map -static -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group")

# Definitions
add_definitions(-DUSE_HAL_DRIVER -DSTM32F446xx)

# Set library-specific define
if(LIBRARY STREQUAL "embeddedproto")
    add_definitions(-DEAMS)
elseif(LIBRARY STREQUAL "nanopb")
    add_definitions(-DNANOPB)
elseif(LIBRARY STREQUAL "arduinojson")
    add_definitions(-DARDUINOJSON)
endif()

# Set scenario-specific define
if(SCENARIO STREQUAL "simple_types")
    add_definitions(-DSCENARIO_simple_types)
elseif(SCENARIO STREQUAL "nested_message")
    add_definitions(-DSCENARIO_nested_message)
else()
    message(FATAL_ERROR "Unknown scenario: ${SCENARIO}")
endif()

# Include directories (common)
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
)

# Library-specific includes
if(LIBRARY STREQUAL "embeddedproto")
    list(APPEND INCLUDE_DIRS
        ${BASE_DIR}/EmbeddedProto/src
        ${BASE_DIR}/generated/EAMS/${SCENARIO}
    )
elseif(LIBRARY STREQUAL "nanopb")
    list(APPEND INCLUDE_DIRS
        ${BASE_DIR}/Nanopb
        ${BASE_DIR}/generated/Nanopb/${SCENARIO}
    )
elseif(LIBRARY STREQUAL "arduinojson")
    list(APPEND INCLUDE_DIRS
        ${BASE_DIR}/ArduinoJson/src
    )
endif()

include_directories(${INCLUDE_DIRS})

# Source files (common)
set(SOURCES
    # Core sources
    Core/Src/main.cpp
    Core/Src/stm32f4xx_hal_msp.c
    Core/Src/stm32f4xx_it.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    Core/Src/system_stm32f4xx.c
    
    # Startup
    Core/Startup/startup_stm32f446retx.s
    
    # HAL drivers
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
)

# Library-specific sources
if(LIBRARY STREQUAL "embeddedproto")
    list(APPEND SOURCES
        ${BASE_DIR}/EmbeddedProto/src/Fields.cpp
        ${BASE_DIR}/EmbeddedProto/src/MessageInterface.cpp
        ${BASE_DIR}/EmbeddedProto/src/ReadBufferSection.cpp
    )
elseif(LIBRARY STREQUAL "nanopb")
    list(APPEND SOURCES
        ${BASE_DIR}/Nanopb/pb_common.c
        ${BASE_DIR}/Nanopb/pb_encode.c
        ${BASE_DIR}/Nanopb/pb_decode.c
        ${BASE_DIR}/generated/Nanopb/${SCENARIO}/message.pb.c
    )
    # ArduinoJson is header-only, no additional sources needed
endif()

# Scenario-specific test sources
if(SCENARIO STREQUAL "simple_types")
    list(APPEND SOURCES
        ${BASE_DIR}/scenarios/simple_types/test_simple_types.cpp
    )
elseif(SCENARIO STREQUAL "nested_message")
    list(APPEND SOURCES
        ${BASE_DIR}/scenarios/nested_message/test_nested_message.cpp
    )
endif()

# Create executable
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Post-build: create bin and hex files, show size
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
    COMMENT "Building ${PROJECT_NAME}.bin and ${PROJECT_NAME}.hex"
)
