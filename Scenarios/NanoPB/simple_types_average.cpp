#include "simple_types.pb.h"
#include <NanoPB/helper_simple_types.h>

void run_scenario()
{
  Test_Simple_Types msg = Test_Simple_Types_init_default;
  HelperSimpleTypesNanoPB::set_average(msg);
}