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

#ifndef HELPER_COMPLETE_DEMO_STATE_NANOPB
#define HELPER_COMPLETE_DEMO_STATE_NANOPB

#include "complete_demo.pb.h"
#include <limits>

namespace HelperCompleteDemoStateNanoPB
{

// Set demo values for GPIO state
void set_gpio_state_demo(State_GPIO& gpio_msg)
{
    gpio_msg.value = true;  // GPIO high
    gpio_msg.last_change_timestamp = 123456789;  // Some timestamp
}

// Set demo values for Motor state
void set_motor_state_demo(State_Motor& motor_msg)
{
    motor_msg.position = 1000;  // 1000 steps position
    motor_msg.target_position = 2000;  // Target 2000 steps
    motor_msg.speed = 500;  // 500 steps per second
    motor_msg.is_moving = true;  // Motor is moving
    motor_msg.is_homed = true;  // Motor is homed
    motor_msg.fault_detected = false;  // No faults
    motor_msg.current = 1500;  // 1500 mA current
}

// Set demo values for Encoder state
void set_encoder_state_demo(State_Encoder& encoder_msg)
{
    encoder_msg.position = 5000;  // 5000 encoder counts
    encoder_msg.velocity = 100;  // 100 counts per second
    encoder_msg.direction = true;  // Forward direction
    encoder_msg.last_pulse_timestamp = 987654321;  // Some timestamp
    encoder_msg.pulses_per_revolution = 1000;  // 1000 PPR encoder
}

// Set a complete State message with GPIO state
void set_state_with_gpio_demo(State& state_msg)
{
    State_GPIO gpio_state = State_GPIO_init_default;
    set_gpio_state_demo(gpio_state);
    state_msg.state.gpio = gpio_state;
    state_msg.timestamp = 1000000;  // 1 million timestamp
    state_msg.which_state = State_gpio_tag;
}

// Set a complete State message with Motor state
void set_state_with_motor_demo(State& state_msg)
{
    State_Motor motor_state = State_Motor_init_default;
    set_motor_state_demo(motor_state);
    state_msg.state.motor = motor_state;
    state_msg.timestamp = 2000000;  // 2 million timestamp
    state_msg.which_state = State_motor_tag;
}

// Set a complete State message with Encoder state
void set_state_with_encoder_demo(State& state_msg)
{
    State_Encoder encoder_state = State_Encoder_init_default;
    set_encoder_state_demo(encoder_state);
    state_msg.state.encoder = encoder_state;
    state_msg.timestamp = 3000000;  // 3 million timestamp
    state_msg.which_state = State_encoder_tag;
}

// Comparison functions for demo values
bool compare_gpio_state_demo(const State_GPIO& gpio_msg)
{
    return gpio_msg.value == true && 
           gpio_msg.last_change_timestamp == 123456789;
}

bool compare_motor_state_demo(const State_Motor& motor_msg)
{
    return motor_msg.position == 1000 &&
           motor_msg.target_position == 2000 &&
           motor_msg.speed == 500 &&
           motor_msg.is_moving == true &&
           motor_msg.is_homed == true &&
           motor_msg.fault_detected == false &&
           motor_msg.current == 1500;
}

bool compare_encoder_state_demo(const State_Encoder& encoder_msg)
{
    return encoder_msg.position == 5000 &&
           encoder_msg.velocity == 100 &&
           encoder_msg.direction == true &&
           encoder_msg.last_pulse_timestamp == 987654321 &&
           encoder_msg.pulses_per_revolution == 1000;
}

// Set minimum values for state messages
void set_gpio_state_min(State_GPIO& gpio_msg)
{
    gpio_msg.value = false;
    gpio_msg.last_change_timestamp = 0;
}

void set_motor_state_min(State_Motor& motor_msg)
{
    motor_msg.position = std::numeric_limits<int32_t>::min();
    motor_msg.target_position = std::numeric_limits<int32_t>::min();
    motor_msg.speed = 0;
    motor_msg.is_moving = false;
    motor_msg.is_homed = false;
    motor_msg.fault_detected = false;
    motor_msg.current = 0;
}

void set_encoder_state_min(State_Encoder& encoder_msg)
{
    encoder_msg.position = std::numeric_limits<int32_t>::min();
    encoder_msg.velocity = std::numeric_limits<int32_t>::min();
    encoder_msg.direction = false;
    encoder_msg.last_pulse_timestamp = 0;
    encoder_msg.pulses_per_revolution = 0;
}

// Set maximum values for state messages
void set_gpio_state_max(State_GPIO& gpio_msg)
{
    gpio_msg.value = true;
    gpio_msg.last_change_timestamp = std::numeric_limits<uint64_t>::max();
}

void set_motor_state_max(State_Motor& motor_msg)
{
    motor_msg.position = std::numeric_limits<int32_t>::max();
    motor_msg.target_position = std::numeric_limits<int32_t>::max();
    motor_msg.speed = std::numeric_limits<uint32_t>::max();
    motor_msg.is_moving = true;
    motor_msg.is_homed = true;
    motor_msg.fault_detected = true;
    motor_msg.current = std::numeric_limits<uint32_t>::max();
}

void set_encoder_state_max(State_Encoder& encoder_msg)
{
    encoder_msg.position = std::numeric_limits<int32_t>::max();
    encoder_msg.velocity = std::numeric_limits<int32_t>::max();
    encoder_msg.direction = true;
    encoder_msg.last_pulse_timestamp = std::numeric_limits<uint64_t>::max();
    encoder_msg.pulses_per_revolution = std::numeric_limits<uint32_t>::max();
}

// Set average values for state messages
void set_gpio_state_average(State_GPIO& gpio_msg)
{
    gpio_msg.value = true;
    gpio_msg.last_change_timestamp = std::numeric_limits<uint64_t>::max() / 2;
}

void set_motor_state_average(State_Motor& motor_msg)
{
    motor_msg.position = std::numeric_limits<int32_t>::max() / 2;
    motor_msg.target_position = std::numeric_limits<int32_t>::max() / 2;
    motor_msg.speed = std::numeric_limits<uint32_t>::max() / 2;
    motor_msg.is_moving = true;
    motor_msg.is_homed = true;
    motor_msg.fault_detected = false;
    motor_msg.current = std::numeric_limits<uint32_t>::max() / 2;
}

void set_encoder_state_average(State_Encoder& encoder_msg)
{
    encoder_msg.position = std::numeric_limits<int32_t>::max() / 2;
    encoder_msg.velocity = std::numeric_limits<int32_t>::max() / 2;
    encoder_msg.direction = true;
    encoder_msg.last_pulse_timestamp = std::numeric_limits<uint64_t>::max() / 2;
    encoder_msg.pulses_per_revolution = std::numeric_limits<uint32_t>::max() / 2;
}

// Set complete state messages with min/max/average values
void set_state_with_gpio_min(State& state_msg)
{
    State_GPIO gpio_state = State_GPIO_init_default;
    set_gpio_state_min(gpio_state);
    state_msg.state.gpio = gpio_state;
    state_msg.timestamp = 0;
    state_msg.which_state = State_gpio_tag;
}

void set_state_with_motor_min(State& state_msg)
{
    State_Motor motor_state = State_Motor_init_default;
    set_motor_state_min(motor_state);
    state_msg.state.motor = motor_state;
    state_msg.timestamp = 0;
    state_msg.which_state = State_motor_tag;
}

void set_state_with_encoder_min(State& state_msg)
{
    State_Encoder encoder_state = State_Encoder_init_default;
    set_encoder_state_min(encoder_state);
    state_msg.state.encoder = encoder_state;
    state_msg.timestamp = 0;
    state_msg.which_state = State_encoder_tag;
}

void set_state_with_gpio_max(State& state_msg)
{
    State_GPIO gpio_state = State_GPIO_init_default;
    set_gpio_state_max(gpio_state);
    state_msg.state.gpio = gpio_state;
    state_msg.timestamp = std::numeric_limits<uint64_t>::max();
    state_msg.which_state = State_gpio_tag;
}

void set_state_with_motor_max(State& state_msg)
{
    State_Motor motor_state = State_Motor_init_default;
    set_motor_state_max(motor_state);
    state_msg.state.motor = motor_state;
    state_msg.timestamp = std::numeric_limits<uint64_t>::max();
    state_msg.which_state = State_motor_tag;
}

void set_state_with_encoder_max(State& state_msg)
{
    State_Encoder encoder_state = State_Encoder_init_default;
    set_encoder_state_max(encoder_state);
    state_msg.state.encoder = encoder_state;
    state_msg.timestamp = std::numeric_limits<uint64_t>::max();
    state_msg.which_state = State_encoder_tag;
}

void set_state_with_gpio_average(State& state_msg)
{
    State_GPIO gpio_state = State_GPIO_init_default;
    set_gpio_state_average(gpio_state);
    state_msg.state.gpio = gpio_state;
    state_msg.timestamp = std::numeric_limits<uint64_t>::max() / 2;
    state_msg.which_state = State_gpio_tag;
}

void set_state_with_motor_average(State& state_msg)
{
    State_Motor motor_state = State_Motor_init_default;
    set_motor_state_average(motor_state);
    state_msg.state.motor = motor_state;
    state_msg.timestamp = std::numeric_limits<uint64_t>::max() / 2;
    state_msg.which_state = State_motor_tag;
}

void set_state_with_encoder_average(State& state_msg)
{
    State_Encoder encoder_state = State_Encoder_init_default;
    set_encoder_state_average(encoder_state);
    state_msg.state.encoder = encoder_state;
    state_msg.timestamp = std::numeric_limits<uint64_t>::max() / 2;
    state_msg.which_state = State_encoder_tag;
}

} // End of namespace HelperCompleteDemoStateNanoPB

#endif // HELPER_COMPLETE_DEMO_STATE_NANOPB