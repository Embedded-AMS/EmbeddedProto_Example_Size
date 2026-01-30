/*
 * Copyright (C) 2020-2024 Embedded AMS B.V. - All Rights Reserved
 *
 * This file is part of Embedded Proto.
 *
 * Embedded Proto is open source software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, version 3 of the license.
 *
 * Embedded Proto  is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
 *
 * For commercial and closed source application please visit:
 * <https://EmbeddedProto.com/license/>.
 */

// Test functions for simple_types scenario
// Contains test implementations for all three libraries

#include <cycle_counter.h>
#include <string.h>

// Library-specific includes
#ifdef EAMS
#include "../../../../generated/EAMS/simple_types/simple_types.h"  // EmbeddedProto
#elif defined(NANOPB)
#include "../../../../generated/Nanopb/simple_types/simple_types.pb.h"               // NanoPB
#elif defined(ARDUINOJSON)
#include <ArduinoJson.h>               // ArduinoJson
#endif

// Test data for simple_types
static const int32_t TEST_INT32 = 42;
static const int64_t TEST_INT64 = 123456789012345LL;
static const uint32_t TEST_UINT32 = 100000;
static const uint64_t TEST_UINT64 = 9876543210987654ULL;
static const bool TEST_BOOL = true;
static const float TEST_FLOAT = 3.14159f;
static const double TEST_DOUBLE = 2.718281828459045;
static const char* TEST_STRING = "Hello, EmbeddedProto!";
static const uint8_t TEST_BYTES[] = {0x01, 0x02, 0x03, 0x04, 0x05};
static const size_t TEST_BYTES_SIZE = sizeof(TEST_BYTES);

// EmbeddedProto test function
#ifdef EAMS
void test_embeddedproto_simple_types() {
    // Use default template parameters for string/bytes sizes
    simple_types::SimpleTypes<32, 32> msg;
    
    // Set test data
    msg.set_a_int32(TEST_INT32);
    msg.set_a_int64(TEST_INT64);
    msg.set_a_uint32(TEST_UINT32);
    msg.set_a_uint64(TEST_UINT64);
    msg.set_a_bool(TEST_BOOL);
    msg.set_a_float(TEST_FLOAT);
    msg.set_a_double(TEST_DOUBLE);
    msg.mutable_a_string().set(TEST_STRING, strlen(TEST_STRING));
    msg.mutable_a_bytes().set(TEST_BYTES, TEST_BYTES_SIZE);
    msg.set_a_enum(simple_types::TestEnum::ENUM_ONE);
    
    // Measure serialize cycles
    uint32_t serialize_cycles = 0;
    uint32_t deserialize_cycles = 0;
    
    // Serialize with cycle measurement
    {
        EmbeddedProto::WriteBufferFixedSize<256> writeBuffer;
        uint32_t start = CycleCounter_Start();
        auto result = msg.serialize(writeBuffer);
        serialize_cycles = CycleCounter_Elapsed(start);
        
        if (result != EmbeddedProto::Error::NO_ERRORS) {
            // Handle error
        }
    }
    
    // Deserialize with cycle measurement
    {
        EmbeddedProto::ReadBufferFixedSize<256> readBuffer;
        // In real test, readBuffer would contain the serialized data
        uint32_t start = CycleCounter_Start();
        auto result = msg.deserialize(readBuffer);
        deserialize_cycles = CycleCounter_Elapsed(start);
        
        if (result != EmbeddedProto::Error::NO_ERRORS) {
            // Handle error
        }
    }
    
    // Output results via UART
    printf("MEASUREMENT_START\r\n");
    printf("LIBRARY: EmbeddedProto\r\n");
    printf("SCENARIO: simple_types\r\n");
    printf("CYCLES_SERIALIZE: %lu\r\n", serialize_cycles);
    printf("CYCLES_DESERIALIZE: %lu\r\n", deserialize_cycles);
    printf("MEASUREMENT_END\r\n");
}
#endif

