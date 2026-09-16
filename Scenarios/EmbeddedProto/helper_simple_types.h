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

#include <simple_types.h>
#include <limits>
#include <cmath>
#include <algorithm>

namespace HelperSimpleTypes
{

// Epsilon values for floating-point comparisons
constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

// Helper function for floating-point comparison using relative epsilon
template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    // Handle exact equality (including infinities and zeros)
    if (a == b) {
        return true;
    }
    
    // Use relative comparison that scales with the magnitude
    T diff = std::abs(a - b);
    T largest = std::max(std::abs(a), std::abs(b));
    
    return diff <= largest * epsilon;
}

void set_min(Simple_Types& msg)
{
  // Set all fields to their minimum values
  msg.set_a_int32(std::numeric_limits<int32_t>::min());
  msg.set_a_int64(std::numeric_limits<int64_t>::min());
  msg.set_a_uint32(std::numeric_limits<uint32_t>::min());
  msg.set_a_uint64(std::numeric_limits<uint64_t>::min());
  msg.set_a_sint32(std::numeric_limits<int32_t>::min());
  msg.set_a_sint64(std::numeric_limits<int64_t>::min());
  msg.set_a_bool(false);
  msg.set_a_enum(Test_Enum::ZERO);
  msg.set_a_fixed64(std::numeric_limits<uint64_t>::min());
  msg.set_a_sfixed64(std::numeric_limits<int64_t>::min());
  msg.set_a_double(std::numeric_limits<double>::lowest());
  msg.set_a_fixed32(std::numeric_limits<uint32_t>::min());
  msg.set_a_sfixed32(std::numeric_limits<int32_t>::min());
  msg.set_a_float(std::numeric_limits<float>::lowest());
  msg.set_a_nested_enum(Simple_Types::Nested_Enum::NE_A);
}

void set_max(Simple_Types& msg)
{
  // Set all fields to their maximum values
  msg.set_a_int32(std::numeric_limits<int32_t>::max());
  msg.set_a_int64(std::numeric_limits<int64_t>::max());
  msg.set_a_uint32(std::numeric_limits<uint32_t>::max());
  msg.set_a_uint64(std::numeric_limits<uint64_t>::max());
  msg.set_a_sint32(std::numeric_limits<int32_t>::max());
  msg.set_a_sint64(std::numeric_limits<int64_t>::max());
  msg.set_a_bool(true);
  msg.set_a_enum(Test_Enum::TWOBILLION);
  msg.set_a_fixed64(std::numeric_limits<uint64_t>::max());
  msg.set_a_sfixed64(std::numeric_limits<int64_t>::max());
  msg.set_a_double(std::numeric_limits<double>::max());
  msg.set_a_fixed32(std::numeric_limits<uint32_t>::max());
  msg.set_a_sfixed32(std::numeric_limits<int32_t>::max());
  msg.set_a_float(std::numeric_limits<float>::max());
  msg.set_a_nested_enum(Simple_Types::Nested_Enum::NE_C);
}

void set_average(Simple_Types& msg)
{
  // Set all fields to average positive values
  msg.set_a_int32(std::numeric_limits<int32_t>::max() / 2);
  msg.set_a_int64(std::numeric_limits<int64_t>::max() / 2);
  msg.set_a_uint32(std::numeric_limits<uint32_t>::max() / 2);
  msg.set_a_uint64(std::numeric_limits<uint64_t>::max() / 2);
  msg.set_a_sint32(std::numeric_limits<int32_t>::max() / 2);
  msg.set_a_sint64(std::numeric_limits<int64_t>::max() / 2);
  msg.set_a_bool(true);
  msg.set_a_enum(Test_Enum::ONEHUNDERTTWENTYSEVEN);
  msg.set_a_fixed64(std::numeric_limits<uint64_t>::max() / 2);
  msg.set_a_sfixed64(std::numeric_limits<int64_t>::max() / 2);
  msg.set_a_double(1234.5678);
  msg.set_a_fixed32(std::numeric_limits<uint32_t>::max() / 2);
  msg.set_a_sfixed32(std::numeric_limits<int32_t>::max() / 2);
  msg.set_a_float(123.4567f);
  msg.set_a_nested_enum(Simple_Types::Nested_Enum::NE_B);
}

