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

#include "firmware_chunk.pb.h"
#include <NanoPB/helper_published.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <scenario.h>

#include <cstring>

uint32_t run_scenario()
{
  FirmwareChunk msg = FirmwareChunk_init_default;
  report_message_size(sizeof(msg));
  HelperPublishedNanoPB::set_firmware_chunk(msg);

  // Serialize the message
  uint8_t buffer[512];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  if (!pb_encode(&stream, FirmwareChunk_fields, &msg)) {
    return 0;
  }

  // Copy buffer to deserialization buffer
  uint8_t deserialize_buffer[512];
  const uint32_t buffer_size = stream.bytes_written;
  memcpy(deserialize_buffer, buffer, buffer_size);

  // Deserialize the buffer into a new message object
  FirmwareChunk msg_deserialized = FirmwareChunk_init_default;
  pb_istream_t istream = pb_istream_from_buffer(deserialize_buffer, buffer_size);
  if (!pb_decode(&istream, FirmwareChunk_fields, &msg_deserialized)) {
    return 0;
  }

  // Compare the values with helper functions
  const bool success = HelperPublishedNanoPB::compare_firmware_chunk(msg_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}
