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

namespace
{
  using Samples = ::EmbeddedProto::RepeatedFieldCallback<EmbeddedProto::int32>;

  // Hands out one sample per call until all samples are produced.
  struct SampleSource
  {
    uint32_t index = 0;

    bool operator()(EmbeddedProto::int32& element)
    {
      const bool produced = index < HelperCallbackDemo::SAMPLE_COUNT;
      if(produced)
      {
        element.set(HelperCallbackDemo::sample_value(index));
        ++index;
      }
      return produced;
    }
  };

  // Receives every parsed sample and checks it against the expected value.
  struct SampleSink
  {
    uint32_t count = 0;
    bool all_correct = true;

    ::EmbeddedProto::Error operator()(const EmbeddedProto::int32& element)
    {
      all_correct = all_correct && (element.get() == HelperCallbackDemo::sample_value(count));
      ++count;
      return ::EmbeddedProto::Error::NO_ERRORS;
    }
  };
}

uint32_t run_scenario()
{
  Sample_Stream msg;
  SampleSource source;
  Samples::SourceCallback source_callback;
  source_callback.set(source);
  msg.mutable_samples().set_source(source_callback);

  // Serialize the data to buffer
  EmbeddedProto::WriteBufferFixedSize<100> write_buffer;
  const auto serialize_result = HelperSerializeMode::serialize_message(msg, write_buffer);

  // Copy buffer to deserialization buffer
  EmbeddedProto::ReadBufferFixedSize<100> read_buffer;
  const uint32_t buffer_size = write_buffer.get_size();
  memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
  read_buffer.set_bytes_written(buffer_size);

  // Deserialize the buffer into a message that streams the samples into the sink
  Sample_Stream msg_deserialized;
  SampleSink sink;
  Samples::SinkCallback sink_callback;
  sink_callback.set(sink);
  msg_deserialized.mutable_samples().set_sink(sink_callback);
  const auto deserialize_result = msg_deserialized.deserialize(read_buffer);

  const bool success = (::EmbeddedProto::Error::NO_ERRORS == serialize_result)
                       && (::EmbeddedProto::Error::NO_ERRORS == deserialize_result)
                       && (HelperCallbackDemo::SAMPLE_COUNT == sink.count)
                       && sink.all_correct;

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}
