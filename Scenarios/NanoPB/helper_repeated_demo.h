#ifndef HELPER_REPEATED_DEMO_NANOPB
#define HELPER_REPEATED_DEMO_NANOPB

#include "repeated_demo.pb.h"
#include "simple_types.pb.h"
#include <pb.h>
#include <limits>
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace HelperRepeatedDemoNanoPB
{

// Epsilon values for floating-point comparisons
constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

// Helper function for floating-point comparison
template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    return std::abs(a - b) <= epsilon;
}

// Set average values for repeated integers
void set_average_repeated_integers(Repeated_Integers& msg)
{
  // Clear existing values
  msg.values_count = 0;

  // Add 10 average integer values
  for (int i = 0; i < 10; i++) {
    // Use values around the middle of int32 range
    int32_t value = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
    msg.values[i] = value;
    msg.values_count++;
  }
}

// Compare average values for repeated integers
bool compare_average_repeated_integers(const Repeated_Integers& msg)
{
  if (msg.values_count != 10) {
    return false;
  }

  for (size_t i = 0; i < 10; i++) {
    int32_t expected = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
    if (msg.values[i] != expected) {
      return false;
    }
  }

  return true;
}

// Set average values for repeated messages
void set_average_repeated_messages(Repeated_Messages& msg)
{
  // Clear existing items
  msg.items_count = 0;

  // Add 5 Simple_Types messages with average values
  for (int i = 0; i < 5; i++) {
    // Copy the default item to the array
    msg.items[i] = Simple_Types_init_default;

    // Set average values similar to HelperSimpleTypesNanoPB::set_average
    msg.items[i].a_int32 = std::numeric_limits<int32_t>::max() / 2;
    msg.items[i].a_int64 = std::numeric_limits<int64_t>::max() / 2;
    msg.items[i].a_uint32 = std::numeric_limits<uint32_t>::max() / 2;
    msg.items[i].a_uint64 = std::numeric_limits<uint64_t>::max() / 2;
    msg.items[i].a_sint32 = std::numeric_limits<int32_t>::max() / 2;
    msg.items[i].a_sint64 = std::numeric_limits<int64_t>::max() / 2;
    msg.items[i].a_bool = true;
    msg.items[i].a_enum = Test_Enum_ONEHUNDERTTWENTYSEVEN;
    msg.items[i].a_fixed64 = std::numeric_limits<uint64_t>::max() / 2;
    msg.items[i].a_sfixed64 = std::numeric_limits<int64_t>::max() / 2;
    msg.items[i].a_double = 1234.5678;
    msg.items[i].a_fixed32 = std::numeric_limits<uint32_t>::max() / 2;
    msg.items[i].a_sfixed32 = std::numeric_limits<int32_t>::max() / 2;
    msg.items[i].a_float = 123.4567f;
    msg.items[i].a_nested_enum = Simple_Types_Nested_Enum_NE_B;

    msg.items_count++;
  }
}

// Compare average values for repeated messages
bool compare_average_repeated_messages(const Repeated_Messages& msg)
{
  if (msg.items_count != 5) {
    return false;
  }

  for (size_t i = 0; i < 5; i++) {
    const Simple_Types& item = msg.items[i];

    bool result = true;
    result &= item.a_int32 == std::numeric_limits<int32_t>::max() / 2;
    result &= item.a_int64 == std::numeric_limits<int64_t>::max() / 2;
    result &= item.a_uint32 == std::numeric_limits<uint32_t>::max() / 2;
    result &= item.a_uint64 == std::numeric_limits<uint64_t>::max() / 2;
    result &= item.a_sint32 == std::numeric_limits<int32_t>::max() / 2;
    result &= item.a_sint64 == std::numeric_limits<int64_t>::max() / 2;
    result &= item.a_bool == true;
    result &= item.a_enum == Test_Enum_ONEHUNDERTTWENTYSEVEN;
    result &= item.a_fixed64 == std::numeric_limits<uint64_t>::max() / 2;
    result &= item.a_sfixed64 == std::numeric_limits<int64_t>::max() / 2;
    result &= almost_equal(item.a_double, 1234.5678, DOUBLE_EPSILON);
    result &= item.a_fixed32 == std::numeric_limits<uint32_t>::max() / 2;
    result &= item.a_sfixed32 == std::numeric_limits<int32_t>::max() / 2;
    result &= almost_equal(item.a_float, 123.4567f, FLOAT_EPSILON);
    result &= item.a_nested_enum == Simple_Types_Nested_Enum_NE_B;

    if (!result) {
      return false;
    }
  }

  return true;
}

} // End of namespace HelperRepeatedDemoNanoPB

#endif // HELPER_REPEATED_DEMO_NANOPB