#include "repeated_demo.pb.h"
#include <NanoPB/helper_repeated_demo.h>
#include <pb_encode.h>
#include <pb_decode.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  Nested_Repeated_Messages msg = Nested_Repeated_Messages_init_default;
  HelperRepeatedDemoNanoPB::set_average_nested_repeated_messages(msg);

  // Create buffer for serialized data
  uint8_t buffer[10000];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  // Serialize the message
  const bool encode_success = pb_encode(&stream, Nested_Repeated_Messages_fields, &msg);
  if (!encode_success) {
    return 0;
  }

  // Create deserialization buffer and copy data
  uint8_t deserialize_buffer[10000];
  const uint32_t buffer_size = stream.bytes_written;
  memcpy(deserialize_buffer, buffer, buffer_size);

  // Clear the message data
  Nested_Repeated_Messages msg_deserialized = Nested_Repeated_Messages_init_default;

  // Deserialize the buffer into the message object
  pb_istream_t istream = pb_istream_from_buffer(deserialize_buffer, buffer_size);
  const bool decode_success = pb_decode(&istream, Nested_Repeated_Messages_fields, &msg_deserialized);
  if (!decode_success) {
    return 0;
  }

  // Compare the values with helper functions
  bool success = HelperRepeatedDemoNanoPB::compare_average_nested_repeated_messages(msg_deserialized);

  // Return buffer size if comparison succeeded, otherwise return 0
  return success ? buffer_size : 0;
}