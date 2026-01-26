#
# Copyright (C) 2020-2024 Embedded AMS B.V. - All Rights Reserved
#
# This file is part of Embedded Proto.
#
# Embedded Proto is open source software: you can redistribute it and/or 
# modify it under the terms of the GNU General Public License as published 
# by the Free Software Foundation, version 3 of the license.
#
# Embedded Proto  is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
#
# For commercial and closed source application please visit:
# <https://EmbeddedProto.com/license/>.
#
# Embedded AMS B.V.
# Info:
#   info at EmbeddedProto dot com
#
# Postal address:
#   Atoomweg 2
#   1627 LE, Hoorn
#   the Netherlands
#

cmake_minimum_required(VERSION 3.5)

project(test_EmbeddedProto)

set(CMAKE_CXX_FLAGS "-std=c++20 -Os -fdata-sections -ffunction-sections -Wl,--gc-sections")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

file(GLOB src_files_EAMS    
    "test/simple_types.cpp"
    "EmbeddedProto/src/*.cpp"
)

file(GLOB src_files_EAMS_generated    
    "test/simple_types.cpp"
    "EmbeddedProto/src/*.cpp"
)

add_executable(size_comparison_EAMS ${src_files_EAMS})
target_link_options(size_comparison_EAMS PRIVATE "LINKER:-Map=${CMAKE_BINARY_DIR}/size_comparison_EAMS.map")
target_include_directories(size_comparison_EAMS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test 
                                                        ${CMAKE_CURRENT_SOURCE_DIR}/generated
                                                        ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS 
                                                        ${CMAKE_CURRENT_SOURCE_DIR}/EmbeddedProto/src)
set_target_properties(size_comparison_EAMS PROPERTIES COMPILE_DEFINITIONS "EAMS")

file(GLOB src_files_EAMS_generated    
    "test/generated_size_test.cpp"
    "EmbeddedProto/src/*.cpp"
)

add_executable(generated_size_test_EAMS ${src_files_EAMS_generated})
target_link_options(generated_size_test_EAMS PRIVATE "LINKER:-Map=${CMAKE_BINARY_DIR}/generated_size_test_EAMS.map")
target_include_directories(generated_size_test_EAMS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test 
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS 
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/subfolder
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/onsemi
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/onsemi/gpio
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/onsemi/i2c
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/onsemi/i2s
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/onsemi/pdm
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/onsemi/pwm
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/onsemi/rcp
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/onsemi/spi
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/generated/EAMS/onsemi/uart
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/EmbeddedProto/src)

#file(GLOB src_files_Google  
#    "generated/Google/*.cc"
#    "test/*.cpp"
#    "GoolgeProtobuf/src/google/protobuf/*.cc"
#)
#
#add_executable(size_comparison_Google ${src_files_Google})
#target_include_directories(size_comparison_Google PUBLIC GoolgeProtobuf/src/google/protobuf)
#set_target_properties(size_comparison_Google PROPERTIES COMPILE_DEFINITIONS "GOOGLE")
#target_link_libraries(size_comparison_Google protobuf)


file(GLOB src_files_Nanopb    
    "test/simple_types.cpp"
    "Nanopb/*.c"
    "generated/Nanopb/*.c"
)

add_executable(size_comparison_Nanopb ${src_files_Nanopb})
target_link_options(size_comparison_Nanopb PRIVATE "LINKER:-Map=${CMAKE_BINARY_DIR}/size_comparison_Nanopb.map")
target_include_directories(size_comparison_Nanopb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test 
                                                          ${CMAKE_CURRENT_SOURCE_DIR}/generated
                                                          ${CMAKE_CURRENT_SOURCE_DIR}/generated/Nanopb 
                                                          ${CMAKE_CURRENT_SOURCE_DIR}/Nanopb)
set_target_properties(size_comparison_Nanopb PROPERTIES COMPILE_DEFINITIONS "NANOPB")
