#include <ArduinoJson.h>
#include <ArduinoJson/helper_oneof.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  // Create a JSON document
  JsonDocument doc;

  // Set different oneof fields and serialize
  HelperOneOf::set_int32_field(doc);

  // Serialize the data to buffer
  char buffer[500];
  const auto n_bytes_written = serializeJson(doc, buffer, sizeof(buffer));

  // Calculate checksum to prevent dead code elimination
  // The compiler cannot optimize away serialize() since buffer content is used
  uint8_t checksum = 0;
  for (uint32_t i = 0; i < n_bytes_written; i++) {
    checksum ^= static_cast<uint8_t>(buffer[i]);
  }
  g_buffer_checksum = checksum;

  // Deserialize the buffer into a new document
  JsonDocument doc_deserialized;
  DeserializationError error = deserializeJson(doc_deserialized, buffer, n_bytes_written);

  // Verify the deserialized data matches the original
  bool success = HelperOneOf::compare_int32_field(doc_deserialized);

  // Clear the oneof fields
  HelperOneOf::clear_oneof_fields(doc_deserialized);

  // Set a different field (float) and verify
  HelperOneOf::set_float_field(doc_deserialized);
  success &= HelperOneOf::compare_float_field(doc_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? static_cast<uint32_t>(n_bytes_written) : 0;
}
