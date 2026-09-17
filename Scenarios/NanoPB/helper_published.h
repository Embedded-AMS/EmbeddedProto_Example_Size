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

#include "telemetry.pb.h"
#include "telemetry_batch.pb.h"
#include "configuration.pb.h"
#include "command.pb.h"
#include "firmware_chunk.pb.h"
#include <scenario_values.h>
#include <cstring>

// Set and check functions for the published scenarios, values from scenario_values.h.
namespace HelperPublishedNanoPB
{

using namespace ScenarioValues;

inline void set_telemetry(Telemetry& msg)
{
  msg.device_id = DEVICE_ID;
  msg.timestamp = TIMESTAMP;
  msg.temperature = TEMPERATURE;
  msg.humidity = HUMIDITY;
  msg.battery_mv = BATTERY_MV;
  msg.status = Status_WARNING;
  msg.rssi = RSSI;
}

inline bool compare_telemetry(const Telemetry& msg)
{
  return (msg.device_id == DEVICE_ID) && (msg.timestamp == TIMESTAMP)
         && (msg.temperature == TEMPERATURE) && (msg.humidity == HUMIDITY)
         && (msg.battery_mv == BATTERY_MV) && (msg.status == Status_WARNING)
         && (msg.rssi == RSSI);
}

inline void set_telemetry_batch(TelemetryBatch& msg)
{
  msg.device_id = DEVICE_ID;
  msg.timestamp = TIMESTAMP;
  msg.samples_count = SAMPLE_COUNT;
  for(uint32_t i = 0; i < SAMPLE_COUNT; ++i)
  {
    Sample& sample = msg.samples[i];
    sample.offset_ms = sample_offset_ms(i);
    sample.x = sample_x(i);
    sample.y = sample_y(i);
    sample.z = sample_z(i);
  }
}

inline bool compare_telemetry_batch(const TelemetryBatch& msg)
{
  bool result = (msg.device_id == DEVICE_ID) && (msg.timestamp == TIMESTAMP)
                && (msg.samples_count == SAMPLE_COUNT);
  for(uint32_t i = 0; result && (i < SAMPLE_COUNT); ++i)
  {
    const Sample& sample = msg.samples[i];
    result = (sample.offset_ms == sample_offset_ms(i)) && (sample.x == sample_x(i))
             && (sample.y == sample_y(i)) && (sample.z == sample_z(i));
  }
  return result;
}

inline void set_configuration(Configuration& msg)
{
  msg.version = CONFIG_VERSION;
  strncpy(msg.device_name, DEVICE_NAME, sizeof(msg.device_name) - 1);
  msg.has_report_interval_ms = true;
  msg.report_interval_ms = REPORT_INTERVAL_MS;
  msg.peripherals_count = PERIPHERAL_COUNT;
  for(uint32_t i = 0; i < PERIPHERAL_COUNT; ++i)
  {
    msg.peripherals[i].index = i;
  }
  msg.peripherals[0].which_kind = Peripheral_uart_tag;
  msg.peripherals[0].kind.uart.baud = UART0_BAUD;
  msg.peripherals[0].kind.uart.data_bits = UART0_DATA_BITS;
  msg.peripherals[0].kind.uart.parity = UART0_PARITY;
  msg.peripherals[1].which_kind = Peripheral_spi_tag;
  msg.peripherals[1].kind.spi.prescaler = SPI1_PRESCALER;
  msg.peripherals[1].kind.spi.cpol = SPI1_CPOL;
  msg.peripherals[1].kind.spi.cpha = SPI1_CPHA;
  msg.peripherals[2].which_kind = Peripheral_motor_tag;
  msg.peripherals[2].kind.motor.max_speed = MOTOR2_MAX_SPEED;
  msg.peripherals[2].kind.motor.invert = MOTOR2_INVERT;
  msg.peripherals[2].kind.motor.home_pin = MOTOR2_HOME_PIN;
  msg.peripherals[3].which_kind = Peripheral_uart_tag;
  msg.peripherals[3].kind.uart.baud = UART3_BAUD;
  msg.peripherals[3].kind.uart.data_bits = UART3_DATA_BITS;
  msg.peripherals[3].kind.uart.parity = UART3_PARITY;
}

inline bool compare_configuration(const Configuration& msg)
{
  bool result = (msg.version == CONFIG_VERSION) && (0 == strcmp(msg.device_name, DEVICE_NAME))
                && msg.has_report_interval_ms && (msg.report_interval_ms == REPORT_INTERVAL_MS)
                && (msg.peripherals_count == PERIPHERAL_COUNT);
  for(uint32_t i = 0; result && (i < PERIPHERAL_COUNT); ++i)
  {
    result = (msg.peripherals[i].index == i);
  }
  if(result)
  {
    const Peripheral& p0 = msg.peripherals[0];
    const Peripheral& p1 = msg.peripherals[1];
    const Peripheral& p2 = msg.peripherals[2];
    const Peripheral& p3 = msg.peripherals[3];
    result = (p0.which_kind == Peripheral_uart_tag) && (p0.kind.uart.baud == UART0_BAUD)
             && (p0.kind.uart.data_bits == UART0_DATA_BITS) && (p0.kind.uart.parity == UART0_PARITY)
             && (p1.which_kind == Peripheral_spi_tag) && (p1.kind.spi.prescaler == SPI1_PRESCALER)
             && (p1.kind.spi.cpol == SPI1_CPOL) && (p1.kind.spi.cpha == SPI1_CPHA)
             && (p2.which_kind == Peripheral_motor_tag) && (p2.kind.motor.max_speed == MOTOR2_MAX_SPEED)
             && (p2.kind.motor.invert == MOTOR2_INVERT) && (p2.kind.motor.home_pin == MOTOR2_HOME_PIN)
             && (p3.which_kind == Peripheral_uart_tag) && (p3.kind.uart.baud == UART3_BAUD)
             && (p3.kind.uart.data_bits == UART3_DATA_BITS) && (p3.kind.uart.parity == UART3_PARITY);
  }
  return result;
}

inline void set_command(DeviceCommand& msg)
{
  msg.sequence = COMMAND_SEQUENCE;
  msg.which_action = DeviceCommand_move_to_tag;
  msg.action.move_to = MOVE_TO;
}

inline bool compare_command(const DeviceCommand& msg)
{
  return (msg.sequence == COMMAND_SEQUENCE) && (msg.which_action == DeviceCommand_move_to_tag)
         && (msg.action.move_to == MOVE_TO);
}

inline void set_ack(DeviceAck& msg)
{
  msg.sequence = COMMAND_SEQUENCE;
  msg.result = Result_ACCEPTED;
}

inline bool compare_ack(const DeviceAck& msg)
{
  return (msg.sequence == COMMAND_SEQUENCE) && (msg.result == Result_ACCEPTED);
}

inline void set_firmware_chunk(FirmwareChunk& msg)
{
  msg.offset = CHUNK_OFFSET;
  msg.total_size = IMAGE_TOTAL_SIZE;
  msg.crc32 = CHUNK_CRC32;
  msg.data.size = CHUNK_SIZE;
  for(uint32_t i = 0; i < CHUNK_SIZE; ++i)
  {
    msg.data.bytes[i] = chunk_byte(i);
  }
}

inline bool compare_firmware_chunk(const FirmwareChunk& msg)
{
  bool result = (msg.offset == CHUNK_OFFSET) && (msg.total_size == IMAGE_TOTAL_SIZE)
                && (msg.crc32 == CHUNK_CRC32) && (msg.data.size == CHUNK_SIZE);
  for(uint32_t i = 0; result && (i < CHUNK_SIZE); ++i)
  {
    result = (msg.data.bytes[i] == chunk_byte(i));
  }
  return result;
}

// Every schema growth struct has the same members, one template serves all twenty.
template<class MSG>
void set_growth(MSG& msg, const uint32_t n)
{
  msg.id = growth_id(n);
  msg.value = growth_value(n);
  msg.measure = growth_measure(n);
  msg.flag = growth_flag(n);
  strncpy(msg.label, GROWTH_LABELS[n - 1], sizeof(msg.label) - 1);
}

template<class MSG>
bool compare_growth(const MSG& msg, const uint32_t n)
{
  return (msg.id == growth_id(n)) && (msg.value == growth_value(n))
         && (msg.measure == growth_measure(n)) && (msg.flag == growth_flag(n))
         && (0 == strcmp(msg.label, GROWTH_LABELS[n - 1]));
}

} // End of namespace HelperPublishedNanoPB

#endif // HELPER_PUBLISHED
