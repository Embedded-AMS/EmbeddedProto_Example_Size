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

#include "string_demo.pb.h"
#include "NanoPB/helper_string.h"
#include <pb_encode.h>
#include <pb_decode.h>

uint32_t run_scenario()
{
  String_Demo_Message msg = String_Demo_Message_init_default;
  HelperString::set_string(msg);

  // Serialize the data to buffer
  uint8_t buffer[200];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  bool serialize_status = pb_encode(&stream, String_Demo_Message_fields, &msg);

  // Deserialize back into a new message
  String_Demo_Message msg_deserialized = String_Demo_Message_init_default;
  pb_istream_t istream = pb_istream_from_buffer(buffer, stream.bytes_written);
  bool deserialize_status = pb_decode(&istream, String_Demo_Message_fields, &msg_deserialized);

  // Compare the values with helper functions
  bool success = HelperString::compare_string(msg_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? stream.bytes_written : 0;
}
