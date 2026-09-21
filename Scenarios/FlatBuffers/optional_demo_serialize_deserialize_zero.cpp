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
#include <FlatBuffers/helper_optional_demo.h>
#include <optional_demo_verifier.h>

uint32_t run_scenario()
{
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);
  Optional_Demo_start_as_root(&builder);
  HelperOptionalDemoFlatBuffers::set_zero_with_presence(&builder);
  Optional_Demo_end_as_root(&builder);

  alignas(8) uint8_t buffer[256];
  const uint32_t buffer_size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer));
  flatcc_builder_clear(&builder);

  alignas(8) uint8_t deserialize_buffer[256];
  memcpy(deserialize_buffer, buffer, buffer_size);

  if (Optional_Demo_verify_as_root(deserialize_buffer, buffer_size) != 0) {
    return 0;
  }
  Optional_Demo_table_t msg_deserialized = Optional_Demo_as_root(deserialize_buffer);

  bool success = HelperOptionalDemoFlatBuffers::compare_zero_with_presence(msg_deserialized);

  return success ? buffer_size : 0;
}
