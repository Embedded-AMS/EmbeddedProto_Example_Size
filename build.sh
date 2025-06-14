#! /bin/bash
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

# Clear any previously genearted results to prevent confusion when files are not regenerated when they should have.
rm -rf ./build
rm -rf ./generated

# Create results directory if it doesn't exist
mkdir -p ../results

# Generate sources using the EAMS plugin.
mkdir -p ./generated/EAMS
source "./EmbeddedProto/venv/bin/activate"
embeddedproto -I./MessageDescriptions --eams_out=./generated/EAMS ./MessageDescriptions/simple_types.proto 
embeddedproto -I./MessageDescriptions --eams_out=./generated/EAMS ./MessageDescriptions/nested_message.proto 
python3 EmbeddedProto/dev/scripts/GenerateSizeTest.py -i./generated/EAMS -o./generated/EAMS

# Generate sources using the default Goolge code.
mkdir -p ./generated/Google
./GoolgeProtobuf/bazel-bin/protoc -I./MessageDescriptions --cpp_out=./generated/Google ./MessageDescriptions/simple_types.proto

# Generate sources using Nanopb.
mkdir -p ./generated/Nanopb
./GoolgeProtobuf/bazel-bin/protoc --plugin=protoc-gen-nanopb=Nanopb/generator/protoc-gen-nanopb -I./MessageDescriptions --nanopb_out=./generated/Nanopb ./MessageDescriptions/simple_types.proto
./GoolgeProtobuf/bazel-bin/protoc --plugin=protoc-gen-nanopb=Nanopb/generator/protoc-gen-nanopb -I./MessageDescriptions --nanopb_out=./generated/Nanopb ./MessageDescriptions/nested_message.proto

# Generte sources for python helper scripts.
mkdir -p ./generated/Python
./GoolgeProtobuf/bazel-bin/protoc -I./MessageDescriptions --python_out=./generated/Python ./MessageDescriptions/simple_types.proto

#MESSAGE(FATAL_ERRORO "Could not find FOO_EXEC.")

# Build the tests
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j16 -Wfatal-errors


#(
#  echo " "
#  echo "======== size_comparison_EAMS at $(date '+%Y-%m-%d %H:%M:%S') =========="
#  echo " " 
#  size size_comparison_EAMS
#  echo " "
#  #objdump -t size_comparison_EAMS | grep -e "F .text" | grep -e main
#  echo "-----------------------------------------------------------------------"
#) | tee -a ../results/history.log

(
  echo " "
  echo "======== Generated size test at $(date '+%Y-%m-%d %H:%M:%S') =========="
  echo " "
  size generated_size_test_EAMS
  echo " "
  echo "-----------------------------------------------------------------------"
) | tee -a ../results/history.log


# Track individual symbol sizes
TIMESTAMP=$(date +%Y%m%d-%H%M%S)
SYM_FILE="../results/symbol_sizes_EAMS_${TIMESTAMP}.csv"

echo -e "\nTracking individual symbol sizes..."
nm --print-size --size-sort -C generated_size_test_EAMS | awk '
BEGIN {print "Symbol;Size;Type"} 
/[tT] [^ ]+$/ {print $4 ";" $2 ";Function"} 
/[bBdD] [^ ]+$/ {print $4 ";" $2 ";Variable"}' > "$SYM_FILE"

# Update master spreadsheet
echo "Updating symbol size history for EAMS..."
(cd .. && python3 symbol_tracker/symbol_history.py "results/symbol_sizes_EAMS_${TIMESTAMP}.csv" "EAMS")

#echo " "
#echo "Google"
#./size_comparison_Google
#objdump -t size_comparison_Google | grep -e "F .text" | grep -e main

#echo " "
#echo "Nanopb"
#size size_comparison_Nanopb
#objdump -t size_comparison_Nanopb | grep -e "F .text" | grep -e main
