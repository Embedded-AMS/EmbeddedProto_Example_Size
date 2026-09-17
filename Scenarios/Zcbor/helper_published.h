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

#include <cbor_messages_types.h>
#include <scenario_values.h>
#include <cstring>

// Set and check functions for the published scenarios, values from scenario_values.h.
// Every entry is marked present, so the maps carry the same fields as the protobuf messages.
namespace HelperPublishedZcbor
{

using namespace ScenarioValues;

#define ZCBOR_SET(msg, field, value) do { (msg).field.field = (value); (msg).field##_present = true; } while(0)
#define ZCBOR_CHECK(msg, field, value) ((msg).field##_present && ((msg).field.field == (value)))

inline void set_telemetry(struct Telemetry& msg)
{
  ZCBOR_SET(msg, device_id, DEVICE_ID);
  ZCBOR_SET(msg, timestamp, TIMESTAMP);
  ZCBOR_SET(msg, temperature, TEMPERATURE);
  ZCBOR_SET(msg, humidity, HUMIDITY);
  ZCBOR_SET(msg, battery_mv, BATTERY_MV);
  ZCBOR_SET(msg, status, STATUS_WARNING);
  ZCBOR_SET(msg, rssi, RSSI);
}

inline bool compare_telemetry(const struct Telemetry& msg)
{
  return ZCBOR_CHECK(msg, device_id, DEVICE_ID) && ZCBOR_CHECK(msg, timestamp, TIMESTAMP)
         && ZCBOR_CHECK(msg, temperature, TEMPERATURE) && ZCBOR_CHECK(msg, humidity, HUMIDITY)
         && ZCBOR_CHECK(msg, battery_mv, BATTERY_MV) && ZCBOR_CHECK(msg, status, STATUS_WARNING)
         && ZCBOR_CHECK(msg, rssi, RSSI);
}

inline void set_telemetry_batch(struct TelemetryBatch& msg)
{
  ZCBOR_SET(msg, device_id, DEVICE_ID);
  ZCBOR_SET(msg, timestamp, TIMESTAMP);
  msg.samples_present = true;
  msg.samples.Sample_m_count = SAMPLE_COUNT;
  for(uint32_t i = 0; i < SAMPLE_COUNT; ++i)
  {
    struct Sample& sample = msg.samples.Sample_m[i];
    ZCBOR_SET(sample, offset_ms, sample_offset_ms(i));
    ZCBOR_SET(sample, x, sample_x(i));
    ZCBOR_SET(sample, y, sample_y(i));
    ZCBOR_SET(sample, z, sample_z(i));
  }
}

inline bool compare_telemetry_batch(const struct TelemetryBatch& msg)
{
  bool result = ZCBOR_CHECK(msg, device_id, DEVICE_ID) && ZCBOR_CHECK(msg, timestamp, TIMESTAMP)
                && msg.samples_present && (msg.samples.Sample_m_count == SAMPLE_COUNT);
  for(uint32_t i = 0; result && (i < SAMPLE_COUNT); ++i)
  {
    const struct Sample& sample = msg.samples.Sample_m[i];
    result = ZCBOR_CHECK(sample, offset_ms, sample_offset_ms(i)) && ZCBOR_CHECK(sample, x, sample_x(i))
             && ZCBOR_CHECK(sample, y, sample_y(i)) && ZCBOR_CHECK(sample, z, sample_z(i));
  }
  return result;
}

inline void set_configuration(struct Configuration& msg)
{
  ZCBOR_SET(msg, version, CONFIG_VERSION);
  msg.device_name.device_name.value = reinterpret_cast<const uint8_t*>(DEVICE_NAME);
  msg.device_name.device_name.len = DEVICE_NAME_LENGTH;
  msg.device_name_present = true;
  ZCBOR_SET(msg, report_interval_ms, REPORT_INTERVAL_MS);
  msg.peripherals_present = true;
  msg.peripherals.Peripheral_m_count = PERIPHERAL_COUNT;
  struct Peripheral* p = msg.peripherals.Peripheral_m;
  for(uint32_t i = 0; i < PERIPHERAL_COUNT; ++i)
  {
    ZCBOR_SET(p[i], index, i);
    p[i].kind_present = true;
  }
  p[0].kind.kind_choice = Peripheral_kind_r::kind_uart_c;
  ZCBOR_SET(p[0].kind.uart, baud, UART0_BAUD);
  ZCBOR_SET(p[0].kind.uart, data_bits, UART0_DATA_BITS);
  ZCBOR_SET(p[0].kind.uart, parity, UART0_PARITY);
  p[1].kind.kind_choice = Peripheral_kind_r::kind_spi_c;
  ZCBOR_SET(p[1].kind.spi, prescaler, SPI1_PRESCALER);
  ZCBOR_SET(p[1].kind.spi, cpol, SPI1_CPOL);
  ZCBOR_SET(p[1].kind.spi, cpha, SPI1_CPHA);
  p[2].kind.kind_choice = Peripheral_kind_r::kind_motor_c;
  ZCBOR_SET(p[2].kind.motor, max_speed, MOTOR2_MAX_SPEED);
  ZCBOR_SET(p[2].kind.motor, invert, MOTOR2_INVERT);
  ZCBOR_SET(p[2].kind.motor, home_pin, MOTOR2_HOME_PIN);
  p[3].kind.kind_choice = Peripheral_kind_r::kind_uart_c;
  ZCBOR_SET(p[3].kind.uart, baud, UART3_BAUD);
  ZCBOR_SET(p[3].kind.uart, data_bits, UART3_DATA_BITS);
  ZCBOR_SET(p[3].kind.uart, parity, UART3_PARITY);
}

inline bool compare_configuration(const struct Configuration& msg)
{
  bool result = ZCBOR_CHECK(msg, version, CONFIG_VERSION) && msg.device_name_present
                && (msg.device_name.device_name.len == DEVICE_NAME_LENGTH)
                && (0 == memcmp(msg.device_name.device_name.value, DEVICE_NAME, DEVICE_NAME_LENGTH))
                && ZCBOR_CHECK(msg, report_interval_ms, REPORT_INTERVAL_MS)
                && msg.peripherals_present && (msg.peripherals.Peripheral_m_count == PERIPHERAL_COUNT);
  const struct Peripheral* p = msg.peripherals.Peripheral_m;
  for(uint32_t i = 0; result && (i < PERIPHERAL_COUNT); ++i)
  {
    result = ZCBOR_CHECK(p[i], index, i) && p[i].kind_present;
  }
  if(result)
  {
    result = (p[0].kind.kind_choice == Peripheral_kind_r::kind_uart_c) && ZCBOR_CHECK(p[0].kind.uart, baud, UART0_BAUD)
             && ZCBOR_CHECK(p[0].kind.uart, data_bits, UART0_DATA_BITS) && ZCBOR_CHECK(p[0].kind.uart, parity, UART0_PARITY)
             && (p[1].kind.kind_choice == Peripheral_kind_r::kind_spi_c) && ZCBOR_CHECK(p[1].kind.spi, prescaler, SPI1_PRESCALER)
             && ZCBOR_CHECK(p[1].kind.spi, cpol, SPI1_CPOL) && ZCBOR_CHECK(p[1].kind.spi, cpha, SPI1_CPHA)
             && (p[2].kind.kind_choice == Peripheral_kind_r::kind_motor_c) && ZCBOR_CHECK(p[2].kind.motor, max_speed, MOTOR2_MAX_SPEED)
             && ZCBOR_CHECK(p[2].kind.motor, invert, MOTOR2_INVERT) && ZCBOR_CHECK(p[2].kind.motor, home_pin, MOTOR2_HOME_PIN)
             && (p[3].kind.kind_choice == Peripheral_kind_r::kind_uart_c) && ZCBOR_CHECK(p[3].kind.uart, baud, UART3_BAUD)
             && ZCBOR_CHECK(p[3].kind.uart, data_bits, UART3_DATA_BITS) && ZCBOR_CHECK(p[3].kind.uart, parity, UART3_PARITY);
  }
  return result;
}

inline void set_command(struct DeviceCommand& msg)
{
  ZCBOR_SET(msg, sequence, COMMAND_SEQUENCE);
  msg.action_present = true;
  msg.action.action_choice = DeviceCommand_action_r::action_move_to_c;
  msg.action.move_to = MOVE_TO;
}

inline bool compare_command(const struct DeviceCommand& msg)
{
  return ZCBOR_CHECK(msg, sequence, COMMAND_SEQUENCE) && msg.action_present
         && (msg.action.action_choice == DeviceCommand_action_r::action_move_to_c) && (msg.action.move_to == MOVE_TO);
}

inline void set_ack(struct DeviceAck& msg)
{
  ZCBOR_SET(msg, sequence, COMMAND_SEQUENCE);
  ZCBOR_SET(msg, result, RESULT_ACCEPTED);
}

inline bool compare_ack(const struct DeviceAck& msg)
{
  return ZCBOR_CHECK(msg, sequence, COMMAND_SEQUENCE) && ZCBOR_CHECK(msg, result, RESULT_ACCEPTED);
}

// zcbor does not copy bytes: the struct points at the caller's array on encode and into
// the received buffer on decode.
inline void set_firmware_chunk(struct FirmwareChunk& msg, const uint8_t* data)
{
  ZCBOR_SET(msg, offset, CHUNK_OFFSET);
  ZCBOR_SET(msg, total_size, IMAGE_TOTAL_SIZE);
  ZCBOR_SET(msg, crc32, CHUNK_CRC32);
  msg.data.data.value = data;
  msg.data.data.len = CHUNK_SIZE;
  msg.data_present = true;
}

inline bool compare_firmware_chunk(const struct FirmwareChunk& msg)
{
  bool result = ZCBOR_CHECK(msg, offset, CHUNK_OFFSET) && ZCBOR_CHECK(msg, total_size, IMAGE_TOTAL_SIZE)
                && ZCBOR_CHECK(msg, crc32, CHUNK_CRC32) && msg.data_present && (msg.data.data.len == CHUNK_SIZE);
  for(uint32_t i = 0; result && (i < CHUNK_SIZE); ++i)
  {
    result = (msg.data.data.value[i] == chunk_byte(i));
  }
  return result;
}

// Every schema growth struct has the same members, one template serves all twenty.
template<class MSG>
void set_growth(MSG& msg, const uint32_t n)
{
  ZCBOR_SET(msg, id, growth_id(n));
  ZCBOR_SET(msg, value, growth_value(n));
  ZCBOR_SET(msg, measure, growth_measure(n));
  ZCBOR_SET(msg, flag, growth_flag(n));
  msg.label.label.value = reinterpret_cast<const uint8_t*>(GROWTH_LABELS[n - 1]);
  msg.label.label.len = GROWTH_LABEL_LENGTH;
  msg.label_present = true;
}

template<class MSG>
bool compare_growth(const MSG& msg, const uint32_t n)
{
  return ZCBOR_CHECK(msg, id, growth_id(n)) && ZCBOR_CHECK(msg, value, growth_value(n))
         && ZCBOR_CHECK(msg, measure, growth_measure(n)) && ZCBOR_CHECK(msg, flag, growth_flag(n))
         && msg.label_present && (msg.label.label.len == GROWTH_LABEL_LENGTH)
         && (0 == memcmp(msg.label.label.value, GROWTH_LABELS[n - 1], GROWTH_LABEL_LENGTH));
}

} // End of namespace HelperPublishedZcbor

#endif // HELPER_PUBLISHED
