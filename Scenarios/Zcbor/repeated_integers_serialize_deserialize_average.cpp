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
#include <Zcbor/helper_repeated_demo.h>
#include <scenario.h>

#include <cstring>

uint32_t run_scenario()
{
  struct Repeated_Integers msg = {};
  report_message_size(sizeof(msg));
  HelperRepeatedDemoZcbor::set_average_repeated_integers(msg, 10);

  uint8_t buffer[500];
  size_t buffer_size = 0;
  if (cbor_encode_Repeated_Integers(buffer, sizeof(buffer), &msg, &buffer_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  uint8_t deserialize_buffer[500];
  memcpy(deserialize_buffer, buffer, buffer_size);

  struct Repeated_Integers msg_deserialized = {};
  report_message_size(sizeof(msg_deserialized));
  size_t decoded_size = 0;
  if (cbor_decode_Repeated_Integers(deserialize_buffer, buffer_size, &msg_deserialized, &decoded_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  bool success = HelperRepeatedDemoZcbor::compare_average_repeated_integers(msg_deserialized, 10);

  return success ? static_cast<uint32_t>(buffer_size) : 0;
}
