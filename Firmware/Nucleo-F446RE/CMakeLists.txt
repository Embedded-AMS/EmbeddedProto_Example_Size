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

# Main platform code configuration for Nucleo-F446RE

# Create the main executable target
add_executable(nucleo_f446re_app)

# Set compiler flags
target_compile_options(nucleo_f446re_app PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
)

# Add STM32F446xx device definition required by CMSIS headers
target_compile_definitions(nucleo_f446re_app PRIVATE
    STM32F446xx
)

# Set linker flags with linker script
target_link_options(nucleo_f446re_app PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F446RETX_FLASH.ld
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,-Map=${CMAKE_BINARY_DIR}/bin/nucleo_f446re_app.map
    --specs=nosys.specs
    --specs=nano.specs
)

# Include directories
target_include_directories(nucleo_f446re_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Generated
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Scenarios/EmbeddedProto
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
)

# Add platform source files
target_sources(nucleo_f446re_app PRIVATE
    Core/Startup/startup_stm32f446retx.s
    Core/Src/main.cpp
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
    Core/Src/system_stm32f4xx.c
    Core/Src/stm32f4xx_hal_msp.c
    Core/Src/stm32f4xx_it.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
)

# Link with scenario library (defined in Scenarios/CMakeLists.txt)
target_link_libraries(nucleo_f446re_app PRIVATE scenario_lib)

# Link with generated code (defined in Generated/CMakeLists.txt) if it exists
# Only EmbeddedProto and NanoPB create generated_code library
if(TARGET generated_code)
    target_link_libraries(nucleo_f446re_app PRIVATE generated_code)
endif()

# Set output directory and .elf suffix
set_target_properties(nucleo_f446re_app PROPERTIES
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Generate HEX and BIN files after build
add_custom_command(TARGET nucleo_f446re_app POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex $<TARGET_FILE:nucleo_f446re_app> ${CMAKE_BINARY_DIR}/bin/nucleo_f446re_app.hex
    COMMAND arm-none-eabi-objcopy -O binary $<TARGET_FILE:nucleo_f446re_app> ${CMAKE_BINARY_DIR}/bin/nucleo_f446re_app.bin
    COMMAND arm-none-eabi-size $<TARGET_FILE:nucleo_f446re_app>
    COMMENT "Generating HEX and BIN files..."
)

# Add custom target for flashing via OpenOCD (ST-Link on Nucleo board)
add_custom_target(flash
    COMMAND openocd -f interface/stlink.cfg -f target/stm32f4x.cfg 
            -c "program $<TARGET_FILE:nucleo_f446re_app> verify reset exit"
    DEPENDS nucleo_f446re_app
    COMMENT "Flashing ${CMAKE_BINARY_DIR}/bin/nucleo_f446re_app.elf to Nucleo-F446RE..."
)
