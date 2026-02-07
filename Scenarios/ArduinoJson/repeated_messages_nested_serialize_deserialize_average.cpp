#include <ArduinoJson.h>
#include <ArduinoJson/helper_repeated_demo.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  JsonDocument doc;

  HelperRepeatedDemoArduinoJson::set_average_nested_repeated_messages(doc);

  // Create buffer for serialized data
  char buffer[10000];
  const auto n_bytes_written = serializeJson(doc, buffer, sizeof(buffer));

  // Create deserialization buffer and copy data
  char deserialize_buffer[10000];
  memcpy(deserialize_buffer, buffer, n_bytes_written);

  // Clear the document data
  JsonDocument doc_deserialized;

  // Deserialize the buffer into the document
  const auto deserialize_error = deserializeJson(doc_deserialized, deserialize_buffer, n_bytes_written);
  if (deserialize_error) {
    return 0;
  }

  // Compare the values with helper functions
  bool success = HelperRepeatedDemoArduinoJson::compare_average_nested_repeated_messages(doc_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? n_bytes_written : 0;
}