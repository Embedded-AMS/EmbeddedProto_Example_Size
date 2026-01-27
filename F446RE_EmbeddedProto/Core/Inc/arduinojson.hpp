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

#endif /* INC_ARDUINOJSON_HPP_ */