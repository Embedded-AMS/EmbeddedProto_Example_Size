/*
 *  Copyright (C) 2020-2026 Embedded AMS B.V. - All Rights Reserved
 *
 *  This file is part of Embedded Proto.
 *
 *  Embedded Proto is open source software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, version 3 of the license.
 *
 *  Embedded Proto is distributed in the hope that it will be useful,
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

#include <complete_demo.h>
#include <EmbeddedProto/helper_complete_demo.h>
#include <EmbeddedProto/WriteBufferFixedSize.h>
#include <EmbeddedProto/ReadBufferFixedSize.h>

#include <EmbeddedProto/helper_serialize_mode.h>
#include <scenario.h>
uint32_t run_scenario()
{
    // Use sufficiently large buffers to handle the serialized data
    // Based on the complex nested structure, we need larger buffers
    EmbeddedProto::WriteBufferFixedSize<2048> write_buffer;
    EmbeddedProto::ReadBufferFixedSize<2048> read_buffer;

    // Step 1: Set configuration data in EnvelopToMCU (5 configurations)
    EnvelopToMCU<5> envelop_to_mcu;
    report_message_size(sizeof(envelop_to_mcu));
    HelperCompleteDemo::set_envelop_to_mcu_configurations_demo(envelop_to_mcu);

    // Step 2: Serialize EnvelopToMCU to a buffer
    const auto serialize_config_result = HelperSerializeMode::serialize_message(envelop_to_mcu, write_buffer);
    if (serialize_config_result != EmbeddedProto::Error::NO_ERRORS)
    {
        return 0; // Serialization failed
    }

    // Step 3: Clear the EnvelopToMCU message
    envelop_to_mcu.clear();

    // Step 4: Deserialize the buffer to EnvelopToMCU
    const uint32_t buffer_size = write_buffer.get_size();
    read_buffer.clear();
    memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
    read_buffer.set_bytes_written(buffer_size);

    EnvelopToMCU<5> envelop_to_mcu_deserialized;

    report_message_size(sizeof(envelop_to_mcu_deserialized));
    const auto deserialize_config_result = envelop_to_mcu_deserialized.deserialize(read_buffer);
    if (deserialize_config_result != EmbeddedProto::Error::NO_ERRORS)
    {
        return 0; // Deserialization failed
    }

    // Step 5: Set command data in EnvelopToMCU
    HelperCompleteDemo::set_envelop_to_mcu_command_demo(envelop_to_mcu_deserialized);

    // Step 6: Serialize EnvelopToMCU to the buffer
    write_buffer.clear();
    const auto serialize_command_result = HelperSerializeMode::serialize_message(envelop_to_mcu_deserialized, write_buffer);
    if (serialize_command_result != EmbeddedProto::Error::NO_ERRORS)
    {
        return 0; // Serialization failed
    }

    // Step 7: Clear EnvelopToMCU message
    envelop_to_mcu_deserialized.clear();

    // Step 8: Deserialize the buffer into EnvelopToMCU
    const uint32_t command_buffer_size = write_buffer.get_size();
    read_buffer.clear();
    memcpy(read_buffer.get_data(), write_buffer.get_data(), command_buffer_size);
    read_buffer.set_bytes_written(command_buffer_size);

    EnvelopToMCU<5> envelop_to_mcu_final;

    report_message_size(sizeof(envelop_to_mcu_final));
    const auto deserialize_command_result = envelop_to_mcu_final.deserialize(read_buffer);
    if (deserialize_command_result != EmbeddedProto::Error::NO_ERRORS)
    {
        return 0; // Deserialization failed
    }

    // Step 9: Loop over the different state types and set state data in EnvelopFromMCU
    // Use 5 states as requested
    EnvelopFromMCU<5, 25, 25> envelop_from_mcu_states;

    report_message_size(sizeof(envelop_from_mcu_states));
    // Set up different state types in the States message
    States<5>& states_msg = envelop_from_mcu_states.mutable_states();

    // Set GPIO state at index 0
    HelperCompleteDemoState::set_state_with_gpio_demo(states_msg.mutable_states(0));

    // Set Motor state at index 1
    HelperCompleteDemoState::set_state_with_motor_demo(states_msg.mutable_states(1));

    // Set Encoder state at index 2
    HelperCompleteDemoState::set_state_with_encoder_demo(states_msg.mutable_states(2));

    // Set another GPIO state at index 3 (different values)
    State::GPIO gpio_state_2;
    gpio_state_2.set_value(false);
    gpio_state_2.set_last_change_timestamp(987654321);
    states_msg.mutable_states(3).set_gpio(gpio_state_2);
    states_msg.mutable_states(3).set_timestamp(4000000);

    // Set another Motor state at index 4 (different values)
    State::Motor motor_state_2;
    motor_state_2.set_position(2500);
    motor_state_2.set_target_position(3000);
    motor_state_2.set_speed(750);
    motor_state_2.set_is_moving(false);
    motor_state_2.set_is_homed(true);
    motor_state_2.set_fault_detected(true);
    motor_state_2.set_current(2000);
    states_msg.mutable_states(4).set_motor(motor_state_2);
    states_msg.mutable_states(4).set_timestamp(5000000);

    // Serialize the states to a buffer
    write_buffer.clear();
    const auto serialize_states_result = HelperSerializeMode::serialize_message(envelop_from_mcu_states, write_buffer);
    if (serialize_states_result != EmbeddedProto::Error::NO_ERRORS)
    {
        return 0; // Serialization failed
    }

    // Step 10: Set the measurement data in EnvelopFromMCU (25 measurements and timestamps)
    EnvelopFromMCU<5, 25, 25> envelop_from_mcu_measurement;

    report_message_size(sizeof(envelop_from_mcu_measurement));
    // Set measurement data using helper function
    Measurement<25, 25>& measurement_msg = envelop_from_mcu_measurement.mutable_measurement();

    // Clear any existing data first
    measurement_msg.clear_data();
    measurement_msg.clear_time();

    // Set 25 measurement data points and timestamps
    for(uint32_t i = 0; i < 25; ++i)
    {
        measurement_msg.set_data(i, 1.23f * i + 0.5f); // Different pattern than helper
        measurement_msg.set_time(i, 50 * i + 100);     // Different pattern than helper
    }

    // Step 11: Serialize the measurement data
    write_buffer.clear();
    const auto serialize_measurement_result = HelperSerializeMode::serialize_message(envelop_from_mcu_measurement, write_buffer);
    if (serialize_measurement_result != EmbeddedProto::Error::NO_ERRORS)
    {
        return 0; // Serialization failed
    }

    // Return the total buffer size used for all serializations as success indicator
    // Use the largest buffer size among all serializations
    uint32_t total_buffer_size = write_buffer.get_size();

    // Additional verification: test deserialization of states and measurement
    EnvelopFromMCU<5, 25, 25> test_envelop;

    report_message_size(sizeof(test_envelop));
    // Test states deserialization
    const uint32_t states_size = write_buffer.get_size();
    read_buffer.clear();
    memcpy(read_buffer.get_data(), write_buffer.get_data(), states_size);
    read_buffer.set_bytes_written(states_size);

    EnvelopFromMCU<5, 25, 25> test_states_envelop;

    report_message_size(sizeof(test_states_envelop));
    const auto test_states_result = test_states_envelop.deserialize(read_buffer);

    // Test measurement deserialization
    const auto test_measurement_result = test_envelop.deserialize(read_buffer);

    // Return success if all operations completed successfully
    if (serialize_config_result == EmbeddedProto::Error::NO_ERRORS &&
        deserialize_config_result == EmbeddedProto::Error::NO_ERRORS &&
        serialize_command_result == EmbeddedProto::Error::NO_ERRORS &&
        deserialize_command_result == EmbeddedProto::Error::NO_ERRORS &&
        serialize_states_result == EmbeddedProto::Error::NO_ERRORS &&
        serialize_measurement_result == EmbeddedProto::Error::NO_ERRORS) {
        return total_buffer_size;
    }

    return 0; // Return 0 if any operation failed
}