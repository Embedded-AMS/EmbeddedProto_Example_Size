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

#ifndef COUNTING_ALLOCATOR_H
#define COUNTING_ALLOCATOR_H

#include <ArduinoJson.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// ArduinoJson 7 keeps a document's data on the heap, so its memory is invisible to sizeof().
// This allocator wraps malloc and tracks the peak number of bytes a document held, which
// is the RAM the scenario reports as the message size.
class CountingAllocator : public ArduinoJson::Allocator
{
  public:
    void* allocate(size_t size) override
    {
      size_t* block = static_cast<size_t*>(malloc(sizeof(size_t) + size));
      if(nullptr == block)
      {
        return nullptr;
      }
      *block = size;
      current_ += size;
      peak_ = (current_ > peak_) ? current_ : peak_;
      return block + 1;
    }

    void deallocate(void* pointer) override
    {
      if(nullptr != pointer)
      {
        size_t* block = static_cast<size_t*>(pointer) - 1;
        current_ -= *block;
        free(block);
      }
    }

    void* reallocate(void* pointer, size_t new_size) override
    {
      void* fresh = allocate(new_size);
      if((nullptr != fresh) && (nullptr != pointer))
      {
        const size_t old_size = *(static_cast<size_t*>(pointer) - 1);
        memcpy(fresh, pointer, (old_size < new_size) ? old_size : new_size);
        deallocate(pointer);
      }
      return fresh;
    }

    uint32_t peak_bytes() const { return static_cast<uint32_t>(peak_); }

  private:
    size_t current_ = 0;
    size_t peak_ = 0;
};

// One allocator shared by every document of a scenario.
inline CountingAllocator g_json_allocator;

#endif // COUNTING_ALLOCATOR_H
