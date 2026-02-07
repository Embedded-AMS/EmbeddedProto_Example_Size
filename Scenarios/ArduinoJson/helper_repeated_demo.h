#ifndef HELPER_REPEATED_DEMO_ARDUINOJSON
#define HELPER_REPEATED_DEMO_ARDUINOJSON

#include <ArduinoJson.h>
#include <limits>
#include <cmath>
#include <algorithm>

namespace HelperRepeatedDemoArduinoJson
{

// Helper function for floating-point comparison
template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    return std::abs(a - b) <= epsilon;
}

// Set average values for repeated integers
void set_average_repeated_integers(JsonDocument& doc)
{
  // Create an array with 10 average integer values
  JsonArray values = doc["values"].to<JsonArray>();

  for (int i = 0; i < 10; i++) {
    // Use values around the middle of int32 range
    int32_t value = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
    values.add(value);
  }
}

// Compare average values for repeated integers
bool compare_average_repeated_integers(JsonDocument& doc)
{
  JsonArray values = doc["values"];
  if (values.isNull() || values.size() != 10) {
    return false;
  }

  for (size_t i = 0; i < 10; i++) {
    int32_t expected = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
    if (values[i] != expected) {
      return false;
    }
  }

  return true;
}

// Set average values for repeated messages
void set_average_repeated_messages(JsonDocument& doc)
{
  // Create an array with 5 Simple_Types messages with average values
  JsonArray items = doc["items"].to<JsonArray>();

  for (int i = 0; i < 5; i++) {
    JsonObject item = items.add<JsonObject>();

    // Set average values similar to HelperSimpleTypesArduinoJson::set_average
    item["a_int32"] = std::numeric_limits<int32_t>::max() / 2;
    item["a_int64"] = std::numeric_limits<int64_t>::max() / 2;
    item["a_uint32"] = std::numeric_limits<uint32_t>::max() / 2;
    item["a_uint64"] = std::numeric_limits<uint64_t>::max() / 2;
    item["a_sint32"] = std::numeric_limits<int32_t>::max() / 2;
    item["a_sint64"] = std::numeric_limits<int64_t>::max() / 2;
    item["a_bool"] = true;
    item["a_enum"] = 127; // ONEHUNDERTTWENTYSEVEN enum value
    item["a_fixed64"] = std::numeric_limits<uint64_t>::max() / 2;
    item["a_sfixed64"] = std::numeric_limits<int64_t>::max() / 2;
    item["a_double"] = 1234.5678;
    item["a_fixed32"] = std::numeric_limits<uint32_t>::max() / 2;
    item["a_sfixed32"] = std::numeric_limits<int32_t>::max() / 2;
    item["a_float"] = 123.4567f;
    item["a_nested_enum"] = 1; // NE_B enum value
  }
}

// Compare average values for repeated messages
bool compare_average_repeated_messages(JsonDocument& doc)
{
  JsonArray items = doc["items"];
  if (items.isNull() || items.size() != 5) {
    return false;
  }

  // Epsilon values for floating-point comparisons
  constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
  constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

  for (size_t i = 0; i < 5; i++) {
    // Access array element directly
    JsonObject item = items[i];

    bool result = true;
    result &= item["a_int32"] == std::numeric_limits<int32_t>::max() / 2;
    result &= item["a_int64"] == std::numeric_limits<int64_t>::max() / 2;
    result &= item["a_uint32"] == std::numeric_limits<uint32_t>::max() / 2;
    result &= item["a_uint64"] == std::numeric_limits<uint64_t>::max() / 2;
    result &= item["a_sint32"] == std::numeric_limits<int32_t>::max() / 2;
    result &= item["a_sint64"] == std::numeric_limits<int64_t>::max() / 2;
    result &= item["a_bool"] == true;
    result &= item["a_enum"] == 127; // ONEHUNDERTTWENTYSEVEN enum value
    result &= item["a_fixed64"] == std::numeric_limits<uint64_t>::max() / 2;
    result &= item["a_sfixed64"] == std::numeric_limits<int64_t>::max() / 2;
    result &= almost_equal(static_cast<double>(item["a_double"]), 1234.5678, DOUBLE_EPSILON);
    result &= item["a_fixed32"] == std::numeric_limits<uint32_t>::max() / 2;
    result &= item["a_sfixed32"] == std::numeric_limits<int32_t>::max() / 2;
    result &= almost_equal(static_cast<float>(item["a_float"]), 123.4567f, FLOAT_EPSILON);
    result &= item["a_nested_enum"] == 1; // NE_B enum value

    if (!result) {
      return false;
    }
  }

  return true;
}

