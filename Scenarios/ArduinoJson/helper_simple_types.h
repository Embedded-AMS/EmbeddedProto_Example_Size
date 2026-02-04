#ifndef HELPER_SIMPLE_TYPES_ARDUINOJSON
#define HELPER_SIMPLE_TYPES_ARDUINOJSON

#include <ArduinoJson.h>
#include <limits>

namespace HelperSimpleTypesArduinoJson
{

void set_min(JsonDocument& doc)
{
    // Set all fields to their minimum values
    doc["a_int32"] = std::numeric_limits<int32_t>::min();
    doc["a_int64"] = std::numeric_limits<int64_t>::min();
    doc["a_uint32"] = std::numeric_limits<uint32_t>::min();
    doc["a_uint64"] = std::numeric_limits<uint64_t>::min();
    doc["a_sint32"] = std::numeric_limits<int32_t>::min();
    doc["a_sint64"] = std::numeric_limits<int64_t>::min();
    doc["a_bool"] = false;
    doc["a_enum"] = 0; // ZERO enum value
    doc["a_fixed64"] = std::numeric_limits<uint64_t>::min();
    doc["a_sfixed64"] = std::numeric_limits<int64_t>::min();
    doc["a_double"] = std::numeric_limits<double>::lowest();
    doc["a_fixed32"] = std::numeric_limits<uint32_t>::min();
    doc["a_sfixed32"] = std::numeric_limits<int32_t>::min();
    doc["a_float"] = std::numeric_limits<float>::lowest();
    doc["a_nested_enum"] = 0; // NE_A enum value
}

void set_max(JsonDocument& doc)
{
    // Set all fields to their maximum values
    doc["a_int32"] = std::numeric_limits<int32_t>::max();
    doc["a_int64"] = std::numeric_limits<int64_t>::max();
    doc["a_uint32"] = std::numeric_limits<uint32_t>::max();
    doc["a_uint64"] = std::numeric_limits<uint64_t>::max();
    doc["a_sint32"] = std::numeric_limits<int32_t>::max();
    doc["a_sint64"] = std::numeric_limits<int64_t>::max();
    doc["a_bool"] = true;
    doc["a_enum"] = 2000000000; // TWOBILLION enum value
    doc["a_fixed64"] = std::numeric_limits<uint64_t>::max();
    doc["a_sfixed64"] = std::numeric_limits<int64_t>::max();
    doc["a_double"] = std::numeric_limits<double>::max();
    doc["a_fixed32"] = std::numeric_limits<uint32_t>::max();
    doc["a_sfixed32"] = std::numeric_limits<int32_t>::max();
    doc["a_float"] = std::numeric_limits<float>::max();
    doc["a_nested_enum"] = 2; // NE_C enum value
}

void set_average(JsonDocument& doc)
{
    // Set all fields to average positive values
    doc["a_int32"] = std::numeric_limits<int32_t>::max() / 2;
    doc["a_int64"] = std::numeric_limits<int64_t>::max() / 2;
    doc["a_uint32"] = std::numeric_limits<uint32_t>::max() / 2;
    doc["a_uint64"] = std::numeric_limits<uint64_t>::max() / 2;
    doc["a_sint32"] = std::numeric_limits<int32_t>::max() / 2;
    doc["a_sint64"] = std::numeric_limits<int64_t>::max() / 2;
    doc["a_bool"] = true;
    doc["a_enum"] = 127; // ONEHUNDERTTWENTYSEVEN enum value
    doc["a_fixed64"] = std::numeric_limits<uint64_t>::max() / 2;
    doc["a_sfixed64"] = std::numeric_limits<int64_t>::max() / 2;
    doc["a_double"] = 1234.5678;
    doc["a_fixed32"] = std::numeric_limits<uint32_t>::max() / 2;
    doc["a_sfixed32"] = std::numeric_limits<int32_t>::max() / 2;
    doc["a_float"] = 123.4567f;
    doc["a_nested_enum"] = 1; // NE_B enum value
}

} // End of namespace HelperSimpleTypesArduinoJson

#endif // HELPER_SIMPLE_TYPES_ARDUINOJSON
