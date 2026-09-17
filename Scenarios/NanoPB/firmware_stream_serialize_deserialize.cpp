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

#include "firmware_stream.pb.h"
#include <pb_encode.h>
#include <pb_decode.h>
#include <scenario.h>
#include <scenario_values.h>

#include <cstring>

// The firmware chunk streamed through callbacks in windows, so the message never holds
// the 256 bytes. The window is the only payload memory either side has.
namespace
{
  using namespace ScenarioValues;

  uint8_t window[WINDOW_SIZE];

  struct ChunkSink
  {
    uint32_t received = 0;
    bool all_correct = true;
  };

  bool encode_chunk(pb_ostream_t* stream, const pb_field_t* field, void* const* arg)
  {
    (void)arg;
    bool ok = pb_encode_tag_for_field(stream, field) && pb_encode_varint(stream, CHUNK_SIZE);
    uint32_t sent = 0;
    while(ok && (sent < CHUNK_SIZE))
    {
      const uint32_t remaining = CHUNK_SIZE - sent;
      const uint32_t n = (remaining < WINDOW_SIZE) ? remaining : WINDOW_SIZE;
      for(uint32_t i = 0; i < n; ++i)
      {
        window[i] = chunk_byte(sent + i);
      }
      ok = pb_write(stream, window, n);
      sent += n;
    }
    return ok;
  }

  bool decode_chunk(pb_istream_t* stream, const pb_field_t* field, void** arg)
  {
    (void)field;
    ChunkSink* sink = static_cast<ChunkSink*>(*arg);
    bool ok = true;
    while(ok && (0 < stream->bytes_left))
    {
      const uint32_t n = (stream->bytes_left < WINDOW_SIZE) ? stream->bytes_left : WINDOW_SIZE;
      ok = pb_read(stream, window, n);
      for(uint32_t i = 0; ok && (i < n); ++i)
      {
        sink->all_correct = sink->all_correct && (window[i] == chunk_byte(sink->received + i));
      }
      sink->received += n;
    }
    return ok;
  }
}

uint32_t run_scenario()
{
  FirmwareStream msg = FirmwareStream_init_default;
  report_message_size(sizeof(msg));
  msg.offset = CHUNK_OFFSET;
  msg.total_size = IMAGE_TOTAL_SIZE;
  msg.crc32 = CHUNK_CRC32;
  msg.data.funcs.encode = &encode_chunk;
  msg.data.arg = nullptr;

  uint8_t buffer[512];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  if (!pb_encode(&stream, FirmwareStream_fields, &msg)) {
    return 0;
  }

  uint8_t deserialize_buffer[512];
  const uint32_t buffer_size = stream.bytes_written;
  memcpy(deserialize_buffer, buffer, buffer_size);

  ChunkSink sink;
  FirmwareStream msg_deserialized = FirmwareStream_init_default;
  msg_deserialized.data.funcs.decode = &decode_chunk;
  msg_deserialized.data.arg = &sink;
  pb_istream_t istream = pb_istream_from_buffer(deserialize_buffer, buffer_size);
  if (!pb_decode(&istream, FirmwareStream_fields, &msg_deserialized)) {
    return 0;
  }

  const bool success = (msg_deserialized.offset == CHUNK_OFFSET) && (msg_deserialized.total_size == IMAGE_TOTAL_SIZE)
                       && (msg_deserialized.crc32 == CHUNK_CRC32) && (CHUNK_SIZE == sink.received) && sink.all_correct;

  return success ? buffer_size : 0;
}
