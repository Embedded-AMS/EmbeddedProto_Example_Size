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
#include <ArduinoJson/helper_published.h>
#include <ArduinoJson/counting_allocator.h>
#include <scenario.h>

#include <cstring>

uint32_t run_scenario()
{
  JsonDocument doc(&g_json_allocator);
  HelperPublishedArduinoJson::set_telemetry(doc);

  // Serialize the document
  char buffer[256];
  report_message_size(sizeof(doc) + g_json_allocator.peak_bytes());
  const size_t buffer_size = serializeJson(doc, buffer, sizeof(buffer));
  if ((0 == buffer_size) || (buffer_size >= sizeof(buffer))) {
    return 0;
  }

  // Copy buffer to deserialization buffer
  char deserialize_buffer[256];
  memcpy(deserialize_buffer, buffer, buffer_size);

  // Deserialize into a new document
  JsonDocument doc_deserialized(&g_json_allocator);
  if (deserializeJson(doc_deserialized, deserialize_buffer, buffer_size)) {
    return 0;
  }

  const bool success = HelperPublishedArduinoJson::compare_telemetry(doc_deserialized);

  return success ? static_cast<uint32_t>(buffer_size) : 0;
}
