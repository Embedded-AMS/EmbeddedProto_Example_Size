/*
 * eams.h
 *
 *  Created on: Nov 23, 2021
 *      Author: bart
 */

#ifndef INC_EAMS_HPP_
#define INC_EAMS_HPP_

#include "../../../../generated/EAMS/simple_types.h"
#include "../../../../generated/EAMS/message.h"
#include "../../../../EmbeddedProto/src/WriteBufferFixedSize.h"
#include "../../../../EmbeddedProto/src/ReadBufferFixedSize.h"
#include "cycle_counter.h"
#include <string>
#include <string.h>

constexpr uint8_t N = 100;

class test_float {
  public:
    float value_;
};

class test_double {
  public:
    double value_;
};

/*
 *
    void size() const
    {
      // In case this does not build because the variable is private change the Field class in EmbeddedProto field.h

      size_t total = 0;
      const size_t size_a_int32       = sizeof(a_int32_);        total += size_a_int32      ;
      const size_t size_a_int64       = sizeof(a_int64_);        total += size_a_int64      ;
      const size_t size_a_uint32      = sizeof(a_uint32_);       total += size_a_uint32     ;
      const size_t size_a_uint64      = sizeof(a_uint64_);       total += size_a_uint64     ;
      const size_t size_a_sint32      = sizeof(a_sint32_);       total += size_a_sint32     ;
      const size_t size_a_sint64      = sizeof(a_sint64_);       total += size_a_sint64     ;
      const size_t size_a_bool        = sizeof(a_bool_);         total += size_a_bool       ;
      const size_t size_a_enum        = sizeof(a_enum_);         total += size_a_enum       ;
      const size_t size_a_fixed64     = sizeof(a_fixed64_);      total += size_a_fixed64    ;
      const size_t size_a_sfixed64    = sizeof(a_sfixed64_);     total += size_a_sfixed64   ;
      const size_t size_a_double      = sizeof(a_double_);       total += size_a_double     ;
      const size_t size_a_fixed32     = sizeof(a_fixed32_);      total += size_a_fixed32    ;
      const size_t size_a_sfixed32    = sizeof(a_sfixed32_);     total += size_a_sfixed32   ;
      const size_t size_a_float       = sizeof(a_float_);        total += size_a_float      ;
      const size_t size_a_nested_enum = sizeof(a_nested_enum_);  total += size_a_nested_enum;


      const size_t size_msg = sizeof(Test_Simple_Types);
    }
 */
/*
void test_field_sizes()
{
  // In case this does not build because the variable is private change the Field class in EmbeddedProto field.h

  size_t total = 0;
  const size_t size_a_int32       = sizeof(EmbeddedProto::int32::value_);        total += size_a_int32      ;
  const size_t size_a_int64       = sizeof(EmbeddedProto::int64::value_);        total += size_a_int64      ;
  const size_t size_a_uint32      = sizeof(EmbeddedProto::uint32::value_);       total += size_a_uint32     ;
  const size_t size_a_uint64      = sizeof(EmbeddedProto::uint64::value_);       total += size_a_uint64     ;
  const size_t size_a_sint32      = sizeof(EmbeddedProto::sint32::value_);       total += size_a_sint32     ;
  const size_t size_a_sint64      = sizeof(EmbeddedProto::sint64::value_);       total += size_a_sint64     ;
  const size_t size_a_bool        = sizeof(EmbeddedProto::boolean::value_);      total += size_a_bool       ;
  const size_t size_a_fixed64     = sizeof(EmbeddedProto::fixed64::value_);      total += size_a_fixed64    ;
  const size_t size_a_sfixed64    = sizeof(EmbeddedProto::sfixed64::value_);     total += size_a_sfixed64   ;
  const size_t size_a_double      = sizeof(EmbeddedProto::doublefixed::value_);  total += size_a_double     ;
  const size_t size_a_fixed32     = sizeof(EmbeddedProto::fixed32::value_);      total += size_a_fixed32    ;
  const size_t size_a_sfixed32    = sizeof(EmbeddedProto::sfixed32::value_);     total += size_a_sfixed32   ;
  const size_t size_a_float       = sizeof(EmbeddedProto::floatfixed::value_);   total += size_a_float      ;

  const size_t size_class_float = sizeof(test_float);
  const size_t size_class_double = sizeof(test_double);

  int dummy = 0;
  dummy += 2;
}
*/
Test_Simple_Types<10,10> msg;

/**
 * @brief Test EmbeddedProto serialization/deserialization with cycle measurement
 * 
 * This function measures CPU cycles for serialize and deserialize operations
 * and outputs the results via UART.
 */
