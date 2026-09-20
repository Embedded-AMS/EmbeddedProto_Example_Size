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
#include <FlatBuffers/helper_published.h>
#include <command_verifier.h>

// A command goes out, an acknowledgement comes back: two tiny round trips with one builder.
namespace
{
  template<class TABLE, class CREATE, class VERIFY, class AS_ROOT, class COMPARE>
  uint32_t round_trip(flatcc_builder_t& builder, CREATE create, VERIFY verify, AS_ROOT as_root, COMPARE compare)
  {
    flatcc_builder_reset(&builder);
    create(&builder);

    alignas(8) uint8_t buffer[64];
    const uint32_t buffer_size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer));

    alignas(8) uint8_t deserialize_buffer[64];
    memcpy(deserialize_buffer, buffer, buffer_size);
    if (verify(deserialize_buffer, buffer_size) != 0) {
      return 0;
    }
    TABLE msg = as_root(deserialize_buffer);
    return compare(msg) ? buffer_size : 0;
  }
}

uint32_t run_scenario()
{
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);
  const uint32_t command_size = round_trip<DeviceCommand_table_t>(builder, HelperPublishedFlatBuffers::create_command,
      DeviceCommand_verify_as_root, DeviceCommand_as_root, HelperPublishedFlatBuffers::compare_command);
  const uint32_t ack_size = round_trip<DeviceAck_table_t>(builder, HelperPublishedFlatBuffers::create_ack,
      DeviceAck_verify_as_root, DeviceAck_as_root, HelperPublishedFlatBuffers::compare_ack);
  flatcc_builder_clear(&builder);
  return ((0 != command_size) && (0 != ack_size)) ? (command_size + ack_size) : 0;
}
