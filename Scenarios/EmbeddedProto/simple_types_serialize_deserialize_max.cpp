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

#include <simple_types.h>
#include <EmbeddedProto/helper_simple_types.h>
#include <EmbeddedProto/WriteBufferFixedSize.h>
#include <EmbeddedProto/ReadBufferFixedSize.h>

#include <EmbeddedProto/helper_serialize_mode.h>
// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  Simple_Types msg;
  HelperSimpleTypes::set_max(msg);

  // Serialize the data to buffer
  EmbeddedProto::WriteBufferFixedSize<150> write_buffer;
  const auto serialize_result = HelperSerializeMode::serialize_message(msg, write_buffer);

  if(serialize_result != EmbeddedProto::Error::NO_ERRORS)
  {
    return 0;
  }

  // Calculate checksum to prevent dead code elimination
  // The compiler cannot optimize away serialize() since buffer content is used
  uint8_t checksum = 0;
  const uint8_t* data = write_buffer.get_data();
  for (uint32_t i = 0; i < write_buffer.get_size(); i++) {
    checksum ^= data[i];
  }
  g_buffer_checksum = checksum;

  // Copy buffer to deserialization buffer
  EmbeddedProto::ReadBufferFixedSize<150> read_buffer;
  const uint32_t buffer_size = write_buffer.get_size();
  memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
  read_buffer.set_bytes_written(buffer_size);

  // Clear the message data
  Simple_Types msg_deserialized;

  // Deserialize the buffer into the message object
  const auto deserialize_result = msg_deserialized.deserialize(read_buffer);

  if(deserialize_result != EmbeddedProto::Error::NO_ERRORS)
  {
    return 0;
  }

  // Compare the values with helper functions
  bool success = HelperSimpleTypes::compare_max(msg_deserialized);

  // Debug output to understand what's failing
  if(!success) {
    // Add debug output to see which fields are mismatched
    // This will help identify if it's a specific field type causing issues
    volatile uint32_t debug_buffer_size = buffer_size; // Prevent optimization
  }

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}