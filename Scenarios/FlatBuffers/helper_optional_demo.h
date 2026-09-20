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

#ifndef HELPER_OPTIONAL_DEMO
#define HELPER_OPTIONAL_DEMO

#include <optional_demo_builder.h>
#include <optional_demo_reader.h>

// Every field set to its zero value with presence, as the other libraries do. The schema
// declares the fields as optional scalars, so a zero is stored and reported present.
namespace HelperOptionalDemoFlatBuffers
{

inline void set_zero_with_presence(flatcc_builder_t* B)
{
  Optional_Demo_a_int32_add(B, 0);
  Optional_Demo_a_int64_add(B, 0);
  Optional_Demo_a_uint32_add(B, 0);
  Optional_Demo_a_uint64_add(B, 0);
  Optional_Demo_a_sint32_add(B, 0);
  Optional_Demo_a_sint64_add(B, 0);
  Optional_Demo_a_bool_add(B, false);
  Optional_Demo_a_enum_add(B, Test_Enum_ZERO);
  Optional_Demo_a_fixed64_add(B, 0);
  Optional_Demo_a_sfixed64_add(B, 0);
  Optional_Demo_a_double_add(B, 0.0);
  Optional_Demo_a_fixed32_add(B, 0);
  Optional_Demo_a_sfixed32_add(B, 0);
  Optional_Demo_a_float_add(B, 0.0f);
  Optional_Demo_a_nested_enum_add(B, Nested_Enum_NE_A);
}

inline bool compare_zero_with_presence(Optional_Demo_table_t msg)
{
  bool result = true;
  result &= Optional_Demo_a_int32_is_present(msg) && Optional_Demo_a_int32(msg) == 0;
  result &= Optional_Demo_a_int64_is_present(msg) && Optional_Demo_a_int64(msg) == 0;
  result &= Optional_Demo_a_uint32_is_present(msg) && Optional_Demo_a_uint32(msg) == 0;
  result &= Optional_Demo_a_uint64_is_present(msg) && Optional_Demo_a_uint64(msg) == 0;
  result &= Optional_Demo_a_sint32_is_present(msg) && Optional_Demo_a_sint32(msg) == 0;
  result &= Optional_Demo_a_sint64_is_present(msg) && Optional_Demo_a_sint64(msg) == 0;
  result &= Optional_Demo_a_bool_is_present(msg) && Optional_Demo_a_bool(msg) == false;
  result &= Optional_Demo_a_enum_is_present(msg) && Optional_Demo_a_enum(msg) == Test_Enum_ZERO;
  result &= Optional_Demo_a_fixed64_is_present(msg) && Optional_Demo_a_fixed64(msg) == 0;
  result &= Optional_Demo_a_sfixed64_is_present(msg) && Optional_Demo_a_sfixed64(msg) == 0;
  result &= Optional_Demo_a_double_is_present(msg) && Optional_Demo_a_double(msg) == 0.0;
  result &= Optional_Demo_a_fixed32_is_present(msg) && Optional_Demo_a_fixed32(msg) == 0;
  result &= Optional_Demo_a_sfixed32_is_present(msg) && Optional_Demo_a_sfixed32(msg) == 0;
  result &= Optional_Demo_a_float_is_present(msg) && Optional_Demo_a_float(msg) == 0.0f;
  result &= Optional_Demo_a_nested_enum_is_present(msg) && Optional_Demo_a_nested_enum(msg) == Nested_Enum_NE_A;
  return result;
}

} // End of namespace HelperOptionalDemoFlatBuffers

#endif // HELPER_OPTIONAL_DEMO
