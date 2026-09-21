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

// The flatcc emitter builds the message in pages on the heap. One page the size of the
// buffer below, rounded up to the multiple of 128 flatcc wants, sized to this scenario the
// way the other libraries size their buffers. The CMake passes it on to the runtime.
#define FLATCC_EMITTER_PAGE_SIZE 256

#include <FlatBuffers/helper_flatbuffers.h>
#include <oneof_builder.h>
#include <oneof_reader.h>
#include <oneof_verifier.h>

// The int32 member of the oneof, as the other libraries set it. A FlatBuffers union holds
// tables only, so the value sits in a one-field table.
uint32_t run_scenario()
{
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);
  OneOf_Demo_start_as_root(&builder);
  OneOf_Demo_data_field_Int32_Value_start(&builder);
  Int32_Value_value_add(&builder, 12345);
  OneOf_Demo_data_field_Int32_Value_end(&builder);
  OneOf_Demo_end_as_root(&builder);

  alignas(8) uint8_t buffer[256];
  const uint32_t buffer_size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer));
  flatcc_builder_clear(&builder);

  alignas(8) uint8_t deserialize_buffer[256];
  memcpy(deserialize_buffer, buffer, buffer_size);

  if (OneOf_Demo_verify_as_root(deserialize_buffer, buffer_size) != 0) {
    return 0;
  }
  OneOf_Demo_table_t msg_deserialized = OneOf_Demo_as_root(deserialize_buffer);

  bool success = (OneOf_Demo_data_field_type(msg_deserialized) == Data_Field_Int32_Value);
  if (success) {
    Int32_Value_table_t value = static_cast<Int32_Value_table_t>(OneOf_Demo_data_field(msg_deserialized));
    success = (Int32_Value_value(value) == 12345);
  }

  return success ? buffer_size : 0;
}
