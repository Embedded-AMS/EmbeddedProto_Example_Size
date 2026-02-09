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

#ifndef HELPER_SIMPLE_TYPES_NANOPB
#define HELPER_SIMPLE_TYPES_NANOPB

#include "simple_types.pb.h"
#include <pb.h>
#include <limits>
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace HelperSimpleTypesNanoPB
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
    msg.a_int32 = std::numeric_limits<int32_t>::min();
    msg.a_int64 = std::numeric_limits<int64_t>::min();
    msg.a_uint32 = std::numeric_limits<uint32_t>::min();
    msg.a_uint64 = std::numeric_limits<uint64_t>::min();
    msg.a_sint32 = std::numeric_limits<int32_t>::min();
    msg.a_sint64 = std::numeric_limits<int64_t>::min();
    msg.a_bool = false;
    msg.a_enum = Test_Enum_ZERO;
    msg.a_fixed64 = std::numeric_limits<uint64_t>::min();
    msg.a_sfixed64 = std::numeric_limits<int64_t>::min();
    msg.a_double = std::numeric_limits<double>::lowest();
    msg.a_fixed32 = std::numeric_limits<uint32_t>::min();
    msg.a_sfixed32 = std::numeric_limits<int32_t>::min();
    msg.a_float = std::numeric_limits<float>::lowest();
    msg.a_nested_enum = Simple_Types_Nested_Enum_NE_A;
}

void set_max(Simple_Types& msg)
{
    // Set all fields to their maximum values
    msg.a_int32 = std::numeric_limits<int32_t>::max();
    msg.a_int64 = std::numeric_limits<int64_t>::max();
    msg.a_uint32 = std::numeric_limits<uint32_t>::max();
    msg.a_uint64 = std::numeric_limits<uint64_t>::max();
    msg.a_sint32 = std::numeric_limits<int32_t>::max();
    msg.a_sint64 = std::numeric_limits<int64_t>::max();
    msg.a_bool = true;
    msg.a_enum = Test_Enum_TWOBILLION;
    msg.a_fixed64 = std::numeric_limits<uint64_t>::max();
    msg.a_sfixed64 = std::numeric_limits<int64_t>::max();
    msg.a_double = std::numeric_limits<double>::max();
    msg.a_fixed32 = std::numeric_limits<uint32_t>::max();
    msg.a_sfixed32 = std::numeric_limits<int32_t>::max();
    msg.a_float = std::numeric_limits<float>::max();
    msg.a_nested_enum = Simple_Types_Nested_Enum_NE_C;
}

void set_average(Simple_Types& msg)
{
    // Set all fields to average positive values
    msg.a_int32 = std::numeric_limits<int32_t>::max() / 2;
    msg.a_int64 = std::numeric_limits<int64_t>::max() / 2;
    msg.a_uint32 = std::numeric_limits<uint32_t>::max() / 2;
    msg.a_uint64 = std::numeric_limits<uint64_t>::max() / 2;
    msg.a_sint32 = std::numeric_limits<int32_t>::max() / 2;
    msg.a_sint64 = std::numeric_limits<int64_t>::max() / 2;
    msg.a_bool = true;
    msg.a_enum = Test_Enum_ONEHUNDERTTWENTYSEVEN;
    msg.a_fixed64 = std::numeric_limits<uint64_t>::max() / 2;
    msg.a_sfixed64 = std::numeric_limits<int64_t>::max() / 2;
    msg.a_double = 1234.5678;
    msg.a_fixed32 = std::numeric_limits<uint32_t>::max() / 2;
    msg.a_sfixed32 = std::numeric_limits<int32_t>::max() / 2;
    msg.a_float = 123.4567f;
    msg.a_nested_enum = Simple_Types_Nested_Enum_NE_B;
}

