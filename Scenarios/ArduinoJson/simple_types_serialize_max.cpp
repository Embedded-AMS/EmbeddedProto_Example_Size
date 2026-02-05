#include <ArduinoJson.h>
#include <ArduinoJson/helper_simple_types.h>

uint32_t run_scenario()
{
  JsonDocument doc;

  HelperSimpleTypesArduinoJson::set_max(doc);

  // Create buffer for serialized data
  char buffer[100];
  const uint32_t n_bytes_written = serializeJson(doc, buffer, sizeof(buffer));

  return n_bytes_written;
}