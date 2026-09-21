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
#include <ArduinoJson/helper_string.h>
#include <scenario.h>
#include <ArduinoJson/counting_allocator.h>

uint32_t run_scenario()
{
  // Create a JSON document with enough capacity for our string
  JsonDocument doc(&g_json_allocator);
  HelperString::set_string(doc);

  // Serialize the JSON document to a buffer
  char buffer[200];
  report_message_size(sizeof(doc) + g_json_allocator.peak_bytes());
  size_t buffer_size = serializeJson(doc, buffer, sizeof(buffer));

  // Create a new document for deserialization
  JsonDocument doc_deserialized(&g_json_allocator);

  // Deserialize from the buffer
  DeserializationError error = deserializeJson(doc_deserialized, buffer);

  // Compare the values with helper functions
  bool success = HelperString::compare_string(doc_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? static_cast<uint32_t>(buffer_size) : 0;
}