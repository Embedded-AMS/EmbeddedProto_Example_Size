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

#include <FlatBuffers/helper_flatbuffers.h>
#include <schema_growth_builder.h>
#include <schema_growth_reader.h>
#include <schema_growth_verifier.h>
#include <scenario_values.h>

#include <cstring>

// Round trips the first 5 of the twenty growth message types, one instance each, with one
// builder. The flash difference between the 1, 5 and 20 variants is what a library spends
// per message type in the schema. Every type has its own generated functions, so a macro
// stands in for the template the struct based libraries use.
namespace
{
  using namespace ScenarioValues;

  #define GROWTH_ROUND_TRIP(T, n) \
    { \
      flatcc_builder_reset(&builder); \
      T##_start_as_root(&builder); \
      T##_id_add(&builder, growth_id(n)); \
      T##_value_add(&builder, growth_value(n)); \
      T##_measure_add(&builder, growth_measure(n)); \
      T##_flag_add(&builder, growth_flag(n)); \
      T##_label_create_str(&builder, GROWTH_LABELS[(n) - 1]); \
      T##_end_as_root(&builder); \
      const uint32_t size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer)); \
      memcpy(deserialize_buffer, buffer, size); \
      bool ok = (0 != size) && (T##_verify_as_root(deserialize_buffer, size) == 0); \
      if (ok) { \
        T##_table_t msg = T##_as_root(deserialize_buffer); \
        flatbuffers_string_t label = T##_label(msg); \
        ok = (T##_id(msg) == growth_id(n)) && (T##_value(msg) == growth_value(n)) \
             && (T##_measure(msg) == growth_measure(n)) && (T##_flag(msg) == growth_flag(n)) \
             && (nullptr != label) && (0 == strcmp(label, GROWTH_LABELS[(n) - 1])); \
      } \
      success = success && ok; \
      total += size; \
    }
}

uint32_t run_scenario()
{
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);
  alignas(8) uint8_t buffer[128];
  alignas(8) uint8_t deserialize_buffer[128];
  uint32_t total = 0;
  bool success = true;
  GROWTH_ROUND_TRIP(Growth_01, 1)
  GROWTH_ROUND_TRIP(Growth_02, 2)
  GROWTH_ROUND_TRIP(Growth_03, 3)
  GROWTH_ROUND_TRIP(Growth_04, 4)
  GROWTH_ROUND_TRIP(Growth_05, 5)
  flatcc_builder_clear(&builder);
  return success ? total : 0;
}
