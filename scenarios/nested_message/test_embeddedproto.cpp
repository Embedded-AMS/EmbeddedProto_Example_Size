/*
 * Test code for EmbeddedProto - nested_message scenario
 * 
 * This file tests serialization and deserialization of nested messages
 * with 2-3 levels of nesting and repeated fields.
 */

#include "message.h"
#include "WriteBufferFixedSize.h"
#include "ReadBufferFixedSize.h"
#include <string.h>

// Buffer size for serialization
constexpr uint32_t BUFFER_SIZE = 256;

// Template parameters for EmbeddedProto
constexpr uint32_t INNER_NAME_SIZE = 16;
constexpr uint32_t MIDDLE_VALUES_SIZE = 5;
constexpr uint32_t OUTER_LABEL_SIZE = 32;
constexpr uint32_t OUTER_ITEMS_SIZE = 3;

// Type aliases for readability
using InnerMsg = nested_message::InnerMessage<INNER_NAME_SIZE>;
using MiddleMsg = nested_message::MiddleMessage<INNER_NAME_SIZE, MIDDLE_VALUES_SIZE>;
using OuterMsg = nested_message::OuterMessage<OUTER_LABEL_SIZE, INNER_NAME_SIZE, MIDDLE_VALUES_SIZE, OUTER_ITEMS_SIZE>;

// Message instance
OuterMsg msg;

/**
 * @brief Populate inner message with test data
 */
void populate_inner(InnerMsg& inner, int32_t value, const char* name)
{
    inner.set_value(value);
    inner.mutable_name().set(name, strlen(name));
}

/**
 * @brief Populate middle message with test data
 */
void populate_middle(MiddleMsg& middle, float x, float y, float z, 
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
 * @brief Populate message with test data (must match test_data.json)
 */
void populate_message(OuterMsg& m)
{
    m.set_id(42);
    m.mutable_label().set("Test Outer Message", 18);
    
    // Middle message
    const int32_t middle_values[] = {10, 20, 30, 40, 50};
    populate_middle(m.mutable_middle(), 1.5f, 2.5f, 3.5f, 100, "Inner1", 
                   middle_values, 5);
    
    // Items (repeated MiddleMessage)
    const int32_t item1_values[] = {1, 2, 3};
    auto& item1 = m.add_items();
    populate_middle(item1, 4.0f, 5.0f, 6.0f, 200, "Inner2", item1_values, 3);
    
    const int32_t item2_values[] = {4, 5};
    auto& item2 = m.add_items();
    populate_middle(item2, 7.0f, 8.0f, 9.0f, 300, "Inner3", item2_values, 2);
}

/**
 * @brief Test EmbeddedProto serialization
 * @return Number of bytes serialized, or 0 on error
 */
uint32_t test_serialize()
{
    populate_message(msg);
    
    ::EmbeddedProto::WriteBufferFixedSize<BUFFER_SIZE> buffer;
    auto result = msg.serialize(buffer);
    
    if (::EmbeddedProto::Error::NO_ERRORS != result) {
        return 0;
    }
    
    return buffer.get_size();
}

/**
 * @brief Test EmbeddedProto deserialization
 * @param data Pointer to serialized data
 * @param size Size of serialized data
 * @return true on success, false on error
 */
bool test_deserialize(const uint8_t* data, uint32_t size)
{
    msg.clear();
    
    ::EmbeddedProto::ReadBufferFixedSize<BUFFER_SIZE> buffer;
    for (uint32_t i = 0; i < size; i++) {
        buffer.push(data[i]);
    }
    
    auto result = msg.deserialize(buffer);
    
    return (::EmbeddedProto::Error::NO_ERRORS == result);
}

/**
 * @brief Get message size in bytes
 */
size_t get_message_size()
{
    return sizeof(OuterMsg);
}
