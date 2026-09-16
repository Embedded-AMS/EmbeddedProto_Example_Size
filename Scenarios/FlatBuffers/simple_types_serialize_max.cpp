/*
 *  Copyright (C) 2020-2026 Embedded AMS B.V. - All Rights Reserved
 *
 *  This file is part of Embedded Proto.
 *
 *  Embedded Proto is open source software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as published 
 *  by the Free Software Foundation, version 3 of the license.
 *
 *  Embedded Proto  is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
 *
 *  For commercial and closed source application please visit:
 * <https://embeddedproto.com/pricing/>.
 *
 *  Embedded AMS B.V.
 *  Info:
 *    info at EmbeddedProto dot com
 *
 *  Postal address:
 *    Atoomweg 2
 *    1627 LE, Hoorn
 *    the Netherlands
 */

#include <FlatBuffers/helper_flatbuffers.h>
#include <FlatBuffers/helper_simple_types.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);
  Simple_Types_start_as_root(&builder);
  HelperSimpleTypesFlatBuffers::set_max(&builder);
  Simple_Types_end_as_root(&builder);

  // Copy the finished buffer into a byte array, like the other libraries serialize into one
  alignas(8) uint8_t buffer[256];
  const uint32_t buffer_size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer));

  // Calculate checksum to prevent dead code elimination
  uint8_t checksum = 0;
  for (uint32_t i = 0; i < buffer_size; i++) {
    checksum ^= buffer[i];
  }
  g_buffer_checksum = checksum;

  return buffer_size;
}
