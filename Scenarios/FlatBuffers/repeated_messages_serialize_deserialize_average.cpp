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

#include <FlatBuffers/helper_flatbuffers.h>
#include <FlatBuffers/helper_repeated_demo.h>
#include <repeated_demo_verifier.h>

uint32_t run_scenario()
{
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);
  Repeated_Messages_start_as_root(&builder);
  HelperRepeatedDemoFlatBuffers::set_average_repeated_messages(&builder, 5);
  Repeated_Messages_end_as_root(&builder);

  alignas(8) uint8_t buffer[2000];
  const uint32_t buffer_size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer));
  flatcc_builder_clear(&builder);

  alignas(8) uint8_t deserialize_buffer[2000];
  memcpy(deserialize_buffer, buffer, buffer_size);

  if (Repeated_Messages_verify_as_root(deserialize_buffer, buffer_size) != 0) {
    return 0;
  }
  Repeated_Messages_table_t msg_deserialized = Repeated_Messages_as_root(deserialize_buffer);

  bool success = HelperRepeatedDemoFlatBuffers::compare_average_repeated_messages(msg_deserialized, 5);

  return success ? buffer_size : 0;
}
