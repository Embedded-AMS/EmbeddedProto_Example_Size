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

set(CMAKE_CXX_FLAGS "-std=c++14")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

include_directories(test generated EmbeddedProto/src Nanopb)


file(GLOB src_files_EAMS    
    "test/*.cpp"
    "EmbeddedProto/src/*.cpp"
)

add_executable(size_comparison_EAMS ${src_files_EAMS})
set_target_properties(size_comparison_EAMS PROPERTIES COMPILE_DEFINITIONS "EAMS")


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
    "test/*.cpp"
    "Nanopb/*.c"
    "generated/Nanopb/*.c"
)

add_executable(size_comparison_Nanopb ${src_files_Nanopb})
set_target_properties(size_comparison_Nanopb PROPERTIES COMPILE_DEFINITIONS "NANOPB")
