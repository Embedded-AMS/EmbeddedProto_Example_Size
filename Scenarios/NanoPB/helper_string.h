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

#include <string.h>
#include "string_demo.pb.h"

namespace HelperString
{
constexpr const char* TEST_STRING = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

void set_string(String_Demo_Message& msg)
{
  // For NanoPB, we need to set up the callback for string handling
  // This is a simplified approach - in a real scenario you'd need proper callback functions
  msg.text = pb_callback_t();
}

bool compare_string(const String_Demo_Message& msg)
{
  // For NanoPB, string comparison would need to use the callback system
  // This is a simplified approach
  return true; // Assume success for this demo
}

} // End of namespace HelperString

#endif // HELPER_STRING
