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
#include <scenario.h>
#include <ArduinoJson/counting_allocator.h>

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
    JsonDocument doc_envelop_to_mcu(&g_json_allocator);
    HelperCompleteDemoArduinoJson::set_envelop_to_mcu_configurations_demo(doc_envelop_to_mcu, 5);

    // Step 2: Serialize EnvelopToMCU to a buffer
    report_message_size(sizeof(doc_envelop_to_mcu) + g_json_allocator.peak_bytes());
    const auto n_bytes_config = serializeJson(doc_envelop_to_mcu, buffer, sizeof(buffer));
    if (n_bytes_config == 0)
    {
        return 0; // Serialization failed
    }

    JsonDocument doc_envelop_to_mcu_deserialized(&g_json_allocator);

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
    report_message_size(sizeof(doc_envelop_to_mcu_deserialized) + g_json_allocator.peak_bytes());
    const auto n_bytes_command = serializeJson(doc_envelop_to_mcu_deserialized, buffer, sizeof(buffer));
    if (n_bytes_command == 0)
    {
        return 0; // Serialization failed
    }

    JsonDocument doc_envelop_to_mcu_final(&g_json_allocator);

    // Step 8: Deserialize the buffer into EnvelopToMCU
    memcpy(deserialize_buffer, buffer, n_bytes_command);
    const auto deserialize_command_error = deserializeJson(doc_envelop_to_mcu_final, deserialize_buffer, n_bytes_command);
    if (deserialize_command_error)
    {
        return 0; // Deserialization failed
    }

    // Step 9: Fill EnvelopFromMCU with the states and serialize it
    JsonDocument doc_envelop_from_mcu_states(&g_json_allocator);
    HelperCompleteDemoArduinoJson::set_envelop_from_mcu_with_states_demo(doc_envelop_from_mcu_states, 5);

    report_message_size(sizeof(doc_envelop_from_mcu_states) + g_json_allocator.peak_bytes());
    const auto n_bytes_states = serializeJson(doc_envelop_from_mcu_states, buffer, sizeof(buffer));
    if (n_bytes_states == 0)
    {
        return 0; // Serialization failed
    }

    // Step 10: Deserialize the states and check every value
    memcpy(deserialize_buffer, buffer, n_bytes_states);
    JsonDocument doc_envelop_from_mcu_states_deserialized(&g_json_allocator);
    const auto deserialize_states_error = deserializeJson(doc_envelop_from_mcu_states_deserialized, deserialize_buffer, n_bytes_states);
    if (deserialize_states_error
        || !HelperCompleteDemoArduinoJson::compare_envelop_from_mcu_states_demo(doc_envelop_from_mcu_states_deserialized, 5))
    {
        return 0; // Deserialization failed
    }

    // Step 11: Fill EnvelopFromMCU with the measurement (25 data points and timestamps) and serialize it
    JsonDocument doc_envelop_from_mcu_measurement(&g_json_allocator);
    HelperCompleteDemoArduinoJson::set_envelop_from_mcu_with_measurement_demo(doc_envelop_from_mcu_measurement, 25, 25);

    report_message_size(sizeof(doc_envelop_from_mcu_measurement) + g_json_allocator.peak_bytes());
    const auto n_bytes_measurement = serializeJson(doc_envelop_from_mcu_measurement, buffer, sizeof(buffer));
    if (n_bytes_measurement == 0)
    {
        return 0; // Serialization failed
    }

    // Step 12: Deserialize the measurement and check every value
    memcpy(deserialize_buffer, buffer, n_bytes_measurement);
    JsonDocument doc_envelop_from_mcu_measurement_deserialized(&g_json_allocator);
    const auto deserialize_measurement_error = deserializeJson(doc_envelop_from_mcu_measurement_deserialized, deserialize_buffer, n_bytes_measurement);
    if (deserialize_measurement_error
        || !HelperCompleteDemoArduinoJson::compare_envelop_from_mcu_measurement_demo(doc_envelop_from_mcu_measurement_deserialized, 25, 25))
    {
        return 0; // Deserialization failed
    }

    // Every round trip succeeded and was checked, report the size of the largest message
    return static_cast<uint32_t>(n_bytes_measurement);
}
