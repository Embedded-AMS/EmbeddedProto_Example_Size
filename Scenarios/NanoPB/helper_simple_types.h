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

// Helper function for floating-point comparison
template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    return std::abs(a - b) <= epsilon;
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
