#ifndef HELPER_OPTIONAL_DEMO

#include <optional_demo.h>
#include <limits>
#include <cmath>
#include <algorithm>

namespace HelperOptionalDemo
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
    // Set all fields to their zero values and explicitly set presence flags
    msg.set_a_int32(0);
    msg.set_a_int64(0);
    msg.set_a_uint32(0);
    msg.set_a_uint64(0);
    msg.set_a_sint32(0);
    msg.set_a_sint64(0);
    msg.set_a_bool(false);
    msg.set_a_enum(Test_Enum::ZERO);
    msg.set_a_fixed64(0);
    msg.set_a_sfixed64(0);
    msg.set_a_double(0.0);
    msg.set_a_fixed32(0);
    msg.set_a_sfixed32(0);
    msg.set_a_float(0.0f);
    msg.set_a_nested_enum(Optional_Demo::Nested_Enum::NE_A);
}

bool compare_zero_with_presence(const Optional_Demo& msg)
{
    bool result = true;
    // Check both the value and presence flag for each field
    result &= msg.has_a_int32() && msg.get_a_int32() == 0;
    result &= msg.has_a_int64() && msg.get_a_int64() == 0;
    result &= msg.has_a_uint32() && msg.get_a_uint32() == 0;
    result &= msg.has_a_uint64() && msg.get_a_uint64() == 0;
    result &= msg.has_a_sint32() && msg.get_a_sint32() == 0;
    result &= msg.has_a_sint64() && msg.get_a_sint64() == 0;
    result &= msg.has_a_bool() && msg.get_a_bool() == false;
    result &= msg.has_a_enum() && msg.get_a_enum() == Test_Enum::ZERO;
    result &= msg.has_a_fixed64() && msg.get_a_fixed64() == 0;
    result &= msg.has_a_sfixed64() && msg.get_a_sfixed64() == 0;
    result &= msg.has_a_double() && almost_equal(msg.get_a_double(), 0.0, DOUBLE_EPSILON);
    result &= msg.has_a_fixed32() && msg.get_a_fixed32() == 0;
    result &= msg.has_a_sfixed32() && msg.get_a_sfixed32() == 0;
    result &= msg.has_a_float() && almost_equal(msg.get_a_float(), 0.0f, FLOAT_EPSILON);
    result &= msg.has_a_nested_enum() && msg.get_a_nested_enum() == Optional_Demo::Nested_Enum::NE_A;
    return result;
}

} // End of namespace HelperOptionalDemo

#endif // HELPER_OPTIONAL_DEMO