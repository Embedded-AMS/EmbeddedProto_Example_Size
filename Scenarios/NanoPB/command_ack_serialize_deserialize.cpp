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

#include "command.pb.h"
#include <NanoPB/helper_published.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <scenario.h>

#include <cstring>

// A command goes out, an acknowledgement comes back: two tiny round trips.
namespace
{
  template<class MSG, class SET, class COMPARE>
  uint32_t round_trip(const pb_msgdesc_t* fields, SET set, COMPARE compare)
  {
    MSG msg = MSG();
    report_message_size(sizeof(msg));
    set(msg);

    uint8_t buffer[32];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    if (!pb_encode(&stream, fields, &msg)) {
      return 0;
    }

    uint8_t deserialize_buffer[32];
    const uint32_t buffer_size = stream.bytes_written;
    memcpy(deserialize_buffer, buffer, buffer_size);

    MSG msg_deserialized = MSG();
    pb_istream_t istream = pb_istream_from_buffer(deserialize_buffer, buffer_size);
    if (!pb_decode(&istream, fields, &msg_deserialized)) {
      return 0;
    }
    return compare(msg_deserialized) ? buffer_size : 0;
  }
}

uint32_t run_scenario()
{
  const uint32_t command_size = round_trip<DeviceCommand>(DeviceCommand_fields, HelperPublishedNanoPB::set_command, HelperPublishedNanoPB::compare_command);
  const uint32_t ack_size = round_trip<DeviceAck>(DeviceAck_fields, HelperPublishedNanoPB::set_ack, HelperPublishedNanoPB::compare_ack);
  return ((0 != command_size) && (0 != ack_size)) ? (command_size + ack_size) : 0;
}
