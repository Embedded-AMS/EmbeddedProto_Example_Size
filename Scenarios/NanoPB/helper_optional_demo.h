#ifndef HELPER_OPTIONAL_DEMO_NANOPB
#define HELPER_OPTIONAL_DEMO_NANOPB

#include "optional_demo.pb.h"
#include <pb.h>
#include <limits>
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace HelperOptionalDemoNanoPB
{

// Epsilon values for floating-point comparisons
constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

// Helper function for floating-point comparison
template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    return std::abs(a - b) <= epsilon;
}

void set_zero_with_presence(Optional_Demo& msg)
{
    // Set all fields to their zero values
    // NanoPB uses has_* fields for presence tracking
    msg.has_a_int32 = true;
    msg.a_int32 = 0;
    msg.has_a_int64 = true;
    msg.a_int64 = 0;
    msg.has_a_uint32 = true;
    msg.a_uint32 = 0;
    msg.has_a_uint64 = true;
    msg.a_uint64 = 0;
    msg.has_a_sint32 = true;
    msg.a_sint32 = 0;
    msg.has_a_sint64 = true;
    msg.a_sint64 = 0;
    msg.has_a_bool = true;
    msg.a_bool = false;
    msg.has_a_enum = true;
    msg.a_enum = Test_Enum_ZERO;
    msg.has_a_fixed64 = true;
    msg.a_fixed64 = 0;
    msg.has_a_sfixed64 = true;
    msg.a_sfixed64 = 0;
    msg.has_a_double = true;
    msg.a_double = 0.0;
    msg.has_a_fixed32 = true;
    msg.a_fixed32 = 0;
    msg.has_a_sfixed32 = true;
    msg.a_sfixed32 = 0;
    msg.has_a_float = true;
    msg.a_float = 0.0f;
    msg.has_a_nested_enum = true;
    msg.a_nested_enum = Optional_Demo_Nested_Enum_NE_A;
}

bool compare_zero_with_presence(const Optional_Demo& msg)
{
    bool result = true;
    // Check both the presence flag and zero value for each field
    result &= msg.has_a_int32 && msg.a_int32 == 0;
    result &= msg.has_a_int64 && msg.a_int64 == 0;
    result &= msg.has_a_uint32 && msg.a_uint32 == 0;
    result &= msg.has_a_uint64 && msg.a_uint64 == 0;
    result &= msg.has_a_sint32 && msg.a_sint32 == 0;
    result &= msg.has_a_sint64 && msg.a_sint64 == 0;
    result &= msg.has_a_bool && msg.a_bool == false;
    result &= msg.has_a_enum && msg.a_enum == Test_Enum_ZERO;
    result &= msg.has_a_fixed64 && msg.a_fixed64 == 0;
    result &= msg.has_a_sfixed64 && msg.a_sfixed64 == 0;
    result &= msg.has_a_double && almost_equal(msg.a_double, 0.0, DOUBLE_EPSILON);
    result &= msg.has_a_fixed32 && msg.a_fixed32 == 0;
    result &= msg.has_a_sfixed32 && msg.a_sfixed32 == 0;
    result &= msg.has_a_float && almost_equal(msg.a_float, 0.0f, FLOAT_EPSILON);
    result &= msg.has_a_nested_enum && msg.a_nested_enum == Optional_Demo_Nested_Enum_NE_A;
    return result;
}

} // End of namespace HelperOptionalDemoNanoPB

#endif // HELPER_OPTIONAL_DEMO_NANOPB