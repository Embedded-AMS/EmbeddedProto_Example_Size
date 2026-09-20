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

#include <telemetry.h>
#include <telemetry_batch.h>
#include <configuration.h>
#include <command.h>
#include <firmware_chunk.h>
#include <scenario_values.h>
#include <cstring>

// Set and check functions for the published scenarios, values from scenario_values.h.
namespace HelperPublished
{

using namespace ScenarioValues;

inline void set_telemetry(Telemetry& msg)
{
  msg.set_device_id(DEVICE_ID);
  msg.set_timestamp(TIMESTAMP);
  msg.set_temperature(TEMPERATURE);
  msg.set_humidity(HUMIDITY);
  msg.set_battery_mv(BATTERY_MV);
  msg.set_status(Status::WARNING);
  msg.set_rssi(RSSI);
}

inline bool compare_telemetry(const Telemetry& msg)
{
  return (msg.get_device_id() == DEVICE_ID) && (msg.get_timestamp() == TIMESTAMP)
         && (msg.get_temperature() == TEMPERATURE) && (msg.get_humidity() == HUMIDITY)
         && (msg.get_battery_mv() == BATTERY_MV) && (msg.get_status() == Status::WARNING)
         && (msg.get_rssi() == RSSI);
}

inline void set_telemetry_batch(TelemetryBatch& msg)
{
  msg.set_device_id(DEVICE_ID);
  msg.set_timestamp(TIMESTAMP);
  for(uint32_t i = 0; i < SAMPLE_COUNT; ++i)
  {
    Sample& sample = msg.mutable_samples(i);
    sample.set_offset_ms(sample_offset_ms(i));
    sample.set_x(sample_x(i));
    sample.set_y(sample_y(i));
    sample.set_z(sample_z(i));
  }
}

inline bool compare_telemetry_batch(const TelemetryBatch& msg)
{
  bool result = (msg.get_device_id() == DEVICE_ID) && (msg.get_timestamp() == TIMESTAMP)
                && (msg.get_samples().get_length() == SAMPLE_COUNT);
  for(uint32_t i = 0; result && (i < SAMPLE_COUNT); ++i)
  {
    const Sample& sample = msg.samples(i);
    result = (sample.get_offset_ms() == sample_offset_ms(i)) && (sample.get_x() == sample_x(i))
             && (sample.get_y() == sample_y(i)) && (sample.get_z() == sample_z(i));
  }
  return result;
}

inline void set_configuration(Configuration& msg)
{
  msg.set_version(CONFIG_VERSION);
  msg.mutable_device_name() = DEVICE_NAME;
  msg.set_report_interval_ms(REPORT_INTERVAL_MS);
  for(uint32_t i = 0; i < PERIPHERAL_COUNT; ++i)
  {
    msg.mutable_peripherals(i).set_index(i);
  }
  UartConfig& uart0 = msg.mutable_peripherals(0).mutable_uart();
  uart0.set_baud(UART0_BAUD);
  uart0.set_data_bits(UART0_DATA_BITS);
  uart0.set_parity(UART0_PARITY);
  SpiConfig& spi1 = msg.mutable_peripherals(1).mutable_spi();
  spi1.set_prescaler(SPI1_PRESCALER);
  spi1.set_cpol(SPI1_CPOL);
  spi1.set_cpha(SPI1_CPHA);
  MotorConfig& motor2 = msg.mutable_peripherals(2).mutable_motor();
  motor2.set_max_speed(MOTOR2_MAX_SPEED);
  motor2.set_invert(MOTOR2_INVERT);
  motor2.set_home_pin(MOTOR2_HOME_PIN);
  UartConfig& uart3 = msg.mutable_peripherals(3).mutable_uart();
  uart3.set_baud(UART3_BAUD);
  uart3.set_data_bits(UART3_DATA_BITS);
  uart3.set_parity(UART3_PARITY);
}

inline bool compare_configuration(const Configuration& msg)
{
  bool result = (msg.get_version() == CONFIG_VERSION) && (msg.get_device_name() == DEVICE_NAME)
                && msg.has_report_interval_ms() && (msg.get_report_interval_ms() == REPORT_INTERVAL_MS)
                && (msg.get_peripherals().get_length() == PERIPHERAL_COUNT);
  for(uint32_t i = 0; result && (i < PERIPHERAL_COUNT); ++i)
  {
    result = (msg.peripherals(i).get_index() == i);
  }
  if(result)
  {
    const Peripheral& p0 = msg.peripherals(0);
    const Peripheral& p1 = msg.peripherals(1);
    const Peripheral& p2 = msg.peripherals(2);
    const Peripheral& p3 = msg.peripherals(3);
    result = (p0.get_which_kind() == Peripheral::FieldNumber::UART) && (p0.get_uart().get_baud() == UART0_BAUD)
             && (p0.get_uart().get_data_bits() == UART0_DATA_BITS) && (p0.get_uart().get_parity() == UART0_PARITY)
             && (p1.get_which_kind() == Peripheral::FieldNumber::SPI) && (p1.get_spi().get_prescaler() == SPI1_PRESCALER)
             && (p1.get_spi().get_cpol() == SPI1_CPOL) && (p1.get_spi().get_cpha() == SPI1_CPHA)
             && (p2.get_which_kind() == Peripheral::FieldNumber::MOTOR) && (p2.get_motor().get_max_speed() == MOTOR2_MAX_SPEED)
             && (p2.get_motor().get_invert() == MOTOR2_INVERT) && (p2.get_motor().get_home_pin() == MOTOR2_HOME_PIN)
             && (p3.get_which_kind() == Peripheral::FieldNumber::UART) && (p3.get_uart().get_baud() == UART3_BAUD)
             && (p3.get_uart().get_data_bits() == UART3_DATA_BITS) && (p3.get_uart().get_parity() == UART3_PARITY);
  }
  return result;
}

inline void set_command(DeviceCommand& msg)
{
  msg.set_sequence(COMMAND_SEQUENCE);
  msg.set_move_to(MOVE_TO);
}

inline bool compare_command(const DeviceCommand& msg)
{
  return (msg.get_sequence() == COMMAND_SEQUENCE) && (msg.get_which_action() == DeviceCommand::FieldNumber::MOVE_TO)
         && (msg.get_move_to() == MOVE_TO);
}

inline void set_ack(DeviceAck& msg)
{
  msg.set_sequence(COMMAND_SEQUENCE);
  msg.set_result(Result::ACCEPTED);
}

inline bool compare_ack(const DeviceAck& msg)
{
  return (msg.get_sequence() == COMMAND_SEQUENCE) && (msg.get_result() == Result::ACCEPTED);
}

inline void set_firmware_chunk(FirmwareChunk& msg)
{
  msg.set_offset(CHUNK_OFFSET);
  msg.set_total_size(IMAGE_TOTAL_SIZE);
  msg.set_crc32(CHUNK_CRC32);
  uint8_t data[CHUNK_SIZE];
  for(uint32_t i = 0; i < CHUNK_SIZE; ++i)
  {
    data[i] = chunk_byte(i);
  }
  msg.mutable_data().set(data, CHUNK_SIZE);
}

inline bool compare_firmware_chunk(const FirmwareChunk& msg)
{
  bool result = (msg.get_offset() == CHUNK_OFFSET) && (msg.get_total_size() == IMAGE_TOTAL_SIZE)
                && (msg.get_crc32() == CHUNK_CRC32) && (msg.get_data().get_length() == CHUNK_SIZE);
  for(uint32_t i = 0; result && (i < CHUNK_SIZE); ++i)
  {
    result = (msg.get_data()[i] == chunk_byte(i));
  }
  return result;
}

// Every schema growth message has the same shape, one template serves all twenty.
template<class MSG>
void set_growth(MSG& msg, const uint32_t n)
{
  msg.set_id(growth_id(n));
  msg.set_value(growth_value(n));
  msg.set_measure(growth_measure(n));
  msg.set_flag(growth_flag(n));
  msg.mutable_label() = GROWTH_LABELS[n - 1];
}

template<class MSG>
bool compare_growth(const MSG& msg, const uint32_t n)
{
  return (msg.get_id() == growth_id(n)) && (msg.get_value() == growth_value(n))
         && (msg.get_measure() == growth_measure(n)) && (msg.get_flag() == growth_flag(n))
         && (msg.get_label() == GROWTH_LABELS[n - 1]);
}

} // End of namespace HelperPublished

#endif // HELPER_PUBLISHED
