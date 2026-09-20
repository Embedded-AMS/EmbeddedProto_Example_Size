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

#ifndef HELPER_COMPLETE_DEMO_NANOPB
#define HELPER_COMPLETE_DEMO_NANOPB

#include "complete_demo.pb.h"
#include <limits>
#include "helper_complete_demo_config.h"
#include "helper_complete_demo_state.h"
#include "helper_complete_demo_command.h"

namespace HelperCompleteDemoNanoPB
{

// Constants matching EmbeddedProto template parameters
constexpr uint32_t STATES_COUNT = 5;
constexpr uint32_t MEASUREMENT_DATA_COUNT = 25;
constexpr uint32_t MEASUREMENT_TIME_COUNT = 25;
constexpr uint32_t CONFIGURATION_COUNT = 5;

// Set demo values for States message with multiple state entries
// Matching EmbeddedProto: sets GPIO demo states for all entries
void set_states_demo(States& states_msg)
{
    states_msg = States_init_default;
    states_msg.states_count = STATES_COUNT;

    for(uint32_t i = 0; i < STATES_COUNT; ++i)
    {
        HelperCompleteDemoStateNanoPB::set_state_with_gpio_demo(states_msg.states[i]);
    }
}

// Set demo values for Measurement message
// Matching EmbeddedProto: data = 1.23f * i, time = 100 * i
void set_measurement_demo(Measurement& measurement_msg)
{
    measurement_msg = Measurement_init_default;
    measurement_msg.data_count = MEASUREMENT_DATA_COUNT;
    measurement_msg.time_count = MEASUREMENT_TIME_COUNT;

    for(uint32_t i = 0; i < MEASUREMENT_DATA_COUNT; ++i)
    {
        measurement_msg.data[i] = 1.23f * i;
        measurement_msg.time[i] = 100 * i;
    }
}

// Set demo values for EnvelopToMCU configurations
// Matching EmbeddedProto: alternating GPIO (odd index) and Motor (even index) configs
void set_envelop_to_mcu_configurations_demo(EnvelopToMCU& envelop_msg)
{
    envelop_msg = EnvelopToMCU_init_default;
    envelop_msg.configuration_count = CONFIGURATION_COUNT;
    envelop_msg.has_command = false;

    for(uint32_t i = 0; i < CONFIGURATION_COUNT; ++i)
    {
        if(0x01 & i)
        {
            HelperCompleteDemoConfigNanoPB::set_config_with_gpio_demo(envelop_msg.configuration[i]);
        }
        else
        {
            HelperCompleteDemoConfigNanoPB::set_config_with_motor_demo(envelop_msg.configuration[i]);
        }
    }
}

// Set demo values for EnvelopToMCU command
void set_envelop_to_mcu_command_demo(EnvelopToMCU& envelop_msg)
{
    // Set a motor command
    HelperCompleteDemoCommandNanoPB::set_command_with_motor_demo(envelop_msg.command);
    envelop_msg.has_command = true;
}

// Set demo values for EnvelopToMCU with configuration and command
void set_envelop_to_mcu_demo(EnvelopToMCU& envelop_msg)
{
    set_envelop_to_mcu_configurations_demo(envelop_msg);
    set_envelop_to_mcu_command_demo(envelop_msg);
}

// Set demo values for EnvelopFromMCU with states
// Matching EmbeddedProto: sets GPIO demo states for all entries
void set_envelop_from_mcu_with_states_demo(EnvelopFromMCU& envelop_msg)
{
    envelop_msg = EnvelopFromMCU_init_default;
    envelop_msg.which_data = EnvelopFromMCU_states_tag;
    
    // Set states with demo values
    set_states_demo(envelop_msg.data.states);
}

// Set demo values for EnvelopFromMCU with measurement
// Matching EmbeddedProto: sets measurement demo values
void set_envelop_from_mcu_with_measurement_demo(EnvelopFromMCU& envelop_msg)
{
    envelop_msg = EnvelopFromMCU_init_default;
    envelop_msg.which_data = EnvelopFromMCU_measurement_tag;
    
    // Set measurement with demo values
    set_measurement_demo(envelop_msg.data.measurement);
}

// Set minimum values for States message
void set_states_min(States& states_msg)
{
    states_msg = States_init_default;
    states_msg.states_count = 1;
    HelperCompleteDemoStateNanoPB::set_state_with_gpio_min(states_msg.states[0]);
}

// Set maximum values for States message
void set_states_max(States& states_msg)
{
    states_msg = States_init_default;
    states_msg.states_count = 1;
    HelperCompleteDemoStateNanoPB::set_state_with_gpio_max(states_msg.states[0]);
}

// Set average values for States message
void set_states_average(States& states_msg)
{
    states_msg = States_init_default;
    states_msg.states_count = 1;
    HelperCompleteDemoStateNanoPB::set_state_with_gpio_average(states_msg.states[0]);
}

// Set minimum values for Measurement message
void set_measurement_min(Measurement& measurement_msg)
{
    measurement_msg = Measurement_init_default;
    measurement_msg.data_count = MEASUREMENT_DATA_COUNT;
    measurement_msg.time_count = MEASUREMENT_TIME_COUNT;

    for(uint32_t i = 0; i < MEASUREMENT_DATA_COUNT; ++i)
    {
        measurement_msg.data[i] = std::numeric_limits<float>::lowest();
        measurement_msg.time[i] = 100 * i;
    }
}

// Set maximum values for Measurement message
void set_measurement_max(Measurement& measurement_msg)
{
    measurement_msg = Measurement_init_default;
    measurement_msg.data_count = MEASUREMENT_DATA_COUNT;
    measurement_msg.time_count = MEASUREMENT_TIME_COUNT;

    for(uint32_t i = 0; i < MEASUREMENT_DATA_COUNT; ++i)
    {
        measurement_msg.data[i] = std::numeric_limits<float>::max();
        measurement_msg.time[i] = 100 * i;
    }
}

// Set average values for Measurement message
void set_measurement_average(Measurement& measurement_msg)
{
    measurement_msg = Measurement_init_default;
    measurement_msg.data_count = MEASUREMENT_DATA_COUNT;
    measurement_msg.time_count = MEASUREMENT_TIME_COUNT;

    for(uint32_t i = 0; i < MEASUREMENT_DATA_COUNT; ++i)
    {
        measurement_msg.data[i] = std::numeric_limits<float>::max() / 2;
        measurement_msg.time[i] = 100 * i;
    }
}

// Set minimum values for EnvelopToMCU
void set_envelop_to_mcu_min(EnvelopToMCU& envelop_msg)
{
    envelop_msg = EnvelopToMCU_init_default;
    envelop_msg.configuration_count = 1;
    HelperCompleteDemoConfigNanoPB::set_config_with_gpio_demo(envelop_msg.configuration[0]);
    HelperCompleteDemoCommandNanoPB::set_command_with_gpio_min(envelop_msg.command);
    envelop_msg.has_command = true;
}

// Set maximum values for EnvelopToMCU
void set_envelop_to_mcu_max(EnvelopToMCU& envelop_msg)
{
    envelop_msg = EnvelopToMCU_init_default;
    envelop_msg.configuration_count = 1;
    HelperCompleteDemoConfigNanoPB::set_config_with_spi_demo(envelop_msg.configuration[0]);
    HelperCompleteDemoCommandNanoPB::set_command_with_motor_max(envelop_msg.command);
    envelop_msg.has_command = true;
}

// Set average values for EnvelopToMCU
void set_envelop_to_mcu_average(EnvelopToMCU& envelop_msg)
{
    envelop_msg = EnvelopToMCU_init_default;
    envelop_msg.configuration_count = 1;
    HelperCompleteDemoConfigNanoPB::set_config_with_uart_demo(envelop_msg.configuration[0]);
    HelperCompleteDemoCommandNanoPB::set_command_with_gpio_average(envelop_msg.command);
    envelop_msg.has_command = true;
}

// Set minimum values for EnvelopFromMCU with states
void set_envelop_from_mcu_states_min(EnvelopFromMCU& envelop_msg)
{
    envelop_msg = EnvelopFromMCU_init_default;
    envelop_msg.which_data = EnvelopFromMCU_states_tag;
    set_states_min(envelop_msg.data.states);
}

// Set maximum values for EnvelopFromMCU with states
void set_envelop_from_mcu_states_max(EnvelopFromMCU& envelop_msg)
{
    envelop_msg = EnvelopFromMCU_init_default;
    envelop_msg.which_data = EnvelopFromMCU_states_tag;
    set_states_max(envelop_msg.data.states);
}

// Set average values for EnvelopFromMCU with states
void set_envelop_from_mcu_states_average(EnvelopFromMCU& envelop_msg)
{
    envelop_msg = EnvelopFromMCU_init_default;
    envelop_msg.which_data = EnvelopFromMCU_states_tag;
    set_states_average(envelop_msg.data.states);
}

// Set minimum values for EnvelopFromMCU with measurement
void set_envelop_from_mcu_measurement_min(EnvelopFromMCU& envelop_msg)
{
    envelop_msg = EnvelopFromMCU_init_default;
    envelop_msg.which_data = EnvelopFromMCU_measurement_tag;
    set_measurement_min(envelop_msg.data.measurement);
}

// Set maximum values for EnvelopFromMCU with measurement
void set_envelop_from_mcu_measurement_max(EnvelopFromMCU& envelop_msg)
{
    envelop_msg = EnvelopFromMCU_init_default;
    envelop_msg.which_data = EnvelopFromMCU_measurement_tag;
    set_measurement_max(envelop_msg.data.measurement);
}

// Set average values for EnvelopFromMCU with measurement
void set_envelop_from_mcu_measurement_average(EnvelopFromMCU& envelop_msg)
{
    envelop_msg = EnvelopFromMCU_init_default;
    envelop_msg.which_data = EnvelopFromMCU_measurement_tag;
    set_measurement_average(envelop_msg.data.measurement);
}

// Comparison functions for demo values
bool compare_states_demo(const States& states_msg)
{
    if (states_msg.states_count != STATES_COUNT)
        return false;

    for (uint32_t i = 0; i < STATES_COUNT; ++i)
    {
        if (!HelperCompleteDemoStateNanoPB::compare_gpio_state_demo(states_msg.states[i].state.gpio))
            return false;
    }
    return true;
}

bool compare_measurement_demo(const Measurement& measurement_msg)
{
    if (measurement_msg.data_count != MEASUREMENT_DATA_COUNT ||
        measurement_msg.time_count != MEASUREMENT_TIME_COUNT)
        return false;

    for (uint32_t i = 0; i < MEASUREMENT_DATA_COUNT; ++i)
    {
        if (measurement_msg.data[i] != 1.23f * i ||
            measurement_msg.time[i] != 100 * i)
            return false;
    }
    return true;
}

bool compare_envelop_to_mcu_demo(const EnvelopToMCU& envelop_msg)
{
    if (envelop_msg.configuration_count != CONFIGURATION_COUNT)
        return false;

    // Check GPIO configuration (index 1, odd)
    if (!HelperCompleteDemoConfigNanoPB::compare_gpio_config_demo(envelop_msg.configuration[1].peripheral.gpio))
        return false;

    // Check Motor configuration (index 0, even)
    if (!HelperCompleteDemoConfigNanoPB::compare_motor_config_demo(envelop_msg.configuration[0].device.motor))
        return false;

    // Check Motor command
    if (!envelop_msg.has_command)
        return false;
    if (!HelperCompleteDemoCommandNanoPB::compare_motor_command_demo(envelop_msg.command.command.motor))
        return false;

    return true;
}

bool compare_envelop_from_mcu_states_demo(const EnvelopFromMCU& envelop_msg)
{
    if (envelop_msg.which_data != EnvelopFromMCU_states_tag)
        return false;
    return compare_states_demo(envelop_msg.data.states);
}

bool compare_envelop_from_mcu_measurement_demo(const EnvelopFromMCU& envelop_msg)
{
    if (envelop_msg.which_data != EnvelopFromMCU_measurement_tag)
        return false;
    return compare_measurement_demo(envelop_msg.data.measurement);
}

// Helper functions for working with repeated fields by index
void set_state_at_index_demo(States& states_msg, size_t index)
{
    if (index >= states_msg.states_count)
        return;

    // Cycle through different state types based on index
    switch (index % 3)
    {
        case 0:
            HelperCompleteDemoStateNanoPB::set_state_with_gpio_demo(states_msg.states[index]);
            break;
        case 1:
            HelperCompleteDemoStateNanoPB::set_state_with_motor_demo(states_msg.states[index]);
            break;
        case 2:
            HelperCompleteDemoStateNanoPB::set_state_with_encoder_demo(states_msg.states[index]);
            break;
    }
}

void set_config_at_index_demo(EnvelopToMCU& envelop_msg, size_t index)
{
    if (index >= envelop_msg.configuration_count)
        return;

    // Cycle through different config types based on index
    switch (index % 6)
    {
        case 0:
            HelperCompleteDemoConfigNanoPB::set_config_with_gpio_demo(envelop_msg.configuration[index]);
            break;
        case 1:
            HelperCompleteDemoConfigNanoPB::set_config_with_uart_demo(envelop_msg.configuration[index]);
            break;
        case 2:
            HelperCompleteDemoConfigNanoPB::set_config_with_i2c_demo(envelop_msg.configuration[index]);
            break;
        case 3:
            HelperCompleteDemoConfigNanoPB::set_config_with_spi_demo(envelop_msg.configuration[index]);
            break;
        case 4:
            HelperCompleteDemoConfigNanoPB::set_config_with_motor_demo(envelop_msg.configuration[index]);
            break;
        case 5:
            HelperCompleteDemoConfigNanoPB::set_config_with_encoder_demo(envelop_msg.configuration[index]);
            break;
    }
}

} // End of namespace HelperCompleteDemoNanoPB

#endif // HELPER_COMPLETE_DEMO_NANOPB
