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

#include <firmware_stream.h>
#include <EmbeddedProto/WriteBufferFixedSize.h>
#include <EmbeddedProto/ReadBufferFixedSize.h>
#include <EmbeddedProto/helper_serialize_mode.h>
#include <scenario.h>
#include <scenario_values.h>

#include <cstring>

// The firmware chunk streamed through callbacks in windows, so the message never holds
// the 256 bytes. The window is the only payload memory either side has.
namespace
{
  using namespace ScenarioValues;
  using Payload = ::EmbeddedProto::BytesStringCallback<uint8_t>;

  struct ChunkSource
  {
    uint32_t sent = 0;
    uint32_t operator()(::EmbeddedProto::bytes_view window)
    {
      const uint32_t remaining = CHUNK_SIZE - sent;
      const uint32_t n = (remaining < window.size) ? remaining : window.size;
      for(uint32_t i = 0; i < n; ++i)
      {
        window.data[i] = chunk_byte(sent + i);
      }
      sent += n;
      return n;
    }
  };

  struct ChunkSink
  {
    uint32_t received = 0;
    bool all_correct = true;
    uint32_t operator()(::EmbeddedProto::bytes_view window)
    {
      for(uint32_t i = 0; i < window.size; ++i)
      {
        all_correct = all_correct && (window.data[i] == chunk_byte(received + i));
      }
      received += window.size;
      return window.size;
    }
  };
}

uint32_t run_scenario()
{
  uint8_t window[WINDOW_SIZE];

  FirmwareStream msg;
  report_message_size(sizeof(msg));
  msg.set_offset(CHUNK_OFFSET);
  msg.set_total_size(IMAGE_TOTAL_SIZE);
  msg.set_crc32(CHUNK_CRC32);
  ChunkSource source;
  Payload::ChunkCallback source_callback;
  source_callback.set(source);
  msg.set_data_window({window, WINDOW_SIZE});
  msg.set_data_on_serialize_chunk(source_callback);
  msg.mutable_data().set_length(CHUNK_SIZE);

  EmbeddedProto::WriteBufferFixedSize<512> write_buffer;
  const auto serialize_result = HelperSerializeMode::serialize_message(msg, write_buffer);

  EmbeddedProto::ReadBufferFixedSize<512> read_buffer;
  const uint32_t buffer_size = write_buffer.get_size();
  memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
  read_buffer.set_bytes_written(buffer_size);

  FirmwareStream msg_deserialized;
  ChunkSink sink;
  Payload::ChunkCallback sink_callback;
  sink_callback.set(sink);
  msg_deserialized.set_data_window({window, WINDOW_SIZE});
  msg_deserialized.set_data_on_deserialize_chunk(sink_callback);
  const auto deserialize_result = msg_deserialized.deserialize(read_buffer);

  const bool success = (::EmbeddedProto::Error::NO_ERRORS == serialize_result)
                       && (::EmbeddedProto::Error::NO_ERRORS == deserialize_result)
                       && (msg_deserialized.get_offset() == CHUNK_OFFSET)
                       && (msg_deserialized.get_total_size() == IMAGE_TOTAL_SIZE)
                       && (msg_deserialized.get_crc32() == CHUNK_CRC32)
                       && (CHUNK_SIZE == sink.received) && sink.all_correct;

  return success ? buffer_size : 0;
}
