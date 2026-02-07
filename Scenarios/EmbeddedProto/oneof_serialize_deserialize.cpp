#include <oneof.h>
#include <EmbeddedProto/helper_oneof.h>
#include <WriteBufferFixedSize.h>
#include <ReadBufferFixedSize.h>

uint32_t run_scenario()
{
  OneOf_Demo msg;

  // Set different oneof fields and serialize
  HelperOneOf::set_int32_field(msg);

  // Serialize the data to buffer
  EmbeddedProto::WriteBufferFixedSize<200> write_buffer;
  const auto serialize_result = msg.serialize(write_buffer);

  // Copy buffer to deserialization buffer
  EmbeddedProto::ReadBufferFixedSize<200> read_buffer;
  const uint32_t buffer_size = write_buffer.get_size();
  memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
  read_buffer.set_bytes_written(buffer_size);

  // Deserialize the buffer into a new message object
  OneOf_Demo msg_deserialized;
  const auto deserialize_result = msg_deserialized.deserialize(read_buffer);

  // Verify the deserialized data matches the original
  bool success = HelperOneOf::compare_int32_field(msg_deserialized);

  // Clear the oneof fields
  msg_deserialized.clear();

  // Set a different field (float) and verify
  HelperOneOf::set_float_field(msg_deserialized);
  success &= HelperOneOf::compare_float_field(msg_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}