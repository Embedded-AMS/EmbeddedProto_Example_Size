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

#include <cbor_messages_types.h>
#include <Zcbor/helper_simple_types.h>
#include <limits>

// The same values and counts as the Embedded Proto and nanopb helpers.
namespace HelperRepeatedDemoZcbor
{

inline void set_average_repeated_integers(struct Repeated_Integers& msg, const uint32_t length)
{
  msg.values_present = true;
  msg.values.Int_count = length;
  for (uint32_t i = 0; i < length; i++) {
    msg.values.Int[i] = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
  }
}

inline bool compare_average_repeated_integers(const struct Repeated_Integers& msg, const uint32_t length)
{
  if (!msg.values_present || (msg.values.Int_count != length)) {
    return false;
  }
  for (uint32_t i = 0; i < length; i++) {
    int32_t expected = std::numeric_limits<int32_t>::max() / 2 - 1000 + (i * 200);
    if (msg.values.Int[i] != expected) {
      return false;
    }
  }
  return true;
}

inline void set_average_repeated_messages(struct Repeated_Messages& msg, const uint32_t length)
{
  msg.items_present = true;
  msg.items.Simple_Types_m_count = length;
  for (uint32_t i = 0; i < length; i++) {
    HelperSimpleTypesZcbor::set_average(msg.items.Simple_Types_m[i]);
  }
}

inline bool compare_average_repeated_messages(const struct Repeated_Messages& msg, const uint32_t length)
{
  if (!msg.items_present || (msg.items.Simple_Types_m_count != length)) {
    return false;
  }
  for (uint32_t i = 0; i < length; i++) {
    if (!HelperSimpleTypesZcbor::compare_average(msg.items.Simple_Types_m[i])) {
      return false;
    }
  }
  return true;
}

inline void set_average_nested_repeated_messages(struct Nested_Repeated_Messages& msg, const uint32_t outer_length, const uint32_t inner_length)
{
  msg.nested_rm_present = true;
  msg.nested_rm.Repeated_Messages_m_count = outer_length;
  for (uint32_t i = 0; i < outer_length; i++) {
    set_average_repeated_messages(msg.nested_rm.Repeated_Messages_m[i], inner_length);
  }
}

inline bool compare_average_nested_repeated_messages(const struct Nested_Repeated_Messages& msg, const uint32_t outer_length, const uint32_t inner_length)
{
  if (!msg.nested_rm_present || (msg.nested_rm.Repeated_Messages_m_count != outer_length)) {
    return false;
  }
  for (uint32_t i = 0; i < outer_length; i++) {
    if (!compare_average_repeated_messages(msg.nested_rm.Repeated_Messages_m[i], inner_length)) {
      return false;
    }
  }
  return true;
}

} // End of namespace HelperRepeatedDemoZcbor

#endif // HELPER_REPEATED_DEMO
