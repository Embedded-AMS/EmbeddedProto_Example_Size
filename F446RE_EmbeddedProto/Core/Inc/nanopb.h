
#ifndef INC_NANOPB_H_
#define INC_NANOPB_H_

#include "simple_types.pb.h"
#include "nested_message.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

Test_Simple_Types msg;
void test_nanopb()
{
  msg.a_int32 = 1;
  uint8_t obuffer[100];
  pb_ostream_t ostream = pb_ostream_from_buffer(obuffer, sizeof(obuffer));
  pb_encode(&ostream, Test_Simple_Types_fields, &msg);

  uint8_t ibuffer[100];
  pb_istream_t istream = pb_istream_from_buffer(ibuffer, sizeof(ibuffer));
  pb_decode(&istream, Test_Simple_Types_fields, &msg);
}

Test_Simple_Types msgArray[100];
void test_nanopb_array()
{
  for(auto& msg : msgArray) {

/*
    size_t total = 0;
    const size_t size_a_int32       = sizeof(msg.a_int32);        total += size_a_int32      ;
    const size_t size_a_int64       = sizeof(msg.a_int64);        total += size_a_int64      ;
    const size_t size_a_uint32      = sizeof(msg.a_uint32);       total += size_a_uint32     ;
    const size_t size_a_uint64      = sizeof(msg.a_uint64);       total += size_a_uint64     ;
    const size_t size_a_sint32      = sizeof(msg.a_sint32);       total += size_a_sint32     ;
    const size_t size_a_sint64      = sizeof(msg.a_sint64);       total += size_a_sint64     ;
    const size_t size_a_bool        = sizeof(msg.a_bool);         total += size_a_bool       ;
    const size_t size_a_enum        = sizeof(msg.a_enum);         total += size_a_enum       ;
    const size_t size_a_fixed64     = sizeof(msg.a_fixed64);      total += size_a_fixed64    ;
    const size_t size_a_sfixed64    = sizeof(msg.a_sfixed64);     total += size_a_sfixed64   ;
    const size_t size_a_double      = sizeof(msg.a_double);       total += size_a_double     ;
    const size_t size_a_fixed32     = sizeof(msg.a_fixed32);      total += size_a_fixed32    ;
    const size_t size_a_sfixed32    = sizeof(msg.a_sfixed32);     total += size_a_sfixed32   ;
    const size_t size_a_float       = sizeof(msg.a_float);        total += size_a_float      ;
    const size_t size_a_nested_enum = sizeof(msg.a_nested_enum);  total += size_a_nested_enum;


    const size_t size_msg = sizeof(msg);
*/
    uint8_t obuffer[10];
    pb_ostream_t ostream = pb_ostream_from_buffer(obuffer, sizeof(obuffer));
    pb_encode(&ostream, Test_Simple_Types_fields, &msg);

    uint8_t ibuffer[10];
    pb_istream_t istream = pb_istream_from_buffer(ibuffer, sizeof(ibuffer));
    pb_decode(&istream, Test_Simple_Types_fields, &msg);
  }
}

demo_space_message_e msg_c;
void test_message_c()
{

}

#endif
