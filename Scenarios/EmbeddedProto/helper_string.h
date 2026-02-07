#ifndef HELPER_STRING

#include <string_demo.h>
#include <cstdint>
#include <cstring>

namespace HelperString
{
constexpr const char* TEST_STRING = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
constexpr uint32_t STRING_LENGTH = 123; // Length for the string field

template<uint32_t LENGTH>
void set_string(String_Demo_Message<LENGTH>& msg)
{
  msg.mutable_text().set(TEST_STRING);
}

template<uint32_t LENGTH>
bool compare_string(const String_Demo_Message<LENGTH>& msg)
{
  return strcmp(msg.text(), TEST_STRING) == 0;
}

} // End of namespace HelperString

#endif // HELPER_STRING
