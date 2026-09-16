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

#include <complete_demo.h>
#include <limits>
#include "helper_complete_demo_config.h"
#include "helper_complete_demo_state.h"
#include "helper_complete_demo_command.h"

namespace HelperCompleteDemo
{

// Set demo values for States message with multiple state entries
template<uint32_t States_states_REP_LENGTH>
void set_states_demo(States<States_states_REP_LENGTH>& states_msg)
{
    // Clear any existing states
    states_msg.clear_states();

    for(uint32_t i = 0; i < States_states_REP_LENGTH; ++i)
    {
      HelperCompleteDemoState::set_state_with_gpio_demo(states_msg.mutable_states(i));
    }
}

// Set demo values for Measurement message
template<uint32_t Measurement_data_REP_LENGTH, uint32_t Measurement_time_REP_LENGTH>
void set_measurement_demo(Measurement<Measurement_data_REP_LENGTH, Measurement_time_REP_LENGTH>& measurement_msg)
{
    // Clear any existing data
    measurement_msg.clear_data();
    measurement_msg.clear_time();

    for(uint32_t i = 0; i < Measurement_data_REP_LENGTH; ++i)
    {
      measurement_msg.set_data(i, 1.23f * i);
      measurement_msg.set_time(i, 100 * i);
    }
}

// Set demo values for EnvelopToMCU configurations
template<uint32_t EnvelopToMCU_configuration_REP_LENGTH>
void set_envelop_to_mcu_configurations_demo(EnvelopToMCU<EnvelopToMCU_configuration_REP_LENGTH>& envelop_msg)
{
    // Clear any existing configurations
    envelop_msg.clear_configuration();

    for(uint32_t i = 0; i < EnvelopToMCU_configuration_REP_LENGTH; ++i)
    {
      if(0x01 & i)
      {
        HelperCompleteDemoConfig::set_config_with_gpio_demo(envelop_msg.mutable_configuration(i));
      }
      else
      {
        HelperCompleteDemoConfig::set_config_with_motor_demo(envelop_msg.mutable_configuration(i));
      }
    }
}

// Set demo values for EnvelopToMCU command
template<uint32_t EnvelopToMCU_configuration_REP_LENGTH>
void set_envelop_to_mcu_command_demo(EnvelopToMCU<EnvelopToMCU_configuration_REP_LENGTH>& envelop_msg)
{
    // Set a motor command
    HelperCompleteDemoCommand::set_command_with_motor_demo(envelop_msg.mutable_command());
}

// Set demo values for EnvelopToMCU with configuration and command
template<uint32_t EnvelopToMCU_configuration_REP_LENGTH>
void set_envelop_to_mcu_demo(EnvelopToMCU<EnvelopToMCU_configuration_REP_LENGTH>& envelop_msg)
{
    set_envelop_to_mcu_configurations_demo(envelop_msg);
    set_envelop_to_mcu_command_demo(envelop_msg);
}

// Set demo values for EnvelopFromMCU with states
template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
void set_envelop_from_mcu_with_states_demo(EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    // Get mutable states and set demo values
    States<EnvelopFromMCU_states_States_states_REP_LENGTH>& states = envelop_msg.mutable_states();
    set_states_demo(states);
}

// Set demo values for EnvelopFromMCU with measurement
template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
void set_envelop_from_mcu_with_measurement_demo(EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    // Get mutable measurement and set demo values
    Measurement<EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
                EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& measurement = envelop_msg.mutable_measurement();
    set_measurement_demo(measurement);
}

// Set minimum values for States message
template<uint32_t States_states_REP_LENGTH>
void set_states_min(States<States_states_REP_LENGTH>& states_msg)
{
    states_msg.clear_states();

    // Set one state with minimum values at index 0
    HelperCompleteDemoState::set_state_with_gpio_min(states_msg.mutable_states(0));
}

// Set maximum values for States message
template<uint32_t States_states_REP_LENGTH>
void set_states_max(States<States_states_REP_LENGTH>& states_msg)
{
    states_msg.clear_states();

    // Set one state with maximum values at index 0
    HelperCompleteDemoState::set_state_with_gpio_max(states_msg.mutable_states(0));
}

// Set average values for States message
template<uint32_t States_states_REP_LENGTH>
void set_states_average(States<States_states_REP_LENGTH>& states_msg)
{
    states_msg.clear_states();

    // Set one state with average values at index 0
    HelperCompleteDemoState::set_state_with_gpio_average(states_msg.mutable_states(0));
}

// Set minimum values for Measurement message
template<uint32_t Measurement_data_REP_LENGTH, uint32_t Measurement_time_REP_LENGTH>
void set_measurement_min(Measurement<Measurement_data_REP_LENGTH, Measurement_time_REP_LENGTH>& measurement_msg)
{
    static_assert(Measurement_data_REP_LENGTH != Measurement_time_REP_LENGTH, "Template parameters must be equal");

    measurement_msg.clear_data();
    measurement_msg.clear_time();

    for(uint32_t i = 0; i < Measurement_data_REP_LENGTH; ++i)
    {
      measurement_msg.set_data(i, std::numeric_limits<float>::lowest());
      measurement_msg.set_time(i, 100 * i);
    }
}

// Set maximum values for Measurement message
template<uint32_t Measurement_data_REP_LENGTH, uint32_t Measurement_time_REP_LENGTH>
void set_measurement_max(Measurement<Measurement_data_REP_LENGTH, Measurement_time_REP_LENGTH>& measurement_msg)
{
    static_assert(Measurement_data_REP_LENGTH != Measurement_time_REP_LENGTH, "Template parameters must be equal");

    measurement_msg.clear_data();
    measurement_msg.clear_time();

    for(uint32_t i = 0; i < Measurement_data_REP_LENGTH; ++i)
    {
      measurement_msg.set_data(i, std::numeric_limits<float>::max());
      measurement_msg.set_time(i, 100 * i);
    }
}

// Set average values for Measurement message
template<uint32_t Measurement_data_REP_LENGTH, uint32_t Measurement_time_REP_LENGTH>
void set_measurement_average(Measurement<Measurement_data_REP_LENGTH, Measurement_time_REP_LENGTH>& measurement_msg)
{
    static_assert(Measurement_data_REP_LENGTH != Measurement_time_REP_LENGTH, "Template parameters must be equal");

    measurement_msg.clear_data();
    measurement_msg.clear_time();

    for(uint32_t i = 0; i < Measurement_data_REP_LENGTH; ++i)
    {
      measurement_msg.set_data(i, std::numeric_limits<float>::max() / 2);
      measurement_msg.set_time(i, 100 * i);
    }
}

// Set minimum values for EnvelopToMCU
template<uint32_t EnvelopToMCU_configuration_REP_LENGTH>
void set_envelop_to_mcu_min(EnvelopToMCU<EnvelopToMCU_configuration_REP_LENGTH>& envelop_msg)
{
    envelop_msg.clear_configuration();

    // Set one configuration with minimum values at index 0
    HelperCompleteDemoConfig::set_config_with_gpio_demo(envelop_msg.mutable_configuration(0));

    // Set a command with minimum values
    HelperCompleteDemoCommand::set_command_with_gpio_min(envelop_msg.mutable_command());
}

// Set maximum values for EnvelopToMCU
template<uint32_t EnvelopToMCU_configuration_REP_LENGTH>
void set_envelop_to_mcu_max(EnvelopToMCU<EnvelopToMCU_configuration_REP_LENGTH>& envelop_msg)
{
    envelop_msg.clear_configuration();

    // Set one configuration with maximum-like values at index 0 (using demo as proxy)
    HelperCompleteDemoConfig::set_config_with_spi_demo(envelop_msg.mutable_configuration(0));

    // Set a command with maximum values
    HelperCompleteDemoCommand::set_command_with_motor_max(envelop_msg.mutable_command());
}

// Set average values for EnvelopToMCU
template<uint32_t EnvelopToMCU_configuration_REP_LENGTH>
void set_envelop_to_mcu_average(EnvelopToMCU<EnvelopToMCU_configuration_REP_LENGTH>& envelop_msg)
{
    envelop_msg.clear_configuration();

    // Set one configuration with average-like values at index 0 (using demo as proxy)
    HelperCompleteDemoConfig::set_config_with_uart_demo(envelop_msg.mutable_configuration(0));

    // Set a command with average values
    HelperCompleteDemoCommand::set_command_with_gpio_average(envelop_msg.mutable_command());
}

// Set minimum values for EnvelopFromMCU with states
template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
void set_envelop_from_mcu_states_min(EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    States<EnvelopFromMCU_states_States_states_REP_LENGTH>& states = envelop_msg.mutable_states();
    set_states_min(states);
}

// Set maximum values for EnvelopFromMCU with states
template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
void set_envelop_from_mcu_states_max(EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    States<EnvelopFromMCU_states_States_states_REP_LENGTH>& states = envelop_msg.mutable_states();
    set_states_max(states);
}

// Set average values for EnvelopFromMCU with states
template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
void set_envelop_from_mcu_states_average(EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    States<EnvelopFromMCU_states_States_states_REP_LENGTH>& states = envelop_msg.mutable_states();
    set_states_average(states);
}

// Set minimum values for EnvelopFromMCU with measurement
template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
void set_envelop_from_mcu_measurement_min(EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    Measurement<EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
                EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& measurement = envelop_msg.mutable_measurement();
    set_measurement_min(measurement);
}

// Set maximum values for EnvelopFromMCU with measurement
template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
void set_envelop_from_mcu_measurement_max(EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    Measurement<EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
                EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& measurement = envelop_msg.mutable_measurement();
    set_measurement_max(measurement);
}

// Set average values for EnvelopFromMCU with measurement
template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
void set_envelop_from_mcu_measurement_average(EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    Measurement<EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
                EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& measurement = envelop_msg.mutable_measurement();
    set_measurement_average(measurement);
}

// Comparison functions for demo values
template<uint32_t States_states_REP_LENGTH>
bool compare_states_demo(const States<States_states_REP_LENGTH>& states_msg)
{
    // Every state is a GPIO state, as set_states_demo() fills them.
    if (states_msg.get_states().get_length() != States_states_REP_LENGTH)
        return false;

    for (uint32_t i = 0; i < States_states_REP_LENGTH; ++i)
    {
        if (!HelperCompleteDemoState::compare_gpio_state_demo(states_msg.states(i).get_gpio()))
            return false;
    }

    return true;
}

template<uint32_t Measurement_data_REP_LENGTH, uint32_t Measurement_time_REP_LENGTH>
bool compare_measurement_demo(const Measurement<Measurement_data_REP_LENGTH, Measurement_time_REP_LENGTH>& measurement_msg)
{
    bool result = (measurement_msg.get_data().get_length() == Measurement_data_REP_LENGTH)
                    && (measurement_msg.get_time().get_length() == Measurement_time_REP_LENGTH);

    for(uint32_t i = 0; (i < measurement_msg.get_data().get_length()) && result; ++i)
    {
      result = (measurement_msg.get_data()[i] == 1.23f * i) && (measurement_msg.get_time()[i] == (i * 100));
    }

    return result;
}

template<uint32_t EnvelopToMCU_configuration_REP_LENGTH>
bool compare_envelop_to_mcu_demo(const EnvelopToMCU<EnvelopToMCU_configuration_REP_LENGTH>& envelop_msg)
{
    if (envelop_msg.get_configuration().get_length() != EnvelopToMCU_configuration_REP_LENGTH)
        return false;

    // Check GPIO configuration (index 0)
    if (!HelperCompleteDemoConfig::compare_gpio_config_demo(envelop_msg.configuration(0).get_gpio()))
        return false;

    // Check Motor configuration (index 1)
    if (!HelperCompleteDemoConfig::compare_motor_config_demo(envelop_msg.configuration(1).get_motor()))
        return false;

    // Check Motor command
    if (!HelperCompleteDemoCommand::compare_motor_command_demo(envelop_msg.get_command().get_motor()))
        return false;

    return true;
}

template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
bool compare_envelop_from_mcu_states_demo(const EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    if (!envelop_msg.has_states())
        return false;

    return compare_states_demo(envelop_msg.get_states());
}

template<
    uint32_t EnvelopFromMCU_states_States_states_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    uint32_t EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH
>
bool compare_envelop_from_mcu_measurement_demo(const EnvelopFromMCU<
    EnvelopFromMCU_states_States_states_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_data_REP_LENGTH,
    EnvelopFromMCU_measurement_Measurement_time_REP_LENGTH>& envelop_msg)
{
    if (!envelop_msg.has_measurement())
        return false;

    return compare_measurement_demo(envelop_msg.get_measurement());
}

// Helper functions for working with repeated fields by index
template<uint32_t States_states_REP_LENGTH>
void set_state_at_index_demo(States<States_states_REP_LENGTH>& states_msg, size_t index)
{
    if (index >= states_msg.get_states_size())
        return;

    // Cycle through different state types based on index
    switch (index % 3)
    {
        case 0:
            HelperCompleteDemoState::set_state_with_gpio_demo(states_msg.mutable_states(index));
            break;
        case 1:
            HelperCompleteDemoState::set_state_with_motor_demo(states_msg.mutable_states(index));
            break;
        case 2:
            HelperCompleteDemoState::set_state_with_encoder_demo(states_msg.mutable_states(index));
            break;
    }
}

template<uint32_t EnvelopToMCU_configuration_REP_LENGTH>
void set_config_at_index_demo(EnvelopToMCU<EnvelopToMCU_configuration_REP_LENGTH>& envelop_msg, size_t index)
{
    if (index >= envelop_msg.get_configuration_size())
        return;

    // Cycle through different config types based on index
    switch (index % 6)
    {
        case 0:
            HelperCompleteDemoConfig::set_config_with_gpio_demo(envelop_msg.mutable_configuration(index));
            break;
        case 1:
            HelperCompleteDemoConfig::set_config_with_uart_demo(envelop_msg.mutable_configuration(index));
            break;
        case 2:
            HelperCompleteDemoConfig::set_config_with_i2c_demo(envelop_msg.mutable_configuration(index));
            break;
        case 3:
            HelperCompleteDemoConfig::set_config_with_spi_demo(envelop_msg.mutable_configuration(index));
            break;
        case 4:
            HelperCompleteDemoConfig::set_config_with_motor_demo(envelop_msg.mutable_configuration(index));
            break;
        case 5:
            HelperCompleteDemoConfig::set_config_with_encoder_demo(envelop_msg.mutable_configuration(index));
            break;
    }
}

} // End of namespace HelperCompleteDemo

#endif // HELPER_COMPLETE_DEMO
