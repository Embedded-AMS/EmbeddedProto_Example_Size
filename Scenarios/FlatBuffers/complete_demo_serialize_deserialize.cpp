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

// The flatcc emitter builds the message in pages on the heap. One page the size of the
// buffer below, rounded up to the multiple of 128 flatcc wants, sized to this scenario the
// way the other libraries size their buffers. The CMake passes it on to the runtime.
#define FLATCC_EMITTER_PAGE_SIZE 2048

#include <FlatBuffers/helper_flatbuffers.h>
#include <FlatBuffers/helper_complete_demo.h>
#include <complete_demo_verifier.h>

// The same steps as the other libraries. A FlatBuffer can not be changed once built, so
// step 5, adding the command to the received configuration, builds the envelope again.
// One builder serves every buffer, reset between them keeps its memory, as flatcc advises.
uint32_t run_scenario()
{
    alignas(8) uint8_t buffer[2048];
    alignas(8) uint8_t deserialize_buffer[2048];
    flatcc_builder_t builder;

    flatcc_builder_init(&builder);

    // Step 1: Set configuration data in EnvelopToMCU (5 configurations)
    EnvelopToMCU_start_as_root(&builder);
    HelperCompleteDemoFlatBuffers::add_configurations(&builder);
    EnvelopToMCU_end_as_root(&builder);

    // Step 2: Serialize EnvelopToMCU to a buffer
    const uint32_t config_size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer));
    if (config_size == 0)
    {
        return 0; // Serialization failed
    }

    // Step 4: Deserialize the buffer to EnvelopToMCU
    memcpy(deserialize_buffer, buffer, config_size);
    if (EnvelopToMCU_verify_as_root(deserialize_buffer, config_size) != 0)
    {
        return 0; // Deserialization failed
    }
    EnvelopToMCU_table_t envelop_to_mcu_deserialized = EnvelopToMCU_as_root(deserialize_buffer);
    if (EnvelopToMCU_configuration(envelop_to_mcu_deserialized) == nullptr)
    {
        return 0;
    }

    // Step 5 and 6: Add the command to the configuration and serialize it again
    flatcc_builder_reset(&builder);
    EnvelopToMCU_start_as_root(&builder);
    HelperCompleteDemoFlatBuffers::add_configurations(&builder);
    HelperCompleteDemoFlatBuffers::add_motor_command(&builder);
    EnvelopToMCU_end_as_root(&builder);
    const uint32_t command_size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer));
    if (command_size == 0)
    {
        return 0; // Serialization failed
    }

    // Step 8: Deserialize the buffer into EnvelopToMCU
    memcpy(deserialize_buffer, buffer, command_size);
    if (EnvelopToMCU_verify_as_root(deserialize_buffer, command_size) != 0)
    {
        return 0; // Deserialization failed
    }
    EnvelopToMCU_table_t envelop_to_mcu_final = EnvelopToMCU_as_root(deserialize_buffer);
    if (EnvelopToMCU_command(envelop_to_mcu_final) == nullptr)
    {
        return 0;
    }

    // Step 9: Fill EnvelopFromMCU with the states and serialize it
    flatcc_builder_reset(&builder);
    EnvelopFromMCU_start_as_root(&builder);
    HelperCompleteDemoFlatBuffers::add_states(&builder);
    EnvelopFromMCU_end_as_root(&builder);
    const uint32_t states_size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer));
    if (states_size == 0)
    {
        return 0; // Serialization failed
    }

    // Step 10: Deserialize the states and check every value
    memcpy(deserialize_buffer, buffer, states_size);
    if (EnvelopFromMCU_verify_as_root(deserialize_buffer, states_size) != 0
        || !HelperCompleteDemoFlatBuffers::compare_states(EnvelopFromMCU_as_root(deserialize_buffer)))
    {
        return 0; // Deserialization failed
    }

    // Step 11: Fill EnvelopFromMCU with the measurement (25 data points and timestamps) and serialize it
    flatcc_builder_reset(&builder);
    EnvelopFromMCU_start_as_root(&builder);
    HelperCompleteDemoFlatBuffers::add_measurement(&builder);
    EnvelopFromMCU_end_as_root(&builder);
    const uint32_t measurement_size = HelperFlatBuffers::finish(builder, buffer, sizeof(buffer));
    flatcc_builder_clear(&builder);
    if (measurement_size == 0)
    {
        return 0; // Serialization failed
    }

    // Step 12: Deserialize the measurement and check every value
    memcpy(deserialize_buffer, buffer, measurement_size);
    if (EnvelopFromMCU_verify_as_root(deserialize_buffer, measurement_size) != 0
        || !HelperCompleteDemoFlatBuffers::compare_measurement(EnvelopFromMCU_as_root(deserialize_buffer)))
    {
        return 0; // Deserialization failed
    }

    // Every round trip succeeded and was checked, report the size of the largest message
    return measurement_size;
}
