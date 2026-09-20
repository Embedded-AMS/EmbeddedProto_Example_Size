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

#ifndef HELPER_COMPLETE_DEMO_CONFIG_ARDUINOJSON
#define HELPER_COMPLETE_DEMO_CONFIG_ARDUINOJSON

#include <ArduinoJson.h>
#include <limits>

namespace HelperCompleteDemoConfigArduinoJson
{

// GPIO_MODE enum values (matching proto definition)
enum GPIO_MODE {
    GPIO_MODE_UNKNOWN = 0,
    INPUT = 1,
    OUTPUT_PP = 2,
    OUTPUT_OD = 3
};

// Set demo values for GPIO configuration
void set_gpio_config_demo(JsonObject gpio_obj)
{
    gpio_obj["pin"] = 13;  // Common GPIO pin
    gpio_obj["mode"] = OUTPUT_PP;  // Push-pull output
}

// Set demo values for UART configuration
void set_uart_config_demo(JsonObject uart_obj)
{
    uart_obj["id"] = 1;  // UART1
    uart_obj["baud"] = 115200;  // Common baud rate
    uart_obj["data_bits"] = 8;  // 8 data bits
    uart_obj["stop_bits"] = 1;  // 1 stop bit
    uart_obj["parity_enable"] = false;  // No parity
    uart_obj["parity_even"] = false;  // Even parity (irrelevant when disabled)
    uart_obj["flow_control"] = false;  // No flow control
}

// Set demo values for I2C configuration
void set_i2c_config_demo(JsonObject i2c_obj)
{
    i2c_obj["id"] = 1;  // I2C1
    i2c_obj["clock_speed"] = 100000;  // 100 kHz standard mode
    i2c_obj["pullup_enable"] = true;  // Enable internal pullups
    i2c_obj["timeout"] = 100;  // 100ms timeout
    i2c_obj["general_call_enable"] = false;  // Disable general call
}

// Set demo values for SPI configuration
void set_spi_config_demo(JsonObject spi_obj)
{
    spi_obj["id"] = 1;  // SPI1
    spi_obj["baud_rate_prescaler"] = 8;  // Prescaler of 8
    spi_obj["clock_polarity"] = false;  // CPOL = 0
    spi_obj["clock_phase"] = false;  // CPHA = 0 (Mode 0)
    spi_obj["msb_first"] = true;  // MSB first
    spi_obj["software_slave_management"] = false;  // Hardware slave management
    spi_obj["internal_slave_select"] = true;  // Use internal NSS pin
}

// Set demo values for Motor configuration
void set_motor_config_demo(JsonObject motor_obj)
{
    motor_obj["spi_id"] = 1;  // Use SPI1
    motor_obj["enabled"] = true;  // Motor enabled
    motor_obj["enable_micro_stepping"] = true;  // Microstepping enabled
    motor_obj["home_direction_inverted"] = false;  // Normal home direction
    motor_obj["home_switch_pin"] = 25;  // GPIO pin 25 for home switch
}

// Set demo values for Encoder configuration
void set_encoder_config_demo(JsonObject encoder_obj)
{
    encoder_obj["gpio_pin_a"] = 10;  // Pin A on GPIO 10
    encoder_obj["gpio_pin_b"] = 11;  // Pin B on GPIO 11
    encoder_obj["inverted"] = false;  // Normal direction
}

// Set a complete Config with GPIO peripheral
void set_config_with_gpio_demo(JsonObject config_obj)
{
    JsonObject gpio = config_obj["gpio"].to<JsonObject>();
    set_gpio_config_demo(gpio);
}

// Set a complete Config with UART peripheral
void set_config_with_uart_demo(JsonObject config_obj)
{
    JsonObject uart = config_obj["uart"].to<JsonObject>();
    set_uart_config_demo(uart);
}

// Set a complete Config with I2C peripheral
void set_config_with_i2c_demo(JsonObject config_obj)
{
    JsonObject i2c = config_obj["i2c"].to<JsonObject>();
    set_i2c_config_demo(i2c);
}

// Set a complete Config with SPI peripheral
void set_config_with_spi_demo(JsonObject config_obj)
{
    JsonObject spi = config_obj["spi"].to<JsonObject>();
    set_spi_config_demo(spi);
}

// Set a complete Config with Motor device
void set_config_with_motor_demo(JsonObject config_obj)
{
    JsonObject motor = config_obj["motor"].to<JsonObject>();
    set_motor_config_demo(motor);
}

// Set a complete Config with Encoder device
void set_config_with_encoder_demo(JsonObject config_obj)
{
    JsonObject encoder = config_obj["encoder"].to<JsonObject>();
    set_encoder_config_demo(encoder);
}

// Comparison functions for demo values
bool compare_gpio_config_demo(JsonObjectConst gpio_obj)
{
    return gpio_obj["pin"] == 13 && 
           gpio_obj["mode"] == static_cast<int>(OUTPUT_PP);
}

bool compare_uart_config_demo(JsonObjectConst uart_obj)
{
    return uart_obj["id"] == 1 && 
           uart_obj["baud"] == 115200 &&
           uart_obj["data_bits"] == 8 &&
           uart_obj["stop_bits"] == 1 &&
           uart_obj["parity_enable"] == false &&
           uart_obj["parity_even"] == false &&
           uart_obj["flow_control"] == false;
}

bool compare_i2c_config_demo(JsonObjectConst i2c_obj)
{
    return i2c_obj["id"] == 1 && 
           i2c_obj["clock_speed"] == 100000 &&
           i2c_obj["pullup_enable"] == true &&
           i2c_obj["timeout"] == 100 &&
           i2c_obj["general_call_enable"] == false;
}

bool compare_spi_config_demo(JsonObjectConst spi_obj)
{
    return spi_obj["id"] == 1 && 
           spi_obj["baud_rate_prescaler"] == 8 &&
           spi_obj["clock_polarity"] == false &&
           spi_obj["clock_phase"] == false &&
           spi_obj["msb_first"] == true &&
           spi_obj["software_slave_management"] == false &&
           spi_obj["internal_slave_select"] == true;
}

bool compare_motor_config_demo(JsonObjectConst motor_obj)
{
    return motor_obj["spi_id"] == 1 && 
           motor_obj["enabled"] == true &&
           motor_obj["enable_micro_stepping"] == true &&
           motor_obj["home_direction_inverted"] == false &&
           motor_obj["home_switch_pin"] == 25;
}

bool compare_encoder_config_demo(JsonObjectConst encoder_obj)
{
    return encoder_obj["gpio_pin_a"] == 10 &&
           encoder_obj["gpio_pin_b"] == 11 &&
           encoder_obj["inverted"] == false;
}

} // End of namespace HelperCompleteDemoConfigArduinoJson

#endif // HELPER_COMPLETE_DEMO_CONFIG_ARDUINOJSON
