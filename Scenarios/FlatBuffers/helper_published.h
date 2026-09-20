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

#include <telemetry_builder.h>
#include <telemetry_reader.h>
#include <telemetry_batch_builder.h>
#include <telemetry_batch_reader.h>
#include <configuration_builder.h>
#include <configuration_reader.h>
#include <command_builder.h>
#include <command_reader.h>
#include <firmware_chunk_builder.h>
#include <firmware_chunk_reader.h>
#include <scenario_values.h>
#include <cstring>

// Build and check functions for the published scenarios, values from scenario_values.h.
// The build functions write the whole root with the generated create helpers where every
// field is known, the flatcc idiom.
namespace HelperPublishedFlatBuffers
{

using namespace ScenarioValues;

inline void create_telemetry(flatcc_builder_t* B)
{
  Telemetry_create_as_root(B, DEVICE_ID, TIMESTAMP, TEMPERATURE, HUMIDITY, BATTERY_MV, Status_WARNING, RSSI);
}

inline bool compare_telemetry(Telemetry_table_t msg)
{
  return (Telemetry_device_id(msg) == DEVICE_ID) && (Telemetry_timestamp(msg) == TIMESTAMP)
         && (Telemetry_temperature(msg) == TEMPERATURE) && (Telemetry_humidity(msg) == HUMIDITY)
         && (Telemetry_battery_mv(msg) == BATTERY_MV) && (Telemetry_status(msg) == Status_WARNING)
         && (Telemetry_rssi(msg) == RSSI);
}

inline void create_telemetry_batch(flatcc_builder_t* B)
{
  TelemetryBatch_start_as_root(B);
  TelemetryBatch_device_id_add(B, DEVICE_ID);
  TelemetryBatch_timestamp_add(B, TIMESTAMP);
  TelemetryBatch_samples_start(B);
  for(uint32_t i = 0; i < SAMPLE_COUNT; ++i)
  {
    TelemetryBatch_samples_push_create(B, sample_offset_ms(i), sample_x(i), sample_y(i), sample_z(i));
  }
  TelemetryBatch_samples_end(B);
  TelemetryBatch_end_as_root(B);
}

inline bool compare_telemetry_batch(TelemetryBatch_table_t msg)
{
  Sample_vec_t samples = TelemetryBatch_samples(msg);
  bool result = (TelemetryBatch_device_id(msg) == DEVICE_ID) && (TelemetryBatch_timestamp(msg) == TIMESTAMP)
                && (Sample_vec_len(samples) == SAMPLE_COUNT);
  for(uint32_t i = 0; result && (i < SAMPLE_COUNT); ++i)
  {
    Sample_table_t sample = Sample_vec_at(samples, i);
    result = (Sample_offset_ms(sample) == sample_offset_ms(i)) && (Sample_x(sample) == sample_x(i))
             && (Sample_y(sample) == sample_y(i)) && (Sample_z(sample) == sample_z(i));
  }
  return result;
}

inline void create_configuration(flatcc_builder_t* B)
{
  Configuration_start_as_root(B);
  Configuration_version_add(B, CONFIG_VERSION);
  Configuration_device_name_create_str(B, DEVICE_NAME);
  Configuration_report_interval_ms_add(B, REPORT_INTERVAL_MS);
  Configuration_peripherals_start(B);
  Configuration_peripherals_push_start(B);
  Peripheral_index_add(B, 0);
  Peripheral_kind_UartConfig_add(B, UartConfig_create(B, UART0_BAUD, UART0_DATA_BITS, UART0_PARITY));
  Configuration_peripherals_push_end(B);
  Configuration_peripherals_push_start(B);
  Peripheral_index_add(B, 1);
  Peripheral_kind_SpiConfig_add(B, SpiConfig_create(B, SPI1_PRESCALER, SPI1_CPOL, SPI1_CPHA));
  Configuration_peripherals_push_end(B);
  Configuration_peripherals_push_start(B);
  Peripheral_index_add(B, 2);
  Peripheral_kind_MotorConfig_add(B, MotorConfig_create(B, MOTOR2_MAX_SPEED, MOTOR2_INVERT, MOTOR2_HOME_PIN));
  Configuration_peripherals_push_end(B);
  Configuration_peripherals_push_start(B);
  Peripheral_index_add(B, 3);
  Peripheral_kind_UartConfig_add(B, UartConfig_create(B, UART3_BAUD, UART3_DATA_BITS, UART3_PARITY));
  Configuration_peripherals_push_end(B);
  Configuration_peripherals_end(B);
  Configuration_end_as_root(B);
}

inline bool compare_configuration(Configuration_table_t msg)
{
  Peripheral_vec_t peripherals = Configuration_peripherals(msg);
  flatbuffers_string_t name = Configuration_device_name(msg);
  bool result = (Configuration_version(msg) == CONFIG_VERSION) && (nullptr != name) && (0 == strcmp(name, DEVICE_NAME))
                && Configuration_report_interval_ms_is_present(msg) && (Configuration_report_interval_ms(msg) == REPORT_INTERVAL_MS)
                && (Peripheral_vec_len(peripherals) == PERIPHERAL_COUNT);
  for(uint32_t i = 0; result && (i < PERIPHERAL_COUNT); ++i)
  {
    result = (Peripheral_index(Peripheral_vec_at(peripherals, i)) == i);
  }
  if(result)
  {
    Peripheral_table_t p0 = Peripheral_vec_at(peripherals, 0);
    Peripheral_table_t p1 = Peripheral_vec_at(peripherals, 1);
    Peripheral_table_t p2 = Peripheral_vec_at(peripherals, 2);
    Peripheral_table_t p3 = Peripheral_vec_at(peripherals, 3);
    result = (Peripheral_kind_type(p0) == Kind_UartConfig) && (Peripheral_kind_type(p1) == Kind_SpiConfig)
             && (Peripheral_kind_type(p2) == Kind_MotorConfig) && (Peripheral_kind_type(p3) == Kind_UartConfig);
    if(result)
    {
      UartConfig_table_t uart0 = static_cast<UartConfig_table_t>(Peripheral_kind(p0));
      SpiConfig_table_t spi1 = static_cast<SpiConfig_table_t>(Peripheral_kind(p1));
      MotorConfig_table_t motor2 = static_cast<MotorConfig_table_t>(Peripheral_kind(p2));
      UartConfig_table_t uart3 = static_cast<UartConfig_table_t>(Peripheral_kind(p3));
      result = (UartConfig_baud(uart0) == UART0_BAUD) && (UartConfig_data_bits(uart0) == UART0_DATA_BITS) && (UartConfig_parity(uart0) == UART0_PARITY)
               && (SpiConfig_prescaler(spi1) == SPI1_PRESCALER) && (SpiConfig_cpol(spi1) == SPI1_CPOL) && (SpiConfig_cpha(spi1) == SPI1_CPHA)
               && (MotorConfig_max_speed(motor2) == MOTOR2_MAX_SPEED) && (MotorConfig_invert(motor2) == MOTOR2_INVERT) && (MotorConfig_home_pin(motor2) == MOTOR2_HOME_PIN)
               && (UartConfig_baud(uart3) == UART3_BAUD) && (UartConfig_data_bits(uart3) == UART3_DATA_BITS) && (UartConfig_parity(uart3) == UART3_PARITY);
    }
  }
  return result;
}

inline void create_command(flatcc_builder_t* B)
{
  DeviceCommand_start_as_root(B);
  DeviceCommand_sequence_add(B, COMMAND_SEQUENCE);
  DeviceCommand_action_MoveTo_add(B, MoveTo_create(B, MOVE_TO));
  DeviceCommand_end_as_root(B);
}

inline bool compare_command(DeviceCommand_table_t msg)
{
  bool result = (DeviceCommand_sequence(msg) == COMMAND_SEQUENCE) && (DeviceCommand_action_type(msg) == Action_MoveTo);
  if(result)
  {
    MoveTo_table_t move = static_cast<MoveTo_table_t>(DeviceCommand_action(msg));
    result = (MoveTo_value(move) == MOVE_TO);
  }
  return result;
}

inline void create_ack(flatcc_builder_t* B)
{
  DeviceAck_create_as_root(B, COMMAND_SEQUENCE, Result_ACCEPTED);
}

inline bool compare_ack(DeviceAck_table_t msg)
{
  return (DeviceAck_sequence(msg) == COMMAND_SEQUENCE) && (DeviceAck_result(msg) == Result_ACCEPTED);
}

inline void create_firmware_chunk(flatcc_builder_t* B)
{
  uint8_t data[CHUNK_SIZE];
  for(uint32_t i = 0; i < CHUNK_SIZE; ++i)
  {
    data[i] = chunk_byte(i);
  }
  FirmwareChunk_start_as_root(B);
  FirmwareChunk_offset_add(B, CHUNK_OFFSET);
  FirmwareChunk_total_size_add(B, IMAGE_TOTAL_SIZE);
  FirmwareChunk_data_create(B, data, CHUNK_SIZE);
  FirmwareChunk_crc32_add(B, CHUNK_CRC32);
  FirmwareChunk_end_as_root(B);
}

inline bool compare_firmware_chunk(FirmwareChunk_table_t msg)
{
  flatbuffers_uint8_vec_t data = FirmwareChunk_data(msg);
  bool result = (FirmwareChunk_offset(msg) == CHUNK_OFFSET) && (FirmwareChunk_total_size(msg) == IMAGE_TOTAL_SIZE)
                && (FirmwareChunk_crc32(msg) == CHUNK_CRC32) && (flatbuffers_uint8_vec_len(data) == CHUNK_SIZE);
  for(uint32_t i = 0; result && (i < CHUNK_SIZE); ++i)
  {
    result = (flatbuffers_uint8_vec_at(data, i) == chunk_byte(i));
  }
  return result;
}

} // End of namespace HelperPublishedFlatBuffers

#endif // HELPER_PUBLISHED
