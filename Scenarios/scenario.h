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

#ifndef SCENARIO_H
#define SCENARIO_H

#include <cstdint>

// The RAM a scenario's message object takes, read from the device over SWD like the
// cycle count. A scenario reports every message it creates, the largest is kept.
extern volatile uint32_t g_message_size_bytes;

inline void report_message_size(const uint32_t bytes)
{
  if(bytes > g_message_size_bytes)
  {
    g_message_size_bytes = bytes;
  }
}

// Implemented by the selected scenario, returns the number of bytes serialized or 0 on failure.
uint32_t run_scenario();

#endif // SCENARIO_H
