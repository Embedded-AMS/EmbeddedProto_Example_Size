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

#ifndef HELPER_SIMPLE_TYPES
#define HELPER_SIMPLE_TYPES

#include <simple_types_builder.h>
#include <simple_types_reader.h>
#include <limits>
#include <cmath>
#include <algorithm>

// The same values as the Embedded Proto and nanopb helpers. The set functions add the
// fields to the table the builder currently has open, the compare functions read them
// back from a finished buffer.
namespace HelperSimpleTypesFlatBuffers
{

constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    if (a == b) {
        return true;
    }
    T diff = std::abs(a - b);
    T largest = std::max(std::abs(a), std::abs(b));
    return diff <= largest * epsilon;
}

// The fifteen field values in schema order, for the generated create() helpers which
// write a whole table in one call. That is the idiomatic flatcc way when every field
// is known up front, and cheaper than fifteen separate add() calls.
#define SIMPLE_TYPES_MIN_ARGS \
  std::numeric_limits<int32_t>::min(), std::numeric_limits<int64_t>::min(), std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint64_t>::min(), \
  std::numeric_limits<int32_t>::min(), std::numeric_limits<int64_t>::min(), false, Test_Enum_ZERO, \
  std::numeric_limits<uint64_t>::min(), std::numeric_limits<int64_t>::min(), std::numeric_limits<double>::lowest(), std::numeric_limits<uint32_t>::min(), \
  std::numeric_limits<int32_t>::min(), std::numeric_limits<float>::lowest(), Nested_Enum_NE_A
#define SIMPLE_TYPES_MAX_ARGS \
  std::numeric_limits<int32_t>::max(), std::numeric_limits<int64_t>::max(), std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint64_t>::max(), \
  std::numeric_limits<int32_t>::max(), std::numeric_limits<int64_t>::max(), true, Test_Enum_TWOBILLION, \
  std::numeric_limits<uint64_t>::max(), std::numeric_limits<int64_t>::max(), std::numeric_limits<double>::max(), std::numeric_limits<uint32_t>::max(), \
  std::numeric_limits<int32_t>::max(), std::numeric_limits<float>::max(), Nested_Enum_NE_C
#define SIMPLE_TYPES_AVERAGE_ARGS \
  std::numeric_limits<int32_t>::max() / 2, std::numeric_limits<int64_t>::max() / 2, std::numeric_limits<uint32_t>::max() / 2, std::numeric_limits<uint64_t>::max() / 2, \
  std::numeric_limits<int32_t>::max() / 2, std::numeric_limits<int64_t>::max() / 2, true, Test_Enum_ONEHUNDERTTWENTYSEVEN, \
  std::numeric_limits<uint64_t>::max() / 2, std::numeric_limits<int64_t>::max() / 2, 1234.5678, std::numeric_limits<uint32_t>::max() / 2, \
  std::numeric_limits<int32_t>::max() / 2, 123.4567f, Nested_Enum_NE_B

inline void create_min_as_root(flatcc_builder_t* B) { Simple_Types_create_as_root(B, SIMPLE_TYPES_MIN_ARGS); }
inline void create_max_as_root(flatcc_builder_t* B) { Simple_Types_create_as_root(B, SIMPLE_TYPES_MAX_ARGS); }
inline void create_average_as_root(flatcc_builder_t* B) { Simple_Types_create_as_root(B, SIMPLE_TYPES_AVERAGE_ARGS); }

inline bool compare_min(Simple_Types_table_t msg)
{
  bool result = true;
  result &= Simple_Types_a_int32(msg) == std::numeric_limits<int32_t>::min();
  result &= Simple_Types_a_int64(msg) == std::numeric_limits<int64_t>::min();
  result &= Simple_Types_a_uint32(msg) == std::numeric_limits<uint32_t>::min();
  result &= Simple_Types_a_uint64(msg) == std::numeric_limits<uint64_t>::min();
  result &= Simple_Types_a_sint32(msg) == std::numeric_limits<int32_t>::min();
  result &= Simple_Types_a_sint64(msg) == std::numeric_limits<int64_t>::min();
  result &= Simple_Types_a_bool(msg) == false;
  result &= Simple_Types_a_enum(msg) == Test_Enum_ZERO;
  result &= Simple_Types_a_fixed64(msg) == std::numeric_limits<uint64_t>::min();
  result &= Simple_Types_a_sfixed64(msg) == std::numeric_limits<int64_t>::min();
  result &= almost_equal(Simple_Types_a_double(msg), std::numeric_limits<double>::lowest(), DOUBLE_EPSILON);
  result &= Simple_Types_a_fixed32(msg) == std::numeric_limits<uint32_t>::min();
  result &= Simple_Types_a_sfixed32(msg) == std::numeric_limits<int32_t>::min();
  result &= almost_equal(Simple_Types_a_float(msg), std::numeric_limits<float>::lowest(), FLOAT_EPSILON);
  result &= Simple_Types_a_nested_enum(msg) == Nested_Enum_NE_A;
  return result;
}

