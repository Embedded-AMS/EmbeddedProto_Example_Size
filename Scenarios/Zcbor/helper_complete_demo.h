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

#ifndef HELPER_COMPLETE_DEMO
#define HELPER_COMPLETE_DEMO

#include <cbor_messages_types.h>

// The same values, counts and layout as the Embedded Proto and nanopb helpers: five
// configurations alternating motor (even index) and GPIO (odd index), one motor command,
// five GPIO states and a measurement of 25 samples.
namespace HelperCompleteDemoZcbor
{

constexpr uint32_t CONFIGURATION_COUNT = 5;
constexpr uint32_t STATES_COUNT = 5;
constexpr uint32_t MEASUREMENT_COUNT = 25;
constexpr int32_t GPIO_MODE_OUTPUT_PP = 2;

#define ZCBOR_SET(msg, field, value) do { (msg).field.field = (value); (msg).field##_present = true; } while(0)

inline void set_config_with_gpio(struct Config& config)
{
  config.peripheral_present = true;
  config.peripheral.peripheral_choice = Config_peripheral_r::peripheral_gpio_c;
  ZCBOR_SET(config.peripheral.gpio, pin, 13);
  ZCBOR_SET(config.peripheral.gpio, mode, GPIO_MODE_OUTPUT_PP);
}

inline void set_config_with_motor(struct Config& config)
{
  config.device_present = true;
  config.device.device_choice = Config_device_r::device_motor_c;
  ZCBOR_SET(config.device.motor, spi_id, 1);
  ZCBOR_SET(config.device.motor, enabled, true);
  ZCBOR_SET(config.device.motor, enable_micro_stepping, true);
  ZCBOR_SET(config.device.motor, home_direction_inverted, false);
  ZCBOR_SET(config.device.motor, home_switch_pin, 25);
}

inline void set_envelop_to_mcu_configurations(struct EnvelopToMCU& envelop)
{
  envelop.configuration_present = true;
  envelop.configuration.Config_m_count = CONFIGURATION_COUNT;
  for(uint32_t i = 0; i < CONFIGURATION_COUNT; ++i)
  {
    if(0x01 & i)
    {
      set_config_with_gpio(envelop.configuration.Config_m[i]);
    }
    else
    {
      set_config_with_motor(envelop.configuration.Config_m[i]);
    }
  }
}

inline void set_envelop_to_mcu_command(struct EnvelopToMCU& envelop)
{
  envelop.command_present = true;
  struct Command& command = envelop.command.command;
  ZCBOR_SET(command, command_id, 0);
  command.command_present = true;
  command.command.command_choice = Command_command_r::command_motor_c;
  ZCBOR_SET(command.command.motor, target_position, 5000);
  ZCBOR_SET(command.command.motor, target_speed, 1000);
  ZCBOR_SET(command.command.motor, enable, true);
  ZCBOR_SET(command.command.motor, home, false);
  ZCBOR_SET(command.command.motor, emergency_stop, false);
  ZCBOR_SET(command.command.motor, clear_fault, false);
  ZCBOR_SET(command.command.motor, acceleration, 2000);
  ZCBOR_SET(command.command.motor, deceleration, 2000);
}

inline void set_envelop_from_mcu_with_states(struct EnvelopFromMCU& envelop)
{
  envelop.data_present = true;
  envelop.data.data_choice = EnvelopFromMCU_data_r::data_states_c;
  struct States& states = envelop.data.states;
  states.states_present = true;
  states.states.State_m_count = STATES_COUNT;
  for(uint32_t i = 0; i < STATES_COUNT; ++i)
  {
    struct State& state = states.states.State_m[i];
    ZCBOR_SET(state, timestamp, 1000000);
    state.state_present = true;
    state.state.state_choice = State_state_r::state_gpio_c;
    ZCBOR_SET(state.state.gpio, value, true);
    ZCBOR_SET(state.state.gpio, last_change_timestamp, 123456789);
  }
}

inline bool compare_envelop_from_mcu_states(const struct EnvelopFromMCU& envelop)
{
  if(!envelop.data_present || (envelop.data.data_choice != EnvelopFromMCU_data_r::data_states_c))
  {
    return false;
  }
  const struct States& states = envelop.data.states;
  if(!states.states_present || (states.states.State_m_count != STATES_COUNT))
  {
    return false;
  }
  for(uint32_t i = 0; i < STATES_COUNT; ++i)
  {
    const struct State& state = states.states.State_m[i];
    if(!state.timestamp_present || (state.timestamp.timestamp != 1000000)
       || !state.state_present || (state.state.state_choice != State_state_r::state_gpio_c)
       || !state.state.gpio.value_present || (state.state.gpio.value.value != true)
       || !state.state.gpio.last_change_timestamp_present || (state.state.gpio.last_change_timestamp.last_change_timestamp != 123456789))
    {
      return false;
    }
  }
  return true;
}

inline void set_envelop_from_mcu_with_measurement(struct EnvelopFromMCU& envelop)
{
  envelop.data_present = true;
  envelop.data.data_choice = EnvelopFromMCU_data_r::data_measurement_c;
  struct Measurement& measurement = envelop.data.measurement;
  measurement.data_present = true;
  measurement.data.float32_count = MEASUREMENT_COUNT;
  measurement.timestamps_present = true;
  measurement.timestamps.uint_count = MEASUREMENT_COUNT;
  for(uint32_t i = 0; i < MEASUREMENT_COUNT; ++i)
  {
    measurement.data.float32[i] = 1.23f * i;
    measurement.timestamps.uint[i] = 100 * i;
  }
}

inline bool compare_envelop_from_mcu_measurement(const struct EnvelopFromMCU& envelop)
{
  if(!envelop.data_present || (envelop.data.data_choice != EnvelopFromMCU_data_r::data_measurement_c))
  {
    return false;
  }
  const struct Measurement& measurement = envelop.data.measurement;
  if(!measurement.data_present || (measurement.data.float32_count != MEASUREMENT_COUNT)
     || !measurement.timestamps_present || (measurement.timestamps.uint_count != MEASUREMENT_COUNT))
  {
    return false;
  }
  for(uint32_t i = 0; i < MEASUREMENT_COUNT; ++i)
  {
    if((measurement.data.float32[i] != 1.23f * i) || (measurement.timestamps.uint[i] != 100 * i))
    {
      return false;
    }
  }
  return true;
}

} // End of namespace HelperCompleteDemoZcbor

#endif // HELPER_COMPLETE_DEMO
