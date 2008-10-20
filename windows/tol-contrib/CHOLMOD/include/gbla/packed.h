/* packed.h
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

/** @file packed.h
 Packed Matrix Storage.

 The packed storage allows you to store symmetric, Hermitian, or triangular
 matrices more compactly: the relevant triangle, as specified by the
 argument uplo, is packed by columns in a one-dimensional array ap:

 \verbatim
 uplo ='U'

 ( a00 a01 a02 a03  )
 (     a11 a12 a13  )  ==> a00 (a01 a11) (a02 a12 a22) (a03 a13 a23 a33)
 (         a22 a23  )
 (              a33  )      aij is stored in ap[i+j*(j+1)/2] for i <= j.
 
 uplo ='L'

 ( a00             )
 ( a10 a11         )  ==> (a00 a10 a20 a30) (a11 a21 a31) (a22 a32) a33
 (  a20 a21 a22    )
 ( a30 a31 a32 a33 )      aij is stored in ap(i+j*(2*n-j-1)/2) for j <= i.
 \endverbatim

 Note that for real or complex symmetric matrices, packing the upper triangle
 by columns is equivalent to packing the lower triangle by rows; packing the
 lower triangle by columns is equivalent to packing the upper triangle by
 rows. For complex Hermitian matrices, packing the upper triangle by columns
 is equivalent to packing the conjugate of the lower triangle by rows; packing
 the lower triangle by columns is equivalent to packing the conjugate of the
 upper triangle by rows.
*/

#ifndef __GBLA_PACKED_H__
#define __GBLA_PACKED_H__

#include <gbla/block.h>

__GBLA_BEGIN_DECLS

/** Packed Matrix Storage.
 
 The packed storage allows you to store symmetric, Hermitian, or triangular
 matrices more compactly: the relevant triangle , as specified by the argument
 @c uplo, is packed by columns in a one-dimensional array. The matrices in
 packed form must be square.
 */
typedef struct
{
  /** Dimension of the matrix, the matrix is size-by-size.*/
  int size;
  /** Internal value equal to 2*n */
  int N2;
  /** uplo flag.
   Flag indicating the relevant triangle stored. Could take the values:
    - 'U' indicating that the upper triangle is stored.
    - 'L' indicating that the lower triangle is stored.
   */
  char uplo;
  /** Address of the first element a(0,0) */
  double * data;
  /** Block of memory containing the elements of the relevant triangle */
  gbla_block_t block;
  /** Owner flag. Indicate if the block of memory is owned by this matrix. */
  int flags;
} gbla_packed_matrix_t;


/** Packed Matrix Initialization.

 This function create, if requested, and initialize a packed matrix
 struct. The elements of the relevant triangle remains uninitialized.

 @param[in] n dimension of the unpacked matrix. The unpacked matrix is n-by-n.
 
 @param[in] uplo flag indicating the relevant triangle stored. Could take the
 values:
    - 'U' indicating that the upper triangle is stored.
    - 'L' indicating that the lower triangle is stored.
    
 @param[in,out] m dm the address of the packed matrix structure to
                initialize. If m is NULL a new packed matrix is created from
                the heap, then initialized.
                
 @return the address of the initialized packed matrix. If no memory is
         available NULL is returned.
 */
GBLA_API gbla_packed_matrix_t * gbla_packed_matrix_init (int n, char uplo,
                                                gbla_packed_matrix_t *m);

/** Packed Matrix Initialization.

 This function create, if requested, and initialize a packed matrix
 struct. The elements of the relevant triangle are set to zero.

 @param[in] n dimension of the unpacked matrix. The unpacked matrix is n-by-n.
 @param[in] uplo flag indicating the relevant triangle stored. Could take the
 values:
    - 'U' indicating that the upper triangle is stored.
    - 'L' indicating that the lower triangle is stored.
    
 @param[in,out] m dm the address of packed matrix structure to initialize. If
                m is NULL a new packed matrix is created from the heap, then
                initialized.
                
 @return the address of the initialized packed matrix. If no memory is
         available NULL is returned.
 */
GBLA_API gbla_packed_matrix_t * gbla_packed_matrix_cinit (int n, char uplo,
                                                 gbla_packed_matrix_t *m);

/** Packed Matrix Freeing.

 This function free the memory area used by a packed matrix.
 
 @param[in,out] m the packed matrix to free. The underlying memory resources
                are released.  On exit, if m was created from the heap, the
                memory area pointed by m is invalid.
 */
GBLA_API void gbla_packed_matrix_free (gbla_packed_matrix_t *m);

/** Packed Matrix Access Element.

 This function access the element m(i,j), where m is a valid packed matrix
 struct. No range checking is performed.

 @param[in] m the packed matrix.
 
 @param[in] i row index of the element. If <tt>uplo=='U'</tt>, i<=j. If
            <tt>uplo=='L'</tt>, i>=j.
 
 @param[in] j column index of the element. If <tt>uplo=='U'</tt>, i<=j. If
            <tt>uplo=='L'</tt>, i>=j.
 
 @return return the value of the element m(i,j).
 */
GBLA_API double gbla_packed_matrix_get(gbla_packed_matrix_t *m, int i, int j);