bool compare_min(const Simple_Types& msg)
{
  bool result = true;
  result &= msg.get_a_int32() == std::numeric_limits<int32_t>::min();
  result &= msg.get_a_int64() == std::numeric_limits<int64_t>::min();
  result &= msg.get_a_uint32() == std::numeric_limits<uint32_t>::min();
  result &= msg.get_a_uint64() == std::numeric_limits<uint64_t>::min();
  result &= msg.get_a_sint32() == std::numeric_limits<int32_t>::min();
  result &= msg.get_a_sint64() == std::numeric_limits<int64_t>::min();
  result &= msg.get_a_bool() == false;
  result &= msg.get_a_enum() == Test_Enum::ZERO;
  result &= msg.get_a_fixed64() == std::numeric_limits<uint64_t>::min();
  result &= msg.get_a_sfixed64() == std::numeric_limits<int64_t>::min();
  result &= almost_equal(msg.get_a_double(), std::numeric_limits<double>::lowest(), DOUBLE_EPSILON);
  result &= msg.get_a_fixed32() == std::numeric_limits<uint32_t>::min();
  result &= msg.get_a_sfixed32() == std::numeric_limits<int32_t>::min();
  result &= almost_equal(msg.get_a_float(), std::numeric_limits<float>::lowest(), FLOAT_EPSILON);
  result &= msg.get_a_nested_enum() == Simple_Types::Nested_Enum::NE_A;
  return result;
}

bool compare_max(const Simple_Types& msg)
{
  bool result = true;
  result &= msg.get_a_int32() == std::numeric_limits<int32_t>::max();
  result &= msg.get_a_int64() == std::numeric_limits<int64_t>::max();
  result &= msg.get_a_uint32() == std::numeric_limits<uint32_t>::max();
  result &= msg.get_a_uint64() == std::numeric_limits<uint64_t>::max();
  result &= msg.get_a_sint32() == std::numeric_limits<int32_t>::max();
  result &= msg.get_a_sint64() == std::numeric_limits<int64_t>::max();
  result &= msg.get_a_bool() == true;
  result &= msg.get_a_enum() == Test_Enum::TWOBILLION;
  result &= msg.get_a_fixed64() == std::numeric_limits<uint64_t>::max();
  result &= msg.get_a_sfixed64() == std::numeric_limits<int64_t>::max();
  result &= almost_equal(msg.get_a_double(), std::numeric_limits<double>::max(), DOUBLE_EPSILON);
  result &= msg.get_a_fixed32() == std::numeric_limits<uint32_t>::max();
  result &= msg.get_a_sfixed32() == std::numeric_limits<int32_t>::max();
  result &= almost_equal(msg.get_a_float(), std::numeric_limits<float>::max(), FLOAT_EPSILON);
  result &= msg.get_a_nested_enum() == Simple_Types::Nested_Enum::NE_C;
  return result;
}

bool compare_average(const Simple_Types& msg)
{
  bool result = true;
  result &= msg.get_a_int32() == std::numeric_limits<int32_t>::max() / 2;
  result &= msg.get_a_int64() == std::numeric_limits<int64_t>::max() / 2;
  result &= msg.get_a_uint32() == std::numeric_limits<uint32_t>::max() / 2;
  result &= msg.get_a_uint64() == std::numeric_limits<uint64_t>::max() / 2;
  result &= msg.get_a_sint32() == std::numeric_limits<int32_t>::max() / 2;
  result &= msg.get_a_sint64() == std::numeric_limits<int64_t>::max() / 2;
  result &= msg.get_a_bool() == true;
  result &= msg.get_a_enum() == Test_Enum::ONEHUNDERTTWENTYSEVEN;
  result &= msg.get_a_fixed64() == std::numeric_limits<uint64_t>::max() / 2;
  result &= msg.get_a_sfixed64() == std::numeric_limits<int64_t>::max() / 2;
  result &= almost_equal(msg.get_a_double(), 1234.5678, DOUBLE_EPSILON);
  result &= msg.get_a_fixed32() == std::numeric_limits<uint32_t>::max() / 2;
  result &= msg.get_a_sfixed32() == std::numeric_limits<int32_t>::max() / 2;
  result &= almost_equal(msg.get_a_float(), 123.4567f, FLOAT_EPSILON);
  result &= msg.get_a_nested_enum() == Simple_Types::Nested_Enum::NE_B;
  return result;
}

} // End of namespace HelperSimpleTypes

#endif // HELPER_SIMPLE_TYPES
