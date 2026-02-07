#include <repeated_demo.h>
#include <EmbeddedProto/helper_repeated_demo.h>
#include <WriteBufferFixedSize.h>
#include <ReadBufferFixedSize.h>

uint32_t run_scenario()
{
  // Create message with nested repeated messages (10 outer, 5 inner each)
  Nested_Repeated_Messages<10, 5> msg;
  HelperRepeatedDemo::set_average_nested_repeated_messages<10, 5>(msg);

  // Serialize the data to buffer
  EmbeddedProto::WriteBufferFixedSize<10000> write_buffer;
  const auto serialize_result = msg.serialize(write_buffer);

  // Copy buffer to deserialization buffer
  EmbeddedProto::ReadBufferFixedSize<10000> read_buffer;
  const uint32_t buffer_size = write_buffer.get_size();
  memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
  read_buffer.set_bytes_written(buffer_size);

  // Clear the message data
  Nested_Repeated_Messages<10, 5> msg_deserialized;

  // Deserialize the buffer into the message object
  const auto deserialize_result = msg_deserialized.deserialize(read_buffer);

  // Compare the values with helper functions
  bool success = HelperRepeatedDemo::compare_average_nested_repeated_messages<10, 5>(msg_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}