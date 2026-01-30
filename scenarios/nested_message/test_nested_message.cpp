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

// Test functions for nested_message scenario
// Contains test implementations for all three libraries

#include <cycle_counter.h>
#include <string.h>

// Library-specific includes
#ifdef EAMS
#include "../../../generated/EAMS/nested_message/nested_message.h"  // EmbeddedProto
#elif defined(NANOPB)
#include "../../../generated/Nanopb/nested_message/nested_message.pb.h"                  // NanoPB
#elif defined(ARDUINOJSON)
#include <ArduinoJson.h>                  // ArduinoJson
#endif

// Test data for nested_message
static const int32_t TEST_ID = 123;
static const char* TEST_LABEL = "NestedTest";
static const float TEST_X = 1.0f, TEST_Y = 2.0f, TEST_Z = 3.0f;
static const int32_t TEST_INNER_VALUE = 456;
static const char* TEST_INNER_NAME = "Inner";
static const int32_t TEST_VALUES[] = {10, 20, 30, 40, 50};
static const size_t TEST_VALUES_COUNT = sizeof(TEST_VALUES) / sizeof(TEST_VALUES[0]);

// EmbeddedProto test function
#ifdef EAMS
void test_embeddedproto_nested_message() {
    nested_message::OuterMessage msg;
    
    // Set test data
    msg.set_id(TEST_ID);
    msg.mutable_label()->set(TEST_LABEL, strlen(TEST_LABEL));
    
    // Set middle message
    auto* middle = msg.mutable_middle();
    middle->set_x(TEST_X);
    middle->set_y(TEST_Y);
    middle->set_z(TEST_Z);
    
    // Set inner message in middle
    auto* inner = middle->mutable_inner();
    inner->set_value(TEST_INNER_VALUE);
    inner->set_name(TEST_INNER_NAME, strlen(TEST_INNER_NAME));
    
    // Set repeated values in middle
    for (size_t i = 0; i < TEST_VALUES_COUNT; i++) {
        middle->add_values(TEST_VALUES[i]);
    }
    
    // Add repeated middle messages
    for (int i = 0; i < 2; i++) {
        auto* item = msg.add_items();
        item->set_x(TEST_X + i);
        item->set_y(TEST_Y + i);
        item->set_z(TEST_Z + i);
        
        auto* item_inner = item->mutable_inner();
        item_inner->set_value(TEST_INNER_VALUE + i);
        item_inner->set_name(TEST_INNER_NAME, strlen(TEST_INNER_NAME));
    }
    
    // Measure serialize cycles
    uint32_t serialize_cycles = 0;
    uint32_t deserialize_cycles = 0;
    
    // Serialize with cycle measurement
    {
        EmbeddedProto::WriteBufferFixedSize<512> writeBuffer;
        DWT_CycleCounter_Start();
        bool success = msg.serialize(writeBuffer);
        serialize_cycles = DWT_CycleCounter_Stop();
        
        if (!success) {
            // Handle error
        }
    }
    
    // Deserialize with cycle measurement
    {
        EmbeddedProto::ReadBufferFixedSize<512> readBuffer;
        // In real test, readBuffer would contain the serialized data
        DWT_CycleCounter_Start();
        bool success = msg.deserialize(readBuffer);
        deserialize_cycles = DWT_CycleCounter_Stop();
        
        if (!success) {
            // Handle error
        }
    }
    
    // Output results via UART
    printf("MEASUREMENT_START\r\n");
    printf("LIBRARY: EmbeddedProto\r\n");
    printf("SCENARIO: nested_message\r\n");
    printf("CYCLES_SERIALIZE: %lu\r\n", serialize_cycles);
    printf("CYCLES_DESERIALIZE: %lu\r\n", deserialize_cycles);
    printf("MEASUREMENT_END\r\n");
}
#endif

