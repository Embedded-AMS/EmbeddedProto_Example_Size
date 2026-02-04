
#include <simple_types.h>
#include <EmbeddedProto/helper_simple_types.h>
#include <WriteBufferFixedSize.h>

void run_scenario()
{
  Test_Simple_Types msg;
  HelperSimpleTypes::set_max(msg);

  EmbeddedProto::WriteBufferFixedSize<100> buffer;
  const auto result = msg.serialize(buffer);
}
