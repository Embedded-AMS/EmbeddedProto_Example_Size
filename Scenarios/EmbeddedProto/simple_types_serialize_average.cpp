
#include <simple_types.h>
#include <EmbeddedProto/helper_simple_types.h>
#include <WriteBufferFixedSize.h>

// Volatile variable to prevent compiler from optimizing away serialization
// The compiler must preserve the buffer write since this checksum depends on it
extern "C" volatile uint8_t g_buffer_checksum;
volatile uint8_t g_buffer_checksum = 0;

uint32_t run_scenario()
{
  Simple_Types msg;
  HelperSimpleTypes::set_average(msg);

  EmbeddedProto::WriteBufferFixedSize<100> buffer;
  const auto result = msg.serialize(buffer);

  // Calculate checksum to prevent dead code elimination
  // The compiler cannot optimize away serialize() since buffer content is used
  uint8_t checksum = 0;
  const uint8_t* data = buffer.get_data();
  for (uint32_t i = 0; i < buffer.get_size(); i++) {
    checksum ^= data[i];
  }
  g_buffer_checksum = checksum;

  return buffer.get_size();
}
