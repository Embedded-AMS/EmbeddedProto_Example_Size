/*
 * Test code for ArduinoJson - simple_types scenario
 * 
 * This file tests serialization and deserialization of the SimpleTypes equivalent.
 * ArduinoJson uses JSON format instead of protobuf binary format.
 */

#include "ArduinoJson.h"
#include <string.h>

// Buffer size for serialization
constexpr uint32_t BUFFER_SIZE = 256;

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

// JSON document for ArduinoJson
// Size calculated for: 10 fields + string + bytes array
StaticJsonDocument<384> doc;

/**
 * @brief Populate JSON document with test data
 */
void populate_document(JsonDocument& d)
{
    d.clear();
    d["a_int32"] = TEST_INT32;
    d["a_int64"] = TEST_INT64;
    d["a_uint32"] = TEST_UINT32;
    d["a_uint64"] = TEST_UINT64;
    d["a_bool"] = TEST_BOOL;
    d["a_float"] = TEST_FLOAT;
    d["a_double"] = TEST_DOUBLE;
    d["a_string"] = TEST_STRING;
    
    // Bytes as array of integers (JSON doesn't have native binary)
    JsonArray bytes = d.createNestedArray("a_bytes");
    for (size_t i = 0; i < TEST_BYTES_LEN; i++) {
        bytes.add(TEST_BYTES[i]);
    }
    
    d["a_enum"] = 2;  // ENUM_TWO
}

/**
 * @brief Test ArduinoJson serialization
 * @param buffer Output buffer for serialized JSON string
 * @return Number of bytes serialized, or 0 on error
 */
uint32_t test_serialize(char* buffer)
{
    populate_document(doc);
    
    size_t len = serializeJson(doc, buffer, BUFFER_SIZE);
    
    return (uint32_t)len;
}

/**
 * @brief Test ArduinoJson deserialization
 * @param data Pointer to JSON string
 * @param size Size of JSON string
 * @return true on success, false on error
 */
bool test_deserialize(const char* data, uint32_t size)
{
    doc.clear();
    
    DeserializationError error = deserializeJson(doc, data, size);
    
    return (error == DeserializationError::Ok);
}

/**
 * @brief Get document capacity in bytes
 * Note: ArduinoJson doesn't have a fixed message struct like protobuf
 */
size_t get_message_size()
{
    return doc.capacity();
}
