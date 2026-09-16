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

#ifndef HELPER_COMPLETE_DEMO_CONFIG
#define HELPER_COMPLETE_DEMO_CONFIG

#include <complete_demo.h>
#include <limits>

namespace HelperCompleteDemoConfig
{

// Set demo values for GPIO configuration
void set_gpio_config_demo(Config::GPIO& gpio_msg)
{
    gpio_msg.set_pin(13);  // Common GPIO pin
    gpio_msg.set_mode(Config::GPIO_MODE::OUTPUT_PP);  // Push-pull output
}

// Set demo values for UART configuration
void set_uart_config_demo(Config::UART& uart_msg)
{
    uart_msg.set_id(1);  // UART1
    uart_msg.set_baud(115200);  // Common baud rate
    uart_msg.set_data_bits(8);  // 8 data bits
    uart_msg.set_stop_bits(1);  // 1 stop bit
    uart_msg.set_parity_enable(false);  // No parity
    uart_msg.set_parity_even(false);  // Even parity (irrelevant when disabled)
    uart_msg.set_flow_control(false);  // No flow control
}

// Set demo values for I2C configuration
void set_i2c_config_demo(Config::I2C& i2c_msg)
{
    i2c_msg.set_id(1);  // I2C1
    i2c_msg.set_clock_speed(100000);  // 100 kHz standard mode
    i2c_msg.set_pullup_enable(true);  // Enable internal pullups
    i2c_msg.set_timeout(100);  // 100ms timeout
    i2c_msg.set_general_call_enable(false);  // Disable general call
}

// Set demo values for SPI configuration
void set_spi_config_demo(Config::SPI& spi_msg)
{
    spi_msg.set_id(1);  // SPI1
    spi_msg.set_baud_rate_prescaler(8);  // Prescaler of 8
    spi_msg.set_clock_polarity(false);  // CPOL = 0
    spi_msg.set_clock_phase(false);  // CPHA = 0 (Mode 0)
    spi_msg.set_msb_first(true);  // MSB first
    spi_msg.set_software_slave_management(false);  // Hardware slave management
    spi_msg.set_internal_slave_select(true);  // Use internal NSS pin
}

// Set demo values for Motor configuration
void set_motor_config_demo(Config::Motor& motor_msg)
{
    motor_msg.set_spi_id(1);  // Use SPI1
    motor_msg.set_enabled(true);  // Motor enabled
    motor_msg.set_enable_micro_stepping(true);  // Microstepping enabled
    motor_msg.set_home_direction_inverted(false);  // Normal home direction
    motor_msg.set_home_switch_pin(25);  // GPIO pin 25 for home switch
}

// Set demo values for Encoder configuration
void set_encoder_config_demo(Config::Encoder& encoder_msg)
{
    encoder_msg.set_gpio_pin_a(10);  // Pin A on GPIO 10
    encoder_msg.set_gpio_pin_b(11);  // Pin B on GPIO 11
    encoder_msg.set_inverted(false);  // Normal direction
}

// Set a complete Config message with GPIO peripheral
void set_config_with_gpio_demo(Config& config_msg)
{
    Config::GPIO gpio_config;
    set_gpio_config_demo(gpio_config);
    config_msg.set_gpio(gpio_config);
}

// Set a complete Config message with UART peripheral
void set_config_with_uart_demo(Config& config_msg)
{
    Config::UART uart_config;
    set_uart_config_demo(uart_config);
    config_msg.set_uart(uart_config);
}

// Set a complete Config message with I2C peripheral
void set_config_with_i2c_demo(Config& config_msg)
{
    Config::I2C i2c_config;
    set_i2c_config_demo(i2c_config);
    config_msg.set_i2c(i2c_config);
}

// Set a complete Config message with SPI peripheral
void set_config_with_spi_demo(Config& config_msg)
{
    Config::SPI spi_config;
    set_spi_config_demo(spi_config);
    config_msg.set_spi(spi_config);
}

// Set a complete Config message with Motor device
void set_config_with_motor_demo(Config& config_msg)
{
    Config::Motor motor_config;
    set_motor_config_demo(motor_config);
    config_msg.set_motor(motor_config);
}

// Set a complete Config message with Encoder device
void set_config_with_encoder_demo(Config& config_msg)
{
    Config::Encoder encoder_config;
    set_encoder_config_demo(encoder_config);
    config_msg.set_encoder(encoder_config);
}

// Comparison functions for demo values
bool compare_gpio_config_demo(const Config::GPIO& gpio_msg)
{
    return gpio_msg.get_pin() == 13 && 
           gpio_msg.get_mode() == Config::GPIO_MODE::OUTPUT_PP;
}

bool compare_uart_config_demo(const Config::UART& uart_msg)
{
    return uart_msg.get_id() == 1 && 
           uart_msg.get_baud() == 115200 &&
           uart_msg.get_data_bits() == 8 &&
           uart_msg.get_stop_bits() == 1 &&
           uart_msg.get_parity_enable() == false &&
           uart_msg.get_parity_even() == false &&
           uart_msg.get_flow_control() == false;
}

bool compare_i2c_config_demo(const Config::I2C& i2c_msg)
{
    return i2c_msg.get_id() == 1 && 
           i2c_msg.get_clock_speed() == 100000 &&
           i2c_msg.get_pullup_enable() == true &&
           i2c_msg.get_timeout() == 100 &&
           i2c_msg.get_general_call_enable() == false;
}

bool compare_spi_config_demo(const Config::SPI& spi_msg)
{
    return spi_msg.get_id() == 1 && 
           spi_msg.get_baud_rate_prescaler() == 8 &&
           spi_msg.get_clock_polarity() == false &&
           spi_msg.get_clock_phase() == false &&
           spi_msg.get_msb_first() == true &&
           spi_msg.get_software_slave_management() == false &&
           spi_msg.get_internal_slave_select() == true;
}

bool compare_motor_config_demo(const Config::Motor& motor_msg)
{
    return motor_msg.get_spi_id() == 1 && 
           motor_msg.get_enabled() == true &&
           motor_msg.get_enable_micro_stepping() == true &&
           motor_msg.get_home_direction_inverted() == false &&
           motor_msg.get_home_switch_pin() == 25;
}

bool compare_encoder_config_demo(const Config::Encoder& encoder_msg)
{
    return encoder_msg.get_gpio_pin_a() == 10 &&
           encoder_msg.get_gpio_pin_b() == 11 &&
           encoder_msg.get_inverted() == false;
}

} // End of namespace HelperCompleteDemoConfig

#endif // HELPER_COMPLETE_DEMO_CONFIG
