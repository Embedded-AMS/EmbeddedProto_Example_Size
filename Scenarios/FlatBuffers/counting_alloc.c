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

#include "counting_alloc.h"

#include <stdlib.h>
#include <string.h>

static size_t current_bytes = 0;
static size_t peak_bytes = 0;

void* flatcc_counting_alloc(size_t size)
{
  size_t* block = (size_t*)malloc(sizeof(size_t) + size);
  if(NULL == block)
  {
    return NULL;
  }
  *block = size;
  current_bytes += size;
  peak_bytes = (current_bytes > peak_bytes) ? current_bytes : peak_bytes;
  return block + 1;
}

void flatcc_counting_free(void* pointer)
{
  if(NULL != pointer)
  {
    size_t* block = ((size_t*)pointer) - 1;
    current_bytes -= *block;
    free(block);
  }
}

void* flatcc_counting_realloc(void* pointer, size_t size)
{
  void* fresh = flatcc_counting_alloc(size);
  if((NULL != fresh) && (NULL != pointer))
  {
    const size_t old_size = *(((size_t*)pointer) - 1);
    memcpy(fresh, pointer, (old_size < size) ? old_size : size);
    flatcc_counting_free(pointer);
  }
  return fresh;
}

uint32_t flatcc_counting_peak_bytes(void)
{
  return (uint32_t)peak_bytes;
}
