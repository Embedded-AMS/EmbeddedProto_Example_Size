#include <ArduinoJson.h>
#include <ArduinoJson/helper_string.h>

uint32_t run_scenario()
{
  // Create a JSON document with enough capacity for our string
  StaticJsonDocument<200> doc;
  HelperString::set_string(doc);

  // Serialize the JSON document to a buffer
  char buffer[200];
  size_t buffer_size = serializeJson(doc, buffer, sizeof(buffer));

  // Create a new document for deserialization
  StaticJsonDocument<200> doc_deserialized;

  // Deserialize from the buffer
  DeserializationError error = deserializeJson(doc_deserialized, buffer);

  // Compare the values with helper functions
  bool success = HelperString::compare_string(doc_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? static_cast<uint32_t>(buffer_size) : 0;
}