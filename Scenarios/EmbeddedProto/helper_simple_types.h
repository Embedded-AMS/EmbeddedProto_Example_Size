

#ifndef HELPER_SIMPLE_TYPES

#include <simple_types.h>
#include <limits>

namespace HelperSimpleTypes
{

void set_min(Test_Simple_Types& msg)
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
  msg.set_a_nested_enum(Test_Simple_Types::Nested_Enum::NE_A);
}

void set_max(Test_Simple_Types& msg)
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
  msg.set_a_nested_enum(Test_Simple_Types::Nested_Enum::NE_C);
}

void set_average(Test_Simple_Types& msg)
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
  msg.set_a_nested_enum(Test_Simple_Types::Nested_Enum::NE_B);
}

} // End of namespace HelperSimpleTypes

#endif // HELPER_SIMPLE_TYPES
