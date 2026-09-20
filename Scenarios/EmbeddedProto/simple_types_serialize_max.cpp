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

#include <simple_types.h>
#include <EmbeddedProto/helper_simple_types.h>
#include <EmbeddedProto/WriteBufferFixedSize.h>

#include <EmbeddedProto/helper_serialize_mode.h>
#include <scenario.h>
// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  Simple_Types msg;
  report_message_size(sizeof(msg));
  HelperSimpleTypes::set_max(msg);

  EmbeddedProto::WriteBufferFixedSize<150> buffer;
  const auto result = HelperSerializeMode::serialize_message(msg, buffer);

  // Calculate checksum to prevent dead code elimination
  // The compiler cannot optimize away serialize() since buffer content is used
  uint8_t checksum = 0;
  const uint8_t* data = buffer.get_data();
  for (uint32_t i = 0; i < buffer.get_size(); i++) {
    checksum ^= data[i];
  }
  g_buffer_checksum = checksum;

  // A serialize error must not be reported as a byte count
  return (::EmbeddedProto::Error::NO_ERRORS == result) ? buffer.get_size() : 0;
}
