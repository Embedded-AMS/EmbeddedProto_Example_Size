/**
 * @file arduinojson.hpp
 * @brief ArduinoJson test functions for size comparison benchmarking
 * 
 * This file provides test functions for ArduinoJson serialization/deserialization
 * with CPU cycle measurement support.
 */

#ifndef INC_ARDUINOJSON_HPP_
#define INC_ARDUINOJSON_HPP_

#include "../../../../ArduinoJson/src/ArduinoJson.h"
#include "cycle_counter.h"
#include <string.h>

/* Buffer size for serialization */
constexpr uint32_t ARDUINOJSON_BUFFER_SIZE = 128;

/* Test data values (must match scenarios/simple_types/test_data.json) */
constexpr int32_t  TEST_INT32  = -12345;
constexpr int64_t  TEST_INT64  = -9876543210LL;
constexpr uint32_t TEST_UINT32 = 12345;
constexpr uint64_t TEST_UINT64 = 9876543210ULL;
constexpr bool     TEST_BOOL   = true;
constexpr float    TEST_FLOAT  = 3.14159f;
constexpr double   TEST_DOUBLE = 2.718281828459045;
const char*        TEST_STRING = "Hello, Proto!";
constexpr uint8_t  TEST_BYTES_DATA[] = {0xDE, 0xAD, 0xBE, 0xEF};
constexpr size_t   TEST_BYTES_LEN = sizeof(TEST_BYTES_DATA);

/* JSON document instance */
StaticJsonDocument<256> jsonDoc;

/**
 * @brief Populate JSON document with test data
 */
static inline void populate_arduinojson_document(StaticJsonDocument<256>& doc)
{
    doc["a_int32"] = TEST_INT32;
    doc["a_int64"] = TEST_INT64;
    doc["a_uint32"] = TEST_UINT32;
    doc["a_uint64"] = TEST_UINT64;
    doc["a_bool"] = TEST_BOOL;
    doc["a_float"] = TEST_FLOAT;
    doc["a_double"] = TEST_DOUBLE;
    doc["a_string"] = TEST_STRING;
    
    /* ArduinoJson stores bytes as JSON array of integers */
    JsonArray bytesArray = doc.createNestedArray("a_bytes");
    for (size_t i = 0; i < TEST_BYTES_LEN; i++) {
        bytesArray.add(TEST_BYTES_DATA[i]);
    }
}

/**
 * @brief Test ArduinoJson serialization/deserialization with cycle measurement
 * 
 * This function measures CPU cycles for serialize and deserialize operations
 * and outputs the results via UART.
 */
void test_arduinojson()
{
    /* Set up test data */
    populate_arduinojson_document(jsonDoc);

    /* Prepare buffers */
    uint8_t obuffer[ARDUINOJSON_BUFFER_SIZE];
    uint8_t ibuffer[ARDUINOJSON_BUFFER_SIZE];
    
    /* Measure serialize cycles */
    uint32_t start_serialize = CycleCounter_Start();
    size_t serialized_size = serializeJson(jsonDoc, obuffer, sizeof(obuffer));
    uint32_t serialize_cycles = CycleCounter_Elapsed(start_serialize);
    
    /* Copy serialized data to input buffer for deserialization */
    memcpy(ibuffer, obuffer, serialized_size);
    
    /* Clear document before deserialize to ensure we're actually reading */
    jsonDoc.clear();
    
    /* Measure deserialize cycles */
    uint32_t start_deserialize = CycleCounter_Start();
    DeserializationError error = deserializeJson(jsonDoc, ibuffer, serialized_size);
    uint32_t deserialize_cycles = CycleCounter_Elapsed(start_deserialize);
    
    /* Prevent unused variable warnings */
    (void)error;
    
    /* Output results via UART */
    CycleMeasurement measurement = {
        .serialize_cycles = serialize_cycles,
        .deserialize_cycles = deserialize_cycles,
        .library = "ArduinoJson",
        .scenario = "simple_types"
    };
    CycleCounter_OutputResults(&measurement);
}

/* Array of documents for array test */
StaticJsonDocument<256> jsonDocArray[100];

/**
 * @brief Test ArduinoJson array serialization/deserialization with cycle measurement
 * 
 * This function measures CPU cycles for serialize and deserialize operations
 * on an array of documents and outputs the average results via UART.
 */
