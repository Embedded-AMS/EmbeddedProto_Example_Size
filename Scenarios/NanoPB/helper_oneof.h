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

#ifndef HELPER_ONEOF

#include <oneof.pb.h>
#include <simple_types.pb.h>
#include <limits>
#include <cmath>
#include <algorithm>

namespace HelperOneOf
{

// Epsilon values for floating-point comparisons
constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

// Helper function for floating-point comparison
template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    return std::abs(a - b) <= epsilon;
}

void set_int32_field(OneOf_Demo& msg)
{
  // Set the int32 field in the oneof
  msg.data_field.int32_value = 12345;
  msg.which_data_field = 1; // ONEOF_DEMO__DATA_FIELD_INT32_VALUE
}

void set_int64_field(OneOf_Demo& msg)
{
  // Set the int64 field in the oneof
  msg.data_field.int64_value = 123456789012345;
  msg.which_data_field = 2; // ONEOF_DEMO__DATA_FIELD_INT64_VALUE
}

void set_bool_field(OneOf_Demo& msg)
{
  // Set the bool field in the oneof
  msg.data_field.bool_value = true;
  msg.which_data_field = 5; // ONEOF_DEMO__DATA_FIELD_BOOL_VALUE
}

void set_float_field(OneOf_Demo& msg)
{
  // Set the float field in the oneof
  msg.data_field.float_value = 3.14159f;
  msg.which_data_field = 6; // ONEOF_DEMO__DATA_FIELD_FLOAT_VALUE
}

void set_simple_types_field(OneOf_Demo& msg)
{
  // Set the Simple_Types field in the oneof
  Simple_Types simple_msg = Simple_Types_init_default;

  simple_msg.a_int32 = 42;
  simple_msg.a_int64 = 1000000;
  simple_msg.a_uint32 = 7;
  simple_msg.a_uint64 = 14;
  simple_msg.a_bool = true;
  simple_msg.a_float = 2.71828f;
  simple_msg.a_double = 1.61803;

  msg.data_field.simple_types_value = simple_msg;
  msg.which_data_field = 8; // ONEOF_DEMO__DATA_FIELD_SIMPLE_TYPES_VALUE
}

void clear_oneof_fields(OneOf_Demo& msg)
{
  // Clear all oneof fields
  msg.which_data_field = 0; // ONEOF_DEMO__DATA_FIELD_NOT_SET
}

bool compare_int32_field(const OneOf_Demo& msg)
{
  return msg.which_data_field == 1 && // ONEOF_DEMO__DATA_FIELD_INT32_VALUE
         msg.data_field.int32_value == 12345;
}

bool compare_int64_field(const OneOf_Demo& msg)
{
  return msg.which_data_field == 2 && // ONEOF_DEMO__DATA_FIELD_INT64_VALUE
         msg.data_field.int64_value == 123456789012345;
}

bool compare_bool_field(const OneOf_Demo& msg)
{
  return msg.which_data_field == 5 && // ONEOF_DEMO__DATA_FIELD_BOOL_VALUE
         msg.data_field.bool_value == true;
}

bool compare_float_field(const OneOf_Demo& msg)
{
  return msg.which_data_field == 6 && // ONEOF_DEMO__DATA_FIELD_FLOAT_VALUE
         almost_equal(msg.data_field.float_value, 3.14159f, FLOAT_EPSILON);
}

bool compare_simple_types_field(const OneOf_Demo& msg)
{
  if (msg.which_data_field != 8) { // ONEOF_DEMO__DATA_FIELD_SIMPLE_TYPES_VALUE
    return false;
  }

  const Simple_Types& simple_msg = msg.data_field.simple_types_value;
  bool result = true;
  result &= simple_msg.a_int32 == 42;
  result &= simple_msg.a_int64 == 1000000;
  result &= simple_msg.a_uint32 == 7;
  result &= simple_msg.a_uint64 == 14;
  result &= simple_msg.a_bool == true;
  result &= almost_equal(simple_msg.a_float, 2.71828f, FLOAT_EPSILON);
  result &= almost_equal(simple_msg.a_double, 1.61803, DOUBLE_EPSILON);
  return result;
}

} // End of namespace HelperOneOf

#endif // HELPER_ONEOF