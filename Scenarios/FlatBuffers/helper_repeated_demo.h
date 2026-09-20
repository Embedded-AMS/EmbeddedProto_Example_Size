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

#ifndef HELPER_REPEATED_DEMO
#define HELPER_REPEATED_DEMO

#include <repeated_demo_builder.h>
#include <repeated_demo_reader.h>
#include <FlatBuffers/helper_simple_types.h>
#include <limits>

// The same values and counts as the Embedded Proto and nanopb helpers.
namespace HelperRepeatedDemoFlatBuffers
{

constexpr uint32_t MAX_INTEGERS = 10;

inline void set_average_repeated_integers(flatcc_builder_t* B, const uint32_t length)
{
  // Fill an array and hand it over in one call, as the other libraries fill their array field.
  int32_t values[MAX_INTEGERS];
  const uint32_t count = (length < MAX_INTEGERS) ? length : MAX_INTEGERS;
  for (uint32_t i = 0; i < count; i++) {
    values[i] = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
  }
  Repeated_Integers_values_create(B, values, count);
}

inline bool compare_average_repeated_integers(Repeated_Integers_table_t msg, const uint32_t length)
{
  flatbuffers_int32_vec_t values = Repeated_Integers_values(msg);
  if (flatbuffers_int32_vec_len(values) != length) {
    return false;
  }
  for (uint32_t i = 0; i < length; i++) {
    int32_t expected = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
    if (flatbuffers_int32_vec_at(values, i) != expected) {
      return false;
    }
  }
  return true;
}

// Adds the items vector to the Repeated_Messages table the builder has open.
inline void set_average_repeated_messages(flatcc_builder_t* B, const uint32_t length)
{
  Repeated_Messages_items_start(B);
  for (uint32_t i = 0; i < length; i++) {
    Repeated_Messages_items_push_create(B, SIMPLE_TYPES_AVERAGE_ARGS);
  }
  Repeated_Messages_items_end(B);
}

inline bool compare_average_repeated_messages(Repeated_Messages_table_t msg, const uint32_t length)
{
  Simple_Types_vec_t items = Repeated_Messages_items(msg);
  if (Simple_Types_vec_len(items) != length) {
    return false;
  }
  for (uint32_t i = 0; i < length; i++) {
    if (!HelperSimpleTypesFlatBuffers::compare_average(Simple_Types_vec_at(items, i))) {
      return false;
    }
  }
  return true;
}

inline void set_average_nested_repeated_messages(flatcc_builder_t* B, const uint32_t outer_length, const uint32_t inner_length)
{
  Nested_Repeated_Messages_nested_rm_start(B);
  for (uint32_t i = 0; i < outer_length; i++) {
    Nested_Repeated_Messages_nested_rm_push_start(B);
    set_average_repeated_messages(B, inner_length);
    Nested_Repeated_Messages_nested_rm_push_end(B);
  }
  Nested_Repeated_Messages_nested_rm_end(B);
}

inline bool compare_average_nested_repeated_messages(Nested_Repeated_Messages_table_t msg, const uint32_t outer_length, const uint32_t inner_length)
{
  Repeated_Messages_vec_t nested = Nested_Repeated_Messages_nested_rm(msg);
  if (Repeated_Messages_vec_len(nested) != outer_length) {
    return false;
  }
  for (uint32_t i = 0; i < outer_length; i++) {
    if (!compare_average_repeated_messages(Repeated_Messages_vec_at(nested, i), inner_length)) {
      return false;
    }
  }
  return true;
}

} // End of namespace HelperRepeatedDemoFlatBuffers

#endif // HELPER_REPEATED_DEMO
