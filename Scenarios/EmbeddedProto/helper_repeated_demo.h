#ifndef HELPER_REPEATED_DEMO
#define HELPER_REPEATED_DEMO

#include <repeated_demo.h>
#include <simple_types.h>
#include <limits>
#include <cmath>
#include <algorithm>

namespace HelperRepeatedDemo
{

// Helper function for floating-point comparison
template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    return std::abs(a - b) <= epsilon;
}

// Set average values for repeated integers
template<uint32_t LENGTH>
void set_average_repeated_integers(Repeated_Integers<LENGTH>& msg)
{
  // Add LENGTH average integer values
  for (uint32_t i = 0; i < LENGTH; i++) {
    // Use values around the middle of int32 range
    int32_t value = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
    msg.add_values(value);
  }
}

// Compare average values for repeated integers
template<uint32_t LENGTH>
bool compare_average_repeated_integers(const Repeated_Integers<LENGTH>& msg)
{
  if (msg.values().get_length() != LENGTH) {
    return false;
  }

  for (size_t i = 0; i < LENGTH; i++) {
    int32_t expected = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
    if (msg.values().get_const(i) != expected) {
      return false;
    }
  }

  return true;
}

// Set average values for repeated messages
template<uint32_t LENGTH>
void set_average_repeated_messages(Repeated_Messages<LENGTH>& msg)
{
  // Add LENGTH Simple_Types messages with average values
  for (uint32_t i = 0; i < LENGTH; i++) {
    Simple_Types item;
    // Set average values similar to HelperSimpleTypes::set_average
    item.set_a_int32(std::numeric_limits<int32_t>::max() / 2);
    item.set_a_int64(std::numeric_limits<int64_t>::max() / 2);
    item.set_a_uint32(std::numeric_limits<uint32_t>::max() / 2);
    item.set_a_uint64(std::numeric_limits<uint64_t>::max() / 2);
    item.set_a_sint32(std::numeric_limits<int32_t>::max() / 2);
    item.set_a_sint64(std::numeric_limits<int64_t>::max() / 2);
    item.set_a_bool(true);
    item.set_a_enum(Test_Enum::ONEHUNDERTTWENTYSEVEN);
    item.set_a_fixed64(std::numeric_limits<uint64_t>::max() / 2);
    item.set_a_sfixed64(std::numeric_limits<int64_t>::max() / 2);
    item.set_a_double(1234.5678);
    item.set_a_fixed32(std::numeric_limits<uint32_t>::max() / 2);
    item.set_a_sfixed32(std::numeric_limits<int32_t>::max() / 2);
    item.set_a_float(123.4567f);
    item.set_a_nested_enum(Simple_Types::Nested_Enum::NE_B);

    msg.add_items(item);
  }
}

// Compare average values for repeated messages
template<uint32_t LENGTH>
bool compare_average_repeated_messages(const Repeated_Messages<LENGTH>& msg)
{
  if (msg.items().get_length() != LENGTH) {
    return false;
  }

  // Epsilon values for floating-point comparisons
  constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
  constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

  for (size_t i = 0; i < LENGTH; i++) {
    const Simple_Types& item = msg.items().get_const(i);

    bool result = true;
    result &= item.get_a_int32() == std::numeric_limits<int32_t>::max() / 2;
    result &= item.get_a_int64() == std::numeric_limits<int64_t>::max() / 2;
    result &= item.get_a_uint32() == std::numeric_limits<uint32_t>::max() / 2;
    result &= item.get_a_uint64() == std::numeric_limits<uint64_t>::max() / 2;
    result &= item.get_a_sint32() == std::numeric_limits<int32_t>::max() / 2;
    result &= item.get_a_sint64() == std::numeric_limits<int64_t>::max() / 2;
    result &= item.get_a_bool() == true;
    result &= item.get_a_enum() == Test_Enum::ONEHUNDERTTWENTYSEVEN;
    result &= item.get_a_fixed64() == std::numeric_limits<uint64_t>::max() / 2;
    result &= item.get_a_sfixed64() == std::numeric_limits<int64_t>::max() / 2;
    result &= almost_equal(item.get_a_double(), 1234.5678, DOUBLE_EPSILON);
    result &= item.get_a_fixed32() == std::numeric_limits<uint32_t>::max() / 2;
    result &= item.get_a_sfixed32() == std::numeric_limits<int32_t>::max() / 2;
    result &= almost_equal(item.get_a_float(), 123.4567f, FLOAT_EPSILON);
    result &= item.get_a_nested_enum() == Simple_Types::Nested_Enum::NE_B;

    if (!result) {
      return false;
    }
  }

  return true;
}

} // End of namespace HelperRepeatedDemo

#endif // HELPER_REPEATED_DEMO