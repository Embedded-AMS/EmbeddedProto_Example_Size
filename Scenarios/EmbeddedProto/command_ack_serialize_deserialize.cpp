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

#include <command.h>
#include <EmbeddedProto/helper_published.h>
#include <EmbeddedProto/WriteBufferFixedSize.h>
#include <EmbeddedProto/ReadBufferFixedSize.h>
#include <EmbeddedProto/helper_serialize_mode.h>
#include <scenario.h>

#include <cstring>

// A command goes out, an acknowledgement comes back: two tiny round trips.
namespace
{
  template<class MSG, class SET, class COMPARE>
  uint32_t round_trip(SET set, COMPARE compare)
  {
    MSG msg;
    report_message_size(sizeof(msg));
    set(msg);

    EmbeddedProto::WriteBufferFixedSize<32> write_buffer;
    if(::EmbeddedProto::Error::NO_ERRORS != HelperSerializeMode::serialize_message(msg, write_buffer))
    {
      return 0;
    }

    EmbeddedProto::ReadBufferFixedSize<32> read_buffer;
    const uint32_t buffer_size = write_buffer.get_size();
    memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
    read_buffer.set_bytes_written(buffer_size);

    MSG msg_deserialized;
    if(::EmbeddedProto::Error::NO_ERRORS != msg_deserialized.deserialize(read_buffer))
    {
      return 0;
    }
    return compare(msg_deserialized) ? buffer_size : 0;
  }
}

uint32_t run_scenario()
{
  const uint32_t command_size = round_trip<Command>(HelperPublished::set_command, HelperPublished::compare_command);
  const uint32_t ack_size = round_trip<Ack>(HelperPublished::set_ack, HelperPublished::compare_ack);
  return ((0 != command_size) && (0 != ack_size)) ? (command_size + ack_size) : 0;
}