// NanoPB test function
#ifdef NANOPB
void test_nanopb_nested_message() {
    OuterMessage msg = OuterMessage_init_default;
    
    // Set test data
    msg.id = TEST_ID;
    strncpy(msg.label, TEST_LABEL, sizeof(msg.label) - 1);
    
    // Set middle message
    msg.middle.x = TEST_X;
    msg.middle.y = TEST_Y;
    msg.middle.z = TEST_Z;
    
    // Set inner message in middle
    msg.middle.inner.value = TEST_INNER_VALUE;
    strncpy(msg.middle.inner.name, TEST_INNER_NAME, sizeof(msg.middle.inner.name) - 1);
    
    // Set repeated values in middle
    for (size_t i = 0; i < TEST_VALUES_COUNT && i < sizeof(msg.middle.values) / sizeof(msg.middle.values[0]); i++) {
        msg.middle.values[i] = TEST_VALUES[i];
    }
    msg.middle.values_count = TEST_VALUES_COUNT < sizeof(msg.middle.values) / sizeof(msg.middle.values[0]) ? 
                              TEST_VALUES_COUNT : sizeof(msg.middle.values) / sizeof(msg.middle.values[0]);
    
    // Add repeated middle messages
    msg.items_count = 2;
    for (int i = 0; i < msg.items_count; i++) {
        msg.items[i].x = TEST_X + i;
        msg.items[i].y = TEST_Y + i;
        msg.items[i].z = TEST_Z + i;
        
        msg.items[i].inner.value = TEST_INNER_VALUE + i;
        strncpy(msg.items[i].inner.name, TEST_INNER_NAME, sizeof(msg.items[i].inner.name) - 1);
    }
    
    // Measure serialize cycles
    uint32_t serialize_cycles = 0;
    uint32_t deserialize_cycles = 0;
    
    // Serialize with cycle measurement
    {
        uint8_t buffer[512];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        DWT_CycleCounter_Start();
        bool success = pb_encode(&stream, OuterMessage_fields, &msg);
        serialize_cycles = DWT_CycleCounter_Stop();
        
        if (!success) {
            // Handle error
        }
    }
    
    // Deserialize with cycle measurement
    {
        uint8_t buffer[512];
        pb_istream_t stream = pb_istream_from_buffer(buffer, sizeof(buffer));
        OuterMessage decoded_msg = OuterMessage_init_default;
        DWT_CycleCounter_Start();
        bool success = pb_decode(&stream, OuterMessage_fields, &decoded_msg);
        deserialize_cycles = DWT_CycleCounter_Stop();
        
        if (!success) {
            // Handle error
        }
    }
    
    // Output results via UART
    printf("MEASUREMENT_START\r\n");
    printf("LIBRARY: NanoPB\r\n");
    printf("SCENARIO: nested_message\r\n");
    printf("CYCLES_SERIALIZE: %lu\r\n", serialize_cycles);
    printf("CYCLES_DESERIALIZE: %lu\r\n", deserialize_cycles);
    printf("MEASUREMENT_END\r\n");
}
#endif

// ArduinoJson test function
#ifdef ARDUINOJSON
void test_arduinojson_nested_message() {
    StaticJsonDocument<512> doc;
    
    // Set test data
    doc["id"] = TEST_ID;
    doc["label"] = TEST_LABEL;
    
    // Set middle message
    JsonObject middle = doc.createNestedObject("middle");
    middle["x"] = TEST_X;
    middle["y"] = TEST_Y;
    middle["z"] = TEST_Z;
    
    // Set inner message in middle
    JsonObject inner = middle.createNestedObject("inner");
    inner["value"] = TEST_INNER_VALUE;
    inner["name"] = TEST_INNER_NAME;
    
    // Set repeated values in middle
    JsonArray values = middle.createNestedArray("values");
    for (size_t i = 0; i < TEST_VALUES_COUNT; i++) {
        values.add(TEST_VALUES[i]);
    }
    
    // Add repeated middle messages
    JsonArray items = doc.createNestedArray("items");
    for (int i = 0; i < 2; i++) {
        JsonObject item = items.createNestedObject();
        item["x"] = TEST_X + i;
        item["y"] = TEST_Y + i;
        item["z"] = TEST_Z + i;
        
        JsonObject item_inner = item.createNestedObject("inner");
        item_inner["value"] = TEST_INNER_VALUE + i;
        item_inner["name"] = TEST_INNER_NAME;
    }
    
    // Measure serialize cycles
    uint32_t serialize_cycles = 0;
    uint32_t deserialize_cycles = 0;
    
    // Serialize with cycle measurement
    {
        char buffer[512];
        DWT_CycleCounter_Start();
        serializeJson(doc, buffer, sizeof(buffer));
        serialize_cycles = DWT_CycleCounter_Stop();
    }
    
    // Deserialize with cycle measurement
    {
        char buffer[512];
        serializeJson(doc, buffer, sizeof(buffer)); // First serialize to get data
        
        StaticJsonDocument<512> decoded_doc;
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
    printf("SCENARIO: nested_message\r\n");
    printf("CYCLES_SERIALIZE: %lu\r\n", serialize_cycles);
    printf("CYCLES_DESERIALIZE: %lu\r\n", deserialize_cycles);
    printf("MEASUREMENT_END\r\n");
}
#endif