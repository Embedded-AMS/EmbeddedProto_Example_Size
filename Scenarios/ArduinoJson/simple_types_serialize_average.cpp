#include <ArduinoJson.h>
#include <ArduinoJson/helper_simple_types.h>

void run_scenario()
{
  JsonDocument doc;

  HelperSimpleTypesArduinoJson::set_average(doc);

  // Create buffer for serialized data
  char buffer[100];
  const auto result = serializeJson(doc, buffer, sizeof(buffer));
}