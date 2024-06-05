#! /bin/sh
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

# Generate sources using the EAMS plugin.
mkdir -p ./generated/EAMS
protoc --plugin=protoc-gen-eams=EmbeddedProto/protoc-gen-eams -I./MessageDescriptions --eams_out=./generated/EAMS ./MessageDescriptions/simple_types.proto


# Generate sources using the default Goolge code.
mkdir -p ./generated/Google
protoc -I./MessageDescriptions --cpp_out=./generated/Google ./MessageDescriptions/simple_types.proto

# Generate sources using Nanopb.
mkdir -p ./generated/Nanopb
protoc --plugin=protoc-gen-nanopb=Nanopb/generator/protoc-gen-nanopb -I./MessageDescriptions --nanopb_out=./generated/Nanopb ./MessageDescriptions/simple_types.proto

# Generte sources for python helper scripts.
mkdir -p ./generated/Python
protoc -I./MessageDescriptions --python_out=./generated/Python ./MessageDescriptions/simple_types.proto


# Build the tests
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../
make -j16


echo " "
echo "EAMS"
./size_comparison_EAMS
objdump -t size_comparison_EAMS | grep -e "F .text" | grep -e main

echo " "
echo "Google"
./size_comparison_Google
objdump -t size_comparison_Google | grep -e "F .text" | grep -e main

echo " "
echo "Nanopb"
./size_comparison_Nanopb
objdump -t size_comparison_Nanopb | grep -e "F .text" | grep -e main
