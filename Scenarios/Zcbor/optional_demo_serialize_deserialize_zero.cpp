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

// Every field set to its zero value with presence, as the other libraries do.
namespace
{
  #define ZCBOR_SET(msg, field, value) do { (msg).field.field = (value); (msg).field##_present = true; } while(0)
  #define ZCBOR_CHECK(msg, field, value) ((msg).field##_present && ((msg).field.field == (value)))

  void set_zero_with_presence(struct Optional_Demo& msg)
  {
    ZCBOR_SET(msg, a_int32, 0);
    ZCBOR_SET(msg, a_int64, 0);
    ZCBOR_SET(msg, a_uint32, 0);
    ZCBOR_SET(msg, a_uint64, 0);
    ZCBOR_SET(msg, a_sint32, 0);
    ZCBOR_SET(msg, a_sint64, 0);
    ZCBOR_SET(msg, a_bool, false);
    ZCBOR_SET(msg, a_enum, 0);
    ZCBOR_SET(msg, a_fixed64, 0);
    ZCBOR_SET(msg, a_sfixed64, 0);
    ZCBOR_SET(msg, a_double, 0.0);
    ZCBOR_SET(msg, a_fixed32, 0);
    ZCBOR_SET(msg, a_sfixed32, 0);
    ZCBOR_SET(msg, a_float, 0.0f);
    ZCBOR_SET(msg, a_nested_enum, 0);
  }

  bool compare_zero_with_presence(const struct Optional_Demo& msg)
  {
    bool result = true;
    result &= ZCBOR_CHECK(msg, a_int32, 0);
    result &= ZCBOR_CHECK(msg, a_int64, 0);
    result &= ZCBOR_CHECK(msg, a_uint32, 0);
    result &= ZCBOR_CHECK(msg, a_uint64, 0);
    result &= ZCBOR_CHECK(msg, a_sint32, 0);
    result &= ZCBOR_CHECK(msg, a_sint64, 0);
    result &= ZCBOR_CHECK(msg, a_bool, false);
    result &= ZCBOR_CHECK(msg, a_enum, 0);
    result &= ZCBOR_CHECK(msg, a_fixed64, 0);
    result &= ZCBOR_CHECK(msg, a_sfixed64, 0);
    result &= ZCBOR_CHECK(msg, a_double, 0.0);
    result &= ZCBOR_CHECK(msg, a_fixed32, 0);
    result &= ZCBOR_CHECK(msg, a_sfixed32, 0);
    result &= ZCBOR_CHECK(msg, a_float, 0.0f);
    result &= ZCBOR_CHECK(msg, a_nested_enum, 0);
    return result;
  }
}

uint32_t run_scenario()
{
  struct Optional_Demo msg = {};
  report_message_size(sizeof(msg));
  set_zero_with_presence(msg);

  uint8_t buffer[200];
  size_t buffer_size = 0;
  if (cbor_encode_Optional_Demo(buffer, sizeof(buffer), &msg, &buffer_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  uint8_t deserialize_buffer[200];
  memcpy(deserialize_buffer, buffer, buffer_size);

  struct Optional_Demo msg_deserialized = {};
  size_t decoded_size = 0;
  if (cbor_decode_Optional_Demo(deserialize_buffer, buffer_size, &msg_deserialized, &decoded_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  bool success = compare_zero_with_presence(msg_deserialized);

  return success ? static_cast<uint32_t>(buffer_size) : 0;
}
