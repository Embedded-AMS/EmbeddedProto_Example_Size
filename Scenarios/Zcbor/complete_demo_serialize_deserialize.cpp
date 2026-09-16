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

#include <cbor_messages_encode.h>
#include <cbor_messages_decode.h>
#include <Zcbor/helper_complete_demo.h>
#include <scenario.h>

#include <cstring>

uint32_t run_scenario()
{
    uint8_t write_buffer[2048];
    uint8_t read_buffer[2048];
    size_t written = 0;
    size_t decoded = 0;

    // Step 1: Set configuration data in EnvelopToMCU (5 configurations)
    struct EnvelopToMCU envelop_to_mcu = {};
    report_message_size(sizeof(envelop_to_mcu));
    HelperCompleteDemoZcbor::set_envelop_to_mcu_configurations(envelop_to_mcu);

    // Step 2: Serialize EnvelopToMCU to a buffer
    if (cbor_encode_EnvelopToMCU(write_buffer, sizeof(write_buffer), &envelop_to_mcu, &written) != ZCBOR_SUCCESS)
    {
        return 0; // Serialization failed
    }

    // Step 4: Deserialize the buffer to EnvelopToMCU
    memcpy(read_buffer, write_buffer, written);
    struct EnvelopToMCU envelop_to_mcu_deserialized = {};
    if (cbor_decode_EnvelopToMCU(read_buffer, written, &envelop_to_mcu_deserialized, &decoded) != ZCBOR_SUCCESS)
    {
        return 0; // Deserialization failed
    }

    // Step 5: Set command data in EnvelopToMCU
    HelperCompleteDemoZcbor::set_envelop_to_mcu_command(envelop_to_mcu_deserialized);

    // Step 6: Serialize EnvelopToMCU to the buffer
    if (cbor_encode_EnvelopToMCU(write_buffer, sizeof(write_buffer), &envelop_to_mcu_deserialized, &written) != ZCBOR_SUCCESS)
    {
        return 0; // Serialization failed
    }

    // Step 8: Deserialize the buffer into EnvelopToMCU
    memcpy(read_buffer, write_buffer, written);
    struct EnvelopToMCU envelop_to_mcu_final = {};
    if (cbor_decode_EnvelopToMCU(read_buffer, written, &envelop_to_mcu_final, &decoded) != ZCBOR_SUCCESS)
    {
        return 0; // Deserialization failed
    }

    // Step 9: Fill EnvelopFromMCU with the states and serialize it
    struct EnvelopFromMCU envelop_from_mcu_states = {};
    report_message_size(sizeof(envelop_from_mcu_states));
    HelperCompleteDemoZcbor::set_envelop_from_mcu_with_states(envelop_from_mcu_states);
    if (cbor_encode_EnvelopFromMCU(write_buffer, sizeof(write_buffer), &envelop_from_mcu_states, &written) != ZCBOR_SUCCESS)
    {
        return 0; // Serialization failed
    }

    // Step 10: Deserialize the states and check every value
    memcpy(read_buffer, write_buffer, written);
    struct EnvelopFromMCU envelop_from_mcu_states_deserialized = {};
    if (cbor_decode_EnvelopFromMCU(read_buffer, written, &envelop_from_mcu_states_deserialized, &decoded) != ZCBOR_SUCCESS
        || !HelperCompleteDemoZcbor::compare_envelop_from_mcu_states(envelop_from_mcu_states_deserialized))
    {
        return 0; // Deserialization failed
    }

    // Step 11: Fill EnvelopFromMCU with the measurement (25 data points and timestamps) and serialize it
    struct EnvelopFromMCU envelop_from_mcu_measurement = {};
    HelperCompleteDemoZcbor::set_envelop_from_mcu_with_measurement(envelop_from_mcu_measurement);
    if (cbor_encode_EnvelopFromMCU(write_buffer, sizeof(write_buffer), &envelop_from_mcu_measurement, &written) != ZCBOR_SUCCESS)
    {
        return 0; // Serialization failed
    }
    const uint32_t measurement_size = static_cast<uint32_t>(written);

    // Step 12: Deserialize the measurement and check every value
    memcpy(read_buffer, write_buffer, written);
    struct EnvelopFromMCU envelop_from_mcu_measurement_deserialized = {};
    if (cbor_decode_EnvelopFromMCU(read_buffer, written, &envelop_from_mcu_measurement_deserialized, &decoded) != ZCBOR_SUCCESS
        || !HelperCompleteDemoZcbor::compare_envelop_from_mcu_measurement(envelop_from_mcu_measurement_deserialized))
    {
        return 0; // Deserialization failed
    }

    // Every round trip succeeded and was checked, report the size of the largest message
    return measurement_size;
}
