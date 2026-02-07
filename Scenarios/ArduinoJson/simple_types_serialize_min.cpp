#include <ArduinoJson.h>
#include <ArduinoJson/helper_simple_types.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  JsonDocument doc;

  HelperSimpleTypesArduinoJson::set_min(doc);

  // Create buffer for serialized data
  char buffer[500];
  const uint32_t n_bytes_written = serializeJson(doc, buffer, sizeof(buffer));

  // Calculate checksum to prevent dead code elimination
  // The compiler cannot optimize away serialize() since buffer content is used
  uint8_t checksum = 0;
  for (uint32_t i = 0; i < n_bytes_written; i++) {
    checksum ^= static_cast<uint8_t>(buffer[i]);
  }
  g_buffer_checksum = checksum;

  return n_bytes_written;
}
