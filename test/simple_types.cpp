
#include <iostream>

#define DESERIALIZE true

#ifdef EAMS
  #include "EAMS/simple_types.h"
  #include "WriteBufferFixedSize.h"
  #ifdef DESERIALIZE
    #include "ReadBufferFixedSize.h"
  #endif
#elif GOOGLE
  #include "Google/simple_types.pb.h"
  #include <string>
#elif NANOPB
  #include "Nanopb/simple_types.pb.h"
  #include "pb_encode.h"
  #ifdef DESERIALIZE
    #include "pb_decode.h"
  #endif
#endif

int main(int argc, char **argv)
{
  Test_Simple_Types msg;

#ifdef DESERIALIZE
  constexpr uint32_t N_DESERIALIZE_BYTES = 53;
  constexpr uint8_t deserialize_data[N_DESERIALIZE_BYTES] = { 0x08, 0x80, 0x80, 0x80, 0x80, 0xf8, 
    0xff, 0xff, 0xff, 0xff, 0x01, 0x10, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01, 
    0x28, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x01, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x6d, 0x00, 0x00, 0x00, 0x80 };
#endif

#ifdef EAMS

  msg.set_a_int32(1);
  msg.set_a_float(2.2F);
  ::EmbeddedProto::WriteBufferFixedSize<10> writeBuffer;
  msg.serialize(writeBuffer);

#ifdef DESERIALIZE
  ::EmbeddedProto::ReadBufferFixedSize<N_DESERIALIZE_BYTES> readBuffer;
  memcpy(readBuffer.get_data(), deserialize_data, N_DESERIALIZE_BYTES);
  readBuffer.set_bytes_written(N_DESERIALIZE_BYTES);
  const auto result = msg.deserialize(readBuffer);
#endif

  std::cout << "EAMS message size: " << sizeof(Test_Simple_Types) << std::endl;

#elif GOOGLE

  msg.set_a_int32(1);
  msg.set_a_float(2.2F);
  std::string str;
  msg.SerializeToString(&str);

#ifdef DESERIALIZE
  const auto result = msg.ParseFromArray(deserialize_data, N_DESERIALIZE_BYTES);
#endif

  std::cout << "Google message size: " << sizeof(Test_Simple_Types) << std::endl;

#elif NANOPB

  msg.a_int32 = 1;
  msg.a_float = 2.2F;
  uint8_t buffer[10];
  pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  pb_encode(&ostream, Test_Simple_Types_fields, &msg);

#ifdef DESERIALIZE
  pb_istream_t istream = pb_istream_from_buffer(deserialize_data, N_DESERIALIZE_BYTES); 
  const auto result = pb_decode(&istream, Test_Simple_Types_fields, &msg);
#endif

  std::cout << "NanoPB message size: " << sizeof(Test_Simple_Types) << std::endl;

#endif
}