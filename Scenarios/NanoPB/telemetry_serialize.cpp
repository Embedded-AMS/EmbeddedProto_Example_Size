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

#include "telemetry.pb.h"
#include <NanoPB/helper_published.h>
#include <pb_encode.h>
#include <scenario.h>

// Volatile variable to prevent compiler from optimizing away serialization
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  Telemetry msg = Telemetry_init_default;
  report_message_size(sizeof(msg));
  HelperPublishedNanoPB::set_telemetry(msg);

  uint8_t buffer[64];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  const bool result = pb_encode(&stream, Telemetry_fields, &msg);

  // Calculate checksum to prevent dead code elimination
  uint8_t checksum = 0;
  for (uint32_t i = 0; i < stream.bytes_written; i++) {
    checksum ^= buffer[i];
  }
  g_buffer_checksum = checksum;

  // A serialize error must not be reported as a byte count
  return result ? stream.bytes_written : 0;
}
