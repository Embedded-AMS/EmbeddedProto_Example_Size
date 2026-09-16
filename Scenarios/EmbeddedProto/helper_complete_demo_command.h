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

#ifndef HELPER_COMPLETE_DEMO_COMMAND
#define HELPER_COMPLETE_DEMO_COMMAND

#include <complete_demo.h>
#include <limits>

namespace HelperCompleteDemoCommand
{

uint32_t global_command_id = 0;

// Set demo values for GPIO command
void set_gpio_command_demo(Command::GPIO& gpio_msg)
{
    gpio_msg.set_set_value(true);  // Set GPIO high
    gpio_msg.set_toggle(false);  // Don't toggle
    gpio_msg.set_pulse_duration_ms(100);  // 100ms pulse
}

// Set demo values for Motor command
void set_motor_command_demo(Command::Motor& motor_msg)
{
    motor_msg.set_target_position(5000);  // Move to position 5000
    motor_msg.set_target_speed(1000);  // At 1000 steps per second
    motor_msg.set_enable(true);  // Enable motor
    motor_msg.set_home(false);  // Don't home
    motor_msg.set_emergency_stop(false);  // No emergency stop
    motor_msg.set_clear_fault(false);  // Don't clear faults
    motor_msg.set_acceleration(2000);  // Acceleration of 2000
    motor_msg.set_deceleration(2000);  // Deceleration of 2000
}

// Set a complete Command message with GPIO command
void set_command_with_gpio_demo(Command& command_msg)
{
    Command::GPIO gpio_command;
    set_gpio_command_demo(gpio_command);
    command_msg.set_gpio(gpio_command);
    command_msg.set_command_id(global_command_id++);
}

// Set a complete Command message with Motor command
void set_command_with_motor_demo(Command& command_msg)
{
    Command::Motor motor_command;
    set_motor_command_demo(motor_command);
    command_msg.set_motor(motor_command);
    command_msg.set_command_id(global_command_id++);
}

// Comparison functions for demo values
bool compare_gpio_command_demo(const Command::GPIO& gpio_msg)
{
    return gpio_msg.get_set_value() == true && 
           gpio_msg.get_toggle() == false &&
           gpio_msg.get_pulse_duration_ms() == 100;
}

bool compare_motor_command_demo(const Command::Motor& motor_msg)
{
    return motor_msg.get_target_position() == 5000 &&
           motor_msg.get_target_speed() == 1000 &&
           motor_msg.get_enable() == true &&
           motor_msg.get_home() == false &&
           motor_msg.get_emergency_stop() == false &&
           motor_msg.get_clear_fault() == false &&
           motor_msg.get_acceleration() == 2000 &&
           motor_msg.get_deceleration() == 2000;
}

// Set minimum values for command messages
void set_gpio_command_min(Command::GPIO& gpio_msg)
{
    gpio_msg.set_set_value(false);
    gpio_msg.set_toggle(false);
    gpio_msg.set_pulse_duration_ms(0);
}

void set_motor_command_min(Command::Motor& motor_msg)
{
    motor_msg.set_target_position(std::numeric_limits<int32_t>::min());
    motor_msg.set_target_speed(0);
    motor_msg.set_enable(false);
    motor_msg.set_home(false);
    motor_msg.set_emergency_stop(false);
    motor_msg.set_clear_fault(false);
    motor_msg.set_acceleration(0);
    motor_msg.set_deceleration(0);
}

// Set maximum values for command messages
void set_gpio_command_max(Command::GPIO& gpio_msg)
{
    gpio_msg.set_set_value(true);
    gpio_msg.set_toggle(true);
    gpio_msg.set_pulse_duration_ms(std::numeric_limits<uint32_t>::max());
}

void set_motor_command_max(Command::Motor& motor_msg)
{
    motor_msg.set_target_position(std::numeric_limits<int32_t>::max());
    motor_msg.set_target_speed(std::numeric_limits<uint32_t>::max());
    motor_msg.set_enable(true);
    motor_msg.set_home(true);
    motor_msg.set_emergency_stop(true);
    motor_msg.set_clear_fault(true);
    motor_msg.set_acceleration(std::numeric_limits<uint32_t>::max());
    motor_msg.set_deceleration(std::numeric_limits<uint32_t>::max());
}

// Set average values for command messages
void set_gpio_command_average(Command::GPIO& gpio_msg)
{
    gpio_msg.set_set_value(true);
    gpio_msg.set_toggle(false);
    gpio_msg.set_pulse_duration_ms(std::numeric_limits<uint32_t>::max() / 2);
}

void set_motor_command_average(Command::Motor& motor_msg)
{
    motor_msg.set_target_position(std::numeric_limits<int32_t>::max() / 2);
    motor_msg.set_target_speed(std::numeric_limits<uint32_t>::max() / 2);
    motor_msg.set_enable(true);
    motor_msg.set_home(false);
    motor_msg.set_emergency_stop(false);
    motor_msg.set_clear_fault(false);
    motor_msg.set_acceleration(std::numeric_limits<uint32_t>::max() / 2);
    motor_msg.set_deceleration(std::numeric_limits<uint32_t>::max() / 2);
}

// Set complete command messages with min/max/average values
void set_command_with_gpio_min(Command& command_msg)
{
    Command::GPIO gpio_command;
    set_gpio_command_min(gpio_command);
    command_msg.set_gpio(gpio_command);
    command_msg.set_command_id(global_command_id++);
}

void set_command_with_motor_min(Command& command_msg)
{
    Command::Motor motor_command;
    set_motor_command_min(motor_command);
    command_msg.set_motor(motor_command);
    command_msg.set_command_id(global_command_id++);
}

void set_command_with_gpio_max(Command& command_msg)
{
    Command::GPIO gpio_command;
    set_gpio_command_max(gpio_command);
    command_msg.set_gpio(gpio_command);
    command_msg.set_command_id(global_command_id++);
}

void set_command_with_motor_max(Command& command_msg)
{
    Command::Motor motor_command;
    set_motor_command_max(motor_command);
    command_msg.set_motor(motor_command);
    command_msg.set_command_id(global_command_id++);
}

void set_command_with_gpio_average(Command& command_msg)
{
    Command::GPIO gpio_command;
    set_gpio_command_average(gpio_command);
    command_msg.set_gpio(gpio_command);
    command_msg.set_command_id(global_command_id++);
}

void set_command_with_motor_average(Command& command_msg)
{
    Command::Motor motor_command;
    set_motor_command_average(motor_command);
    command_msg.set_motor(motor_command);
    command_msg.set_command_id(global_command_id++);
}

// Convenience functions for common GPIO commands
void set_gpio_set_high_command(Command::GPIO& gpio_msg)
{
    gpio_msg.set_set_value(true);
    gpio_msg.set_toggle(false);
    gpio_msg.set_pulse_duration_ms(0);
}

void set_gpio_set_low_command(Command::GPIO& gpio_msg)
{
    gpio_msg.set_set_value(false);
    gpio_msg.set_toggle(false);
    gpio_msg.set_pulse_duration_ms(0);
}

void set_gpio_toggle_command(Command::GPIO& gpio_msg)
{
    gpio_msg.set_set_value(false);
    gpio_msg.set_toggle(true);
    gpio_msg.set_pulse_duration_ms(0);
}

void set_gpio_pulse_command(Command::GPIO& gpio_msg, uint32_t duration_ms)
{
    gpio_msg.set_set_value(true);
    gpio_msg.set_toggle(false);
    gpio_msg.set_pulse_duration_ms(duration_ms);
}

// Convenience functions for common Motor commands
void set_motor_enable_command(Command::Motor& motor_msg)
{
    motor_msg.set_target_position(0);
    motor_msg.set_target_speed(0);
    motor_msg.set_enable(true);
    motor_msg.set_home(false);
    motor_msg.set_emergency_stop(false);
    motor_msg.set_clear_fault(false);
    motor_msg.set_acceleration(0);
    motor_msg.set_deceleration(0);
}

void set_motor_disable_command(Command::Motor& motor_msg)
{
    motor_msg.set_target_position(0);
    motor_msg.set_target_speed(0);
    motor_msg.set_enable(false);
    motor_msg.set_home(false);
    motor_msg.set_emergency_stop(false);
    motor_msg.set_clear_fault(false);
    motor_msg.set_acceleration(0);
    motor_msg.set_deceleration(0);
}

void set_motor_home_command(Command::Motor& motor_msg)
{
    motor_msg.set_target_position(0);
    motor_msg.set_target_speed(0);
    motor_msg.set_enable(true);
    motor_msg.set_home(true);
    motor_msg.set_emergency_stop(false);
    motor_msg.set_clear_fault(false);
    motor_msg.set_acceleration(0);
    motor_msg.set_deceleration(0);
}

void set_motor_emergency_stop_command(Command::Motor& motor_msg)
{
    motor_msg.set_target_position(0);
    motor_msg.set_target_speed(0);
    motor_msg.set_enable(false);
    motor_msg.set_home(false);
    motor_msg.set_emergency_stop(true);
    motor_msg.set_clear_fault(false);
    motor_msg.set_acceleration(0);
    motor_msg.set_deceleration(0);
}

void set_motor_move_to_position_command(Command::Motor& motor_msg, int32_t position, uint32_t speed)
{
    motor_msg.set_target_position(position);
    motor_msg.set_target_speed(speed);
    motor_msg.set_enable(true);
    motor_msg.set_home(false);
    motor_msg.set_emergency_stop(false);
    motor_msg.set_clear_fault(false);
    motor_msg.set_acceleration(speed * 2);  // Simple acceleration calculation
    motor_msg.set_deceleration(speed * 2);  // Simple deceleration calculation
}

} // End of namespace HelperCompleteDemoCommand

#endif // HELPER_COMPLETE_DEMO_COMMAND
