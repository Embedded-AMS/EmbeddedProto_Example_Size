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

uint32_t run_scenario()
{
  struct TelemetryBatch msg = {};
  report_message_size(sizeof(msg));
  HelperPublishedZcbor::set_telemetry_batch(msg);

  // Serialize the message
  uint8_t buffer[512];
  size_t buffer_size = 0;
  if (cbor_encode_TelemetryBatch(buffer, sizeof(buffer), &msg, &buffer_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  // Copy buffer to deserialization buffer
  uint8_t deserialize_buffer[512];
  memcpy(deserialize_buffer, buffer, buffer_size);

  // Deserialize the buffer into a new message object
  struct TelemetryBatch msg_deserialized = {};
  size_t decoded_size = 0;
  if (cbor_decode_TelemetryBatch(deserialize_buffer, buffer_size, &msg_deserialized, &decoded_size) != ZCBOR_SUCCESS) {
    return 0;
  }

  const bool success = HelperPublishedZcbor::compare_telemetry_batch(msg_deserialized);

  return success ? static_cast<uint32_t>(buffer_size) : 0;
}