inline bool compare_max(Simple_Types_table_t msg)
{
  bool result = true;
  result &= Simple_Types_a_int32(msg) == std::numeric_limits<int32_t>::max();
  result &= Simple_Types_a_int64(msg) == std::numeric_limits<int64_t>::max();
  result &= Simple_Types_a_uint32(msg) == std::numeric_limits<uint32_t>::max();
  result &= Simple_Types_a_uint64(msg) == std::numeric_limits<uint64_t>::max();
  result &= Simple_Types_a_sint32(msg) == std::numeric_limits<int32_t>::max();
  result &= Simple_Types_a_sint64(msg) == std::numeric_limits<int64_t>::max();
  result &= Simple_Types_a_bool(msg) == true;
  result &= Simple_Types_a_enum(msg) == Test_Enum_TWOBILLION;
  result &= Simple_Types_a_fixed64(msg) == std::numeric_limits<uint64_t>::max();
  result &= Simple_Types_a_sfixed64(msg) == std::numeric_limits<int64_t>::max();
  result &= almost_equal(Simple_Types_a_double(msg), std::numeric_limits<double>::max(), DOUBLE_EPSILON);
  result &= Simple_Types_a_fixed32(msg) == std::numeric_limits<uint32_t>::max();
  result &= Simple_Types_a_sfixed32(msg) == std::numeric_limits<int32_t>::max();
  result &= almost_equal(Simple_Types_a_float(msg), std::numeric_limits<float>::max(), FLOAT_EPSILON);
  result &= Simple_Types_a_nested_enum(msg) == Nested_Enum_NE_C;
  return result;
}

inline bool compare_average(Simple_Types_table_t msg)
{
  bool result = true;
  result &= Simple_Types_a_int32(msg) == std::numeric_limits<int32_t>::max() / 2;
  result &= Simple_Types_a_int64(msg) == std::numeric_limits<int64_t>::max() / 2;
  result &= Simple_Types_a_uint32(msg) == std::numeric_limits<uint32_t>::max() / 2;
  result &= Simple_Types_a_uint64(msg) == std::numeric_limits<uint64_t>::max() / 2;
  result &= Simple_Types_a_sint32(msg) == std::numeric_limits<int32_t>::max() / 2;
  result &= Simple_Types_a_sint64(msg) == std::numeric_limits<int64_t>::max() / 2;
  result &= Simple_Types_a_bool(msg) == true;
  result &= Simple_Types_a_enum(msg) == Test_Enum_ONEHUNDERTTWENTYSEVEN;
  result &= Simple_Types_a_fixed64(msg) == std::numeric_limits<uint64_t>::max() / 2;
  result &= Simple_Types_a_sfixed64(msg) == std::numeric_limits<int64_t>::max() / 2;
  result &= almost_equal(Simple_Types_a_double(msg), 1234.5678, DOUBLE_EPSILON);
  result &= Simple_Types_a_fixed32(msg) == std::numeric_limits<uint32_t>::max() / 2;
  result &= Simple_Types_a_sfixed32(msg) == std::numeric_limits<int32_t>::max() / 2;
  result &= almost_equal(Simple_Types_a_float(msg), 123.4567f, FLOAT_EPSILON);
  result &= Simple_Types_a_nested_enum(msg) == Nested_Enum_NE_B;
  return result;
}

} // End of namespace HelperSimpleTypesFlatBuffers

#endif // HELPER_SIMPLE_TYPES
