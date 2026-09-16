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

#include "complete_demo.pb.h"
#include <NanoPB/helper_complete_demo.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <scenario.h>

uint32_t run_scenario()
{
    // Use sufficiently large buffers to handle the serialized data
    // Based on the complex nested structure, we need larger buffers
    uint8_t write_buffer[2048];
    uint8_t read_buffer[2048];

    // Step 1: Set configuration data in EnvelopToMCU (5 configurations)
    EnvelopToMCU envelop_to_mcu = EnvelopToMCU_init_default;
    report_message_size(sizeof(envelop_to_mcu));
    HelperCompleteDemoNanoPB::set_envelop_to_mcu_configurations_demo(envelop_to_mcu);

    // Step 2: Serialize EnvelopToMCU to a buffer
    pb_ostream_t stream = pb_ostream_from_buffer(write_buffer, sizeof(write_buffer));
    bool serialize_config_result = pb_encode(&stream, EnvelopToMCU_fields, &envelop_to_mcu);
    if (!serialize_config_result)
    {
        return 0; // Serialization failed
    }

    // Step 3: Clear the EnvelopToMCU message
    envelop_to_mcu = EnvelopToMCU_init_default;

    // Step 4: Deserialize the buffer to EnvelopToMCU
    const uint32_t buffer_size = stream.bytes_written;
    memcpy(read_buffer, write_buffer, buffer_size);

    EnvelopToMCU envelop_to_mcu_deserialized = EnvelopToMCU_init_default;

    report_message_size(sizeof(envelop_to_mcu_deserialized));
    pb_istream_t istream = pb_istream_from_buffer(read_buffer, buffer_size);
    bool deserialize_config_result = pb_decode(&istream, EnvelopToMCU_fields, &envelop_to_mcu_deserialized);
    if (!deserialize_config_result)
    {
        return 0; // Deserialization failed
    }

    // Step 5: Set command data in EnvelopToMCU
    HelperCompleteDemoNanoPB::set_envelop_to_mcu_command_demo(envelop_to_mcu_deserialized);

    // Step 6: Serialize EnvelopToMCU to the buffer
    stream = pb_ostream_from_buffer(write_buffer, sizeof(write_buffer));
    bool serialize_command_result = pb_encode(&stream, EnvelopToMCU_fields, &envelop_to_mcu_deserialized);
    if (!serialize_command_result)
    {
        return 0; // Serialization failed
    }

    // Step 7: Clear EnvelopToMCU message
    envelop_to_mcu_deserialized = EnvelopToMCU_init_default;

    // Step 8: Deserialize the buffer into EnvelopToMCU
    const uint32_t command_buffer_size = stream.bytes_written;
    memcpy(read_buffer, write_buffer, command_buffer_size);

    EnvelopToMCU envelop_to_mcu_final = EnvelopToMCU_init_default;

    report_message_size(sizeof(envelop_to_mcu_final));
    istream = pb_istream_from_buffer(read_buffer, command_buffer_size);
    bool deserialize_command_result = pb_decode(&istream, EnvelopToMCU_fields, &envelop_to_mcu_final);
    if (!deserialize_command_result)
    {
        return 0; // Deserialization failed
    }

    // Step 9: Loop over the different state types and set state data in EnvelopFromMCU
    // Use 5 states as requested - matching EmbeddedProto scenario exactly
    EnvelopFromMCU envelop_from_mcu_states = EnvelopFromMCU_init_default;
    report_message_size(sizeof(envelop_from_mcu_states));
    envelop_from_mcu_states.which_data = EnvelopFromMCU_states_tag;
    
    // Set up different state types in the States message
    States& states_msg = envelop_from_mcu_states.data.states;
    states_msg.states_count = 5;

    // Set GPIO state at index 0
    HelperCompleteDemoStateNanoPB::set_state_with_gpio_demo(states_msg.states[0]);

    // Set Motor state at index 1
    HelperCompleteDemoStateNanoPB::set_state_with_motor_demo(states_msg.states[1]);

    // Set Encoder state at index 2
    HelperCompleteDemoStateNanoPB::set_state_with_encoder_demo(states_msg.states[2]);

    // Set another GPIO state at index 3 (different values - matching EmbeddedProto)
    State_GPIO gpio_state_2 = State_GPIO_init_default;
    gpio_state_2.value = false;
    gpio_state_2.last_change_timestamp = 987654321;
    states_msg.states[3].state.gpio = gpio_state_2;
    states_msg.states[3].which_state = State_gpio_tag;
    states_msg.states[3].timestamp = 4000000;

    // Set another Motor state at index 4 (different values - matching EmbeddedProto)
    State_Motor motor_state_2 = State_Motor_init_default;
    motor_state_2.position = 2500;
    motor_state_2.target_position = 3000;
    motor_state_2.speed = 750;
    motor_state_2.is_moving = false;
    motor_state_2.is_homed = true;
    motor_state_2.fault_detected = true;
    motor_state_2.current = 2000;
    states_msg.states[4].state.motor = motor_state_2;
    states_msg.states[4].which_state = State_motor_tag;
    states_msg.states[4].timestamp = 5000000;

    // Serialize the states to a buffer
    stream = pb_ostream_from_buffer(write_buffer, sizeof(write_buffer));
    bool serialize_states_result = pb_encode(&stream, EnvelopFromMCU_fields, &envelop_from_mcu_states);
    if (!serialize_states_result)
    {
        return 0; // Serialization failed
    }

    // Step 10: Set the measurement data in EnvelopFromMCU (25 measurements and timestamps)
    // Matching EmbeddedProto scenario exactly
    EnvelopFromMCU envelop_from_mcu_measurement = EnvelopFromMCU_init_default;
    report_message_size(sizeof(envelop_from_mcu_measurement));
    envelop_from_mcu_measurement.which_data = EnvelopFromMCU_measurement_tag;
    
    // Set measurement data using same pattern as EmbeddedProto
    Measurement& measurement_msg = envelop_from_mcu_measurement.data.measurement;
    measurement_msg.data_count = 25;
    measurement_msg.time_count = 25;

    // Set 25 measurement data points and timestamps - matching EmbeddedProto pattern
    for(uint32_t i = 0; i < 25; ++i)
    {
        measurement_msg.data[i] = 1.23f * i + 0.5f; // Same pattern as EmbeddedProto
        measurement_msg.time[i] = 50 * i + 100;     // Same pattern as EmbeddedProto
    }

    // Step 11: Serialize the measurement data
    stream = pb_ostream_from_buffer(write_buffer, sizeof(write_buffer));
    bool serialize_measurement_result = pb_encode(&stream, EnvelopFromMCU_fields, &envelop_from_mcu_measurement);
    if (!serialize_measurement_result)
    {
        return 0; // Serialization failed
    }

    // Return the total buffer size used for all serializations as success indicator
    // Use the largest buffer size among all serializations
    uint32_t total_buffer_size = stream.bytes_written;

    // Additional verification: test deserialization of states and measurement
    EnvelopFromMCU test_envelop = EnvelopFromMCU_init_default;

    report_message_size(sizeof(test_envelop));
    // Test states deserialization
    const uint32_t states_size = stream.bytes_written;
    memcpy(read_buffer, write_buffer, states_size);

    EnvelopFromMCU test_states_envelop = EnvelopFromMCU_init_default;

    report_message_size(sizeof(test_states_envelop));
    istream = pb_istream_from_buffer(read_buffer, states_size);
    bool test_states_result = pb_decode(&istream, EnvelopFromMCU_fields, &test_states_envelop);

    // Test measurement deserialization
    istream = pb_istream_from_buffer(read_buffer, states_size);
    bool test_measurement_result = pb_decode(&istream, EnvelopFromMCU_fields, &test_envelop);

    // Return success if all operations completed successfully
    if (serialize_config_result &&
        deserialize_config_result &&
        serialize_command_result &&
        deserialize_command_result &&
        serialize_states_result &&
        serialize_measurement_result) {
        return total_buffer_size;
    }

    return 0; // Return 0 if any operation failed
}
