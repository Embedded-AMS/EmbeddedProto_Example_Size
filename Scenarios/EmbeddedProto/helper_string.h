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

#ifndef HELPER_STRING

#include <string_demo.h>
#include <cstdint>
#include <cstring>

namespace HelperString
{
constexpr const char* TEST_STRING = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
constexpr uint32_t STRING_LENGTH = 123; // Length for the string field

template<uint32_t LENGTH>
void set_string(String_Demo_Message<LENGTH>& msg)
{
  msg.mutable_text().set(TEST_STRING);
}

template<uint32_t LENGTH>
bool compare_string(const String_Demo_Message<LENGTH>& msg)
{
  return strcmp(msg.text(), TEST_STRING) == 0;
}

} // End of namespace HelperString

#endif // HELPER_STRING
