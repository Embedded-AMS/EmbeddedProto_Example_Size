/*
 *  Copyright (C) 2020-2026 Embedded AMS B.V. - All Rights Reserved
 *
 *  This file is part of Embedded Proto.
 *
 *  Embedded Proto is open source software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as published 
 *  by the Free Software Foundation, version 3 of the license.
 *
 *  Embedded Proto  is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
 *
 *  For commercial and closed source application please visit:
 * <https://embeddedproto.com/pricing/>.
 *
 *  Embedded AMS B.V.
 *  Info:
 *    info at EmbeddedProto dot com
 *
 *  Postal address:
 *    Atoomweg 2
 *    1627 LE, Hoorn
 *    the Netherlands
 */

#ifndef HELPER_FLATBUFFERS
#define HELPER_FLATBUFFERS

#include <flatcc/flatcc_builder.h>
#include <FlatBuffers/counting_alloc.h>
#include <scenario.h>

#include <cstdint>
#include <cstring>

namespace HelperFlatBuffers
{

// Copy the finished buffer out of the builder's pages into the caller's array, report
// the RAM the build took and release the builder. Returns the buffer size, or 0 when
// the array is too small.
inline uint32_t finish(flatcc_builder_t& builder, uint8_t* buffer, const uint32_t capacity)
{
  const size_t size = flatcc_builder_get_buffer_size(&builder);
  report_message_size(static_cast<uint32_t>(sizeof(builder)) + flatcc_counting_peak_bytes());
  void* copied = (size <= capacity) ? flatcc_builder_copy_buffer(&builder, buffer, capacity) : nullptr;
  flatcc_builder_clear(&builder);
  return (nullptr != copied) ? static_cast<uint32_t>(size) : 0;
}

} // End of namespace HelperFlatBuffers

#endif // HELPER_FLATBUFFERS