// Set average values for nested repeated messages
void set_average_nested_repeated_messages(JsonDocument& doc)
{
  // Create an array with 10 Repeated_Messages objects, each with 5 Simple_Types messages
  JsonArray nested_rm = doc["nested_rm"].to<JsonArray>();

  for (int i = 0; i < 10; i++) {
    // Create a Repeated_Messages object (which is an array of Simple_Types)
    JsonArray items = nested_rm.add<JsonArray>();

    for (int j = 0; j < 5; j++) {
      // Create a Simple_Types message
      JsonObject item = items.add<JsonObject>();

      // Set average values similar to HelperSimpleTypesArduinoJson::set_average
      item["a_int32"] = std::numeric_limits<int32_t>::max() / 2;
      item["a_int64"] = std::numeric_limits<int64_t>::max() / 2;
      item["a_uint32"] = std::numeric_limits<uint32_t>::max() / 2;
      item["a_uint64"] = std::numeric_limits<uint64_t>::max() / 2;
      item["a_sint32"] = std::numeric_limits<int32_t>::max() / 2;
      item["a_sint64"] = std::numeric_limits<int64_t>::max() / 2;
      item["a_bool"] = true;
      item["a_enum"] = 127; // ONEHUNDERTTWENTYSEVEN enum value
      item["a_fixed64"] = std::numeric_limits<uint64_t>::max() / 2;
      item["a_sfixed64"] = std::numeric_limits<int64_t>::max() / 2;
      item["a_double"] = 1234.5678;
      item["a_fixed32"] = std::numeric_limits<uint32_t>::max() / 2;
      item["a_sfixed32"] = std::numeric_limits<int32_t>::max() / 2;
      item["a_float"] = 123.4567f;
      item["a_nested_enum"] = 1; // NE_B enum value
    }
  }
}

// Compare average values for nested repeated messages
bool compare_average_nested_repeated_messages(JsonDocument& doc)
{
  JsonArray nested_rm = doc["nested_rm"];
  if (nested_rm.isNull() || nested_rm.size() != 10) {
    return false;
  }

  // Epsilon values for floating-point comparisons
  constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
  constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

  for (size_t i = 0; i < 10; i++) {
    // Each nested_rm element is an array of Simple_Types (Repeated_Messages)
    JsonArray items = nested_rm[i];
    if (items.isNull() || items.size() != 5) {
      return false;
    }

    for (size_t j = 0; j < 5; j++) {
      // Each items element is a Simple_Types object
      JsonObject item = items[j];

      bool result = true;
      result &= item["a_int32"] == std::numeric_limits<int32_t>::max() / 2;
      result &= item["a_int64"] == std::numeric_limits<int64_t>::max() / 2;
      result &= item["a_uint32"] == std::numeric_limits<uint32_t>::max() / 2;
      result &= item["a_uint64"] == std::numeric_limits<uint64_t>::max() / 2;
      result &= item["a_sint32"] == std::numeric_limits<int32_t>::max() / 2;
      result &= item["a_sint64"] == std::numeric_limits<int64_t>::max() / 2;
      result &= item["a_bool"] == true;
      result &= item["a_enum"] == 127; // ONEHUNDERTTWENTYSEVEN enum value
      result &= item["a_fixed64"] == std::numeric_limits<uint64_t>::max() / 2;
      result &= item["a_sfixed64"] == std::numeric_limits<int64_t>::max() / 2;
      result &= almost_equal(static_cast<double>(item["a_double"]), 1234.5678, DOUBLE_EPSILON);
      result &= item["a_fixed32"] == std::numeric_limits<uint32_t>::max() / 2;
      result &= item["a_sfixed32"] == std::numeric_limits<int32_t>::max() / 2;
      result &= almost_equal(static_cast<float>(item["a_float"]), 123.4567f, FLOAT_EPSILON);
      result &= item["a_nested_enum"] == 1; // NE_B enum value

      if (!result) {
        return false;
      }
    }
  }

  return true;
}

} // End of namespace HelperRepeatedDemoArduinoJson

#endif // HELPER_REPEATED_DEMO_ARDUINOJSON
