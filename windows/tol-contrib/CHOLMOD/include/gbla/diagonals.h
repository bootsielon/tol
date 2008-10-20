/* diagonals.h
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

#ifndef __GBLA_DIAGONALS_H__
#define __GBLA_DIAGONALS_H__

#include <gbla/block.h>

__GBLA_BEGIN_DECLS

/** @file diagonals.h

 Bi/Tri-diagonal Matrix Storage.
 
 An unsymmetric tridiagonal matrix of order n is stored in three
 one-dimensional arrays, one of length n containing the diagonal elements, and
 two of length n - 1 containing the subdiagonal and superdiagonal elements in
 elements 0 : n - 2.

 A symmetric tridiagonal or bidiagonal matrix is stored in two one-dimensional
 arrays, one of length n containing the diagonal elements, and one of length n
 containing the off-diagonal elements.

 Both bidiagonal and tridiagonal matrices are square.
*/

/** Bidiagonal Matrix

 A symmetric tridiagonal or bidiagonal matrix is stored in two one-dimensional
 arrays, one of length n containing the diagonal elements, and one of length n
 containing the off-diagonal elements. The unpacked matrix is assumed to be
 square of dimension n-by-n.
 */
typedef struct
{
  /**  Dimension of the unpacked square matrix: size-by-size */
  int size;
  /** UPLO flag. Indicate the corresponding band stored in case of a
   * symmetric tridiagonal. Could take the values:
   * - 'U' meaning the superdiagonal is stored in d1.
   * - 'L' meaning the subdiagonal is stored in d1.*/
  char UPLO;
  /** Main diagonal. It is an address pointing into the memory area allocated
   * in member block. Its valid range is d[0:size-1].*/
  double *d;
  /** Super- or sub- diagonal. It is an address pointing into the memory area
   * allocated in member block. Its valid range is d[0:size-2].*/
  double *d1;
  /** block of memory allocated of at least 2*size - 1 elements.*/
  gbla_block_t block;
  /** Owner flag. Indicate if the block of memory is owned by this matrix. */
  int flags;
} gbla_bidiagonal_t;

/** Bidiagonal Matrix Initialization.

 This function create, if requested, and initialize a bidiagonal matrix
 structure. The elements of the main diagonal and the relevant band diagonal
 are uninitialized.
 
 @param[in] n the dimension of the unpacked matrix that is n-by-n.
 @param[in] UPLO flag indicating what side we are storing.
            - 'U' means that the superdiagonal is stored.
            - 'L' means that the subdiagonal is stored.

 @param[in,out] dm the address of the bidiagonal matrix structure to
                initialize. If dm is NULL a new bidiagonal matrix is created
                from the heap, then initialized.

 @return the address of the initialized bidiagonal matrix. If no memory is
         available NULL is returned.
 */
GBLA_API gbla_bidiagonal_t * gbla_bidiagonal_init (int n, char UPLO,
                                          gbla_bidiagonal_t *dm);

/** Bidiagonal Matrix Initialization.

 This function create, if requested, and initialize a bidiagonal matrix
 structure. The elements of the main diagonal and the relevant band diagonal
 are set to zero.
 
 @param[in] n the dimension of the unpacked matrix that is n-by-n.
 @param[in] UPLO flag indicating what side we are storing.
            - 'U' means that the superdiagonal is stored.
            - 'L' means that the subdiagonal is stored.
            
 @param[in,out] dm the address of the bidiagonal matrix structure. If dm is
                NULL a new bidiagonal matrix is created from the heap, then
                initialized. The elements of the matrix are set to zero.

 @return the address of the initialized bidiagonal matrix. If no memory is
         available NULL is returned.
*/
GBLA_API gbla_bidiagonal_t * gbla_bidiagonal_cinit (int n, char UPLO,
                                           gbla_bidiagonal_t *dm);

