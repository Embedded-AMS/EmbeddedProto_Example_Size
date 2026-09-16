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

namespace
{
  // Receives every parsed sample and checks it against the expected value.
  struct SampleSink
  {
    uint32_t count = 0;
    bool all_correct = true;
  };

  // Emits one tag and varint per sample, the same expanded encoding Embedded Proto streams.
  bool encode_samples(pb_ostream_t* stream, const pb_field_t* field, void* const* arg)
  {
    (void)arg;
    bool ok = true;
    for(uint32_t i = 0; ok && (i < HelperCallbackDemo::SAMPLE_COUNT); ++i)
    {
      const int64_t value = HelperCallbackDemo::sample_value(i);
      ok = pb_encode_tag_for_field(stream, field)
           && pb_encode_varint(stream, static_cast<uint64_t>(value));
    }
    return ok;
  }

  bool decode_samples(pb_istream_t* stream, const pb_field_t* field, void** arg)
  {
    (void)field;
    SampleSink* sink = static_cast<SampleSink*>(*arg);
    uint64_t value = 0;
    const bool ok = pb_decode_varint(stream, &value);
    if(ok)
    {
      const int32_t sample = static_cast<int32_t>(static_cast<int64_t>(value));
      sink->all_correct = sink->all_correct && (sample == HelperCallbackDemo::sample_value(sink->count));
      ++sink->count;
    }
    return ok;
  }
}

uint32_t run_scenario()
{
  Sample_Stream msg = Sample_Stream_init_default;
  msg.samples.funcs.encode = &encode_samples;
  msg.samples.arg = nullptr;

  // Create buffer for serialized data
  uint8_t buffer[100];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  // Serialize the message
  const bool encode_success = pb_encode(&stream, Sample_Stream_fields, &msg);
  if (!encode_success) {
    return 0;
  }

  // Create deserialization buffer and copy data
  uint8_t deserialize_buffer[100];
  const uint32_t buffer_size = stream.bytes_written;
  memcpy(deserialize_buffer, buffer, buffer_size);

  // Deserialize the buffer into a message that streams the samples into the sink
  SampleSink sink;
  Sample_Stream msg_deserialized = Sample_Stream_init_default;
  msg_deserialized.samples.funcs.decode = &decode_samples;
  msg_deserialized.samples.arg = &sink;

  pb_istream_t istream = pb_istream_from_buffer(deserialize_buffer, buffer_size);
  const bool decode_success = pb_decode(&istream, Sample_Stream_fields, &msg_deserialized);
  if (!decode_success) {
    return 0;
  }

  const bool success = (HelperCallbackDemo::SAMPLE_COUNT == sink.count) && sink.all_correct;

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}
