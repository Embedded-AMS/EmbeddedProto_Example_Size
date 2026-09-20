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

#ifndef HELPER_CALLBACK_DEMO
#define HELPER_CALLBACK_DEMO

#include <cstdint>
#include <limits>

// The data streamed through the callback scenarios. Both libraries use exactly the same
// values so the serialized bytes, and therefore the work done, are identical.
namespace HelperCallbackDemo
{

// Number of int32 samples streamed through the repeated field callback.
constexpr uint32_t SAMPLE_COUNT = 10;

// The same average values as the resident repeated_integers scenario uses.
inline int32_t sample_value(uint32_t index)
{
  return std::numeric_limits<int32_t>::max() / 2 - 1000 + static_cast<int32_t>(index * 200);
}

// Total length of the bytes payload and the size of the window it is streamed in.
constexpr uint32_t PAYLOAD_SIZE = 64;
constexpr uint32_t WINDOW_SIZE = 16;

inline uint8_t payload_byte(uint32_t index)
{
  return static_cast<uint8_t>((index * 7) + 3);
}

} // End of namespace HelperCallbackDemo

#endif // HELPER_CALLBACK_DEMO