bool compare_min(const Simple_Types& msg)
{
    bool result = true;
    result &= msg.a_int32 == std::numeric_limits<int32_t>::min();
    result &= msg.a_int64 == std::numeric_limits<int64_t>::min();
    result &= msg.a_uint32 == std::numeric_limits<uint32_t>::min();
    result &= msg.a_uint64 == std::numeric_limits<uint64_t>::min();
    result &= msg.a_sint32 == std::numeric_limits<int32_t>::min();
    result &= msg.a_sint64 == std::numeric_limits<int64_t>::min();
    result &= msg.a_bool == false;
    result &= msg.a_enum == Test_Enum_ZERO;
    result &= msg.a_fixed64 == std::numeric_limits<uint64_t>::min();
    result &= msg.a_sfixed64 == std::numeric_limits<int64_t>::min();
    result &= almost_equal(msg.a_double, std::numeric_limits<double>::lowest(), DOUBLE_EPSILON);
    result &= msg.a_fixed32 == std::numeric_limits<uint32_t>::min();
    result &= msg.a_sfixed32 == std::numeric_limits<int32_t>::min();
    result &= almost_equal(msg.a_float, std::numeric_limits<float>::lowest(), FLOAT_EPSILON);
    result &= msg.a_nested_enum == Simple_Types_Nested_Enum_NE_A;
    return result;
}

bool compare_max(const Simple_Types& msg)
{
    bool result = true;
    result &= msg.a_int32 == std::numeric_limits<int32_t>::max();
    result &= msg.a_int64 == std::numeric_limits<int64_t>::max();
    result &= msg.a_uint32 == std::numeric_limits<uint32_t>::max();
    result &= msg.a_uint64 == std::numeric_limits<uint64_t>::max();
    result &= msg.a_sint32 == std::numeric_limits<int32_t>::max();
    result &= msg.a_sint64 == std::numeric_limits<int64_t>::max();
    result &= msg.a_bool == true;
    result &= msg.a_enum == Test_Enum_TWOBILLION;
    result &= msg.a_fixed64 == std::numeric_limits<uint64_t>::max();
    result &= msg.a_sfixed64 == std::numeric_limits<int64_t>::max();
    result &= almost_equal(msg.a_double, std::numeric_limits<double>::max(), DOUBLE_EPSILON);
    result &= msg.a_fixed32 == std::numeric_limits<uint32_t>::max();
    result &= msg.a_sfixed32 == std::numeric_limits<int32_t>::max();
    result &= almost_equal(msg.a_float, std::numeric_limits<float>::max(), FLOAT_EPSILON);
    result &= msg.a_nested_enum == Simple_Types_Nested_Enum_NE_C;
    return result;
}

bool compare_average(const Simple_Types& msg)
{
    bool result = true;
    result &= msg.a_int32 == std::numeric_limits<int32_t>::max() / 2;
    result &= msg.a_int64 == std::numeric_limits<int64_t>::max() / 2;
    result &= msg.a_uint32 == std::numeric_limits<uint32_t>::max() / 2;
    result &= msg.a_uint64 == std::numeric_limits<uint64_t>::max() / 2;
    result &= msg.a_sint32 == std::numeric_limits<int32_t>::max() / 2;
    result &= msg.a_sint64 == std::numeric_limits<int64_t>::max() / 2;
    result &= msg.a_bool == true;
    result &= msg.a_enum == Test_Enum_ONEHUNDERTTWENTYSEVEN;
    result &= msg.a_fixed64 == std::numeric_limits<uint64_t>::max() / 2;
    result &= msg.a_sfixed64 == std::numeric_limits<int64_t>::max() / 2;
    result &= almost_equal(msg.a_double, 1234.5678, DOUBLE_EPSILON);
    result &= msg.a_fixed32 == std::numeric_limits<uint32_t>::max() / 2;
    result &= msg.a_sfixed32 == std::numeric_limits<int32_t>::max() / 2;
    result &= almost_equal(msg.a_float, 123.4567f, FLOAT_EPSILON);
    result &= msg.a_nested_enum == Simple_Types_Nested_Enum_NE_B;
    return result;
}

} // End of namespace HelperSimpleTypesNanoPB

#endif // HELPER_SIMPLE_TYPES_NANOPB