/** Bidiagonal Matrix Freeing.

 This function free the memory area used by a bidiagonal matrix structure.
 
 @param[in,out] dm the bidiagonal matrix to release its memory resources. The
                underlying memory resources are released. On exit, if dm was
                created from the heap, the memory area pointed by dm is
                invalid.
 */
GBLA_API void gbla_bidiagonal_free (gbla_bidiagonal_t *dm);

/** Bidiagonal matrix random initialization

 This function sets all the elements of a bidiagonal matrix, given as
 argument, to a random value from the interval @f$[0,1]@f$. The random values
 are generated using the POSIX function rand().
 
 @param[in,out] dm the matrix to initialize.
 */
GBLA_API void gbla_bidiagonal_set_all_random (gbla_bidiagonal_t * dm);

/** Tridiagonal Matrix

 An unsymmetric tridiagonal matrix of order n is stored in three
 one-dimensional arrays, one of length n containing the diagonal elements, and
 two of length n - 1 containing the subdiagonal and superdiagonal elements in
 elements 0 : n - 1.
 */
typedef struct
{
  /**  Dimension of the unpacked square matrix: size-by-size */
  int size;
  /** Subdiagonal. It is an address pointing into the memory area allocated
   * in member block. Its valid range is dl[0:size-2].*/
  double *dl;
  /** Main diagonal. It is an address pointing into the memory area allocated
   * in member block. Its valid range is d[0:size-1].*/
  double *d;
  /** Superdiagonal. It is an address pointing into the memory area allocated
   * in member block. Its valid range is dl[0:size-2].*/
  double *du;
  /** block of memory allocated of at least 3*size - 2 elements.*/
  gbla_block_t block;  
  /** Owner flag. Indicate if the block of memory is owned by this matrix. */
  int flags;
} gbla_tridiagonal_t;

/** Tridiagonal Matrix Initialization.

 This function create, if requested, and initialize a tridiagonal matrix
 structure. The elements of the main diagonal, superdiagonal and subdiagonal
 are uninitialized.
 
 @param[in] n the dimension of the unpacked matrix that is n-by-n.
 
 @param[in,out] tm the previously allocated tridiagonal matrix structure. If
                tm is NULL a new tridiagonal matrix is created from the heap,
                then initialized.

 @return the address of the initialized tridiagonal matrix. If no memory is
         available NULL is returned.
 */
GBLA_API gbla_tridiagonal_t *gbla_tridiagonal_init (int n, gbla_tridiagonal_t *tm);

/** Tridiagonal Matrix Initialization.

 This function create, if requested, and initialize a tridiagonal matrix
 structure. The elements of the main diagonal, superdiagonal and subdiagonal
 are set to zero.
 
 @param[in] n the dimension of the unpacked matrix that is n-by-n.
 @param[in,out] tm the previously allocated tridiagonal matrix structure. If
                tm is NULL a new tridiagonal matrix is created from the heap,
                then initialized. The elements of the matrix are set to zero.

 @return the address of the initialized tridiagonal matrix. If no memory is
         available NULL is returned.
 */
GBLA_API gbla_tridiagonal_t * gbla_tridiagonal_cinit (int n, gbla_tridiagonal_t *tm);

/** Tridiagonal Matrix Resource Freeing.
 
 This function release the memory resources allocated to the tridiagonal matrix
 given in the argument dm.
 
 @param[in,out] dm the tridiagonal matrix to release its memory resources. The
                underlying memory resources are released. On exit, if dm was
                created from the heap, the memory area pointed by dm is
                invalid.
 */
GBLA_API void gbla_tridiagonal_free (gbla_tridiagonal_t *dm);

/** Tridiagonal matrix random initialization

 This function sets all the elements of a tridiagonal matrix, given as
 argument, to a random value from the interval @f$[0,1]@f$. The random values
 are generated using the POSIX function rand().
 
 @param[in,out] m the matrix to initialize.
 */
GBLA_API void gbla_tridiagonal_set_all_random (gbla_tridiagonal_t * m);

__GBLA_END_DECLS

#endif /* __GBLA_PACKED_H__ */
