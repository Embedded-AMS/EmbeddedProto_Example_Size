#ifndef HELPER_STRING

#include <string.h>
#include <ArduinoJson.h>

namespace HelperString
{
constexpr const char* TEST_STRING = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

void set_string(JsonDocument& doc)
{
  doc["text"] = TEST_STRING;
}

bool compare_string(const JsonDocument& doc)
{
  const char* actual_text = doc["text"];
  return actual_text != nullptr && strcmp(actual_text, TEST_STRING) == 0;
}

} // End of namespace HelperString

#endif // HELPER_STRING
