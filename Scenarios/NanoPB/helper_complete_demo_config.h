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

#ifndef HELPER_COMPLETE_DEMO_CONFIG_NANOPB
#define HELPER_COMPLETE_DEMO_CONFIG_NANOPB

#include "complete_demo.pb.h"
#include <limits>

namespace HelperCompleteDemoConfigNanoPB
{

// Set demo values for GPIO configuration
void set_gpio_config_demo(Config_GPIO& gpio_msg)
{
    gpio_msg.pin = 13;  // Common GPIO pin
    gpio_msg.mode = Config_GPIO_MODE_OUTPUT_PP;  // Push-pull output
}

// Set demo values for UART configuration
void set_uart_config_demo(Config_UART& uart_msg)
{
    uart_msg.id = 1;  // UART1
    uart_msg.baud = 115200;  // Common baud rate
    uart_msg.data_bits = 8;  // 8 data bits
    uart_msg.stop_bits = 1;  // 1 stop bit
    uart_msg.parity_enable = false;  // No parity
    uart_msg.parity_even = false;  // Even parity (irrelevant when disabled)
    uart_msg.flow_control = false;  // No flow control
}

// Set demo values for I2C configuration
void set_i2c_config_demo(Config_I2C& i2c_msg)
{
    i2c_msg.id = 1;  // I2C1
    i2c_msg.clock_speed = 100000;  // 100 kHz standard mode
    i2c_msg.pullup_enable = true;  // Enable internal pullups
    i2c_msg.timeout = 100;  // 100ms timeout
    i2c_msg.general_call_enable = false;  // Disable general call
}

// Set demo values for SPI configuration
void set_spi_config_demo(Config_SPI& spi_msg)
{
    spi_msg.id = 1;  // SPI1
    spi_msg.baud_rate_prescaler = 8;  // Prescaler of 8
    spi_msg.clock_polarity = false;  // CPOL = 0
    spi_msg.clock_phase = false;  // CPHA = 0 (Mode 0)
    spi_msg.msb_first = true;  // MSB first
    spi_msg.software_slave_management = false;  // Hardware slave management
    spi_msg.internal_slave_select = true;  // Use internal NSS pin
}

// Set demo values for Motor configuration
void set_motor_config_demo(Config_Motor& motor_msg)
{
    motor_msg.spi_id = 1;  // Use SPI1
    motor_msg.enabled = true;  // Motor enabled
    motor_msg.enable_micro_stepping = true;  // Microstepping enabled
    motor_msg.home_direction_inverted = false;  // Normal home direction
    motor_msg.home_switch_pin = 25;  // GPIO pin 25 for home switch
}

// Set demo values for Encoder configuration
void set_encoder_config_demo(Config_Encoder& encoder_msg)
{
    encoder_msg.gpio_pin_a = 10;  // Pin A on GPIO 10
    encoder_msg.gpio_pin_b = 11;  // Pin B on GPIO 11
    encoder_msg.inverted = false;  // Normal direction
}

// Set a complete Config message with GPIO peripheral
void set_config_with_gpio_demo(Config& config_msg)
{
    Config_GPIO gpio_config = Config_GPIO_init_default;
    set_gpio_config_demo(gpio_config);
    config_msg.peripheral.gpio = gpio_config;
    config_msg.which_peripheral = Config_gpio_tag;
}

// Set a complete Config message with UART peripheral
void set_config_with_uart_demo(Config& config_msg)
{
    Config_UART uart_config = Config_UART_init_default;
    set_uart_config_demo(uart_config);
    config_msg.peripheral.uart = uart_config;
    config_msg.which_peripheral = Config_uart_tag;
}

// Set a complete Config message with I2C peripheral
void set_config_with_i2c_demo(Config& config_msg)
{
    Config_I2C i2c_config = Config_I2C_init_default;
    set_i2c_config_demo(i2c_config);
    config_msg.peripheral.i2c = i2c_config;
    config_msg.which_peripheral = Config_i2c_tag;
}

// Set a complete Config message with SPI peripheral
void set_config_with_spi_demo(Config& config_msg)
{
    Config_SPI spi_config = Config_SPI_init_default;
    set_spi_config_demo(spi_config);
    config_msg.peripheral.spi = spi_config;
    config_msg.which_peripheral = Config_spi_tag;
}

// Set a complete Config message with Motor device
void set_config_with_motor_demo(Config& config_msg)
{
    Config_Motor motor_config = Config_Motor_init_default;
    set_motor_config_demo(motor_config);
    config_msg.device.motor = motor_config;
    config_msg.which_device = Config_motor_tag;
}

// Set a complete Config message with Encoder device
void set_config_with_encoder_demo(Config& config_msg)
{
    Config_Encoder encoder_config = Config_Encoder_init_default;
    set_encoder_config_demo(encoder_config);
    config_msg.device.encoder = encoder_config;
    config_msg.which_device = Config_encoder_tag;
}

// Comparison functions for demo values
bool compare_gpio_config_demo(const Config_GPIO& gpio_msg)
{
    return gpio_msg.pin == 13 && 
           gpio_msg.mode == Config_GPIO_MODE_OUTPUT_PP;
}

bool compare_uart_config_demo(const Config_UART& uart_msg)
{
    return uart_msg.id == 1 && 
           uart_msg.baud == 115200 &&
           uart_msg.data_bits == 8 &&
           uart_msg.stop_bits == 1 &&
           uart_msg.parity_enable == false &&
           uart_msg.parity_even == false &&
           uart_msg.flow_control == false;
}

bool compare_i2c_config_demo(const Config_I2C& i2c_msg)
{
    return i2c_msg.id == 1 && 
           i2c_msg.clock_speed == 100000 &&
           i2c_msg.pullup_enable == true &&
           i2c_msg.timeout == 100 &&
           i2c_msg.general_call_enable == false;
}

bool compare_spi_config_demo(const Config_SPI& spi_msg)
{
    return spi_msg.id == 1 && 
           spi_msg.baud_rate_prescaler == 8 &&
           spi_msg.clock_polarity == false &&
           spi_msg.clock_phase == false &&
           spi_msg.msb_first == true &&
           spi_msg.software_slave_management == false &&
           spi_msg.internal_slave_select == true;
}

bool compare_motor_config_demo(const Config_Motor& motor_msg)
{
    return motor_msg.spi_id == 1 && 
           motor_msg.enabled == true &&
           motor_msg.enable_micro_stepping == true &&
           motor_msg.home_direction_inverted == false &&
           motor_msg.home_switch_pin == 25;
}

bool compare_encoder_config_demo(const Config_Encoder& encoder_msg)
{
    return encoder_msg.gpio_pin_a == 10 &&
           encoder_msg.gpio_pin_b == 11 &&
           encoder_msg.inverted == false;
}

} // End of namespace HelperCompleteDemoConfigNanoPB

#endif // HELPER_COMPLETE_DEMO_CONFIG_NANOPB