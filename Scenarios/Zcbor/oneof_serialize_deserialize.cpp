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

// The int32 member of the oneof, as the other libraries set it.
uint32_t run_scenario()
{
  struct OneOf_Demo msg = {};
  report_message_size(sizeof(msg));
  msg.data_field_present = true;
  msg.data_field.data_field_choice = OneOf_Demo_data_field_r::data_field_int32_value_c;
  msg.data_field.int32_value = 12345;

  uint8_t buffer[200];
  size_t buffer_size = 0;
  if (cbor_encode_OneOf_Demo(buffer, sizeof(buffer), &msg, &buffer_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  uint8_t deserialize_buffer[200];
  memcpy(deserialize_buffer, buffer, buffer_size);

  struct OneOf_Demo msg_deserialized = {};
  size_t decoded_size = 0;
  if (cbor_decode_OneOf_Demo(deserialize_buffer, buffer_size, &msg_deserialized, &decoded_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  bool success = msg_deserialized.data_field_present
                 && (msg_deserialized.data_field.data_field_choice == OneOf_Demo_data_field_r::data_field_int32_value_c)
                 && (msg_deserialized.data_field.int32_value == 12345);

  return success ? static_cast<uint32_t>(buffer_size) : 0;
}
