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

#include <complete_demo_builder.h>
#include <complete_demo_reader.h>

// The same values, counts and layout as the Embedded Proto and nanopb helpers: five
// configurations alternating motor (even index) and GPIO (odd index), one motor command,
// five GPIO states and a measurement of 25 samples.
namespace HelperCompleteDemoFlatBuffers
{

constexpr uint32_t CONFIGURATION_COUNT = 5;
constexpr uint32_t STATES_COUNT = 5;
constexpr uint32_t MEASUREMENT_COUNT = 25;

inline void add_config_with_gpio(flatcc_builder_t* B)
{
  Config_peripheral_Config_GPIO_start(B);
  Config_GPIO_pin_add(B, 13);
  Config_GPIO_mode_add(B, Config_GPIO_MODE_OUTPUT_PP);
  Config_peripheral_Config_GPIO_end(B);
}

inline void add_config_with_motor(flatcc_builder_t* B)
{
  Config_device_Config_Motor_start(B);
  Config_Motor_spi_id_add(B, 1);
  Config_Motor_enabled_add(B, true);
  Config_Motor_enable_micro_stepping_add(B, true);
  Config_Motor_home_direction_inverted_add(B, false);
  Config_Motor_home_switch_pin_add(B, 25);
  Config_device_Config_Motor_end(B);
}

// Adds the configuration vector to the EnvelopToMCU table the builder has open.
inline void add_configurations(flatcc_builder_t* B)
{
  EnvelopToMCU_configuration_start(B);
  for(uint32_t i = 0; i < CONFIGURATION_COUNT; ++i)
  {
    EnvelopToMCU_configuration_push_start(B);
    if(0x01 & i)
    {
      add_config_with_gpio(B);
    }
    else
    {
      add_config_with_motor(B);
    }
    EnvelopToMCU_configuration_push_end(B);
  }
  EnvelopToMCU_configuration_end(B);
}

// Adds the motor command to the EnvelopToMCU table the builder has open.
inline void add_motor_command(flatcc_builder_t* B)
{
  EnvelopToMCU_command_start(B);
  Command_command_id_add(B, 0);
  Command_command_Command_Motor_start(B);
  Command_Motor_target_position_add(B, 5000);
  Command_Motor_target_speed_add(B, 1000);
  Command_Motor_enable_add(B, true);
  Command_Motor_home_add(B, false);
  Command_Motor_emergency_stop_add(B, false);
  Command_Motor_clear_fault_add(B, false);
  Command_Motor_acceleration_add(B, 2000);
  Command_Motor_deceleration_add(B, 2000);
  Command_command_Command_Motor_end(B);
  EnvelopToMCU_command_end(B);
}

// Adds the states to the EnvelopFromMCU table the builder has open.
inline void add_states(flatcc_builder_t* B)
{
  EnvelopFromMCU_data_States_start(B);
  States_states_start(B);
  for(uint32_t i = 0; i < STATES_COUNT; ++i)
  {
    States_states_push_start(B);
    State_timestamp_add(B, 1000000);
    State_state_State_GPIO_start(B);
    State_GPIO_value_add(B, true);
    State_GPIO_last_change_timestamp_add(B, 123456789);
    State_state_State_GPIO_end(B);
    States_states_push_end(B);
  }
  States_states_end(B);
  EnvelopFromMCU_data_States_end(B);
}

inline bool compare_states(EnvelopFromMCU_table_t envelop)
{
  if(EnvelopFromMCU_data_type(envelop) != EnvelopFromMCU_Data_States)
  {
    return false;
  }
  States_table_t states = static_cast<States_table_t>(EnvelopFromMCU_data(envelop));
  State_vec_t vector = States_states(states);
  if(State_vec_len(vector) != STATES_COUNT)
  {
    return false;
  }
  for(uint32_t i = 0; i < STATES_COUNT; ++i)
  {
    State_table_t state = State_vec_at(vector, i);
    if((State_timestamp(state) != 1000000) || (State_state_type(state) != State_State_State_GPIO))
    {
      return false;
    }
    State_GPIO_table_t gpio = static_cast<State_GPIO_table_t>(State_state(state));
    if((State_GPIO_value(gpio) != true) || (State_GPIO_last_change_timestamp(gpio) != 123456789))
    {
      return false;
    }
  }
  return true;
}

// Adds the measurement to the EnvelopFromMCU table the builder has open.
inline void add_measurement(flatcc_builder_t* B)
{
  EnvelopFromMCU_data_Measurement_start(B);
  // Fill arrays and hand them over in one call each, as the other libraries fill their array fields.
  float data[MEASUREMENT_COUNT];
  uint64_t time[MEASUREMENT_COUNT];
  for(uint32_t i = 0; i < MEASUREMENT_COUNT; ++i)
  {
    data[i] = 1.23f * i;
    time[i] = 100 * i;
  }
  Measurement_data_create(B, data, MEASUREMENT_COUNT);
  Measurement_time_create(B, time, MEASUREMENT_COUNT);
  EnvelopFromMCU_data_Measurement_end(B);
}

inline bool compare_measurement(EnvelopFromMCU_table_t envelop)
{
  if(EnvelopFromMCU_data_type(envelop) != EnvelopFromMCU_Data_Measurement)
  {
    return false;
  }
  Measurement_table_t measurement = static_cast<Measurement_table_t>(EnvelopFromMCU_data(envelop));
  flatbuffers_float_vec_t data = Measurement_data(measurement);
  flatbuffers_uint64_vec_t time = Measurement_time(measurement);
  if((flatbuffers_float_vec_len(data) != MEASUREMENT_COUNT) || (flatbuffers_uint64_vec_len(time) != MEASUREMENT_COUNT))
  {
    return false;
  }
  for(uint32_t i = 0; i < MEASUREMENT_COUNT; ++i)
  {
    if((flatbuffers_float_vec_at(data, i) != 1.23f * i) || (flatbuffers_uint64_vec_at(time, i) != 100 * i))
    {
      return false;
    }
  }
  return true;
}

} // End of namespace HelperCompleteDemoFlatBuffers

#endif // HELPER_COMPLETE_DEMO
