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

#include <ArduinoJson.h>
#include <ArduinoJson/helper_complete_demo.h>

// Volatile variable to prevent compiler from optimizing away serialization
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
    // Use sufficiently large buffers to handle the serialized data
    // Based on the complex nested structure, we need larger buffers
    char buffer[2048];
    char deserialize_buffer[2048];

    // Step 1: Set configuration data in EnvelopToMCU (5 configurations)
    JsonDocument doc_envelop_to_mcu;
    HelperCompleteDemoArduinoJson::set_envelop_to_mcu_configurations_demo(doc_envelop_to_mcu, 5);

    // Step 2: Serialize EnvelopToMCU to a buffer
    const auto n_bytes_config = serializeJson(doc_envelop_to_mcu, buffer, sizeof(buffer));
    if (n_bytes_config == 0)
    {
        return 0; // Serialization failed
    }

    // Step 3: Clear the EnvelopToMCU message (create fresh document)
    JsonDocument doc_envelop_to_mcu_deserialized;

    // Step 4: Deserialize the buffer to EnvelopToMCU
    memcpy(deserialize_buffer, buffer, n_bytes_config);
    const auto deserialize_config_error = deserializeJson(doc_envelop_to_mcu_deserialized, deserialize_buffer, n_bytes_config);
    if (deserialize_config_error)
    {
        return 0; // Deserialization failed
    }

    // Step 5: Set command data in EnvelopToMCU
    HelperCompleteDemoArduinoJson::set_envelop_to_mcu_command_demo(doc_envelop_to_mcu_deserialized);

    // Step 6: Serialize EnvelopToMCU to the buffer
    const auto n_bytes_command = serializeJson(doc_envelop_to_mcu_deserialized, buffer, sizeof(buffer));
    if (n_bytes_command == 0)
    {
        return 0; // Serialization failed
    }

    // Step 7: Clear EnvelopToMCU message (create fresh document)
    JsonDocument doc_envelop_to_mcu_final;

    // Step 8: Deserialize the buffer into EnvelopToMCU
    memcpy(deserialize_buffer, buffer, n_bytes_command);
    const auto deserialize_command_error = deserializeJson(doc_envelop_to_mcu_final, deserialize_buffer, n_bytes_command);
    if (deserialize_command_error)
    {
        return 0; // Deserialization failed
    }

    // Step 9: Loop over the different state types and set state data in EnvelopFromMCU
    // Use 5 states as requested
    JsonDocument doc_envelop_from_mcu_states;

    // Set up different state types in the States message
    JsonObject states_obj = doc_envelop_from_mcu_states["states"].to<JsonObject>();
    JsonArray states_array = states_obj["states"].to<JsonArray>();

    // Set GPIO state at index 0
    JsonObject state0 = states_array.add<JsonObject>();
    HelperCompleteDemoStateArduinoJson::set_state_with_gpio_demo(state0);

    // Set Motor state at index 1
    JsonObject state1 = states_array.add<JsonObject>();
    HelperCompleteDemoStateArduinoJson::set_state_with_motor_demo(state1);

    // Set Encoder state at index 2
    JsonObject state2 = states_array.add<JsonObject>();
    HelperCompleteDemoStateArduinoJson::set_state_with_encoder_demo(state2);

    // Set another GPIO state at index 3 (different values)
    JsonObject state3 = states_array.add<JsonObject>();
    JsonObject gpio_state_2 = state3["gpio"].to<JsonObject>();
    gpio_state_2["value"] = false;
    gpio_state_2["last_change_timestamp"] = 987654321;
    state3["timestamp"] = 4000000;

    // Set another Motor state at index 4 (different values)
    JsonObject state4 = states_array.add<JsonObject>();
    JsonObject motor_state_2 = state4["motor"].to<JsonObject>();
    motor_state_2["position"] = 2500;
    motor_state_2["target_position"] = 3000;
    motor_state_2["speed"] = 750;
    motor_state_2["is_moving"] = false;
    motor_state_2["is_homed"] = true;
    motor_state_2["fault_detected"] = true;
    motor_state_2["current"] = 2000;
    state4["timestamp"] = 5000000;

    // Serialize the states to a buffer
    const auto n_bytes_states = serializeJson(doc_envelop_from_mcu_states, buffer, sizeof(buffer));
    if (n_bytes_states == 0)
    {
        return 0; // Serialization failed
    }

    // Step 10: Set the measurement data in EnvelopFromMCU (25 measurements and timestamps)
    JsonDocument doc_envelop_from_mcu_measurement;

    // Set measurement data
    JsonObject measurement_obj = doc_envelop_from_mcu_measurement["measurement"].to<JsonObject>();
    JsonArray data_array = measurement_obj["data"].to<JsonArray>();
    JsonArray time_array = measurement_obj["time"].to<JsonArray>();

    // Set 25 measurement data points and timestamps
    for(uint32_t i = 0; i < 25; ++i)
    {
        data_array.add(1.23f * i + 0.5f); // Different pattern than helper
        time_array.add(50 * i + 100);     // Different pattern than helper
    }

    // Step 11: Serialize the measurement data
    const auto n_bytes_measurement = serializeJson(doc_envelop_from_mcu_measurement, buffer, sizeof(buffer));
    if (n_bytes_measurement == 0)
    {
        return 0; // Serialization failed
    }

    // Return the total buffer size used for all serializations as success indicator
    // Use the largest buffer size among all serializations
    uint32_t total_buffer_size = n_bytes_measurement;

    // Additional verification: test deserialization of states and measurement
    JsonDocument test_envelop;

    // Test states deserialization
    memcpy(deserialize_buffer, buffer, n_bytes_measurement);
    const auto test_measurement_error = deserializeJson(test_envelop, deserialize_buffer, n_bytes_measurement);

    // Return success if all operations completed successfully
    if (n_bytes_config > 0 &&
        !deserialize_config_error &&
        n_bytes_command > 0 &&
        !deserialize_command_error &&
        n_bytes_states > 0 &&
        n_bytes_measurement > 0 &&
        !test_measurement_error)
    {
        return total_buffer_size;
    }

    return 0; // Return 0 if any operation failed
}
