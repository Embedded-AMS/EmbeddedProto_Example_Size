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

#ifndef HELPER_COMPLETE_DEMO_STATE
#define HELPER_COMPLETE_DEMO_STATE

#include <complete_demo.h>
#include <limits>

namespace HelperCompleteDemoState
{

// Set demo values for GPIO state
void set_gpio_state_demo(State::GPIO& gpio_msg)
{
    gpio_msg.set_value(true);  // GPIO high
    gpio_msg.set_last_change_timestamp(123456789);  // Some timestamp
}

// Set demo values for Motor state
void set_motor_state_demo(State::Motor& motor_msg)
{
    motor_msg.set_position(1000);  // 1000 steps position
    motor_msg.set_target_position(2000);  // Target 2000 steps
    motor_msg.set_speed(500);  // 500 steps per second
    motor_msg.set_is_moving(true);  // Motor is moving
    motor_msg.set_is_homed(true);  // Motor is homed
    motor_msg.set_fault_detected(false);  // No faults
    motor_msg.set_current(1500);  // 1500 mA current
}

// Set demo values for Encoder state
void set_encoder_state_demo(State::Encoder& encoder_msg)
{
    encoder_msg.set_position(5000);  // 5000 encoder counts
    encoder_msg.set_velocity(100);  // 100 counts per second
    encoder_msg.set_direction(true);  // Forward direction
    encoder_msg.set_last_pulse_timestamp(987654321);  // Some timestamp
    encoder_msg.set_pulses_per_revolution(1000);  // 1000 PPR encoder
}

// Set a complete State message with GPIO state
void set_state_with_gpio_demo(State& state_msg)
{
    State::GPIO gpio_state;
    set_gpio_state_demo(gpio_state);
    state_msg.set_gpio(gpio_state);
    state_msg.set_timestamp(1000000);  // 1 million timestamp
}

// Set a complete State message with Motor state
void set_state_with_motor_demo(State& state_msg)
{
    State::Motor motor_state;
    set_motor_state_demo(motor_state);
    state_msg.set_motor(motor_state);
    state_msg.set_timestamp(2000000);  // 2 million timestamp
}

// Set a complete State message with Encoder state
void set_state_with_encoder_demo(State& state_msg)
{
    State::Encoder encoder_state;
    set_encoder_state_demo(encoder_state);
    state_msg.set_encoder(encoder_state);
    state_msg.set_timestamp(3000000);  // 3 million timestamp
}

// Comparison functions for demo values
bool compare_gpio_state_demo(const State::GPIO& gpio_msg)
{
    return gpio_msg.get_value() == true && 
           gpio_msg.get_last_change_timestamp() == 123456789;
}

bool compare_motor_state_demo(const State::Motor& motor_msg)
{
    return motor_msg.get_position() == 1000 &&
           motor_msg.get_target_position() == 2000 &&
           motor_msg.get_speed() == 500 &&
           motor_msg.get_is_moving() == true &&
           motor_msg.get_is_homed() == true &&
           motor_msg.get_fault_detected() == false &&
           motor_msg.get_current() == 1500;
}

bool compare_encoder_state_demo(const State::Encoder& encoder_msg)
{
    return encoder_msg.get_position() == 5000 &&
           encoder_msg.get_velocity() == 100 &&
           encoder_msg.get_direction() == true &&
           encoder_msg.get_last_pulse_timestamp() == 987654321 &&
           encoder_msg.get_pulses_per_revolution() == 1000;
}

// Set minimum values for state messages
void set_gpio_state_min(State::GPIO& gpio_msg)
{
    gpio_msg.set_value(false);
    gpio_msg.set_last_change_timestamp(0);
}

void set_motor_state_min(State::Motor& motor_msg)
{
    motor_msg.set_position(std::numeric_limits<int32_t>::min());
    motor_msg.set_target_position(std::numeric_limits<int32_t>::min());
    motor_msg.set_speed(0);
    motor_msg.set_is_moving(false);
    motor_msg.set_is_homed(false);
    motor_msg.set_fault_detected(false);
    motor_msg.set_current(0);
}

void set_encoder_state_min(State::Encoder& encoder_msg)
{
    encoder_msg.set_position(std::numeric_limits<int32_t>::min());
    encoder_msg.set_velocity(std::numeric_limits<int32_t>::min());
    encoder_msg.set_direction(false);
    encoder_msg.set_last_pulse_timestamp(0);
    encoder_msg.set_pulses_per_revolution(0);
}

// Set maximum values for state messages
void set_gpio_state_max(State::GPIO& gpio_msg)
{
    gpio_msg.set_value(true);
    gpio_msg.set_last_change_timestamp(std::numeric_limits<uint64_t>::max());
}

void set_motor_state_max(State::Motor& motor_msg)
{
    motor_msg.set_position(std::numeric_limits<int32_t>::max());
    motor_msg.set_target_position(std::numeric_limits<int32_t>::max());
    motor_msg.set_speed(std::numeric_limits<uint32_t>::max());
    motor_msg.set_is_moving(true);
    motor_msg.set_is_homed(true);
    motor_msg.set_fault_detected(true);
    motor_msg.set_current(std::numeric_limits<uint32_t>::max());
}

void set_encoder_state_max(State::Encoder& encoder_msg)
{
    encoder_msg.set_position(std::numeric_limits<int32_t>::max());
    encoder_msg.set_velocity(std::numeric_limits<int32_t>::max());
    encoder_msg.set_direction(true);
    encoder_msg.set_last_pulse_timestamp(std::numeric_limits<uint64_t>::max());
    encoder_msg.set_pulses_per_revolution(std::numeric_limits<uint32_t>::max());
}

// Set average values for state messages
void set_gpio_state_average(State::GPIO& gpio_msg)
{
    gpio_msg.set_value(true);
    gpio_msg.set_last_change_timestamp(std::numeric_limits<uint64_t>::max() / 2);
}

void set_motor_state_average(State::Motor& motor_msg)
{
    motor_msg.set_position(std::numeric_limits<int32_t>::max() / 2);
    motor_msg.set_target_position(std::numeric_limits<int32_t>::max() / 2);
    motor_msg.set_speed(std::numeric_limits<uint32_t>::max() / 2);
    motor_msg.set_is_moving(true);
    motor_msg.set_is_homed(true);
    motor_msg.set_fault_detected(false);
    motor_msg.set_current(std::numeric_limits<uint32_t>::max() / 2);
}

void set_encoder_state_average(State::Encoder& encoder_msg)
{
    encoder_msg.set_position(std::numeric_limits<int32_t>::max() / 2);
    encoder_msg.set_velocity(std::numeric_limits<int32_t>::max() / 2);
    encoder_msg.set_direction(true);
    encoder_msg.set_last_pulse_timestamp(std::numeric_limits<uint64_t>::max() / 2);
    encoder_msg.set_pulses_per_revolution(std::numeric_limits<uint32_t>::max() / 2);
}

// Set complete state messages with min/max/average values
void set_state_with_gpio_min(State& state_msg)
{
    State::GPIO gpio_state;
    set_gpio_state_min(gpio_state);
    state_msg.set_gpio(gpio_state);
    state_msg.set_timestamp(0);
}

void set_state_with_motor_min(State& state_msg)
{
    State::Motor motor_state;
    set_motor_state_min(motor_state);
    state_msg.set_motor(motor_state);
    state_msg.set_timestamp(0);
}

void set_state_with_encoder_min(State& state_msg)
{
    State::Encoder encoder_state;
    set_encoder_state_min(encoder_state);
    state_msg.set_encoder(encoder_state);
    state_msg.set_timestamp(0);
}

void set_state_with_gpio_max(State& state_msg)
{
    State::GPIO gpio_state;
    set_gpio_state_max(gpio_state);
    state_msg.set_gpio(gpio_state);
    state_msg.set_timestamp(std::numeric_limits<uint64_t>::max());
}

void set_state_with_motor_max(State& state_msg)
{
    State::Motor motor_state;
    set_motor_state_max(motor_state);
    state_msg.set_motor(motor_state);
    state_msg.set_timestamp(std::numeric_limits<uint64_t>::max());
}

void set_state_with_encoder_max(State& state_msg)
{
    State::Encoder encoder_state;
    set_encoder_state_max(encoder_state);
    state_msg.set_encoder(encoder_state);
    state_msg.set_timestamp(std::numeric_limits<uint64_t>::max());
}

void set_state_with_gpio_average(State& state_msg)
{
    State::GPIO gpio_state;
    set_gpio_state_average(gpio_state);
    state_msg.set_gpio(gpio_state);
    state_msg.set_timestamp(std::numeric_limits<uint64_t>::max() / 2);
}

void set_state_with_motor_average(State& state_msg)
{
    State::Motor motor_state;
    set_motor_state_average(motor_state);
    state_msg.set_motor(motor_state);
    state_msg.set_timestamp(std::numeric_limits<uint64_t>::max() / 2);
}

void set_state_with_encoder_average(State& state_msg)
{
    State::Encoder encoder_state;
    set_encoder_state_average(encoder_state);
    state_msg.set_encoder(encoder_state);
    state_msg.set_timestamp(std::numeric_limits<uint64_t>::max() / 2);
}

} // End of namespace HelperCompleteDemoState

#endif // HELPER_COMPLETE_DEMO_STATE