void test_eams()
{
  // Set up test data
  msg.set_a_int32(1);
  msg.set_a_int64(1);
  msg.set_a_uint32(1);
  msg.set_a_uint64(1);
  msg.set_a_sint32(1);
  msg.set_a_sint64(1);
  msg.set_a_bool(true);
  msg.set_a_enum(Test_Enum::TWOHUNDERDFIFTYFIVE);
  msg.set_a_fixed64(1.0);
  msg.set_a_sfixed64(1.0);
  msg.set_a_double(1.0);
  msg.set_a_fixed32(1.0);
  msg.set_a_sfixed32(1.0);
  msg.set_a_float(1.0);

  // Prepare buffers
  ::EmbeddedProto::WriteBufferFixedSize<N> wbuffer;
  ::EmbeddedProto::ReadBufferFixedSize<N> rbuffer;
  
  // Measure serialize cycles
  uint32_t start_serialize = CycleCounter_Start();
  msg.serialize(wbuffer);
  uint32_t serialize_cycles = CycleCounter_Elapsed(start_serialize);

  // Copy serialized data to read buffer for deserialization
  // (In real use, this would come from a communication channel)
  const uint8_t* data = wbuffer.get_data();
  uint32_t size = wbuffer.get_size();
  for (uint32_t i = 0; i < size; i++) {
    rbuffer.push(data[i]);
  }

  // Clear message before deserialize to ensure we're actually reading
  msg.clear();

  // Measure deserialize cycles
  uint32_t start_deserialize = CycleCounter_Start();
  msg.deserialize(rbuffer);
  uint32_t deserialize_cycles = CycleCounter_Elapsed(start_deserialize);

  // Output results via UART
  CycleMeasurement measurement = {
    .serialize_cycles = serialize_cycles,
    .deserialize_cycles = deserialize_cycles,
    .library = "EmbeddedProto",
    .scenario = "simple_types"
  };
  CycleCounter_OutputResults(&measurement);
}

Test_Simple_Types<10,10> msgArray[100];

/**
 * @brief Test EmbeddedProto array serialization/deserialization with cycle measurement
 * 
 * This function measures CPU cycles for serialize and deserialize operations
 * on an array of messages and outputs the average results via UART.
 */
void test_eams_array()
{
  uint32_t total_serialize_cycles = 0;
  uint32_t total_deserialize_cycles = 0;
  const uint32_t num_iterations = 100;

  for(auto& m : msgArray) {
    // Set up test data
    m.set_a_int32(1);
    m.set_a_int64(1);
    m.set_a_uint32(1);
    m.set_a_uint64(1);
    m.set_a_sint32(1);
    m.set_a_sint64(1);
    m.set_a_bool(true);
    m.set_a_enum(Test_Enum::TWOHUNDERDFIFTYFIVE);
    m.set_a_fixed64(1.0);
    m.set_a_sfixed64(1.0);
    m.set_a_double(1.0);
    m.set_a_fixed32(1.0);
    m.set_a_sfixed32(1.0);
    m.set_a_float(1.0);

    // Prepare buffers
    ::EmbeddedProto::WriteBufferFixedSize<N> wbuffer;
    ::EmbeddedProto::ReadBufferFixedSize<N> rbuffer;

    // Measure serialize cycles
    uint32_t start_serialize = CycleCounter_Start();
    m.serialize(wbuffer);
    total_serialize_cycles += CycleCounter_Elapsed(start_serialize);

    // Copy serialized data to read buffer
    const uint8_t* data = wbuffer.get_data();
    uint32_t size = wbuffer.get_size();
    for (uint32_t i = 0; i < size; i++) {
      rbuffer.push(data[i]);
    }

    // Clear message before deserialize
    m.clear();

    // Measure deserialize cycles
    uint32_t start_deserialize = CycleCounter_Start();
    m.deserialize(rbuffer);
    total_deserialize_cycles += CycleCounter_Elapsed(start_deserialize);
  }

  // Output average results via UART
  CycleMeasurement measurement = {
    .serialize_cycles = total_serialize_cycles / num_iterations,
    .deserialize_cycles = total_deserialize_cycles / num_iterations,
    .library = "EmbeddedProto",
    .scenario = "simple_types_array_avg"
  };
  CycleCounter_OutputResults(&measurement);
}

// ============================================================================
// Nested Message Scenario Test Functions
// ============================================================================

// Template parameters for nested_message scenario
constexpr uint32_t INNER_NAME_SIZE = 16;
constexpr uint32_t MIDDLE_VALUES_SIZE = 5;
constexpr uint32_t OUTER_LABEL_SIZE = 32;
constexpr uint32_t OUTER_ITEMS_SIZE = 3;

