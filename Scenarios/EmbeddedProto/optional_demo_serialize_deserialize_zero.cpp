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

#include <optional_demo.h>
#include <EmbeddedProto/helper_optional_demo.h>
#include <EmbeddedProto/WriteBufferFixedSize.h>
#include <EmbeddedProto/ReadBufferFixedSize.h>

#include <EmbeddedProto/helper_serialize_mode.h>
// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  Optional_Demo msg;
  HelperOptionalDemo::set_zero_with_presence(msg);

  // Serialize the data to buffer
  EmbeddedProto::WriteBufferFixedSize<100> write_buffer;
  const auto serialize_result = HelperSerializeMode::serialize_message(msg, write_buffer);

  // Copy buffer to deserialization buffer
  EmbeddedProto::ReadBufferFixedSize<100> read_buffer;
  const uint32_t buffer_size = write_buffer.get_size();
  memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
  read_buffer.set_bytes_written(buffer_size);

  // Clear the message data
  Optional_Demo msg_deserialized;

  // Deserialize the buffer into the message object
  const auto deserialize_result = msg_deserialized.deserialize(read_buffer);

  // Compare the values with helper functions - check both presence and zero values
  bool success = HelperOptionalDemo::compare_zero_with_presence(msg_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}