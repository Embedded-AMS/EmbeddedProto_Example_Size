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

#include <oneof.h>
#include <simple_types.h>
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
  msg.set_int32_value(12345);
}

void set_int64_field(OneOf_Demo& msg)
{
  // Set the int64 field in the oneof
  msg.set_int64_value(123456789012345);
}

void set_bool_field(OneOf_Demo& msg)
{
  // Set the bool field in the oneof
  msg.set_bool_value(true);
}

void set_float_field(OneOf_Demo& msg)
{
  // Set the float field in the oneof
  msg.set_float_value(3.14159f);
}

void set_simple_types_field(OneOf_Demo& msg)
{
  // Set the Simple_Types field in the oneof
  Simple_Types simple_msg;
  simple_msg.set_a_int32(42);
  simple_msg.set_a_int64(1000000);
  simple_msg.set_a_uint32(7);
  simple_msg.set_a_uint64(14);
  simple_msg.set_a_bool(true);
  simple_msg.set_a_float(2.71828f);
  simple_msg.set_a_double(1.61803);

  msg.set_simple_types_value(simple_msg);
}

bool compare_int32_field(const OneOf_Demo& msg)
{
  return msg.get_int32_value() == 12345;
}

bool compare_int64_field(const OneOf_Demo& msg)
{
  return msg.get_int64_value() == 123456789012345;
}

bool compare_bool_field(const OneOf_Demo& msg)
{
  return msg.get_bool_value() == true;
}

bool compare_float_field(const OneOf_Demo& msg)
{
  return almost_equal(msg.get_float_value(), 3.14159f, FLOAT_EPSILON);
}

bool compare_simple_types_field(const OneOf_Demo& msg)
{
  const Simple_Types& simple_msg = msg.get_simple_types_value();
  bool result = true;
  result &= simple_msg.get_a_int32() == 42;
  result &= simple_msg.get_a_int64() == 1000000;
  result &= simple_msg.get_a_uint32() == 7;
  result &= simple_msg.get_a_uint64() == 14;
  result &= simple_msg.get_a_bool() == true;
  result &= almost_equal(simple_msg.get_a_float(), 2.71828f, FLOAT_EPSILON);
  result &= almost_equal(simple_msg.get_a_double(), 1.61803, DOUBLE_EPSILON);
  return result;
}

} // End of namespace HelperOneOf

#endif // HELPER_ONEOF