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

    // Step 9: Fill EnvelopFromMCU with the states and serialize it
    EnvelopFromMCU envelop_from_mcu_states = EnvelopFromMCU_init_default;
    report_message_size(sizeof(envelop_from_mcu_states));
    HelperCompleteDemoNanoPB::set_envelop_from_mcu_with_states_demo(envelop_from_mcu_states);

    stream = pb_ostream_from_buffer(write_buffer, sizeof(write_buffer));
    if (!pb_encode(&stream, EnvelopFromMCU_fields, &envelop_from_mcu_states))
    {
        return 0; // Serialization failed
    }

    // Step 10: Deserialize the states and check every value
    const uint32_t states_size = stream.bytes_written;
    memcpy(read_buffer, write_buffer, states_size);

    EnvelopFromMCU envelop_from_mcu_states_deserialized = EnvelopFromMCU_init_default;
    istream = pb_istream_from_buffer(read_buffer, states_size);
    if (!pb_decode(&istream, EnvelopFromMCU_fields, &envelop_from_mcu_states_deserialized)
        || !HelperCompleteDemoNanoPB::compare_envelop_from_mcu_states_demo(envelop_from_mcu_states_deserialized))
    {
        return 0; // Deserialization failed
    }

    // Step 11: Fill EnvelopFromMCU with the measurement (25 data points and timestamps) and serialize it
    EnvelopFromMCU envelop_from_mcu_measurement = EnvelopFromMCU_init_default;
    HelperCompleteDemoNanoPB::set_envelop_from_mcu_with_measurement_demo(envelop_from_mcu_measurement);

    stream = pb_ostream_from_buffer(write_buffer, sizeof(write_buffer));
    if (!pb_encode(&stream, EnvelopFromMCU_fields, &envelop_from_mcu_measurement))
    {
        return 0; // Serialization failed
    }

    // Step 12: Deserialize the measurement and check every value
    const uint32_t measurement_size = stream.bytes_written;
    memcpy(read_buffer, write_buffer, measurement_size);

    EnvelopFromMCU envelop_from_mcu_measurement_deserialized = EnvelopFromMCU_init_default;
    istream = pb_istream_from_buffer(read_buffer, measurement_size);
    if (!pb_decode(&istream, EnvelopFromMCU_fields, &envelop_from_mcu_measurement_deserialized)
        || !HelperCompleteDemoNanoPB::compare_envelop_from_mcu_measurement_demo(envelop_from_mcu_measurement_deserialized))
    {
        return 0; // Deserialization failed
    }

    // Every round trip succeeded and was checked, report the size of the largest message
    return measurement_size;
}
