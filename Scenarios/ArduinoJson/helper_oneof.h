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

#include <ArduinoJson.h>
#include <cmath>

namespace HelperOneOf
{

// Epsilon values for floating-point comparisons
constexpr float FLOAT_EPSILON = 1e-6f;
constexpr double DOUBLE_EPSILON = 1e-10;

// Helper function for floating-point comparison
bool almost_equal_float(float a, float b) {
    return std::abs(a - b) <= FLOAT_EPSILON;
}

bool almost_equal_double(double a, double b) {
    return std::abs(a - b) <= DOUBLE_EPSILON;
}

void set_int32_field(JsonDocument& doc)
{
  // Set the int32 field in the oneof
  doc["int32_value"] = 12345;
  doc["data_field"] = "int32_value";
}

void set_int64_field(JsonDocument& doc)
{
  // Set the int64 field in the oneof
  doc["int64_value"] = 123456789012345LL;
  doc["data_field"] = "int64_value";
}

void set_bool_field(JsonDocument& doc)
{
  // Set the bool field in the oneof
  doc["bool_value"] = true;
  doc["data_field"] = "bool_value";
}

void set_float_field(JsonDocument& doc)
{
  // Set the float field in the oneof
  doc["float_value"] = 3.14159f;
  doc["data_field"] = "float_value";
}

void set_simple_types_field(JsonDocument& doc)
{
  // For ArduinoJson, we'll use a simplified approach for the nested object
  // Create a nested object directly
  doc["simple_types_value"]["a_int32"] = 42;
  doc["simple_types_value"]["a_int64"] = 1000000;
  doc["simple_types_value"]["a_uint32"] = 7;
  doc["simple_types_value"]["a_uint64"] = 14;
  doc["simple_types_value"]["a_bool"] = true;
  doc["simple_types_value"]["a_float"] = 2.71828f;
  doc["simple_types_value"]["a_double"] = 1.61803;

  doc["data_field"] = "simple_types_value";
}

bool compare_int32_field(const JsonDocument& doc)
{
  return doc["data_field"] == "int32_value" &&
         doc["int32_value"] == 12345;
}

bool compare_int64_field(const JsonDocument& doc)
{
  return doc["data_field"] == "int64_value" &&
         doc["int64_value"] == 123456789012345LL;
}

bool compare_bool_field(const JsonDocument& doc)
{
  return doc["data_field"] == "bool_value" &&
         doc["bool_value"] == true;
}

bool compare_float_field(const JsonDocument& doc)
{
  return doc["data_field"] == "float_value" &&
         almost_equal_float(doc["float_value"], 3.14159f);
}

void clear_oneof_fields(JsonDocument& doc)
{
  // Clear all oneof fields
  doc["data_field"] = "NOT_SET";
}

bool compare_simple_types_field(const JsonDocument& doc)
{
  if (doc["data_field"] != "simple_types_value") {
    return false;
  }

  // Access the nested object directly
  bool result = true;
  result &= doc["simple_types_value"]["a_int32"] == 42;
  result &= doc["simple_types_value"]["a_int64"] == 1000000;
  result &= doc["simple_types_value"]["a_uint32"] == 7;
  result &= doc["simple_types_value"]["a_uint64"] == 14;
  result &= doc["simple_types_value"]["a_bool"] == true;
  result &= almost_equal_float(static_cast<float>(doc["simple_types_value"]["a_float"]), 2.71828f);
  result &= almost_equal_double(static_cast<double>(doc["simple_types_value"]["a_double"]), 1.61803);
  return result;
}


} // End of namespace HelperOneOf

#endif // HELPER_ONEOF