// Type aliases for nested_message
// OuterMessage template parameters:
// - OuterMessage_label_LENGTH
// - OuterMessage_middle_MiddleMessage_inner_InnerMessage_name_LENGTH
// - OuterMessage_middle_MiddleMessage_values_REP_LENGTH
// - OuterMessage_items_REP_LENGTH
// - OuterMessage_items_MiddleMessage_inner_InnerMessage_name_LENGTH
// - OuterMessage_items_MiddleMessage_values_REP_LENGTH
using InnerMsg = nested_message::InnerMessage<INNER_NAME_SIZE>;
using MiddleMsg = nested_message::MiddleMessage<INNER_NAME_SIZE, MIDDLE_VALUES_SIZE>;
using OuterMsg = nested_message::OuterMessage<
    OUTER_LABEL_SIZE,           // label length
    INNER_NAME_SIZE,            // middle.inner.name length
    MIDDLE_VALUES_SIZE,         // middle.values count
    OUTER_ITEMS_SIZE,           // items count
    INNER_NAME_SIZE,            // items[].inner.name length
    MIDDLE_VALUES_SIZE          // items[].values count
>;

// Nested message instance
OuterMsg nested_msg;

/**
 * @brief Populate inner message with test data
 */
static inline void populate_inner(InnerMsg& inner, int32_t value, const char* name)
{
    inner.set_value(value);
    inner.mutable_name().set(name, strlen(name));
}

/**
 * @brief Populate middle message with test data
 */
static inline void populate_middle(MiddleMsg& middle, float x, float y, float z, 
                                   int32_t inner_value, const char* inner_name,
                                   const int32_t* values, size_t values_count)
{
    middle.set_x(x);
    middle.set_y(y);
    middle.set_z(z);
    populate_inner(middle.mutable_inner(), inner_value, inner_name);
    
    for (size_t i = 0; i < values_count; i++) {
        middle.add_values(values[i]);
    }
}

/**
 * @brief Populate nested message with test data (must match test_data.json)
 */
static inline void populate_nested_message(OuterMsg& m)
{
    m.set_id(42);
    m.mutable_label().set("Test Outer Message", 18);
    
    // Middle message
    const int32_t middle_values[] = {10, 20, 30, 40, 50};
    populate_middle(m.mutable_middle(), 1.5f, 2.5f, 3.5f, 100, "Inner1", 
                   middle_values, 5);
    
    // Items (repeated MiddleMessage)
    // Create item1 and add to repeated field
    MiddleMsg item1;
    const int32_t item1_values[] = {1, 2, 3};
    populate_middle(item1, 4.0f, 5.0f, 6.0f, 200, "Inner2", item1_values, 3);
    m.add_items(item1);
    
    // Create item2 and add to repeated field
    MiddleMsg item2;
    const int32_t item2_values[] = {4, 5};
    populate_middle(item2, 7.0f, 8.0f, 9.0f, 300, "Inner3", item2_values, 2);
    m.add_items(item2);
}

/**
 * @brief Test EmbeddedProto nested message serialization/deserialization with cycle measurement
 */
void test_eams_nested()
{
    // Set up test data
    populate_nested_message(nested_msg);

    // Prepare buffers (larger for nested message)
    constexpr uint32_t NESTED_BUFFER_SIZE = 256;
    ::EmbeddedProto::WriteBufferFixedSize<NESTED_BUFFER_SIZE> wbuffer;
    ::EmbeddedProto::ReadBufferFixedSize<NESTED_BUFFER_SIZE> rbuffer;
    
    // Measure serialize cycles
    uint32_t start_serialize = CycleCounter_Start();
    nested_msg.serialize(wbuffer);
    uint32_t serialize_cycles = CycleCounter_Elapsed(start_serialize);

    // Copy serialized data to read buffer for deserialization
    const uint8_t* data = wbuffer.get_data();
    uint32_t size = wbuffer.get_size();
    for (uint32_t i = 0; i < size; i++) {
        rbuffer.push(data[i]);
    }

    // Clear message before deserialize to ensure we're actually reading
    nested_msg.clear();

    // Measure deserialize cycles
    uint32_t start_deserialize = CycleCounter_Start();
    nested_msg.deserialize(rbuffer);
    uint32_t deserialize_cycles = CycleCounter_Elapsed(start_deserialize);

    // Output results via UART
    CycleMeasurement measurement = {
        .serialize_cycles = serialize_cycles,
        .deserialize_cycles = deserialize_cycles,
        .library = "EmbeddedProto",
        .scenario = "nested_message"
    };
    CycleCounter_OutputResults(&measurement);
}

#endif /* INC_EAMS_HPP_ */
