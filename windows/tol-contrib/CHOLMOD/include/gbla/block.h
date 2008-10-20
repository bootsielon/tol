/* block.h
 *
 * Copyright (C) 2005 Jorge Suit Perez Ronda
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __GBLA_BLOCK_H__
#define __GBLA_BLOCK_H__

/** \file block.h
 Memory Block Allocation.

 All memory is allocated through a gbla_block_t
 structure. The structure contains two components, the size of an area of
 memory and a pointer to the memory.

 Vectors and matrices are made by slicing an underlying block. A slice is a
 set of elements formed from an initial offset and a combination of indices
 and step-sizes. In the case of a matrix the step-size for the row index
 represents the column-length. The step-size for a vector is known as the
 stride.
 */

#include <gbla/gbla_common.h>

__GBLA_BEGIN_DECLS

/** Blocks

 The structure contains two components, the size of an area of memory and a
 pointer to the memory.
 */
typedef struct
{
  /** size of the block */
  int size;
  /** pointer to the memory area */
  double *data;
  /** internal flag */
  int flags;
} gbla_block_t;

/** Block structure initialization.

 This function try to allocate an area of n double-precision elements and
 initialize the block structure data member with this address.
 
 @param[in] n number of elements to fit into the memory area.
 
 @param[in,out] b block structure to initialize. If b is NULL then a new block
 is requested from the heap.

 @return the address of the initialized block (possibly created).  If no memory
 is available NULL is returned.
 */

GBLA_API gbla_block_t * gbla_block_init(int n, gbla_block_t *b);

/** Block structure initialization.

 This function try to allocate an area of n double-precision elements and
 initialize the block structure data member with this address. In case of
 success the memory if filled with zero elements.
 
 @param[in] n number of elements to fit into the memory area.
 
 @param[in,out] b block structure to initialize.  If b is NULL then a new block
 is requested from the heap.

 @return the address of the initialized block (possibly created). If no memory
 is available NULL is returned.
 */

GBLA_API gbla_block_t * gbla_block_cinit(int n, gbla_block_t *b);

/** Block structure releasing.

  This function frees the memory used by a block b previously initialized with
  gsl_block_init or gsl_block_cinit.

  @param[in,out] b the block to release. On return b is invalid.
 */
GBLA_API void gbla_block_free (gbla_block_t *b);

__GBLA_END_DECLS

#endif
