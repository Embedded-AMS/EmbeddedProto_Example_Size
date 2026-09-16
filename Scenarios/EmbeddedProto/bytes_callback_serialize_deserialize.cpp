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

#include <callback_demo.h>
#include <EmbeddedProto/helper_callback_demo.h>
#include <EmbeddedProto/WriteBufferFixedSize.h>
#include <EmbeddedProto/ReadBufferFixedSize.h>

#include <EmbeddedProto/helper_serialize_mode.h>

#include <cstring>
#include <scenario.h>

namespace
{
  using Payload = ::EmbeddedProto::BytesStringCallback<uint8_t>;

  // Fills each offered window with the next slice of the payload.
  struct PayloadSource
  {
    uint32_t sent = 0;

    uint32_t operator()(::EmbeddedProto::bytes_view window)
    {
      const uint32_t remaining = HelperCallbackDemo::PAYLOAD_SIZE - sent;
      const uint32_t n = (remaining < window.size) ? remaining : window.size;
      for(uint32_t i = 0; i < n; ++i)
      {
        window.data[i] = HelperCallbackDemo::payload_byte(sent + i);
      }
      sent += n;
      return n;
    }
  };

  // Checks each received window against the expected payload.
  struct PayloadSink
  {
    uint32_t received = 0;
    bool all_correct = true;

    uint32_t operator()(::EmbeddedProto::bytes_view window)
    {
      for(uint32_t i = 0; i < window.size; ++i)
      {
        all_correct = all_correct && (window.data[i] == HelperCallbackDemo::payload_byte(received + i));
      }
      received += window.size;
      return window.size;
    }
  };
}

uint32_t run_scenario()
{
  // The window is the only payload memory either message holds.
  uint8_t window[HelperCallbackDemo::WINDOW_SIZE];

  Blob msg;

  report_message_size(sizeof(msg));
  PayloadSource source;
  Payload::ChunkCallback source_callback;
  source_callback.set(source);
  msg.set_payload_window({window, HelperCallbackDemo::WINDOW_SIZE});
  msg.set_payload_on_serialize_chunk(source_callback);
  msg.mutable_payload().set_length(HelperCallbackDemo::PAYLOAD_SIZE);

  // Serialize the data to buffer
  EmbeddedProto::WriteBufferFixedSize<80> write_buffer;
  const auto serialize_result = HelperSerializeMode::serialize_message(msg, write_buffer);

  // Copy buffer to deserialization buffer
  EmbeddedProto::ReadBufferFixedSize<80> read_buffer;
  const uint32_t buffer_size = write_buffer.get_size();
  memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
  read_buffer.set_bytes_written(buffer_size);

  // Deserialize the buffer into a message that streams the payload into the sink
  Blob msg_deserialized;
  report_message_size(sizeof(msg_deserialized));
  PayloadSink sink;
  Payload::ChunkCallback sink_callback;
  sink_callback.set(sink);
  msg_deserialized.set_payload_window({window, HelperCallbackDemo::WINDOW_SIZE});
  msg_deserialized.set_payload_on_deserialize_chunk(sink_callback);
  const auto deserialize_result = msg_deserialized.deserialize(read_buffer);

  const bool success = (::EmbeddedProto::Error::NO_ERRORS == serialize_result)
                       && (::EmbeddedProto::Error::NO_ERRORS == deserialize_result)
                       && (HelperCallbackDemo::PAYLOAD_SIZE == sink.received)
                       && sink.all_correct;

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}
