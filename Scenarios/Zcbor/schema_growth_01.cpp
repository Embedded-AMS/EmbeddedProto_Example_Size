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

// Round trips the first 1 of the twenty growth message types, one instance each. The
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
  return success ? total : 0;
}