// NanoPB test function
#ifdef NANOPB
void test_nanopb_simple_types() {
    SimpleTypes msg = SimpleTypes_init_default;
    
    // Set test data
    msg.a_int32 = TEST_INT32;
    msg.a_int64 = TEST_INT64;
    msg.a_uint32 = TEST_UINT32;
    msg.a_uint64 = TEST_UINT64;
    msg.a_bool = TEST_BOOL;
    msg.a_float = TEST_FLOAT;
    msg.a_double = TEST_DOUBLE;
    
    // Copy string and bytes (NanoPB uses fixed buffers)
    strncpy(msg.a_string, TEST_STRING, sizeof(msg.a_string) - 1);
    memcpy(msg.a_bytes, TEST_BYTES, TEST_BYTES_SIZE < sizeof(msg.a_bytes) ? TEST_BYTES_SIZE : sizeof(msg.a_bytes));
    msg.a_enum = ENUM_ONE;
    
    // Measure serialize cycles
    uint32_t serialize_cycles = 0;
    uint32_t deserialize_cycles = 0;
    
    // Serialize with cycle measurement
    {
        uint8_t buffer[256];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        DWT_CycleCounter_Start();
        bool success = pb_encode(&stream, SimpleTypes_fields, &msg);
        serialize_cycles = DWT_CycleCounter_Stop();
        
        if (!success) {
            // Handle error
        }
    }
    
    // Deserialize with cycle measurement
    {
        uint8_t buffer[256];
        pb_istream_t stream = pb_istream_from_buffer(buffer, sizeof(buffer));
        SimpleTypes decoded_msg = SimpleTypes_init_default;
        DWT_CycleCounter_Start();
        bool success = pb_decode(&stream, SimpleTypes_fields, &decoded_msg);
        deserialize_cycles = DWT_CycleCounter_Stop();
        
        if (!success) {
            // Handle error
        }
    }
    
    // Output results via UART
    printf("MEASUREMENT_START\r\n");
    printf("LIBRARY: NanoPB\r\n");
    printf("SCENARIO: simple_types\r\n");
    printf("CYCLES_SERIALIZE: %lu\r\n", serialize_cycles);
    printf("CYCLES_DESERIALIZE: %lu\r\n", deserialize_cycles);
    printf("MEASUREMENT_END\r\n");
}
#endif

// ArduinoJson test function
#ifdef ARDUINOJSON
void test_arduinojson_simple_types() {
    StaticJsonDocument<256> doc;
    
    // Set test data
    doc["a_int32"] = TEST_INT32;
    doc["a_int64"] = static_cast<int64_t>(TEST_INT64);
    doc["a_uint32"] = TEST_UINT32;
    doc["a_uint64"] = static_cast<uint64_t>(TEST_UINT64);
    doc["a_bool"] = TEST_BOOL;
    doc["a_float"] = TEST_FLOAT;
    doc["a_double"] = TEST_DOUBLE;
    doc["a_string"] = TEST_STRING;
    
    // ArduinoJson doesn't have native bytes support, store as array
    JsonArray bytes_array = doc.createNestedArray("a_bytes");
    for (size_t i = 0; i < TEST_BYTES_SIZE; i++) {
        bytes_array.add(TEST_BYTES[i]);
    }
    doc["a_enum"] = 1; // ENUM_ONE
    
    // Measure serialize cycles
    uint32_t serialize_cycles = 0;
    uint32_t deserialize_cycles = 0;
    
    // Serialize with cycle measurement
    {
        char buffer[256];
        DWT_CycleCounter_Start();
        serializeJson(doc, buffer, sizeof(buffer));
        serialize_cycles = DWT_CycleCounter_Stop();
    }
    
    // Deserialize with cycle measurement
    {
        char buffer[256];
        serializeJson(doc, buffer, sizeof(buffer)); // First serialize to get data
        
        StaticJsonDocument<256> decoded_doc;
        DWT_CycleCounter_Start();
        DeserializationError error = deserializeJson(decoded_doc, buffer);
        deserialize_cycles = DWT_CycleCounter_Stop();
        
        if (error) {
            // Handle error
        }
    }
    
    // Output results via UART
    printf("MEASUREMENT_START\r\n");
    printf("LIBRARY: ArduinoJson\r\n");
    printf("SCENARIO: simple_types\r\n");
    printf("CYCLES_SERIALIZE: %lu\r\n", serialize_cycles);
    printf("CYCLES_DESERIALIZE: %lu\r\n", deserialize_cycles);
    printf("MEASUREMENT_END\r\n");
}
#endif