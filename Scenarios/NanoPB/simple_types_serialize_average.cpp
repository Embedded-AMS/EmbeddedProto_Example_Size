#include "simple_types.pb.h"
#include <NanoPB/helper_simple_types.h>
#include <pb_encode.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  Simple_Types msg = Simple_Types_init_default;
  HelperSimpleTypesNanoPB::set_average(msg);

  // Create buffer for serialized data
  uint8_t buffer[100];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  // Serialize the message
  const bool result = pb_encode(&stream, Simple_Types_fields, &msg);

  // Calculate checksum to prevent dead code elimination
  // The compiler cannot optimize away serialize() since buffer content is used
  uint8_t checksum = 0;
  for (uint32_t i = 0; i < stream.bytes_written; i++) {
    checksum ^= buffer[i];
  }
  g_buffer_checksum = checksum;

  return stream.bytes_written;
}
