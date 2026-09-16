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

#include <cbor_messages_types.h>
#include <limits>
#include <cmath>
#include <algorithm>

// The same values as the Embedded Proto and nanopb helpers. Every entry is marked
// present, so the map carries all fifteen fields.
namespace HelperSimpleTypesZcbor
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

#define ZCBOR_SET(msg, field, value) do { (msg).field.field = (value); (msg).field##_present = true; } while(0)

inline void set_min(struct Simple_Types& msg)
{
  ZCBOR_SET(msg, a_int32, std::numeric_limits<int32_t>::min());
  ZCBOR_SET(msg, a_int64, std::numeric_limits<int64_t>::min());
  ZCBOR_SET(msg, a_uint32, std::numeric_limits<uint32_t>::min());
  ZCBOR_SET(msg, a_uint64, std::numeric_limits<uint64_t>::min());
  ZCBOR_SET(msg, a_sint32, std::numeric_limits<int32_t>::min());
  ZCBOR_SET(msg, a_sint64, std::numeric_limits<int64_t>::min());
  ZCBOR_SET(msg, a_bool, false);
  ZCBOR_SET(msg, a_enum, 0);
  ZCBOR_SET(msg, a_fixed64, std::numeric_limits<uint64_t>::min());
  ZCBOR_SET(msg, a_sfixed64, std::numeric_limits<int64_t>::min());
  ZCBOR_SET(msg, a_double, std::numeric_limits<double>::lowest());
  ZCBOR_SET(msg, a_fixed32, std::numeric_limits<uint32_t>::min());
  ZCBOR_SET(msg, a_sfixed32, std::numeric_limits<int32_t>::min());
  ZCBOR_SET(msg, a_float, std::numeric_limits<float>::lowest());
  ZCBOR_SET(msg, a_nested_enum, 0);
}

inline void set_max(struct Simple_Types& msg)
{
  ZCBOR_SET(msg, a_int32, std::numeric_limits<int32_t>::max());
  ZCBOR_SET(msg, a_int64, std::numeric_limits<int64_t>::max());
  ZCBOR_SET(msg, a_uint32, std::numeric_limits<uint32_t>::max());
  ZCBOR_SET(msg, a_uint64, std::numeric_limits<uint64_t>::max());
  ZCBOR_SET(msg, a_sint32, std::numeric_limits<int32_t>::max());
  ZCBOR_SET(msg, a_sint64, std::numeric_limits<int64_t>::max());
  ZCBOR_SET(msg, a_bool, true);
  ZCBOR_SET(msg, a_enum, 2000000000);
  ZCBOR_SET(msg, a_fixed64, std::numeric_limits<uint64_t>::max());
  ZCBOR_SET(msg, a_sfixed64, std::numeric_limits<int64_t>::max());
  ZCBOR_SET(msg, a_double, std::numeric_limits<double>::max());
  ZCBOR_SET(msg, a_fixed32, std::numeric_limits<uint32_t>::max());
  ZCBOR_SET(msg, a_sfixed32, std::numeric_limits<int32_t>::max());
  ZCBOR_SET(msg, a_float, std::numeric_limits<float>::max());
  ZCBOR_SET(msg, a_nested_enum, 2);
}

inline void set_average(struct Simple_Types& msg)
{
  ZCBOR_SET(msg, a_int32, std::numeric_limits<int32_t>::max() / 2);
  ZCBOR_SET(msg, a_int64, std::numeric_limits<int64_t>::max() / 2);
  ZCBOR_SET(msg, a_uint32, std::numeric_limits<uint32_t>::max() / 2);
  ZCBOR_SET(msg, a_uint64, std::numeric_limits<uint64_t>::max() / 2);
  ZCBOR_SET(msg, a_sint32, std::numeric_limits<int32_t>::max() / 2);
  ZCBOR_SET(msg, a_sint64, std::numeric_limits<int64_t>::max() / 2);
  ZCBOR_SET(msg, a_bool, true);
  ZCBOR_SET(msg, a_enum, 127);
  ZCBOR_SET(msg, a_fixed64, std::numeric_limits<uint64_t>::max() / 2);
  ZCBOR_SET(msg, a_sfixed64, std::numeric_limits<int64_t>::max() / 2);
  ZCBOR_SET(msg, a_double, 1234.5678);
  ZCBOR_SET(msg, a_fixed32, std::numeric_limits<uint32_t>::max() / 2);
  ZCBOR_SET(msg, a_sfixed32, std::numeric_limits<int32_t>::max() / 2);
  ZCBOR_SET(msg, a_float, 123.4567f);
  ZCBOR_SET(msg, a_nested_enum, 1);
}

