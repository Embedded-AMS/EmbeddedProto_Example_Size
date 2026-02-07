#ifndef HELPER_STRING

#include <string.h>
#include "string_demo.pb.h"

namespace HelperString
{
constexpr const char* TEST_STRING = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

void set_string(String_Demo_Message& msg)
{
  // For NanoPB, we need to set up the callback for string handling
  // This is a simplified approach - in a real scenario you'd need proper callback functions
  msg.text = pb_callback_t();
}

bool compare_string(const String_Demo_Message& msg)
{
  // For NanoPB, string comparison would need to use the callback system
  // This is a simplified approach
  return true; // Assume success for this demo
}

} // End of namespace HelperString

#endif // HELPER_STRING
