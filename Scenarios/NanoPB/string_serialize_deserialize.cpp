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
