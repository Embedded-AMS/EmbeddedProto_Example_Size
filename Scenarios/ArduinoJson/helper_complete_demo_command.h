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

#ifndef HELPER_COMPLETE_DEMO_COMMAND_ARDUINOJSON
#define HELPER_COMPLETE_DEMO_COMMAND_ARDUINOJSON

#include <ArduinoJson.h>
#include <limits>

namespace HelperCompleteDemoCommandArduinoJson
{

static uint32_t global_command_id = 0;

// Set demo values for GPIO command
void set_gpio_command_demo(JsonObject gpio_obj)
{
    gpio_obj["set_value"] = true;  // Set GPIO high
    gpio_obj["toggle"] = false;  // Don't toggle
    gpio_obj["pulse_duration_ms"] = 100;  // 100ms pulse
}

// Set demo values for Motor command
void set_motor_command_demo(JsonObject motor_obj)
{
    motor_obj["target_position"] = 5000;  // Move to position 5000
    motor_obj["target_speed"] = 1000;  // At 1000 steps per second
    motor_obj["enable"] = true;  // Enable motor
    motor_obj["home"] = false;  // Don't home
    motor_obj["emergency_stop"] = false;  // No emergency stop
    motor_obj["clear_fault"] = false;  // Don't clear faults
    motor_obj["acceleration"] = 2000;  // Acceleration of 2000
    motor_obj["deceleration"] = 2000;  // Deceleration of 2000
}

// Set a complete Command with GPIO command
void set_command_with_gpio_demo(JsonObject command_obj)
{
    JsonObject gpio = command_obj["gpio"].to<JsonObject>();
    set_gpio_command_demo(gpio);
    command_obj["command_id"] = global_command_id++;
}

// Set a complete Command with Motor command
void set_command_with_motor_demo(JsonObject command_obj)
{
    JsonObject motor = command_obj["motor"].to<JsonObject>();
    set_motor_command_demo(motor);
    command_obj["command_id"] = global_command_id++;
}

// Comparison functions for demo values
bool compare_gpio_command_demo(JsonObjectConst gpio_obj)
{
    return gpio_obj["set_value"] == true && 
           gpio_obj["toggle"] == false &&
           gpio_obj["pulse_duration_ms"] == 100;
}

bool compare_motor_command_demo(JsonObjectConst motor_obj)
{
    return motor_obj["target_position"] == 5000 &&
           motor_obj["target_speed"] == 1000 &&
           motor_obj["enable"] == true &&
           motor_obj["home"] == false &&
           motor_obj["emergency_stop"] == false &&
           motor_obj["clear_fault"] == false &&
           motor_obj["acceleration"] == 2000 &&
           motor_obj["deceleration"] == 2000;
}

// Set minimum values for command messages
void set_gpio_command_min(JsonObject gpio_obj)
{
    gpio_obj["set_value"] = false;
    gpio_obj["toggle"] = false;
    gpio_obj["pulse_duration_ms"] = 0;
}

void set_motor_command_min(JsonObject motor_obj)
{
    motor_obj["target_position"] = std::numeric_limits<int32_t>::min();
    motor_obj["target_speed"] = 0;
    motor_obj["enable"] = false;
    motor_obj["home"] = false;
    motor_obj["emergency_stop"] = false;
    motor_obj["clear_fault"] = false;
    motor_obj["acceleration"] = 0;
    motor_obj["deceleration"] = 0;
}

// Set maximum values for command messages
void set_gpio_command_max(JsonObject gpio_obj)
{
    gpio_obj["set_value"] = true;
    gpio_obj["toggle"] = true;
    gpio_obj["pulse_duration_ms"] = std::numeric_limits<uint32_t>::max();
}

void set_motor_command_max(JsonObject motor_obj)
{
    motor_obj["target_position"] = std::numeric_limits<int32_t>::max();
    motor_obj["target_speed"] = std::numeric_limits<uint32_t>::max();
    motor_obj["enable"] = true;
    motor_obj["home"] = true;
    motor_obj["emergency_stop"] = true;
    motor_obj["clear_fault"] = true;
    motor_obj["acceleration"] = std::numeric_limits<uint32_t>::max();
    motor_obj["deceleration"] = std::numeric_limits<uint32_t>::max();
}

// Set average values for command messages
void set_gpio_command_average(JsonObject gpio_obj)
{
    gpio_obj["set_value"] = true;
    gpio_obj["toggle"] = false;
    gpio_obj["pulse_duration_ms"] = std::numeric_limits<uint32_t>::max() / 2;
}

void set_motor_command_average(JsonObject motor_obj)
{
    motor_obj["target_position"] = std::numeric_limits<int32_t>::max() / 2;
    motor_obj["target_speed"] = std::numeric_limits<uint32_t>::max() / 2;
    motor_obj["enable"] = true;
    motor_obj["home"] = false;
    motor_obj["emergency_stop"] = false;
    motor_obj["clear_fault"] = false;
    motor_obj["acceleration"] = std::numeric_limits<uint32_t>::max() / 2;
    motor_obj["deceleration"] = std::numeric_limits<uint32_t>::max() / 2;
}

// Set complete command messages with min/max/average values
void set_command_with_gpio_min(JsonObject command_obj)
{
    JsonObject gpio = command_obj["gpio"].to<JsonObject>();
    set_gpio_command_min(gpio);
    command_obj["command_id"] = global_command_id++;
}

void set_command_with_motor_min(JsonObject command_obj)
{
    JsonObject motor = command_obj["motor"].to<JsonObject>();
    set_motor_command_min(motor);
    command_obj["command_id"] = global_command_id++;
}

void set_command_with_gpio_max(JsonObject command_obj)
{
    JsonObject gpio = command_obj["gpio"].to<JsonObject>();
    set_gpio_command_max(gpio);
    command_obj["command_id"] = global_command_id++;
}

void set_command_with_motor_max(JsonObject command_obj)
{
    JsonObject motor = command_obj["motor"].to<JsonObject>();
    set_motor_command_max(motor);
    command_obj["command_id"] = global_command_id++;
}

void set_command_with_gpio_average(JsonObject command_obj)
{
    JsonObject gpio = command_obj["gpio"].to<JsonObject>();
    set_gpio_command_average(gpio);
    command_obj["command_id"] = global_command_id++;
}

void set_command_with_motor_average(JsonObject command_obj)
{
    JsonObject motor = command_obj["motor"].to<JsonObject>();
    set_motor_command_average(motor);
    command_obj["command_id"] = global_command_id++;
}

// Convenience functions for common GPIO commands
void set_gpio_set_high_command(JsonObject gpio_obj)
{
    gpio_obj["set_value"] = true;
    gpio_obj["toggle"] = false;
    gpio_obj["pulse_duration_ms"] = 0;
}

void set_gpio_set_low_command(JsonObject gpio_obj)
{
    gpio_obj["set_value"] = false;
    gpio_obj["toggle"] = false;
    gpio_obj["pulse_duration_ms"] = 0;
}

void set_gpio_toggle_command(JsonObject gpio_obj)
{
    gpio_obj["set_value"] = false;
    gpio_obj["toggle"] = true;
    gpio_obj["pulse_duration_ms"] = 0;
}

void set_gpio_pulse_command(JsonObject gpio_obj, uint32_t duration_ms)
{
    gpio_obj["set_value"] = true;
    gpio_obj["toggle"] = false;
    gpio_obj["pulse_duration_ms"] = duration_ms;
}

// Convenience functions for common Motor commands
void set_motor_enable_command(JsonObject motor_obj)
{
    motor_obj["target_position"] = 0;
    motor_obj["target_speed"] = 0;
    motor_obj["enable"] = true;
    motor_obj["home"] = false;
    motor_obj["emergency_stop"] = false;
    motor_obj["clear_fault"] = false;
    motor_obj["acceleration"] = 0;
    motor_obj["deceleration"] = 0;
}

void set_motor_disable_command(JsonObject motor_obj)
{
    motor_obj["target_position"] = 0;
    motor_obj["target_speed"] = 0;
    motor_obj["enable"] = false;
    motor_obj["home"] = false;
    motor_obj["emergency_stop"] = false;
    motor_obj["clear_fault"] = false;
    motor_obj["acceleration"] = 0;
    motor_obj["deceleration"] = 0;
}

void set_motor_home_command(JsonObject motor_obj)
{
    motor_obj["target_position"] = 0;
    motor_obj["target_speed"] = 0;
    motor_obj["enable"] = true;
    motor_obj["home"] = true;
    motor_obj["emergency_stop"] = false;
    motor_obj["clear_fault"] = false;
    motor_obj["acceleration"] = 0;
    motor_obj["deceleration"] = 0;
}

void set_motor_emergency_stop_command(JsonObject motor_obj)
{
    motor_obj["target_position"] = 0;
    motor_obj["target_speed"] = 0;
    motor_obj["enable"] = false;
    motor_obj["home"] = false;
    motor_obj["emergency_stop"] = true;
    motor_obj["clear_fault"] = false;
    motor_obj["acceleration"] = 0;
    motor_obj["deceleration"] = 0;
}

void set_motor_move_to_position_command(JsonObject motor_obj, int32_t position, uint32_t speed)
{
    motor_obj["target_position"] = position;
    motor_obj["target_speed"] = speed;
    motor_obj["enable"] = true;
    motor_obj["home"] = false;
    motor_obj["emergency_stop"] = false;
    motor_obj["clear_fault"] = false;
    motor_obj["acceleration"] = speed * 2;  // Simple acceleration calculation
    motor_obj["deceleration"] = speed * 2;  // Simple deceleration calculation
}

} // End of namespace HelperCompleteDemoCommandArduinoJson

#endif // HELPER_COMPLETE_DEMO_COMMAND_ARDUINOJSON
