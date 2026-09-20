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

#include "schema_growth.pb.h"
#include <NanoPB/helper_published.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <scenario.h>

#include <cstring>

// Round trips the first 20 of the twenty growth message types, one instance each. The
// flash difference between the 1, 5 and 20 variants is what a library spends per
// message type in the schema.
namespace
{
  template<class MSG>
  uint32_t round_trip(const pb_msgdesc_t* fields, const uint32_t n)
  {
    MSG msg = MSG();
    report_message_size(sizeof(msg));
    HelperPublishedNanoPB::set_growth(msg, n);

    uint8_t buffer[64];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    if (!pb_encode(&stream, fields, &msg)) {
      return 0;
    }

    uint8_t deserialize_buffer[64];
    const uint32_t buffer_size = stream.bytes_written;
    memcpy(deserialize_buffer, buffer, buffer_size);

    MSG msg_deserialized = MSG();
    pb_istream_t istream = pb_istream_from_buffer(deserialize_buffer, buffer_size);
    if (!pb_decode(&istream, fields, &msg_deserialized)) {
      return 0;
    }
    return HelperPublishedNanoPB::compare_growth(msg_deserialized, n) ? buffer_size : 0;
  }
}

uint32_t run_scenario()
{
  uint32_t total = 0;
  bool success = true;
  { const uint32_t size = round_trip<Growth_01>(Growth_01_fields, 1); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_02>(Growth_02_fields, 2); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_03>(Growth_03_fields, 3); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_04>(Growth_04_fields, 4); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_05>(Growth_05_fields, 5); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_06>(Growth_06_fields, 6); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_07>(Growth_07_fields, 7); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_08>(Growth_08_fields, 8); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_09>(Growth_09_fields, 9); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_10>(Growth_10_fields, 10); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_11>(Growth_11_fields, 11); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_12>(Growth_12_fields, 12); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_13>(Growth_13_fields, 13); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_14>(Growth_14_fields, 14); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_15>(Growth_15_fields, 15); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_16>(Growth_16_fields, 16); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_17>(Growth_17_fields, 17); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_18>(Growth_18_fields, 18); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_19>(Growth_19_fields, 19); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<Growth_20>(Growth_20_fields, 20); success = success && (0 != size); total += size; }
  return success ? total : 0;
}
