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

#include <cbor_messages_encode.h>
#include <cbor_messages_decode.h>
#include <scenario.h>

#include <cstring>

namespace
{
  constexpr const char* TEST_STRING = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
  constexpr size_t TEST_STRING_LENGTH = 123;
}

// zcbor does not copy the text: on encode the struct points at the caller's characters,
// on decode it points into the received buffer, like a FlatBuffers string.
uint32_t run_scenario()
{
  struct String_Demo_Message msg = {};
  report_message_size(sizeof(msg));
  msg.message_text.message_text.value = reinterpret_cast<const uint8_t*>(TEST_STRING);
  msg.message_text.message_text.len = TEST_STRING_LENGTH;
  msg.message_text_present = true;

  uint8_t buffer[200];
  size_t buffer_size = 0;
  if (cbor_encode_String_Demo_Message(buffer, sizeof(buffer), &msg, &buffer_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  uint8_t deserialize_buffer[200];
  memcpy(deserialize_buffer, buffer, buffer_size);

  struct String_Demo_Message msg_deserialized = {};
  size_t decoded_size = 0;
  if (cbor_decode_String_Demo_Message(deserialize_buffer, buffer_size, &msg_deserialized, &decoded_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  bool success = msg_deserialized.message_text_present
                 && (msg_deserialized.message_text.message_text.len == TEST_STRING_LENGTH)
                 && (0 == memcmp(msg_deserialized.message_text.message_text.value, TEST_STRING, TEST_STRING_LENGTH));

  return success ? static_cast<uint32_t>(buffer_size) : 0;
}
