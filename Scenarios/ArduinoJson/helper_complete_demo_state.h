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

#ifndef HELPER_COMPLETE_DEMO_STATE_ARDUINOJSON
#define HELPER_COMPLETE_DEMO_STATE_ARDUINOJSON

#include <ArduinoJson.h>
#include <limits>

namespace HelperCompleteDemoStateArduinoJson
{

// Set demo values for GPIO state
void set_gpio_state_demo(JsonObject gpio_obj)
{
    gpio_obj["value"] = true;  // GPIO high
    gpio_obj["last_change_timestamp"] = 123456789;  // Some timestamp
}

// Set demo values for Motor state
void set_motor_state_demo(JsonObject motor_obj)
{
    motor_obj["position"] = 1000;  // 1000 steps position
    motor_obj["target_position"] = 2000;  // Target 2000 steps
    motor_obj["speed"] = 500;  // 500 steps per second
    motor_obj["is_moving"] = true;  // Motor is moving
    motor_obj["is_homed"] = true;  // Motor is homed
    motor_obj["fault_detected"] = false;  // No faults
    motor_obj["current"] = 1500;  // 1500 mA current
}

// Set demo values for Encoder state
void set_encoder_state_demo(JsonObject encoder_obj)
{
    encoder_obj["position"] = 5000;  // 5000 encoder counts
    encoder_obj["velocity"] = 100;  // 100 counts per second
    encoder_obj["direction"] = true;  // Forward direction
    encoder_obj["last_pulse_timestamp"] = 987654321;  // Some timestamp
    encoder_obj["pulses_per_revolution"] = 1000;  // 1000 PPR encoder
}

// Set a complete State with GPIO state
void set_state_with_gpio_demo(JsonObject state_obj)
{
    JsonObject gpio = state_obj["gpio"].to<JsonObject>();
    set_gpio_state_demo(gpio);
    state_obj["timestamp"] = 1000000;  // 1 million timestamp
}

// Set a complete State with Motor state
void set_state_with_motor_demo(JsonObject state_obj)
{
    JsonObject motor = state_obj["motor"].to<JsonObject>();
    set_motor_state_demo(motor);
    state_obj["timestamp"] = 2000000;  // 2 million timestamp
}

// Set a complete State with Encoder state
void set_state_with_encoder_demo(JsonObject state_obj)
{
    JsonObject encoder = state_obj["encoder"].to<JsonObject>();
    set_encoder_state_demo(encoder);
    state_obj["timestamp"] = 3000000;  // 3 million timestamp
}

// Comparison functions for demo values
bool compare_gpio_state_demo(JsonObjectConst gpio_obj)
{
    return gpio_obj["value"] == true && 
           gpio_obj["last_change_timestamp"] == 123456789;
}

bool compare_motor_state_demo(JsonObjectConst motor_obj)
{
    return motor_obj["position"] == 1000 &&
           motor_obj["target_position"] == 2000 &&
           motor_obj["speed"] == 500 &&
           motor_obj["is_moving"] == true &&
           motor_obj["is_homed"] == true &&
           motor_obj["fault_detected"] == false &&
           motor_obj["current"] == 1500;
}

bool compare_encoder_state_demo(JsonObjectConst encoder_obj)
{
    return encoder_obj["position"] == 5000 &&
           encoder_obj["velocity"] == 100 &&
           encoder_obj["direction"] == true &&
           encoder_obj["last_pulse_timestamp"] == 987654321 &&
           encoder_obj["pulses_per_revolution"] == 1000;
}

// Set minimum values for state messages
void set_gpio_state_min(JsonObject gpio_obj)
{
    gpio_obj["value"] = false;
    gpio_obj["last_change_timestamp"] = 0;
}

void set_motor_state_min(JsonObject motor_obj)
{
    motor_obj["position"] = std::numeric_limits<int32_t>::min();
    motor_obj["target_position"] = std::numeric_limits<int32_t>::min();
    motor_obj["speed"] = 0;
    motor_obj["is_moving"] = false;
    motor_obj["is_homed"] = false;
    motor_obj["fault_detected"] = false;
    motor_obj["current"] = 0;
}

void set_encoder_state_min(JsonObject encoder_obj)
{
    encoder_obj["position"] = std::numeric_limits<int32_t>::min();
    encoder_obj["velocity"] = std::numeric_limits<int32_t>::min();
    encoder_obj["direction"] = false;
    encoder_obj["last_pulse_timestamp"] = 0;
    encoder_obj["pulses_per_revolution"] = 0;
}

// Set maximum values for state messages
void set_gpio_state_max(JsonObject gpio_obj)
{
    gpio_obj["value"] = true;
    gpio_obj["last_change_timestamp"] = std::numeric_limits<uint64_t>::max();
}

void set_motor_state_max(JsonObject motor_obj)
{
    motor_obj["position"] = std::numeric_limits<int32_t>::max();
    motor_obj["target_position"] = std::numeric_limits<int32_t>::max();
    motor_obj["speed"] = std::numeric_limits<uint32_t>::max();
    motor_obj["is_moving"] = true;
    motor_obj["is_homed"] = true;
    motor_obj["fault_detected"] = true;
    motor_obj["current"] = std::numeric_limits<uint32_t>::max();
}

void set_encoder_state_max(JsonObject encoder_obj)
{
    encoder_obj["position"] = std::numeric_limits<int32_t>::max();
    encoder_obj["velocity"] = std::numeric_limits<int32_t>::max();
    encoder_obj["direction"] = true;
    encoder_obj["last_pulse_timestamp"] = std::numeric_limits<uint64_t>::max();
    encoder_obj["pulses_per_revolution"] = std::numeric_limits<uint32_t>::max();
}

// Set average values for state messages
void set_gpio_state_average(JsonObject gpio_obj)
{
    gpio_obj["value"] = true;
    gpio_obj["last_change_timestamp"] = std::numeric_limits<uint64_t>::max() / 2;
}

void set_motor_state_average(JsonObject motor_obj)
{
    motor_obj["position"] = std::numeric_limits<int32_t>::max() / 2;
    motor_obj["target_position"] = std::numeric_limits<int32_t>::max() / 2;
    motor_obj["speed"] = std::numeric_limits<uint32_t>::max() / 2;
    motor_obj["is_moving"] = true;
    motor_obj["is_homed"] = true;
    motor_obj["fault_detected"] = false;
    motor_obj["current"] = std::numeric_limits<uint32_t>::max() / 2;
}

void set_encoder_state_average(JsonObject encoder_obj)
{
    encoder_obj["position"] = std::numeric_limits<int32_t>::max() / 2;
    encoder_obj["velocity"] = std::numeric_limits<int32_t>::max() / 2;
    encoder_obj["direction"] = true;
    encoder_obj["last_pulse_timestamp"] = std::numeric_limits<uint64_t>::max() / 2;
    encoder_obj["pulses_per_revolution"] = std::numeric_limits<uint32_t>::max() / 2;
}

// Set complete state messages with min/max/average values
void set_state_with_gpio_min(JsonObject state_obj)
{
    JsonObject gpio = state_obj["gpio"].to<JsonObject>();
    set_gpio_state_min(gpio);
    state_obj["timestamp"] = 0;
}

void set_state_with_motor_min(JsonObject state_obj)
{
    JsonObject motor = state_obj["motor"].to<JsonObject>();
    set_motor_state_min(motor);
    state_obj["timestamp"] = 0;
}

void set_state_with_encoder_min(JsonObject state_obj)
{
    JsonObject encoder = state_obj["encoder"].to<JsonObject>();
    set_encoder_state_min(encoder);
    state_obj["timestamp"] = 0;
}

void set_state_with_gpio_max(JsonObject state_obj)
{
    JsonObject gpio = state_obj["gpio"].to<JsonObject>();
    set_gpio_state_max(gpio);
    state_obj["timestamp"] = std::numeric_limits<uint64_t>::max();
}

void set_state_with_motor_max(JsonObject state_obj)
{
    JsonObject motor = state_obj["motor"].to<JsonObject>();
    set_motor_state_max(motor);
    state_obj["timestamp"] = std::numeric_limits<uint64_t>::max();
}

void set_state_with_encoder_max(JsonObject state_obj)
{
    JsonObject encoder = state_obj["encoder"].to<JsonObject>();
    set_encoder_state_max(encoder);
    state_obj["timestamp"] = std::numeric_limits<uint64_t>::max();
}

void set_state_with_gpio_average(JsonObject state_obj)
{
    JsonObject gpio = state_obj["gpio"].to<JsonObject>();
    set_gpio_state_average(gpio);
    state_obj["timestamp"] = std::numeric_limits<uint64_t>::max() / 2;
}

void set_state_with_motor_average(JsonObject state_obj)
{
    JsonObject motor = state_obj["motor"].to<JsonObject>();
    set_motor_state_average(motor);
    state_obj["timestamp"] = std::numeric_limits<uint64_t>::max() / 2;
}

void set_state_with_encoder_average(JsonObject state_obj)
{
    JsonObject encoder = state_obj["encoder"].to<JsonObject>();
    set_encoder_state_average(encoder);
    state_obj["timestamp"] = std::numeric_limits<uint64_t>::max() / 2;
}

} // End of namespace HelperCompleteDemoStateArduinoJson

#endif // HELPER_COMPLETE_DEMO_STATE_ARDUINOJSON
