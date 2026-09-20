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

#include "callback_demo.pb.h"
#include <NanoPB/helper_callback_demo.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include <cstring>
#include <scenario.h>

namespace
{
  // The window is the only payload memory either message holds.
  uint8_t window[HelperCallbackDemo::WINDOW_SIZE];

  // Checks each received window against the expected payload.
  struct PayloadSink
  {
    uint32_t received = 0;
    bool all_correct = true;
  };

  // Writes the length prefix and then streams the payload one window at a time.
  bool encode_payload(pb_ostream_t* stream, const pb_field_t* field, void* const* arg)
  {
    (void)arg;
    bool ok = pb_encode_tag_for_field(stream, field)
              && pb_encode_varint(stream, HelperCallbackDemo::PAYLOAD_SIZE);
    uint32_t sent = 0;
    while(ok && (sent < HelperCallbackDemo::PAYLOAD_SIZE))
    {
      const uint32_t remaining = HelperCallbackDemo::PAYLOAD_SIZE - sent;
      const uint32_t n = (remaining < HelperCallbackDemo::WINDOW_SIZE) ? remaining : HelperCallbackDemo::WINDOW_SIZE;
      for(uint32_t i = 0; i < n; ++i)
      {
        window[i] = HelperCallbackDemo::payload_byte(sent + i);
      }
      ok = pb_write(stream, window, n);
      sent += n;
    }
    return ok;
  }

  // Reads the payload one window at a time from the length delimited substream.
  bool decode_payload(pb_istream_t* stream, const pb_field_t* field, void** arg)
  {
    (void)field;
    PayloadSink* sink = static_cast<PayloadSink*>(*arg);
    bool ok = true;
    while(ok && (0 < stream->bytes_left))
    {
      const uint32_t n = (stream->bytes_left < HelperCallbackDemo::WINDOW_SIZE) ? stream->bytes_left : HelperCallbackDemo::WINDOW_SIZE;
      ok = pb_read(stream, window, n);
      for(uint32_t i = 0; ok && (i < n); ++i)
      {
        sink->all_correct = sink->all_correct && (window[i] == HelperCallbackDemo::payload_byte(sink->received + i));
      }
      sink->received += n;
    }
    return ok;
  }
}

uint32_t run_scenario()
{
  Blob msg = Blob_init_default;
  report_message_size(sizeof(msg));
  msg.payload.funcs.encode = &encode_payload;
  msg.payload.arg = nullptr;

  // Create buffer for serialized data
  uint8_t buffer[80];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  // Serialize the message
  const bool encode_success = pb_encode(&stream, Blob_fields, &msg);
  if (!encode_success) {
    return 0;
  }

  // Create deserialization buffer and copy data
  uint8_t deserialize_buffer[80];
  const uint32_t buffer_size = stream.bytes_written;
  memcpy(deserialize_buffer, buffer, buffer_size);

  // Deserialize the buffer into a message that streams the payload into the sink
  PayloadSink sink;
  Blob msg_deserialized = Blob_init_default;
  report_message_size(sizeof(msg_deserialized));
  msg_deserialized.payload.funcs.decode = &decode_payload;
  msg_deserialized.payload.arg = &sink;

  pb_istream_t istream = pb_istream_from_buffer(deserialize_buffer, buffer_size);
  const bool decode_success = pb_decode(&istream, Blob_fields, &msg_deserialized);
  if (!decode_success) {
    return 0;
  }

  const bool success = (HelperCallbackDemo::PAYLOAD_SIZE == sink.received) && sink.all_correct;

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}
