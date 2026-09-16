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

#ifndef HELPER_COMPLETE_DEMO_ARDUINOJSON
#define HELPER_COMPLETE_DEMO_ARDUINOJSON

#include <ArduinoJson.h>
#include <limits>
#include "helper_complete_demo_config.h"
#include "helper_complete_demo_state.h"
#include "helper_complete_demo_command.h"

namespace HelperCompleteDemoArduinoJson
{

// Set demo values for States message with multiple state entries
void set_states_demo(JsonArray states_array, uint32_t count)
{
    for(uint32_t i = 0; i < count; ++i)
    {
        JsonObject state = states_array.add<JsonObject>();
        HelperCompleteDemoStateArduinoJson::set_state_with_gpio_demo(state);
    }
}

// Set demo values for Measurement message
void set_measurement_demo(JsonObject measurement_obj, uint32_t data_count, uint32_t time_count)
{
    JsonArray data_array = measurement_obj["data"].to<JsonArray>();
    JsonArray time_array = measurement_obj["time"].to<JsonArray>();

    for(uint32_t i = 0; i < data_count; ++i)
    {
        data_array.add(1.23f * i);
    }

    for(uint32_t i = 0; i < time_count; ++i)
    {
        time_array.add(100 * i);
    }
}

// Set demo values for EnvelopToMCU configurations
void set_envelop_to_mcu_configurations_demo(JsonDocument& doc, uint32_t config_count)
{
    JsonArray config_array = doc["configuration"].to<JsonArray>();

    for(uint32_t i = 0; i < config_count; ++i)
    {
        JsonObject config = config_array.add<JsonObject>();
        if(0x01 & i)
        {
            HelperCompleteDemoConfigArduinoJson::set_config_with_gpio_demo(config);
        }
        else
        {
            HelperCompleteDemoConfigArduinoJson::set_config_with_motor_demo(config);
        }
    }
}

// Set demo values for EnvelopToMCU command
void set_envelop_to_mcu_command_demo(JsonDocument& doc)
{
    JsonObject command = doc["command"].to<JsonObject>();
    HelperCompleteDemoCommandArduinoJson::set_command_with_motor_demo(command);
}

// Set demo values for EnvelopToMCU with configuration and command
void set_envelop_to_mcu_demo(JsonDocument& doc, uint32_t config_count)
{
    set_envelop_to_mcu_configurations_demo(doc, config_count);
    set_envelop_to_mcu_command_demo(doc);
}

// Set demo values for EnvelopFromMCU with states
void set_envelop_from_mcu_with_states_demo(JsonDocument& doc, uint32_t states_count)
{
    JsonObject states_obj = doc["states"].to<JsonObject>();
    JsonArray states_array = states_obj["states"].to<JsonArray>();
    set_states_demo(states_array, states_count);
}

// Set demo values for EnvelopFromMCU with measurement
void set_envelop_from_mcu_with_measurement_demo(JsonDocument& doc, uint32_t data_count, uint32_t time_count)
{
    JsonObject measurement = doc["measurement"].to<JsonObject>();
    set_measurement_demo(measurement, data_count, time_count);
}

// Set minimum values for States message
void set_states_min(JsonArray states_array)
{
    JsonObject state = states_array.add<JsonObject>();
    HelperCompleteDemoStateArduinoJson::set_state_with_gpio_min(state);
}

// Set maximum values for States message
void set_states_max(JsonArray states_array)
{
    JsonObject state = states_array.add<JsonObject>();
    HelperCompleteDemoStateArduinoJson::set_state_with_gpio_max(state);
}

// Set average values for States message
void set_states_average(JsonArray states_array)
{
    JsonObject state = states_array.add<JsonObject>();
    HelperCompleteDemoStateArduinoJson::set_state_with_gpio_average(state);
}

// Set minimum values for Measurement message
void set_measurement_min(JsonObject measurement_obj, uint32_t count)
{
    JsonArray data_array = measurement_obj["data"].to<JsonArray>();
    JsonArray time_array = measurement_obj["time"].to<JsonArray>();

    for(uint32_t i = 0; i < count; ++i)
    {
        data_array.add(std::numeric_limits<float>::lowest());
        time_array.add(100 * i);
    }
}

// Set maximum values for Measurement message
void set_measurement_max(JsonObject measurement_obj, uint32_t count)
{
    JsonArray data_array = measurement_obj["data"].to<JsonArray>();
    JsonArray time_array = measurement_obj["time"].to<JsonArray>();

    for(uint32_t i = 0; i < count; ++i)
    {
        data_array.add(std::numeric_limits<float>::max());
        time_array.add(100 * i);
    }
}

// Set average values for Measurement message
void set_measurement_average(JsonObject measurement_obj, uint32_t count)
{
    JsonArray data_array = measurement_obj["data"].to<JsonArray>();
    JsonArray time_array = measurement_obj["time"].to<JsonArray>();

    for(uint32_t i = 0; i < count; ++i)
    {
        data_array.add(std::numeric_limits<float>::max() / 2);
        time_array.add(100 * i);
    }
}

// Set minimum values for EnvelopToMCU
void set_envelop_to_mcu_min(JsonDocument& doc)
{
    JsonArray config_array = doc["configuration"].to<JsonArray>();
    JsonObject config = config_array.add<JsonObject>();
    HelperCompleteDemoConfigArduinoJson::set_config_with_gpio_demo(config);

    JsonObject command = doc["command"].to<JsonObject>();
    HelperCompleteDemoCommandArduinoJson::set_command_with_gpio_min(command);
}

// Set maximum values for EnvelopToMCU
void set_envelop_to_mcu_max(JsonDocument& doc)
{
    JsonArray config_array = doc["configuration"].to<JsonArray>();
    JsonObject config = config_array.add<JsonObject>();
    HelperCompleteDemoConfigArduinoJson::set_config_with_spi_demo(config);

    JsonObject command = doc["command"].to<JsonObject>();
    HelperCompleteDemoCommandArduinoJson::set_command_with_motor_max(command);
}

// Set average values for EnvelopToMCU
void set_envelop_to_mcu_average(JsonDocument& doc)
{
    JsonArray config_array = doc["configuration"].to<JsonArray>();
    JsonObject config = config_array.add<JsonObject>();
    HelperCompleteDemoConfigArduinoJson::set_config_with_uart_demo(config);

    JsonObject command = doc["command"].to<JsonObject>();
    HelperCompleteDemoCommandArduinoJson::set_command_with_gpio_average(command);
}

// Set minimum values for EnvelopFromMCU with states
void set_envelop_from_mcu_states_min(JsonDocument& doc)
{
    JsonObject states_obj = doc["states"].to<JsonObject>();
    JsonArray states_array = states_obj["states"].to<JsonArray>();
    set_states_min(states_array);
}

// Set maximum values for EnvelopFromMCU with states
void set_envelop_from_mcu_states_max(JsonDocument& doc)
{
    JsonObject states_obj = doc["states"].to<JsonObject>();
    JsonArray states_array = states_obj["states"].to<JsonArray>();
    set_states_max(states_array);
}

// Set average values for EnvelopFromMCU with states
void set_envelop_from_mcu_states_average(JsonDocument& doc)
{
    JsonObject states_obj = doc["states"].to<JsonObject>();
    JsonArray states_array = states_obj["states"].to<JsonArray>();
    set_states_average(states_array);
}

// Set minimum values for EnvelopFromMCU with measurement
void set_envelop_from_mcu_measurement_min(JsonDocument& doc, uint32_t count)
{
    JsonObject measurement = doc["measurement"].to<JsonObject>();
    set_measurement_min(measurement, count);
}

// Set maximum values for EnvelopFromMCU with measurement
void set_envelop_from_mcu_measurement_max(JsonDocument& doc, uint32_t count)
{
    JsonObject measurement = doc["measurement"].to<JsonObject>();
    set_measurement_max(measurement, count);
}

// Set average values for EnvelopFromMCU with measurement
void set_envelop_from_mcu_measurement_average(JsonDocument& doc, uint32_t count)
{
    JsonObject measurement = doc["measurement"].to<JsonObject>();
    set_measurement_average(measurement, count);
}

// Comparison functions for demo values
bool compare_states_demo(JsonArrayConst states_array, uint32_t expected_count)
{
    if (states_array.size() != expected_count)
        return false;

    // Check GPIO state (all states are GPIO in set_states_demo)
    for(uint32_t i = 0; i < expected_count; ++i)
    {
        JsonObjectConst state = states_array[i];
        if (!state["gpio"].is<JsonObjectConst>())
            return false;
        if (!HelperCompleteDemoStateArduinoJson::compare_gpio_state_demo(state["gpio"]))
            return false;
    }

    return true;
}

bool compare_measurement_demo(JsonObjectConst measurement_obj, uint32_t data_count, uint32_t time_count)
{
    JsonArrayConst data_array = measurement_obj["data"];
    JsonArrayConst time_array = measurement_obj["time"];

    if (data_array.size() != data_count || time_array.size() != time_count)
        return false;

    for(uint32_t i = 0; i < data_count; ++i)
    {
        float expected_data = 1.23f * i;
        uint64_t expected_time = 100 * i;

        if (static_cast<float>(data_array[i]) != expected_data)
            return false;
        if (static_cast<uint64_t>(time_array[i]) != expected_time)
            return false;
    }

    return true;
}

bool compare_envelop_to_mcu_demo(JsonDocument& doc, uint32_t config_count)
{
    JsonArrayConst config_array = doc["configuration"];
    if (config_array.size() != config_count)
        return false;

    // Check first config (motor based on i=0, which is 0x01 & 0 == false)
    if (!config_array[0]["motor"].is<JsonObjectConst>())
        return false;
    if (!HelperCompleteDemoConfigArduinoJson::compare_motor_config_demo(config_array[0]["motor"]))
        return false;

    // Check second config if exists (gpio based on i=1, which is 0x01 & 1 == true)
    if (config_count > 1)
    {
        if (!config_array[1]["gpio"].is<JsonObjectConst>())
            return false;
        if (!HelperCompleteDemoConfigArduinoJson::compare_gpio_config_demo(config_array[1]["gpio"]))
            return false;
    }

    // Check Motor command
    JsonObjectConst command = doc["command"];
    if (!command["motor"].is<JsonObjectConst>())
        return false;
    if (!HelperCompleteDemoCommandArduinoJson::compare_motor_command_demo(command["motor"]))
        return false;

    return true;
}

bool compare_envelop_from_mcu_states_demo(JsonDocument& doc, uint32_t states_count)
{
    if (!doc["states"].is<JsonObjectConst>())
        return false;

    JsonObjectConst states_obj = doc["states"];
    JsonArrayConst states_array = states_obj["states"];
    return compare_states_demo(states_array, states_count);
}

bool compare_envelop_from_mcu_measurement_demo(JsonDocument& doc, uint32_t data_count, uint32_t time_count)
{
    if (!doc["measurement"].is<JsonObjectConst>())
        return false;

    JsonObjectConst measurement = doc["measurement"];
    return compare_measurement_demo(measurement, data_count, time_count);
}

// Helper functions for working with repeated fields by index
void set_state_at_index_demo(JsonArray states_array, size_t index)
{
    JsonObject state = states_array[index].to<JsonObject>();

    // Cycle through different state types based on index
    switch (index % 3)
    {
        case 0:
            HelperCompleteDemoStateArduinoJson::set_state_with_gpio_demo(state);
            break;
        case 1:
            HelperCompleteDemoStateArduinoJson::set_state_with_motor_demo(state);
            break;
        case 2:
            HelperCompleteDemoStateArduinoJson::set_state_with_encoder_demo(state);
            break;
    }
}

void set_config_at_index_demo(JsonArray config_array, size_t index)
{
    JsonObject config = config_array[index].to<JsonObject>();

    // Cycle through different config types based on index
    switch (index % 6)
    {
        case 0:
            HelperCompleteDemoConfigArduinoJson::set_config_with_gpio_demo(config);
            break;
        case 1:
            HelperCompleteDemoConfigArduinoJson::set_config_with_uart_demo(config);
            break;
        case 2:
            HelperCompleteDemoConfigArduinoJson::set_config_with_i2c_demo(config);
            break;
        case 3:
            HelperCompleteDemoConfigArduinoJson::set_config_with_spi_demo(config);
            break;
        case 4:
            HelperCompleteDemoConfigArduinoJson::set_config_with_motor_demo(config);
            break;
        case 5:
            HelperCompleteDemoConfigArduinoJson::set_config_with_encoder_demo(config);
            break;
    }
}

} // End of namespace HelperCompleteDemoArduinoJson

#endif // HELPER_COMPLETE_DEMO_ARDUINOJSON
