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

#include <oneof.pb.h>
#include <NanoPB/helper_oneof.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <scenario.h>

uint32_t run_scenario()
{
  OneOf_Demo msg = OneOf_Demo_init_default;

  report_message_size(sizeof(msg));
  // Set different oneof fields and serialize
  HelperOneOf::set_int32_field(msg);

  // Serialize the data to buffer
  uint8_t buffer[200];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  bool serialize_success = pb_encode(&stream, OneOf_Demo_fields, &msg);

  // Deserialize the buffer into a new message object
  OneOf_Demo msg_deserialized = OneOf_Demo_init_default;
  report_message_size(sizeof(msg_deserialized));
  pb_istream_t istream = pb_istream_from_buffer(buffer, stream.bytes_written);
  bool deserialize_success = pb_decode(&istream, OneOf_Demo_fields, &msg_deserialized);

  // Verify the deserialized data matches the original
  bool success = HelperOneOf::compare_int32_field(msg_deserialized);

  // Clear the oneof fields
  HelperOneOf::clear_oneof_fields(msg_deserialized);

  // Set a different field (float) and verify
  HelperOneOf::set_float_field(msg_deserialized);
  success &= HelperOneOf::compare_float_field(msg_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? stream.bytes_written : 0;
}