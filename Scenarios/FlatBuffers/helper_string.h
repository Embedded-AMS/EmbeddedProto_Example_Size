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
#define HELPER_STRING

#include <string_demo_builder.h>
#include <string_demo_reader.h>
#include <cstring>

namespace HelperString
{

constexpr const char* TEST_STRING = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

inline void set_string(flatcc_builder_t* B)
{
  String_Demo_Message_text_create_str(B, TEST_STRING);
}

inline bool compare_string(String_Demo_Message_table_t msg)
{
  flatbuffers_string_t text = String_Demo_Message_text(msg);
  return (nullptr != text) && (0 == strcmp(text, TEST_STRING));
}

} // End of namespace HelperString

#endif // HELPER_STRING
