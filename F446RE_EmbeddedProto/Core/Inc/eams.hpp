/*
 * eams.h
 *
 *  Created on: Nov 23, 2021
 *      Author: bart
 */

#ifndef INC_EAMS_HPP_
#define INC_EAMS_HPP_

#include "../../../../generated/EAMS/simple_types.h"
#include "../../../../EmbeddedProto/src/WriteBufferFixedSize.h"
#include "../../../../EmbeddedProto/src/ReadBufferFixedSize.h"
#include <string>

constexpr uint8_t N = 10;

class test_float {
  public:
    float value_;
};

class test_double {
  public:
    double value_;
};

/*
 *
    void size() const
    {
      size_t total = 0;
      const size_t size_a_int32       = sizeof(a_int32_);        total += size_a_int32      ;
      const size_t size_a_int64       = sizeof(a_int64_);        total += size_a_int64      ;
      const size_t size_a_uint32      = sizeof(a_uint32_);       total += size_a_uint32     ;
      const size_t size_a_uint64      = sizeof(a_uint64_);       total += size_a_uint64     ;
      const size_t size_a_sint32      = sizeof(a_sint32_);       total += size_a_sint32     ;
      const size_t size_a_sint64      = sizeof(a_sint64_);       total += size_a_sint64     ;
      const size_t size_a_bool        = sizeof(a_bool_);         total += size_a_bool       ;
      const size_t size_a_enum        = sizeof(a_enum_);         total += size_a_enum       ;
      const size_t size_a_fixed64     = sizeof(a_fixed64_);      total += size_a_fixed64    ;
      const size_t size_a_sfixed64    = sizeof(a_sfixed64_);     total += size_a_sfixed64   ;
      const size_t size_a_double      = sizeof(a_double_);       total += size_a_double     ;
      const size_t size_a_fixed32     = sizeof(a_fixed32_);      total += size_a_fixed32    ;
      const size_t size_a_sfixed32    = sizeof(a_sfixed32_);     total += size_a_sfixed32   ;
      const size_t size_a_float       = sizeof(a_float_);        total += size_a_float      ;
      const size_t size_a_nested_enum = sizeof(a_nested_enum_);  total += size_a_nested_enum;


      const size_t size_msg = sizeof(Test_Simple_Types);
    }
 */

void test_field_sizes()
{
  size_t total = 0;
  const size_t size_a_int32       = sizeof(EmbeddedProto::int32::value_);        total += size_a_int32      ;
  const size_t size_a_int64       = sizeof(EmbeddedProto::int64::value_);        total += size_a_int64      ;
  const size_t size_a_uint32      = sizeof(EmbeddedProto::uint32::value_);       total += size_a_uint32     ;
  const size_t size_a_uint64      = sizeof(EmbeddedProto::uint64::value_);       total += size_a_uint64     ;
  const size_t size_a_sint32      = sizeof(EmbeddedProto::sint32::value_);       total += size_a_sint32     ;
  const size_t size_a_sint64      = sizeof(EmbeddedProto::sint64::value_);       total += size_a_sint64     ;
  const size_t size_a_bool        = sizeof(EmbeddedProto::boolean::value_);      total += size_a_bool       ;
  const size_t size_a_fixed64     = sizeof(EmbeddedProto::fixed64::value_);      total += size_a_fixed64    ;
  const size_t size_a_sfixed64    = sizeof(EmbeddedProto::sfixed64::value_);     total += size_a_sfixed64   ;
  const size_t size_a_double      = sizeof(EmbeddedProto::doublefixed::value_);  total += size_a_double     ;
  const size_t size_a_fixed32     = sizeof(EmbeddedProto::fixed32::value_);      total += size_a_fixed32    ;
  const size_t size_a_sfixed32    = sizeof(EmbeddedProto::sfixed32::value_);     total += size_a_sfixed32   ;
  const size_t size_a_float       = sizeof(EmbeddedProto::floatfixed::value_);   total += size_a_float      ;

  const size_t size_class_float = sizeof(test_float);
  const size_t size_class_double = sizeof(test_double);

  int dummy = 0;
  dummy += 2;
}

Test_Simple_Types msg;
void test_eams()
{
  msg.set_a_int32(1);
  ::EmbeddedProto::WriteBufferFixedSize<N> wbuffer;
  msg.serialize(wbuffer);

  ::EmbeddedProto::ReadBufferFixedSize<N> rbuffer;
  msg.deserialize(rbuffer);
}

Test_Simple_Types msgArray[100];
void test_eams_array()
{
  for(auto& msg : msgArray) {

    //msg.size();

    msg.set_a_int32(1);
    ::EmbeddedProto::WriteBufferFixedSize<N> wbuffer;
    msg.serialize(wbuffer);

    ::EmbeddedProto::ReadBufferFixedSize<N> rbuffer;
    msg.deserialize(rbuffer);
  }
}

#endif /* INC_EAMS_HPP_ */
