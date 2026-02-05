#include "simple_types.pb.h"
#include <NanoPB/helper_simple_types.h>
#include <pb_encode.h>

uint32_t run_scenario()
{
  Test_Simple_Types msg = Test_Simple_Types_init_default;
  HelperSimpleTypesNanoPB::set_min(msg);

  // Create buffer for serialized data
  uint8_t buffer[100];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  // Serialize the message
  const bool result = pb_encode(&stream, Test_Simple_Types_fields, &msg);

  return stream.bytes_written;
}
