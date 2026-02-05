
#include <simple_types.h>
#include <EmbeddedProto/helper_simple_types.h>
#include <WriteBufferFixedSize.h>

uint32_t run_scenario()
{
  Test_Simple_Types msg;
  HelperSimpleTypes::set_average(msg);

  EmbeddedProto::WriteBufferFixedSize<100> buffer;
  const auto result = msg.serialize(buffer);

  return buffer.get_size();
}
