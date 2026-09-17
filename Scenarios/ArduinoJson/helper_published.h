/*
 *  Copyright (C) 2020-2026 Embedded AMS B.V. - All Rights Reserved
 *
 *  This file is part of Embedded Proto.
 *
 *  Embedded Proto is open source software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as published 
 *  by the Free Software Foundation, version 3 of the license.
 *
 *  Embedded Proto  is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
 *
 *  For commercial and closed source application please visit:
 * <https://embeddedproto.com/pricing/>.
 *
 *  Embedded AMS B.V.
 *  Info:
 *    info at EmbeddedProto dot com
 *
 *  Postal address:
 *    Atoomweg 2
 *    1627 LE, Hoorn
 *    the Netherlands
 */

#ifndef HELPER_PUBLISHED
#define HELPER_PUBLISHED

#include <ArduinoJson.h>
#include <scenario_values.h>
#include <cstring>

// Set and check functions for the published scenarios, values from scenario_values.h.
// JSON has no schema, so the field names become the keys. Every float value in the set is
// a binary fraction that prints and parses exactly, so the checks are exact.
namespace HelperPublishedArduinoJson
{

using namespace ScenarioValues;

inline void set_telemetry(JsonDocument& doc)
{
  doc["device_id"] = DEVICE_ID;
  doc["timestamp"] = TIMESTAMP;
  doc["temperature"] = TEMPERATURE;
  doc["humidity"] = HUMIDITY;
  doc["battery_mv"] = BATTERY_MV;
  doc["status"] = STATUS_WARNING;
  doc["rssi"] = RSSI;
}

inline bool compare_telemetry(JsonDocument& doc)
{
  return (doc["device_id"].as<uint32_t>() == DEVICE_ID) && (doc["timestamp"].as<uint64_t>() == TIMESTAMP)
         && (doc["temperature"].as<float>() == TEMPERATURE) && (doc["humidity"].as<float>() == HUMIDITY)
         && (doc["battery_mv"].as<uint32_t>() == BATTERY_MV) && (doc["status"].as<int32_t>() == STATUS_WARNING)
         && (doc["rssi"].as<int32_t>() == RSSI);
}

inline void set_telemetry_batch(JsonDocument& doc)
{
  doc["device_id"] = DEVICE_ID;
  doc["timestamp"] = TIMESTAMP;
  JsonArray samples = doc["samples"].to<JsonArray>();
  for(uint32_t i = 0; i < SAMPLE_COUNT; ++i)
  {
    JsonObject sample = samples.add<JsonObject>();
    sample["offset_ms"] = sample_offset_ms(i);
    sample["x"] = sample_x(i);
    sample["y"] = sample_y(i);
    sample["z"] = sample_z(i);
  }
}

inline bool compare_telemetry_batch(JsonDocument& doc)
{
  JsonArrayConst samples = doc["samples"];
  bool result = (doc["device_id"].as<uint32_t>() == DEVICE_ID) && (doc["timestamp"].as<uint64_t>() == TIMESTAMP)
                && (samples.size() == SAMPLE_COUNT);
  for(uint32_t i = 0; result && (i < SAMPLE_COUNT); ++i)
  {
    JsonObjectConst sample = samples[i];
    result = (sample["offset_ms"].as<uint32_t>() == sample_offset_ms(i)) && (sample["x"].as<float>() == sample_x(i))
             && (sample["y"].as<float>() == sample_y(i)) && (sample["z"].as<float>() == sample_z(i));
  }
  return result;
}

inline void set_configuration(JsonDocument& doc)
{
  doc["version"] = CONFIG_VERSION;
  doc["device_name"] = DEVICE_NAME;
  doc["report_interval_ms"] = REPORT_INTERVAL_MS;
  JsonArray peripherals = doc["peripherals"].to<JsonArray>();
  JsonObject p0 = peripherals.add<JsonObject>();
  p0["index"] = 0;
  JsonObject uart0 = p0["uart"].to<JsonObject>();
  uart0["baud"] = UART0_BAUD;
  uart0["data_bits"] = UART0_DATA_BITS;
  uart0["parity"] = UART0_PARITY;
  JsonObject p1 = peripherals.add<JsonObject>();
  p1["index"] = 1;
  JsonObject spi1 = p1["spi"].to<JsonObject>();
  spi1["prescaler"] = SPI1_PRESCALER;
  spi1["cpol"] = SPI1_CPOL;
  spi1["cpha"] = SPI1_CPHA;
  JsonObject p2 = peripherals.add<JsonObject>();
  p2["index"] = 2;
  JsonObject motor2 = p2["motor"].to<JsonObject>();
  motor2["max_speed"] = MOTOR2_MAX_SPEED;
  motor2["invert"] = MOTOR2_INVERT;
  motor2["home_pin"] = MOTOR2_HOME_PIN;
  JsonObject p3 = peripherals.add<JsonObject>();
  p3["index"] = 3;
  JsonObject uart3 = p3["uart"].to<JsonObject>();
  uart3["baud"] = UART3_BAUD;
  uart3["data_bits"] = UART3_DATA_BITS;
  uart3["parity"] = UART3_PARITY;
}

inline bool compare_configuration(JsonDocument& doc)
{
  JsonArrayConst peripherals = doc["peripherals"];
  const char* name = doc["device_name"];
  bool result = (doc["version"].as<uint32_t>() == CONFIG_VERSION) && (nullptr != name) && (0 == strcmp(name, DEVICE_NAME))
                && doc["report_interval_ms"].is<uint32_t>() && (doc["report_interval_ms"].as<uint32_t>() == REPORT_INTERVAL_MS)
                && (peripherals.size() == PERIPHERAL_COUNT);
  for(uint32_t i = 0; result && (i < PERIPHERAL_COUNT); ++i)
  {
    result = (peripherals[i]["index"].as<uint32_t>() == i);
  }
  if(result)
  {
    JsonObjectConst uart0 = peripherals[0]["uart"];
    JsonObjectConst spi1 = peripherals[1]["spi"];
    JsonObjectConst motor2 = peripherals[2]["motor"];
    JsonObjectConst uart3 = peripherals[3]["uart"];
    result = (uart0["baud"].as<uint32_t>() == UART0_BAUD) && (uart0["data_bits"].as<uint32_t>() == UART0_DATA_BITS) && (uart0["parity"].as<bool>() == UART0_PARITY)
             && (spi1["prescaler"].as<uint32_t>() == SPI1_PRESCALER) && (spi1["cpol"].as<bool>() == SPI1_CPOL) && (spi1["cpha"].as<bool>() == SPI1_CPHA)
             && (motor2["max_speed"].as<uint32_t>() == MOTOR2_MAX_SPEED) && (motor2["invert"].as<bool>() == MOTOR2_INVERT) && (motor2["home_pin"].as<uint32_t>() == MOTOR2_HOME_PIN)
             && (uart3["baud"].as<uint32_t>() == UART3_BAUD) && (uart3["data_bits"].as<uint32_t>() == UART3_DATA_BITS) && (uart3["parity"].as<bool>() == UART3_PARITY);
  }
  return result;
}

inline void set_command(JsonDocument& doc)
{
  doc["sequence"] = COMMAND_SEQUENCE;
  doc["move_to"] = MOVE_TO;
}

inline bool compare_command(JsonDocument& doc)
{
  return (doc["sequence"].as<uint32_t>() == COMMAND_SEQUENCE) && doc["move_to"].is<int32_t>()
         && (doc["move_to"].as<int32_t>() == MOVE_TO);
}

inline void set_ack(JsonDocument& doc)
{
  doc["sequence"] = COMMAND_SEQUENCE;
  doc["result"] = RESULT_ACCEPTED;
}

inline bool compare_ack(JsonDocument& doc)
{
  return (doc["sequence"].as<uint32_t>() == COMMAND_SEQUENCE) && (doc["result"].as<int32_t>() == RESULT_ACCEPTED);
}

// JSON has no bytes type. The chunk goes as an array of numbers, the plain JSON way; base64
// would need a codec that ArduinoJson does not ship.
inline void set_firmware_chunk(JsonDocument& doc)
{
  doc["offset"] = CHUNK_OFFSET;
  doc["total_size"] = IMAGE_TOTAL_SIZE;
  JsonArray data = doc["data"].to<JsonArray>();
  for(uint32_t i = 0; i < CHUNK_SIZE; ++i)
  {
    data.add(chunk_byte(i));
  }
  doc["crc32"] = CHUNK_CRC32;
}

inline bool compare_firmware_chunk(JsonDocument& doc)
{
  JsonArrayConst data = doc["data"];
  bool result = (doc["offset"].as<uint32_t>() == CHUNK_OFFSET) && (doc["total_size"].as<uint32_t>() == IMAGE_TOTAL_SIZE)
                && (doc["crc32"].as<uint32_t>() == CHUNK_CRC32) && (data.size() == CHUNK_SIZE);
  for(uint32_t i = 0; result && (i < CHUNK_SIZE); ++i)
  {
    result = (data[i].as<uint8_t>() == chunk_byte(i));
  }
  return result;
}

} // End of namespace HelperPublishedArduinoJson

#endif // HELPER_PUBLISHED
