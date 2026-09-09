/*
 *  Copyright (C) 2020-2026 Embedded AMS B.V. - All Rights Reserved
 */

#ifndef HELPER_SERIALIZE_MODE
#define HELPER_SERIALIZE_MODE

#include <Defines.h>
#include <MessageState.h>

namespace HelperSerializeMode
{

template<typename TMessage, typename TBuffer>
EmbeddedProto::Error serialize_message(const TMessage& message, TBuffer& buffer)
{
#ifdef PARTIAL_SERIALIZATION_ENABLED
  EmbeddedProto::MessageStateStack<16U> state_stack;
  return message.serialize_partial(buffer, state_stack.root());
#else
  return message.serialize(buffer);
#endif
}

} // End namespace HelperSerializeMode

#endif // HELPER_SERIALIZE_MODE