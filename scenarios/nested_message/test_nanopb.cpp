/*
 * Test code for NanoPB - nested_message scenario
 * 
 * This file tests serialization and deserialization of nested messages
 * with 2-3 levels of nesting and repeated fields.
 */

#include "message.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include <string.h>

// Buffer size for serialization
constexpr uint32_t BUFFER_SIZE = 256;

// Message instance
nested_message_OuterMessage msg;

/**
 * @brief Populate inner message with test data
 */
static inline void populate_inner(nested_message_InnerMessage& inner, 
                                  int32_t value, const char* name)
{
    inner.value = value;
    strncpy(inner.name, name, sizeof(inner.name) - 1);
    inner.name[sizeof(inner.name) - 1] = '\0';
}

/**
 * @brief Populate middle message with test data
 */
static inline void populate_middle(nested_message_MiddleMessage& middle,
                                   float x, float y, float z,
                                   int32_t inner_value, const char* inner_name,
                                   const int32_t* values, size_t values_count)
{
    middle.x = x;
    middle.y = y;
    middle.z = z;
    populate_inner(middle.inner, inner_value, inner_name);
    
    middle.values_count = (pb_size_t)values_count;
    for (size_t i = 0; i < values_count && i < sizeof(middle.values)/sizeof(middle.values[0]); i++) {
        middle.values[i] = values[i];
    }
}

/**
 * @brief Populate message with test data (must match test_data.json)
 */
void populate_message(nested_message_OuterMessage& m)
{
    memset(&m, 0, sizeof(m));
    
    m.id = 42;
    strncpy(m.label, "Test Outer Message", sizeof(m.label) - 1);
    m.label[sizeof(m.label) - 1] = '\0';
    
    // Middle message
    const int32_t middle_values[] = {10, 20, 30, 40, 50};
    m.has_middle = true;
    populate_middle(m.middle, 1.5f, 2.5f, 3.5f, 100, "Inner1", 
                   middle_values, 5);
    
    // Items (repeated MiddleMessage)
    m.items_count = 2;
    
    const int32_t item1_values[] = {1, 2, 3};
    populate_middle(m.items[0], 4.0f, 5.0f, 6.0f, 200, "Inner2", item1_values, 3);
    
    const int32_t item2_values[] = {4, 5};
    populate_middle(m.items[1], 7.0f, 8.0f, 9.0f, 300, "Inner3", item2_values, 2);
}

/**
 * @brief Test NanoPB serialization
 * @param buffer Output buffer for serialized data
 * @return Number of bytes serialized, or 0 on error
 */
uint32_t test_serialize(uint8_t* buffer)
{
    populate_message(msg);
    
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, BUFFER_SIZE);
    bool status = pb_encode(&stream, nested_message_OuterMessage_fields, &msg);
    
    if (!status) {
        return 0;
    }
    
    return stream.bytes_written;
}

/**
 * @brief Test NanoPB deserialization
 * @param data Pointer to serialized data
 * @param size Size of serialized data
 * @return true on success, false on error
 */
bool test_deserialize(const uint8_t* data, uint32_t size)
{
    memset(&msg, 0, sizeof(msg));
    
    pb_istream_t stream = pb_istream_from_buffer(data, size);
    bool status = pb_decode(&stream, nested_message_OuterMessage_fields, &msg);
    
    return status;
}

/**
 * @brief Get message size in bytes
 */
size_t get_message_size()
{
    return sizeof(nested_message_OuterMessage);
}
