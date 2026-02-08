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

#include "simple_types.pb.h"
#include <NanoPB/helper_simple_types.h>
#include <pb_encode.h>
#include <pb_decode.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  Simple_Types msg = Simple_Types_init_default;
  HelperSimpleTypesNanoPB::set_min(msg);

  // Create buffer for serialized data
  uint8_t buffer[100];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  // Serialize the message
  const bool encode_success = pb_encode(&stream, Simple_Types_fields, &msg);
  if (!encode_success) {
    return 0;
  }

  // Create deserialization buffer and copy data
  uint8_t deserialize_buffer[100];
  const uint32_t buffer_size = stream.bytes_written;
  memcpy(deserialize_buffer, buffer, buffer_size);

  // Clear the message data
  Simple_Types msg_deserialized = Simple_Types_init_default;

  // Deserialize the buffer into the message object
  pb_istream_t istream = pb_istream_from_buffer(deserialize_buffer, buffer_size);
  const bool decode_success = pb_decode(&istream, Simple_Types_fields, &msg_deserialized);
  if (!decode_success) {
    return 0;
  }

  // Compare the values with helper functions
  bool success = HelperSimpleTypesNanoPB::compare_min(msg_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}