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
#include <Zcbor/helper_published.h>
#include <scenario.h>

#include <cstring>

// A command goes out, an acknowledgement comes back: two tiny round trips.
namespace
{
  template<class MSG, class ENCODE, class DECODE, class SET, class COMPARE>
  uint32_t round_trip(ENCODE encode, DECODE decode, SET set, COMPARE compare)
  {
    MSG msg = {};
    report_message_size(sizeof(msg));
    set(msg);

    uint8_t buffer[32];
    size_t buffer_size = 0;
    if (encode(buffer, sizeof(buffer), &msg, &buffer_size) != ZCBOR_SUCCESS) {
      return 0;
    }

    uint8_t deserialize_buffer[32];
    memcpy(deserialize_buffer, buffer, buffer_size);

    MSG msg_deserialized = {};
    size_t decoded_size = 0;
    if (decode(deserialize_buffer, buffer_size, &msg_deserialized, &decoded_size) != ZCBOR_SUCCESS) {
      return 0;
    }
    return compare(msg_deserialized) ? static_cast<uint32_t>(buffer_size) : 0;
  }
}

uint32_t run_scenario()
{
  const uint32_t command_size = round_trip<struct DeviceCommand>(cbor_encode_DeviceCommand, cbor_decode_DeviceCommand,
      HelperPublishedZcbor::set_command, HelperPublishedZcbor::compare_command);
  const uint32_t ack_size = round_trip<struct DeviceAck>(cbor_encode_DeviceAck, cbor_decode_DeviceAck,
      HelperPublishedZcbor::set_ack, HelperPublishedZcbor::compare_ack);
  return ((0 != command_size) && (0 != ack_size)) ? (command_size + ack_size) : 0;
}
