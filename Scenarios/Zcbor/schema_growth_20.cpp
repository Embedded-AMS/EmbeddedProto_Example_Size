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

#include <cbor_messages_encode.h>
#include <cbor_messages_decode.h>
#include <Zcbor/helper_published.h>
#include <scenario.h>

#include <cstring>

// Round trips the first 20 of the twenty growth message types, one instance each. The
// flash difference between the 1, 5 and 20 variants is what a library spends per
// message type in the schema.
namespace
{
  template<class MSG, class ENCODE, class DECODE>
  uint32_t round_trip(ENCODE encode, DECODE decode, const uint32_t n)
  {
    MSG msg = {};
    report_message_size(sizeof(msg));
    HelperPublishedZcbor::set_growth(msg, n);

    uint8_t buffer[64];
    size_t buffer_size = 0;
    if (encode(buffer, sizeof(buffer), &msg, &buffer_size) != ZCBOR_SUCCESS) {
      return 0;
    }

    uint8_t deserialize_buffer[64];
    memcpy(deserialize_buffer, buffer, buffer_size);

    MSG msg_deserialized = {};
    size_t decoded_size = 0;
    if (decode(deserialize_buffer, buffer_size, &msg_deserialized, &decoded_size) != ZCBOR_SUCCESS) {
      return 0;
    }
    return HelperPublishedZcbor::compare_growth(msg_deserialized, n) ? static_cast<uint32_t>(buffer_size) : 0;
  }
}

uint32_t run_scenario()
{
  uint32_t total = 0;
  bool success = true;
  { const uint32_t size = round_trip<struct Growth_01>(cbor_encode_Growth_01, cbor_decode_Growth_01, 1); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_02>(cbor_encode_Growth_02, cbor_decode_Growth_02, 2); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_03>(cbor_encode_Growth_03, cbor_decode_Growth_03, 3); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_04>(cbor_encode_Growth_04, cbor_decode_Growth_04, 4); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_05>(cbor_encode_Growth_05, cbor_decode_Growth_05, 5); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_06>(cbor_encode_Growth_06, cbor_decode_Growth_06, 6); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_07>(cbor_encode_Growth_07, cbor_decode_Growth_07, 7); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_08>(cbor_encode_Growth_08, cbor_decode_Growth_08, 8); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_09>(cbor_encode_Growth_09, cbor_decode_Growth_09, 9); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_10>(cbor_encode_Growth_10, cbor_decode_Growth_10, 10); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_11>(cbor_encode_Growth_11, cbor_decode_Growth_11, 11); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_12>(cbor_encode_Growth_12, cbor_decode_Growth_12, 12); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_13>(cbor_encode_Growth_13, cbor_decode_Growth_13, 13); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_14>(cbor_encode_Growth_14, cbor_decode_Growth_14, 14); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_15>(cbor_encode_Growth_15, cbor_decode_Growth_15, 15); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_16>(cbor_encode_Growth_16, cbor_decode_Growth_16, 16); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_17>(cbor_encode_Growth_17, cbor_decode_Growth_17, 17); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_18>(cbor_encode_Growth_18, cbor_decode_Growth_18, 18); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_19>(cbor_encode_Growth_19, cbor_decode_Growth_19, 19); success = success && (0 != size); total += size; }
  { const uint32_t size = round_trip<struct Growth_20>(cbor_encode_Growth_20, cbor_decode_Growth_20, 20); success = success && (0 != size); total += size; }
  return success ? total : 0;
}
