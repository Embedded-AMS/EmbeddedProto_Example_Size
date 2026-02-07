#include <oneof.pb.h>
#include <NanoPB/helper_oneof.h>
#include <pb_encode.h>
#include <pb_decode.h>

uint32_t run_scenario()
{
  OneOf_Demo msg = OneOf_Demo_init_default;

  // Set different oneof fields and serialize
  HelperOneOf::set_int32_field(msg);

  // Serialize the data to buffer
  uint8_t buffer[200];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  bool serialize_success = pb_encode(&stream, OneOf_Demo_fields, &msg);

  // Deserialize the buffer into a new message object
  OneOf_Demo msg_deserialized = OneOf_Demo_init_default;
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