/** Packed Matrix Modify Element.

 This function change the element m(i,j) to the value x, where m is a valid
 packed matrix struct. No range checking is performed.

 @param[in,out] m the packed matrix.
 
 @param[in] i row index of the element. If <tt>uplo=='U'</tt>, i<=j. If
            <tt>uplo=='L'</tt>, i>=j.

 @param[in] j column index of the element. If <tt>uplo=='U'</tt>, i<=j. If
            <tt>uplo=='L'</tt>, i>=j.
 
 @param[in] x new value for element m(i,j).
 */
GBLA_API void gbla_packed_matrix_set(gbla_packed_matrix_t *m, int i, int j, double x);

/** Symmetric Packed Matrix Access Element.

 This function access the element m(i,j), where m is a valid packed matrix
 struct. The matrix is asummed to be symmetric, thus if (<tt>uplo=='U'</tt>
 and i>j) or (<tt>uplo=='L'</tt> and i<j) the indices i and j are exchanged and
 the element (j,i) is accessed instead of (i,j). The valid range is 0<=i,j<n,
 but no range checking is performed, thus if i or j are out of range you can
 get unexpected results.

 @param[in] m the packed matrix.
 @param[in] i row index of the element.
 @param[in] j column index of the element.
 @return the value of the element m(i,j).
 */
GBLA_API double gbla_packed_matrix_sym_get(gbla_packed_matrix_t *m, int i, int j);

/** Symmetric Packed Matrix Modify Element.

 This function change the element m(i,j) to the value x, where m is a valid
 packed matrix struct. The matrix is asummed to be symmetric, thus if
 (<tt>uplo=='U'</tt> and i>j) or (<tt>uplo=='L'</tt> and i<j) the indices i
 and j are exchanged and the element (j,i) is accessed instead of (i,j). The
 valid range is 0<=i,j<n, but no range checking is performed, thus if i or j
 are out of range you can get unexpected results.

 @param[in,out] m the packed matrix.
 @param[in] i row index of the element.
 @param[in] j column index of the element.
 @param[in] x the new value for element m(i,j).
 */
GBLA_API void gbla_packed_matrix_sym_set(gbla_packed_matrix_t *m, int i, int j, double x);

/** Address of Element in Packed Matrix.

 This macro returns the address in memory of the element m(i,j), where m is
 a valid packed matrix struct. No range checking is performed, thus if the
 indices i,j are out of the relevant triangle, as specified by the member
 @c uplo, you can get unexpected results when you use the address returned. 

 @param[in] m the packed matrix.
 @param[in] i row index of the element.
 @param[in] j column index of the element.
 @return the address in memory of the element m(i,j).
 */
#define gbla_packed_matrix_ptr(m, i, j) \
(m->data + (m->uplo=='U'?(i)+(j)*((j)+1)/2:(i)+(j)*(m->N2-j-1)/2))

/** Packed matrix random initialization

 This function sets all the elements of a matrix in packed form, given as
 argument, to a random value from the interval @f$[0,1]@f$. The random values
 are generated using the POSIX function rand().
 
 @param[in,out] m the matrix to initialize.
 */
GBLA_API void gbla_packed_matrix_set_all_random (gbla_packed_matrix_t * m);

/** Copying packed matrices.
 
 This function copies the elements of the matrix src into the matrix dest. The
 two matrices must have the same dimensions.

 @param[out] dest matrix where to copy elements.
 @param[in] src matrix where to read elements.
 @return GBLA_SUCCESS (no dimension check is done).
 */
GBLA_API int gbla_packed_matrix_copy(gbla_packed_matrix_t * dest,
                            const gbla_packed_matrix_t * src);

/** Packed matrix substraction

 This function substract one packed matrix from other. The matrices must be
 compatible in @c uplo and @c size. The operation is @f$m1=m1-m2@f$.

 @param[in,out] m1 On entry the original packed matrix @c m1. On exit @c m1 is
                replaced by <tt>m1-m2</tt>.

 @param[in] m2 a packed matrix to substract from @c m2.
 @return GBLA_SUCCESS (no dimension check is done). 
*/ 
GBLA_API int gbla_packed_matrix_sub (gbla_packed_matrix_t * m1,
                            const gbla_packed_matrix_t * m2);

/** Packed matrix test for null.

 This function returns 1 if all the elements of the matrix m are zero, and 0
 otherwise.
 
 @param[in] m
 @return
        - 1 if all the elements are null.
        - 0 otherwise.
 */
GBLA_API int gbla_packed_matrix_isnull (gbla_packed_matrix_t * m);

/** Packed matrix test for null with epsilon

 This function returns 1 if all the elements of the packed matrix @f$m@f$ are
 in a neighborhood of zero. The test is defined for @f$\epsilon>0@f$ as
 @f$|m_{ij}|<=\epsilon@f$ and 0 otherwise.

 @param[in] m the packed matrix to test.

 @param[in] eps is the value of @f$\epsilon>0@f$ which define de neighborhood
            @f$[-\epsilon,+\epsilon]@f$.
 
 @return
        - 1 if @f$|m_{ij}|<=\epsilon, \forall i,j@f$.
        - 0 otherwise.
 */
GBLA_API int gbla_packed_matrix_isnull_eps (gbla_packed_matrix_t * m, double eps);

__GBLA_END_DECLS

#endif
