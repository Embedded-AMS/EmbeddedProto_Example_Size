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
#include <ArduinoJson/helper_oneof.h>
#include <scenario.h>
#include <ArduinoJson/counting_allocator.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  // Create a JSON document
  JsonDocument doc(&g_json_allocator);

  // Set different oneof fields and serialize
  HelperOneOf::set_int32_field(doc);

  // Serialize the data to buffer
  char buffer[500];
  report_message_size(sizeof(doc) + g_json_allocator.peak_bytes());
  const auto n_bytes_written = serializeJson(doc, buffer, sizeof(buffer));

  // Calculate checksum to prevent dead code elimination
  // The compiler cannot optimize away serialize() since buffer content is used
  uint8_t checksum = 0;
  for (uint32_t i = 0; i < n_bytes_written; i++) {
    checksum ^= static_cast<uint8_t>(buffer[i]);
  }
  g_buffer_checksum = checksum;

  // Deserialize the buffer into a new document
  JsonDocument doc_deserialized(&g_json_allocator);
  DeserializationError error = deserializeJson(doc_deserialized, buffer, n_bytes_written);

  // Verify the deserialized data matches the original
  bool success = HelperOneOf::compare_int32_field(doc_deserialized);

  // Clear the oneof fields
  HelperOneOf::clear_oneof_fields(doc_deserialized);

  // Set a different field (float) and verify
  HelperOneOf::set_float_field(doc_deserialized);
  success &= HelperOneOf::compare_float_field(doc_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? static_cast<uint32_t>(n_bytes_written) : 0;
}
