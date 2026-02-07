#include <optional_demo.h>
#include <EmbeddedProto/helper_optional_demo.h>
#include <WriteBufferFixedSize.h>
#include <ReadBufferFixedSize.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  Optional_Demo msg;
  HelperOptionalDemo::set_zero_with_presence(msg);

  // Serialize the data to buffer
  EmbeddedProto::WriteBufferFixedSize<100> write_buffer;
  const auto serialize_result = msg.serialize(write_buffer);

  // Copy buffer to deserialization buffer
  EmbeddedProto::ReadBufferFixedSize<100> read_buffer;
  const uint32_t buffer_size = write_buffer.get_size();
  memcpy(read_buffer.get_data(), write_buffer.get_data(), buffer_size);
  read_buffer.set_bytes_written(buffer_size);

  // Clear the message data
  Optional_Demo msg_deserialized;

  // Deserialize the buffer into the message object
  const auto deserialize_result = msg_deserialized.deserialize(read_buffer);

  // Compare the values with helper functions - check both presence and zero values
  bool success = HelperOptionalDemo::compare_zero_with_presence(msg_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}