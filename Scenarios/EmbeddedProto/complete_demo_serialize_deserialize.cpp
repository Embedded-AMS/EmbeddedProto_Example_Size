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

    // Step 9: Fill EnvelopFromMCU with the states and serialize it
    EnvelopFromMCU<5, 25, 25> envelop_from_mcu_states;
    report_message_size(sizeof(envelop_from_mcu_states));
    HelperCompleteDemo::set_envelop_from_mcu_with_states_demo(envelop_from_mcu_states);

    write_buffer.clear();
    const auto serialize_states_result = HelperSerializeMode::serialize_message(envelop_from_mcu_states, write_buffer);
    if (serialize_states_result != EmbeddedProto::Error::NO_ERRORS)
    {
        return 0; // Serialization failed
    }

    // Step 10: Deserialize the states and check every value
    const uint32_t states_size = write_buffer.get_size();
    read_buffer.clear();
    memcpy(read_buffer.get_data(), write_buffer.get_data(), states_size);
    read_buffer.set_bytes_written(states_size);

    EnvelopFromMCU<5, 25, 25> envelop_from_mcu_states_deserialized;
    const auto deserialize_states_result = envelop_from_mcu_states_deserialized.deserialize(read_buffer);
    if ((deserialize_states_result != EmbeddedProto::Error::NO_ERRORS)
        || !HelperCompleteDemo::compare_envelop_from_mcu_states_demo(envelop_from_mcu_states_deserialized))
    {
        return 0; // Deserialization failed
    }

    // Step 11: Fill EnvelopFromMCU with the measurement (25 data points and timestamps) and serialize it
    EnvelopFromMCU<5, 25, 25> envelop_from_mcu_measurement;
    HelperCompleteDemo::set_envelop_from_mcu_with_measurement_demo(envelop_from_mcu_measurement);

    write_buffer.clear();
    const auto serialize_measurement_result = HelperSerializeMode::serialize_message(envelop_from_mcu_measurement, write_buffer);
    if (serialize_measurement_result != EmbeddedProto::Error::NO_ERRORS)
    {
        return 0; // Serialization failed
    }

    // Step 12: Deserialize the measurement and check every value
    const uint32_t measurement_size = write_buffer.get_size();
    read_buffer.clear();
    memcpy(read_buffer.get_data(), write_buffer.get_data(), measurement_size);
    read_buffer.set_bytes_written(measurement_size);

    EnvelopFromMCU<5, 25, 25> envelop_from_mcu_measurement_deserialized;
    const auto deserialize_measurement_result = envelop_from_mcu_measurement_deserialized.deserialize(read_buffer);
    if ((deserialize_measurement_result != EmbeddedProto::Error::NO_ERRORS)
        || !HelperCompleteDemo::compare_envelop_from_mcu_measurement_demo(envelop_from_mcu_measurement_deserialized))
    {
        return 0; // Deserialization failed
    }

    // Every round trip succeeded and was checked, report the size of the largest message
    return measurement_size;
}