void test_arduinojson_array()
{
    uint32_t total_serialize_cycles = 0;
    uint32_t total_deserialize_cycles = 0;
    const uint32_t num_iterations = 100;
    
    for(auto& doc : jsonDocArray) {
        /* Set up test data */
        populate_arduinojson_document(doc);
        
        /* Prepare buffers */
        uint8_t obuffer[ARDUINOJSON_BUFFER_SIZE];
        uint8_t ibuffer[ARDUINOJSON_BUFFER_SIZE];
        
        /* Measure serialize cycles */
        uint32_t start_serialize = CycleCounter_Start();
        size_t serialized_size = serializeJson(doc, obuffer, sizeof(obuffer));
        total_serialize_cycles += CycleCounter_Elapsed(start_serialize);
        
        /* Copy serialized data to input buffer */
        memcpy(ibuffer, obuffer, serialized_size);
        
        /* Clear document before deserialize */
        doc.clear();
        
        /* Measure deserialize cycles */
        uint32_t start_deserialize = CycleCounter_Start();
        deserializeJson(doc, ibuffer, serialized_size);
        total_deserialize_cycles += CycleCounter_Elapsed(start_deserialize);
    }
    
    /* Output average results via UART */
    CycleMeasurement measurement = {
        .serialize_cycles = total_serialize_cycles / num_iterations,
        .deserialize_cycles = total_deserialize_cycles / num_iterations,
        .library = "ArduinoJson",
        .scenario = "simple_types_array_avg"
    };
    CycleCounter_OutputResults(&measurement);
}

// ============================================================================
// Nested Message Scenario Test Functions
// ============================================================================

/* JSON document instance for nested message (larger capacity) */
StaticJsonDocument<768> nestedJsonDoc;

/**
 * @brief Populate inner object with test data
 */
static inline void populate_nested_inner_json(JsonObject& inner, int32_t value, const char* name)
{
    inner["value"] = value;
    inner["name"] = name;
}

/**
 * @brief Populate middle object with test data
 */
static inline void populate_nested_middle_json(JsonObject& middle, float x, float y, float z,
                                               int32_t inner_value, const char* inner_name,
                                               const int32_t* values, size_t values_count)
{
    middle["x"] = x;
    middle["y"] = y;
    middle["z"] = z;
    
    JsonObject inner = middle.createNestedObject("inner");
    populate_nested_inner_json(inner, inner_value, inner_name);
    
    JsonArray valuesArray = middle.createNestedArray("values");
    for (size_t i = 0; i < values_count; i++) {
        valuesArray.add(values[i]);
    }
}

/**
 * @brief Populate JSON document with nested message test data (must match test_data.json)
 */
static inline void populate_nested_document(JsonDocument& d)
{
    d.clear();
    
    d["id"] = 42;
    d["label"] = "Test Outer Message";
    
    // Middle message
    JsonObject middle = d.createNestedObject("middle");
    const int32_t middle_values[] = {10, 20, 30, 40, 50};
    populate_nested_middle_json(middle, 1.5f, 2.5f, 3.5f, 100, "Inner1", middle_values, 5);
    
    // Items (repeated MiddleMessage)
    JsonArray items = d.createNestedArray("items");
    
    JsonObject item1 = items.createNestedObject();
    const int32_t item1_values[] = {1, 2, 3};
    populate_nested_middle_json(item1, 4.0f, 5.0f, 6.0f, 200, "Inner2", item1_values, 3);
    
    JsonObject item2 = items.createNestedObject();
    const int32_t item2_values[] = {4, 5};
    populate_nested_middle_json(item2, 7.0f, 8.0f, 9.0f, 300, "Inner3", item2_values, 2);
}

/**
 * @brief Test ArduinoJson nested message serialization/deserialization with cycle measurement
 */
void test_arduinojson_nested()
{
    /* Set up test data */
    populate_nested_document(nestedJsonDoc);

    /* Prepare buffers (larger for nested JSON) */
    constexpr uint32_t NESTED_BUFFER_SIZE = 512;
    uint8_t obuffer[NESTED_BUFFER_SIZE];
    uint8_t ibuffer[NESTED_BUFFER_SIZE];
    
    /* Measure serialize cycles */
    uint32_t start_serialize = CycleCounter_Start();
    size_t serialized_size = serializeJson(nestedJsonDoc, obuffer, sizeof(obuffer));
    uint32_t serialize_cycles = CycleCounter_Elapsed(start_serialize);
    
    /* Copy serialized data to input buffer for deserialization */
    memcpy(ibuffer, obuffer, serialized_size);
    
    /* Clear document before deserialize to ensure we're actually reading */
    nestedJsonDoc.clear();
    
    /* Measure deserialize cycles */
    uint32_t start_deserialize = CycleCounter_Start();
    DeserializationError error = deserializeJson(nestedJsonDoc, ibuffer, serialized_size);
    uint32_t deserialize_cycles = CycleCounter_Elapsed(start_deserialize);
    
    /* Prevent unused variable warnings */
    (void)error;
    
    /* Output results via UART */
    CycleMeasurement measurement = {
        .serialize_cycles = serialize_cycles,
        .deserialize_cycles = deserialize_cycles,
        .library = "ArduinoJson",
        .scenario = "nested_message"
    };
    CycleCounter_OutputResults(&measurement);
}

#endif /* INC_ARDUINOJSON_HPP_ */