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

// ArduinoJson 7 keeps a document in pools of slots on the heap, one pool of this many slots at
// a time. The pool is sized to the largest document of this scenario, rounded up to eight
// slots, the way the other libraries size their buffers. ArduinoJson is header only, so the
// define here is all it takes.
#define ARDUINOJSON_POOL_CAPACITY 8

#include <ArduinoJson.h>
#include <ArduinoJson/helper_published.h>
#include <ArduinoJson/counting_allocator.h>
#include <scenario.h>

#include <cstring>

// A command goes out, an acknowledgement comes back: two tiny round trips.
namespace
{
  template<class SET, class COMPARE>
  uint32_t round_trip(SET set, COMPARE compare)
  {
    JsonDocument doc(&g_json_allocator);
    set(doc);

    char buffer[64];
    report_message_size(sizeof(doc) + g_json_allocator.peak_bytes());
    const size_t buffer_size = serializeJson(doc, buffer, sizeof(buffer));
    if ((0 == buffer_size) || (buffer_size >= sizeof(buffer))) {
      return 0;
    }

    char deserialize_buffer[64];
    memcpy(deserialize_buffer, buffer, buffer_size);

    JsonDocument doc_deserialized(&g_json_allocator);
    if (deserializeJson(doc_deserialized, deserialize_buffer, buffer_size)) {
      return 0;
    }
    return compare(doc_deserialized) ? static_cast<uint32_t>(buffer_size) : 0;
  }
}

uint32_t run_scenario()
{
  const uint32_t command_size = round_trip(HelperPublishedArduinoJson::set_command, HelperPublishedArduinoJson::compare_command);
  const uint32_t ack_size = round_trip(HelperPublishedArduinoJson::set_ack, HelperPublishedArduinoJson::compare_ack);
  return ((0 != command_size) && (0 != ack_size)) ? (command_size + ack_size) : 0;
}
