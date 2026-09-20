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
#include <Zcbor/helper_published.h>
#include <scenario.h>

// Volatile variable to prevent compiler from optimizing away serialization
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  struct Telemetry msg = {};
  report_message_size(sizeof(msg));
  HelperPublishedZcbor::set_telemetry(msg);

  uint8_t buffer[64];
  size_t buffer_size = 0;
  const int result = cbor_encode_Telemetry(buffer, sizeof(buffer), &msg, &buffer_size);

  // Calculate checksum to prevent dead code elimination
  uint8_t checksum = 0;
  for (uint32_t i = 0; i < buffer_size; i++) {
    checksum ^= buffer[i];
  }
  g_buffer_checksum = checksum;

  return (ZCBOR_SUCCESS == result) ? static_cast<uint32_t>(buffer_size) : 0;
}
