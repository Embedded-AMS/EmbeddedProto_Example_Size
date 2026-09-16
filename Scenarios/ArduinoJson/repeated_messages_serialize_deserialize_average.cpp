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

#include <ArduinoJson.h>
#include <ArduinoJson/helper_repeated_demo.h>
#include <scenario.h>
#include <ArduinoJson/counting_allocator.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  JsonDocument doc(&g_json_allocator);

  HelperRepeatedDemoArduinoJson::set_average_repeated_messages(doc);

  // Create buffer for serialized data
  char buffer[2000];
  report_message_size(sizeof(doc) + g_json_allocator.peak_bytes());
  const auto n_bytes_written = serializeJson(doc, buffer, sizeof(buffer));

  // Create deserialization buffer and copy data
  char deserialize_buffer[2000];
  memcpy(deserialize_buffer, buffer, n_bytes_written);

  // Clear the document data
  JsonDocument doc_deserialized(&g_json_allocator);

  // Deserialize the buffer into the document
  const auto deserialize_error = deserializeJson(doc_deserialized, deserialize_buffer, n_bytes_written);
  if (deserialize_error) {
    return 0;
  }

  // Compare the values with helper functions
  bool success = HelperRepeatedDemoArduinoJson::compare_average_repeated_messages(doc_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? n_bytes_written : 0;
}