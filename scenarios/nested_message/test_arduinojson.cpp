/*
 * Test code for ArduinoJson - nested_message scenario
 * 
 * This file tests serialization and deserialization of nested messages
 * with 2-3 levels of nesting and repeated fields.
 * ArduinoJson uses JSON format instead of protobuf binary format.
 */

#include "ArduinoJson.h"
#include <string.h>

// Buffer size for serialization (JSON is larger than binary)
constexpr uint32_t BUFFER_SIZE = 512;

// JSON document size (calculated for nested structure)
// InnerMessage: ~50 bytes each
// MiddleMessage: ~150 bytes each (with inner + values array)
// OuterMessage: ~600 bytes (with middle + 3 items)
StaticJsonDocument<768> doc;

/**
 * @brief Populate inner object with test data
 */
static inline void populate_inner(JsonObject& inner, int32_t value, const char* name)
{
    inner["value"] = value;
    inner["name"] = name;
}

/**
 * @brief Populate middle object with test data
 */
static inline void populate_middle(JsonObject& middle, float x, float y, float z,
                                   int32_t inner_value, const char* inner_name,
                                   const int32_t* values, size_t values_count)
{
    middle["x"] = x;
    middle["y"] = y;
    middle["z"] = z;
    
    JsonObject inner = middle.createNestedObject("inner");
    populate_inner(inner, inner_value, inner_name);
    
    JsonArray valuesArray = middle.createNestedArray("values");
    for (size_t i = 0; i < values_count; i++) {
        valuesArray.add(values[i]);
    }
}

/**
 * @brief Populate JSON document with test data (must match test_data.json)
 */
void populate_document(JsonDocument& d)
{
    d.clear();
    
    d["id"] = 42;
    d["label"] = "Test Outer Message";
    
    // Middle message
    JsonObject middle = d.createNestedObject("middle");
    const int32_t middle_values[] = {10, 20, 30, 40, 50};
    populate_middle(middle, 1.5f, 2.5f, 3.5f, 100, "Inner1", middle_values, 5);
    
    // Items (repeated MiddleMessage)
    JsonArray items = d.createNestedArray("items");
    
    JsonObject item1 = items.createNestedObject();
    const int32_t item1_values[] = {1, 2, 3};
    populate_middle(item1, 4.0f, 5.0f, 6.0f, 200, "Inner2", item1_values, 3);
    
    JsonObject item2 = items.createNestedObject();
    const int32_t item2_values[] = {4, 5};
    populate_middle(item2, 7.0f, 8.0f, 9.0f, 300, "Inner3", item2_values, 2);
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