inline bool compare_min(const struct Simple_Types& msg)
{
  bool result = true;
  result &= msg.a_int32.a_int32 == std::numeric_limits<int32_t>::min();
  result &= msg.a_int64.a_int64 == std::numeric_limits<int64_t>::min();
  result &= msg.a_uint32.a_uint32 == std::numeric_limits<uint32_t>::min();
  result &= msg.a_uint64.a_uint64 == std::numeric_limits<uint64_t>::min();
  result &= msg.a_sint32.a_sint32 == std::numeric_limits<int32_t>::min();
  result &= msg.a_sint64.a_sint64 == std::numeric_limits<int64_t>::min();
  result &= msg.a_bool.a_bool == false;
  result &= msg.a_enum.a_enum == 0;
  result &= msg.a_fixed64.a_fixed64 == std::numeric_limits<uint64_t>::min();
  result &= msg.a_sfixed64.a_sfixed64 == std::numeric_limits<int64_t>::min();
  result &= almost_equal(msg.a_double.a_double, std::numeric_limits<double>::lowest(), DOUBLE_EPSILON);
  result &= msg.a_fixed32.a_fixed32 == std::numeric_limits<uint32_t>::min();
  result &= msg.a_sfixed32.a_sfixed32 == std::numeric_limits<int32_t>::min();
  result &= almost_equal(msg.a_float.a_float, std::numeric_limits<float>::lowest(), FLOAT_EPSILON);
  result &= msg.a_nested_enum.a_nested_enum == 0;
  return result;
}

inline bool compare_max(const struct Simple_Types& msg)
{
  bool result = true;
  result &= msg.a_int32.a_int32 == std::numeric_limits<int32_t>::max();
  result &= msg.a_int64.a_int64 == std::numeric_limits<int64_t>::max();
  result &= msg.a_uint32.a_uint32 == std::numeric_limits<uint32_t>::max();
  result &= msg.a_uint64.a_uint64 == std::numeric_limits<uint64_t>::max();
  result &= msg.a_sint32.a_sint32 == std::numeric_limits<int32_t>::max();
  result &= msg.a_sint64.a_sint64 == std::numeric_limits<int64_t>::max();
  result &= msg.a_bool.a_bool == true;
  result &= msg.a_enum.a_enum == 2000000000;
  result &= msg.a_fixed64.a_fixed64 == std::numeric_limits<uint64_t>::max();
  result &= msg.a_sfixed64.a_sfixed64 == std::numeric_limits<int64_t>::max();
  result &= almost_equal(msg.a_double.a_double, std::numeric_limits<double>::max(), DOUBLE_EPSILON);
  result &= msg.a_fixed32.a_fixed32 == std::numeric_limits<uint32_t>::max();
  result &= msg.a_sfixed32.a_sfixed32 == std::numeric_limits<int32_t>::max();
  result &= almost_equal(msg.a_float.a_float, std::numeric_limits<float>::max(), FLOAT_EPSILON);
  result &= msg.a_nested_enum.a_nested_enum == 2;
  return result;
}

inline bool compare_average(const struct Simple_Types& msg)
{
  bool result = true;
  result &= msg.a_int32.a_int32 == std::numeric_limits<int32_t>::max() / 2;
  result &= msg.a_int64.a_int64 == std::numeric_limits<int64_t>::max() / 2;
  result &= msg.a_uint32.a_uint32 == std::numeric_limits<uint32_t>::max() / 2;
  result &= msg.a_uint64.a_uint64 == std::numeric_limits<uint64_t>::max() / 2;
  result &= msg.a_sint32.a_sint32 == std::numeric_limits<int32_t>::max() / 2;
  result &= msg.a_sint64.a_sint64 == std::numeric_limits<int64_t>::max() / 2;
  result &= msg.a_bool.a_bool == true;
  result &= msg.a_enum.a_enum == 127;
  result &= msg.a_fixed64.a_fixed64 == std::numeric_limits<uint64_t>::max() / 2;
  result &= msg.a_sfixed64.a_sfixed64 == std::numeric_limits<int64_t>::max() / 2;
  result &= almost_equal(msg.a_double.a_double, 1234.5678, DOUBLE_EPSILON);
  result &= msg.a_fixed32.a_fixed32 == std::numeric_limits<uint32_t>::max() / 2;
  result &= msg.a_sfixed32.a_sfixed32 == std::numeric_limits<int32_t>::max() / 2;
  result &= almost_equal(msg.a_float.a_float, 123.4567f, FLOAT_EPSILON);
  result &= msg.a_nested_enum.a_nested_enum == 1;
  return result;
}

} // End of namespace HelperSimpleTypesZcbor

#endif // HELPER_SIMPLE_TYPES
