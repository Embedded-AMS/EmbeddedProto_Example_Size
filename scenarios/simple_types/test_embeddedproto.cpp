/*
 * Test code for EmbeddedProto - simple_types scenario
 * 
 * This file tests serialization and deserialization of the SimpleTypes message.
 */

#include "message.h"
#include "WriteBufferFixedSize.h"
#include "ReadBufferFixedSize.h"
#include <string.h>

// Buffer size for serialization
constexpr uint32_t BUFFER_SIZE = 128;

// String/bytes max sizes for EmbeddedProto template parameters
constexpr uint32_t STRING_SIZE = 32;
constexpr uint32_t BYTES_SIZE = 32;

// Test data values (must match test_data.json)
constexpr int32_t  TEST_INT32  = -12345;
constexpr int64_t  TEST_INT64  = -9876543210LL;
constexpr uint32_t TEST_UINT32 = 12345;
constexpr uint64_t TEST_UINT64 = 9876543210ULL;
constexpr bool     TEST_BOOL   = true;
constexpr float    TEST_FLOAT  = 3.14159f;
constexpr double   TEST_DOUBLE = 2.718281828459045;
const char*        TEST_STRING = "Hello, Proto!";
constexpr uint8_t  TEST_BYTES[] = {0xDE, 0xAD, 0xBE, 0xEF};
constexpr size_t   TEST_BYTES_LEN = sizeof(TEST_BYTES);

// Message instance
simple_types::SimpleTypes<STRING_SIZE, BYTES_SIZE> msg;

/**
 * @brief Populate message with test data
 */
void populate_message(simple_types::SimpleTypes<STRING_SIZE, BYTES_SIZE>& m)
{
    m.set_a_int32(TEST_INT32);
    m.set_a_int64(TEST_INT64);
    m.set_a_uint32(TEST_UINT32);
    m.set_a_uint64(TEST_UINT64);
    m.set_a_bool(TEST_BOOL);
    m.set_a_float(TEST_FLOAT);
    m.set_a_double(TEST_DOUBLE);
    m.mutable_a_string().set(TEST_STRING, strlen(TEST_STRING));
    m.mutable_a_bytes().set(TEST_BYTES, TEST_BYTES_LEN);
    m.set_a_enum(simple_types::TestEnum::ENUM_TWO);
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
    return sizeof(simple_types::SimpleTypes<STRING_SIZE, BYTES_SIZE>);
}
