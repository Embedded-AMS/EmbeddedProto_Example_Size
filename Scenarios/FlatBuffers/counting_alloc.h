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

#ifndef FLATCC_COUNTING_ALLOC_H
#define FLATCC_COUNTING_ALLOC_H

#include <stddef.h>
#include <stdint.h>

// The flatcc builder and its emitter allocate their stacks and pages from the heap.
// These functions wrap malloc and track the peak number of bytes held, which is the
// RAM a scenario reports as the message size next to sizeof(flatcc_builder_t).
// The header is force-included in every FlatBuffers translation unit, so the
// FLATCC_* macros below reach the runtime sources before flatcc_alloc.h does.

#ifdef __cplusplus
extern "C" {
#endif

void* flatcc_counting_alloc(size_t size);
void* flatcc_counting_realloc(void* pointer, size_t size);
void flatcc_counting_free(void* pointer);
uint32_t flatcc_counting_peak_bytes(void);

#ifdef __cplusplus
}
#endif

#define FLATCC_ALLOC(n) flatcc_counting_alloc(n)
#define FLATCC_REALLOC(p, n) flatcc_counting_realloc(p, n)
#define FLATCC_FREE(p) flatcc_counting_free(p)

#endif // FLATCC_COUNTING_ALLOC_H
