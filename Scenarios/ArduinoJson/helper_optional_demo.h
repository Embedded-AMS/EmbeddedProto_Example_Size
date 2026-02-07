#ifndef HELPER_OPTIONAL_DEMO_ARDUINOJSON
#define HELPER_OPTIONAL_DEMO_ARDUINOJSON

#include <ArduinoJson.h>
#include <limits>
#include <cmath>
#include <algorithm>

namespace HelperOptionalDemoArduinoJson
{

// Epsilon values for floating-point comparisons
constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

// Helper function for floating-point comparison
template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    return std::abs(a - b) <= epsilon;
}

void set_zero_with_presence(JsonDocument& doc)
{
    // Set all fields to their zero values
    // ArduinoJson naturally handles zero values in JSON serialization
    doc["a_int32"] = 0;
    doc["a_int64"] = 0;
    doc["a_uint32"] = 0;
    doc["a_uint64"] = 0;
    doc["a_sint32"] = 0;
    doc["a_sint64"] = 0;
    doc["a_bool"] = false;
    doc["a_enum"] = 0; // ZERO enum value
    doc["a_fixed64"] = 0;
    doc["a_sfixed64"] = 0;
    doc["a_double"] = 0.0;
    doc["a_fixed32"] = 0;
    doc["a_sfixed32"] = 0;
    doc["a_float"] = 0.0f;
    doc["a_nested_enum"] = 0; // NE_A enum value
}

bool compare_zero_with_presence(const JsonDocument& doc)
{
    bool result = true;
    // Check that zero values are preserved in JSON
    result &= doc["a_int32"] == 0;
    result &= doc["a_int64"] == 0;
    result &= doc["a_uint32"] == 0;
    result &= doc["a_uint64"] == 0;
    result &= doc["a_sint32"] == 0;
    result &= doc["a_sint64"] == 0;
    result &= doc["a_bool"] == false;
    result &= doc["a_enum"] == 0; // ZERO enum value
    result &= doc["a_fixed64"] == 0;
    result &= doc["a_sfixed64"] == 0;
    result &= almost_equal(static_cast<double>(doc["a_double"]), 0.0, DOUBLE_EPSILON);
    result &= doc["a_fixed32"] == 0;
    result &= doc["a_sfixed32"] == 0;
    result &= almost_equal(static_cast<float>(doc["a_float"]), 0.0f, FLOAT_EPSILON);
    result &= doc["a_nested_enum"] == 0; // NE_A enum value
    return result;
}

} // End of namespace HelperOptionalDemoArduinoJson

#endif // HELPER_OPTIONAL_DEMO_ARDUINOJSON