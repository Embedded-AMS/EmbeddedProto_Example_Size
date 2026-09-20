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

// Volatile variable to prevent compiler from optimizing away serialization
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  JsonDocument doc(&g_json_allocator);
  HelperPublishedArduinoJson::set_telemetry(doc);

  char buffer[256];
  report_message_size(sizeof(doc) + g_json_allocator.peak_bytes());
  const size_t buffer_size = serializeJson(doc, buffer, sizeof(buffer));

  // Calculate checksum to prevent dead code elimination
  uint8_t checksum = 0;
  for (size_t i = 0; i < buffer_size; i++) {
    checksum ^= static_cast<uint8_t>(buffer[i]);
  }
  g_buffer_checksum = checksum;

  return (buffer_size < sizeof(buffer)) ? static_cast<uint32_t>(buffer_size) : 0;
}
