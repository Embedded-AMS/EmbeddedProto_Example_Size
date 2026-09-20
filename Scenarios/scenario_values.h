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

#ifndef SCENARIO_VALUES_H
#define SCENARIO_VALUES_H

#include <cstdint>

// The values every library sets and checks in the published scenarios. One header for all
// libraries, so the bytes on the wire and the work done are identical by construction.
namespace ScenarioValues
{

// Telemetry
constexpr uint32_t DEVICE_ID = 4711;
constexpr uint64_t TIMESTAMP = 1726560000123ULL;
constexpr float TEMPERATURE = 21.5f;
constexpr float HUMIDITY = 48.25f;
constexpr uint32_t BATTERY_MV = 3712;
constexpr int32_t STATUS_WARNING = 1;
constexpr int32_t RSSI = -67;

// Telemetry batch
constexpr uint32_t SAMPLE_COUNT = 20;
// No sample value is zero: a zero is a default protobuf omits, and -0.0f is a zero to
// one library and not to another.
constexpr uint32_t sample_offset_ms(uint32_t i) { return (i + 1) * 50; }
constexpr float sample_x(uint32_t i) { return 0.125f * static_cast<float>(i + 1); }
constexpr float sample_y(uint32_t i) { return -0.25f * static_cast<float>(i + 1); }
constexpr float sample_z(uint32_t i) { return 9.75f + 0.0625f * static_cast<float>(i); }

// Configuration
constexpr uint32_t CONFIG_VERSION = 3;
constexpr const char* DEVICE_NAME = "sensor-node-07";
constexpr uint32_t DEVICE_NAME_LENGTH = 14;
constexpr uint32_t PERIPHERAL_COUNT = 4;
constexpr uint32_t REPORT_INTERVAL_MS = 1000;
// Peripheral 0 and 3 are UARTs, 1 is an SPI, 2 is a motor.
constexpr uint32_t UART0_BAUD = 115200;
constexpr uint32_t UART0_DATA_BITS = 8;
constexpr bool UART0_PARITY = false;
constexpr uint32_t SPI1_PRESCALER = 8;
constexpr bool SPI1_CPOL = false;
constexpr bool SPI1_CPHA = true;
constexpr uint32_t MOTOR2_MAX_SPEED = 1200;
constexpr bool MOTOR2_INVERT = true;
constexpr uint32_t MOTOR2_HOME_PIN = 25;
constexpr uint32_t UART3_BAUD = 9600;
constexpr uint32_t UART3_DATA_BITS = 8;
constexpr bool UART3_PARITY = true;

// Command and acknowledgement
constexpr uint32_t COMMAND_SEQUENCE = 77;
constexpr int32_t MOVE_TO = -1500;
constexpr int32_t RESULT_ACCEPTED = 1;

// Firmware chunk
constexpr uint32_t CHUNK_OFFSET = 65536;
constexpr uint32_t IMAGE_TOTAL_SIZE = 262144;
constexpr uint32_t CHUNK_SIZE = 256;
constexpr uint32_t WINDOW_SIZE = 32;
constexpr uint32_t CHUNK_CRC32 = 0xDEADBEEF;
constexpr uint8_t chunk_byte(uint32_t i) { return static_cast<uint8_t>((i * 31) + 7); }

// Schema growth, message type n of 1..20
constexpr uint32_t growth_id(uint32_t n) { return n; }
constexpr int32_t growth_value(uint32_t n) { return -100 * static_cast<int32_t>(n); }
constexpr float growth_measure(uint32_t n) { return 1.5f * static_cast<float>(n); }
constexpr bool growth_flag(uint32_t n) { return 0 != (n & 1); }
constexpr const char* GROWTH_LABELS[20] = {
  "type_01", "type_02", "type_03", "type_04", "type_05", "type_06", "type_07", "type_08", "type_09", "type_10",
  "type_11", "type_12", "type_13", "type_14", "type_15", "type_16", "type_17", "type_18", "type_19", "type_20" };
constexpr uint32_t GROWTH_LABEL_LENGTH = 7;

} // End of namespace ScenarioValues

#endif // SCENARIO_VALUES_H
