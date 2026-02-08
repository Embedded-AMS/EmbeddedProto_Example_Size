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

#ifndef HELPER_SIMPLE_TYPES_ARDUINOJSON
#define HELPER_SIMPLE_TYPES_ARDUINOJSON

#include <ArduinoJson.h>
#include <limits>
#include <cmath>
#include <algorithm>

namespace HelperSimpleTypesArduinoJson
{

// Epsilon values for floating-point comparisons
constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon() * 100;
constexpr double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon() * 100;

// Helper function for floating-point comparison
template<typename T>
bool almost_equal(T a, T b, T epsilon) {
    return std::abs(a - b) <= epsilon;
}

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

bool compare_min(const JsonDocument& doc)
{
    bool result = true;
    result &= doc["a_int32"] == std::numeric_limits<int32_t>::min();
    result &= doc["a_int64"] == std::numeric_limits<int64_t>::min();
    result &= doc["a_uint32"] == std::numeric_limits<uint32_t>::min();
    result &= doc["a_uint64"] == std::numeric_limits<uint64_t>::min();
    result &= doc["a_sint32"] == std::numeric_limits<int32_t>::min();
    result &= doc["a_sint64"] == std::numeric_limits<int64_t>::min();
    result &= doc["a_bool"] == false;
    result &= doc["a_enum"] == 0; // ZERO enum value
    result &= doc["a_fixed64"] == std::numeric_limits<uint64_t>::min();
    result &= doc["a_sfixed64"] == std::numeric_limits<int64_t>::min();
    result &= almost_equal(static_cast<double>(doc["a_double"]), std::numeric_limits<double>::lowest(), DOUBLE_EPSILON);
    result &= doc["a_fixed32"] == std::numeric_limits<uint32_t>::min();
    result &= doc["a_sfixed32"] == std::numeric_limits<int32_t>::min();
    result &= almost_equal(static_cast<float>(doc["a_float"]), std::numeric_limits<float>::lowest(), FLOAT_EPSILON);
    result &= doc["a_nested_enum"] == 0; // NE_A enum value
    return result;
}

bool compare_max(const JsonDocument& doc)
{
    bool result = true;
    result &= doc["a_int32"] == std::numeric_limits<int32_t>::max();
    result &= doc["a_int64"] == std::numeric_limits<int64_t>::max();
    result &= doc["a_uint32"] == std::numeric_limits<uint32_t>::max();
    result &= doc["a_uint64"] == std::numeric_limits<uint64_t>::max();
    result &= doc["a_sint32"] == std::numeric_limits<int32_t>::max();
    result &= doc["a_sint64"] == std::numeric_limits<int64_t>::max();
    result &= doc["a_bool"] == true;
    result &= doc["a_enum"] == 2000000000; // TWOBILLION enum value
    result &= doc["a_fixed64"] == std::numeric_limits<uint64_t>::max();
    result &= doc["a_sfixed64"] == std::numeric_limits<int64_t>::max();
    result &= almost_equal(static_cast<double>(doc["a_double"]), std::numeric_limits<double>::max(), DOUBLE_EPSILON);
    result &= doc["a_fixed32"] == std::numeric_limits<uint32_t>::max();
    result &= doc["a_sfixed32"] == std::numeric_limits<int32_t>::max();
    result &= almost_equal(static_cast<float>(doc["a_float"]), std::numeric_limits<float>::max(), FLOAT_EPSILON);
    result &= doc["a_nested_enum"] == 2; // NE_C enum value
    return result;
}

bool compare_average(const JsonDocument& doc)
{
    bool result = true;
    result &= doc["a_int32"] == std::numeric_limits<int32_t>::max() / 2;
    result &= doc["a_int64"] == std::numeric_limits<int64_t>::max() / 2;
    result &= doc["a_uint32"] == std::numeric_limits<uint32_t>::max() / 2;
    result &= doc["a_uint64"] == std::numeric_limits<uint64_t>::max() / 2;
    result &= doc["a_sint32"] == std::numeric_limits<int32_t>::max() / 2;
    result &= doc["a_sint64"] == std::numeric_limits<int64_t>::max() / 2;
    result &= doc["a_bool"] == true;
    result &= doc["a_enum"] == 127; // ONEHUNDERTTWENTYSEVEN enum value
    result &= doc["a_fixed64"] == std::numeric_limits<uint64_t>::max() / 2;
    result &= doc["a_sfixed64"] == std::numeric_limits<int64_t>::max() / 2;
    result &= almost_equal(static_cast<double>(doc["a_double"]), 1234.5678, DOUBLE_EPSILON);
    result &= doc["a_fixed32"] == std::numeric_limits<uint32_t>::max() / 2;
    result &= doc["a_sfixed32"] == std::numeric_limits<int32_t>::max() / 2;
    result &= almost_equal(static_cast<float>(doc["a_float"]), 123.4567f, FLOAT_EPSILON);
    result &= doc["a_nested_enum"] == 1; // NE_B enum value
    return result;
}

} // End of namespace HelperSimpleTypesArduinoJson

#endif // HELPER_SIMPLE_TYPES_ARDUINOJSON
