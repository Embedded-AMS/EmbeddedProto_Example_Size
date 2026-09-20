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

#include <schema_growth.h>
#include <EmbeddedProto/helper_published.h>
#include <EmbeddedProto/WriteBufferFixedSize.h>
#include <EmbeddedProto/ReadBufferFixedSize.h>
#include <EmbeddedProto/helper_serialize_mode.h>
#include <scenario.h>

#include <cstring>

// Round trips the first 20 of the twenty growth message types, one instance each. The
// flash difference between the 1, 5 and 20 variants is what a library spends per
// message type in the schema.
namespace
{
  template<class MSG>
  uint32_t round_trip(const uint32_t n)
  {
    MSG msg;
    report_message_size(sizeof(msg));
    HelperPublished::set_growth(msg, n);

    EmbeddedProto::WriteBufferFixedSize<64> write_buffer;
    if(::EmbeddedProto::Error::NO_ERRORS != HelperSerializeMode::serialize_message(msg, write_buffer))
    {
      return 0;
    }

    EmbeddedProto::ReadBufferFixedSize<64> read_buffer;
    const uint32_t buffer_size = write_buffer.get_size();
    memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
    read_buffer.set_bytes_written(buffer_size);

    MSG msg_deserialized;
    if(::EmbeddedProto::Error::NO_ERRORS != msg_deserialized.deserialize(read_buffer))
    {
      return 0;
    }
    return HelperPublished::compare_growth(msg_deserialized, n) ? buffer_size : 0;
  }
}

uint32_t run_scenario()
{
  uint32_t total = 0;
  bool success = true;
  { const uint32_t size = round_trip<Growth_01>(1); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_02>(2); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_03>(3); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_04>(4); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_05>(5); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_06>(6); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_07>(7); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_08>(8); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_09>(9); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_10>(10); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_11>(11); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_12>(12); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_13>(13); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_14>(14); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_15>(15); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_16>(16); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_17>(17); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_18>(18); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_19>(19); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_20>(20); success = success && (0 != size); total += size; }
  return success ? total : 0;
}
