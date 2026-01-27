/*
 * Test code for NanoPB - simple_types scenario
 * 
 * This file tests serialization and deserialization of the SimpleTypes message.
 */

#include "message.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include <string.h>

// Buffer size for serialization
constexpr uint32_t BUFFER_SIZE = 128;

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
simple_types_SimpleTypes msg;

/**
 * @brief Populate message with test data
 */
void populate_message(simple_types_SimpleTypes& m)
{
    m.a_int32 = TEST_INT32;
    m.a_int64 = TEST_INT64;
    m.a_uint32 = TEST_UINT32;
    m.a_uint64 = TEST_UINT64;
    m.a_bool = TEST_BOOL;
    m.a_float = TEST_FLOAT;
    m.a_double = TEST_DOUBLE;
    strncpy(m.a_string, TEST_STRING, sizeof(m.a_string) - 1);
    m.a_string[sizeof(m.a_string) - 1] = '\0';
    memcpy(m.a_bytes.bytes, TEST_BYTES, TEST_BYTES_LEN);
    m.a_bytes.size = (pb_size_t)TEST_BYTES_LEN;
    m.a_enum = simple_types_TestEnum_ENUM_TWO;
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
    bool status = pb_encode(&stream, simple_types_SimpleTypes_fields, &msg);
    
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
    bool status = pb_decode(&stream, simple_types_SimpleTypes_fields, &msg);
    
    return status;
}

/**
 * @brief Get message size in bytes
 */
size_t get_message_size()
{
    return sizeof(simple_types_SimpleTypes);
}
