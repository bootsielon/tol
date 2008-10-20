/* blas2.h : GBLA BLAS Level II API
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

#ifndef __GBLA_BLAS2_H__
#define __GBLA_BLAS2_H__

#include <gbla/packed.h>
#include <gbla/matrix.h>
#include <gbla/bandmatrix.h>

__GBLA_BEGIN_DECLS

/** @file blas2.h

 @b BLAS Level II API

 BLAS Level II include routines and functions, which perform matrix-vector
 operations, e.g. @f$y = \alpha x + y@f$. The routines provided here are based
 on the vectors and matrix types. The corresponding low level routines are
 provided in blapi.h
*/

/** Matrix-vector product for a symmetric matrix: @f$y = \alpha*A*x +
  \beta*y@f$.

 This function compute a matrix-vector product for a real symmetric matrix:
 @f$y = \alpha*A*x + \beta*y@f$.

 @f$\alpha@f$ and @f$beta@f$ are scalars, @f$x@f$ and @f$y@f$ are vectors with
 @f$n@f$ elements, and @f$A@f$ is an @f$n@f$ by @f$n@f$ symmetric matrix.

 @param[in] uplo specifies whether the upper or lower triangular part of the
                 matrix @c A is referenced:
                 - If <tt>uplo = 'U'</tt>, the upper-triangular part of @c A
                   is referenced.
                 - If <tt>uplo = 'L'</tt>, the lower-triangular part of @c A
                   is referenced.

 @param[in] alpha  the scalar @f$\alpha@f$.

 @param[in] A is a square @f$n@f$ by @f$n@f$ symmetric matrix. Argument @c
            uplo specifies which part of the matrix is used in the
            computation.

 @param[in] X a one-dimensional array @c X. Argument @c X contains the vector
            @f$x@f$.

 @param[in] beta  the scalar @f$\beta@f$.

 @param[in,out] Y On entry, a one-dimensional array @c Y containing the vector
                @f$y@f$.  On exit, @f$y@f$ is overwritten by the updated
                vector @f$y@f$.
                
 @return GBLA_SUCCESS.
 */
GBLA_API int  gbla_blas_dsymv (char uplo,
			       double alpha,
			       const gbla_matrix_t * A,
			       const gbla_vector_t * X,
			       double beta,
			       gbla_vector_t * Y);

/** Matrix-vector product for a general band matrix: @f$y = \alpha*op(A)*x +
    \beta*y@f$.

 The ::gbla_blas_dgbmv function compute a matrix-vector product for either a
 general band matrix or its transpose:

 @f{align*}
 y =& \alpha*A*x + \beta*y \\
 \text{or} \\
 y =& \alpha*A^T*x + \beta*y
 @f}

 @f$\alpha@f$ and @f$\beta@f$ are scalars, @f$x@f$ and @f$y@f$ are vectors,
 and @f$A@f$ is an @f$m@f$ by @f$n@f$ band matrix.

 @param[in] trans a character, specifies the operation to be performed:
            - If <tt>trans ='N'</tt>, the operation is @f$y = \alpha*A*x +
              \beta*y@f$.
            - If <tt>trans ='T'</tt>, the operation is @f$y = \alpha*A^T*x +
              \beta*y@f$.

 @param[in] alpha  the scalar @f$\alpha@f$.

 @param[in] A a two-dimensional @c m by @c n band matrix with @c ku
            super-diagonals and @c kl sub-diagonals.

 @param[in] X a one-dimensional array containing the vector @f$x@f$.  When @c
            trans is equal to @c 'N' its length must be @c n otherwise its
            length must be @c m.

 @param[in] beta  the scalar @f$\beta@f$.

 @param[in, out] Y On entry, a one-dimensional array containing the vector
            @f$y@f$.  When @c trans is equal to @c 'N' its length must be @c m
            otherwise its length muste be @c n. If @f$\beta=0@f$, @c Y need
            not be set.  On exit, @c Y is overwritten by the updated vector
            @f$y@f$.

 @return GBLA_SUCCESS
 */
GBLA_API int gbla_blas_dgbmv (char trans,
			      double alpha,
			      const gbla_band_matrix_t * A,
			      const gbla_vector_t * X,
			      double beta,
			      gbla_vector_t * Y);

/** Matrix-vector product for a general matrix: @f$y = \alpha*op(A)*x +
    \beta*y@f$.

 The ::gbla_blas_dgemv function compute a matrix-vector product for either a
 general matrix or its transpose:

 @f{align*}
 y =& \alpha*A*x + \beta*y \\
 \text{or} \\
 y =& \alpha*A^T*x + \beta*y
 @f}

 @f$\alpha@f$ and @f$\beta@f$ are scalars, @f$x@f$ and @f$y@f$ are vectors,
 and @f$A@f$ is an @f$m@f$ by @f$n@f$ matrix.

 @param[in] trans a character, specifies the operation to be performed:
            - If <tt>trans ='N'</tt>, the operation is @f$y = \alpha*A*x +
              \beta*y@f$.
            - If <tt>trans ='T'</tt>, the operation is @f$y = \alpha*A^T*x +
              \beta*y@f$.


 @param[in] alpha  the scalar @f$\alpha@f$.

 @param[in] A a two-dimensional @c m by @c n matrix.

 @param[in] X a one-dimensional array containing the vector @f$x@f$.  When @c
            trans is equal to @c 'N' its length is @c n otherwise its length
            is @c m.

 @param[in] beta  the scalar @f$\beta@f$.

 @param[in, out] Y On entry, a one-dimensional array containing the vector
            @f$y@f$.  When @c trans is equal to @c 'N' its length is @c m
            otherwise its length is @n. If @f$\beta=0@f$, @c Y need not be
            set.  On exit, @c Y is overwritten by the updated vector @f$y@f$.

 @return GBLA_SUCCESS
 */
GBLA_API int  gbla_blas_dgemv (char trans,
			       double alpha,
			       const gbla_matrix_t * A,
			       const gbla_vector_t * X,
			       double beta,
			       gbla_vector_t * Y);

/** Rank-one update of a general matrix: @f$A=\alpha*x*y^T+A@f$

 This function perform a rank-one update of a real general matrix:
 @f$A=\alpha*x*y^T+A@f$.

 @f$\alpha@f$ is a scalar, @f$x@f$ is an m-element vector, @f$y@f$ is an
 n-element vector, and @f$A@f$ is an @f$m@f$ by @f$n@f$ matrix.

 @param[in] alpha the scalar @f$\alpha@f$.
 
 @param[in] X a one-dimensional array of length @f$m@f$. The argument X
            contains the vector @f$x@f$.
 
 @param[in] Y a one-dimensional array of length @f$n@f$. The argument Y
            contains the vector @f$y@f$.
            
 @param[in,out] A On entry, an @f$m@f$ by @f$n@f$ general matrix.  On exit, a
                is overwritten by the updated matrix.

 @return GBLA_SUCCESS
*/
GBLA_API int  gbla_blas_dger (double alpha,
			      const gbla_vector_t * X,
			      const gbla_vector_t * Y,
			      gbla_matrix_t * A);

/** Matrix-vector product for a symmetric band matrix: @f$y = \alpha*A*x +
    \beta*y@f$.

 This function compute a matrix-vector product for a n by n real symmetric
 band matrix described by the following operation: @f$y = \alpha*A*x +
 \beta*y@f$.

 @f$\alpha@f$ and @f$\beta@f$ are scalars, and @f$x@f$ and @f$y@f$ are vectors
 with @f$n@f$ elements.
 
 @param[in] alpha the scalar @f$\alpha@f$.

 @param[in] A n by n real symmetric band matrix. The argument @c uplo
            specifies which portion of the matrix is stored.

 @param[in] X a one-dimensional array @c X of length @c n.  Argument @c X
            contains the vector @f$x@f$.

 @param[in] beta the scalar @f$\beta@f$.

 @param[in,out] Y On entry, a one-dimensional array @c Y of length @c n. If
                 @f$\beta=0@f$, @c Y need not be set. On exit, @c Y is
                 overwritten by the updated vector @f$y@f$.

 @return GBLA_SUCCESS
 */
GBLA_API int gbla_blas_dsbmv (double alpha,
			      const gbla_band_matrix_t * A,
			      const gbla_vector_t * X,
			      double beta,
			      gbla_vector_t * Y);

/** Matrix-vector product for a symmetric matrix stored in packed form: @f$y = \alpha*A*x + \beta*y@f$.

 Compute a matrix-vector product for a real symmetric matrix stored in packed
 form as described by the following operation: @f$y = \alpha*A*x + \beta*y@f$.
 
 @f$\alpha@f$ and @f$\beta@f$ are scalars, and @f$x@f$ and @f$y@f$ are vectors
 with @f$n@f$ elements. @f$A@f$ is an @f$n@f$ by @f$n@f$ symmetric matrix.

 @param[in] alpha the scalar @f$\alpha@f$.

 @param[in] A an n by n symmetric packed matrix.

 @param[in] X a one-dimensional array @c X of length @c n.  Argument @c X
            contains the vector @f$x@f$.

 @param[in] beta the scalar @f$\beta@f$.

 @param[in,out] Y On entry, a one-dimensional array @c Y of length @c n. If
                 @f$\beta=0@f$, @c Y need not be set. On exit, @c Y is
                 overwritten by the updated vector @f$y@f$.

 @return GBLA_SUCCESS
 */
GBLA_API int gbla_blas_dspmv (double alpha,
			      const gbla_packed_matrix_t * A,
			      const gbla_vector_t * X,
			      double beta,
			      gbla_vector_t * Y);

/** Rank-one update of a symmetric matrix stored in packed form: @f$A = \alpha*x*x^T + A@f$.

 Perform the rank-one update of a real symmetric matrix stored in packed form:
 @f$A = \alpha*x*x^T + A@f$.

 @f$\alpha@f$ is a scalar, @f$x@f$ is vector with @f$n@f$ elements, and
 @f$A@f$ is an @f$n@f$ by @f$n@f$ matrix in packed form.

 @param[in] alpha the scalar @f$\alpha@f$.

 @param[in] X a one-dimensional array @c X of length @c n.  Argument @c X
            contains the vector @f$x@f$.

 @param[in,out] A an n by n symmetric packed matrix.
 @return GBLA_SUCCESS.
*/
GBLA_API int gbla_blas_dspr (double alpha,
			     const gbla_vector_t * X,
			     gbla_packed_matrix_t * A);

/** Rank-two update of a symmetric matrix stored in packed form: @f$A = \alpha*x*y^T + \alpha*y*x^T + A@f$.

 Perform the rank-two update of a real symmetric matrix stored in packed form:
 @f$A = \alpha*x*y^T + \alpha*y*x^T + A@f$.

 @f$\alpha@f$ is a scalar, @f$x@f$ is vector with @f$n@f$ elements, and
 @f$A@f$ is an @f$n@f$ by @f$n@f$ symmetric matrix in packed form.

 @param[in] alpha the scalar @f$\alpha@f$.

 @param[in] X a one-dimensional array @c X of length @c n.  Argument @c X
            contains the vector @f$x@f$.

 @param[in] Y a one-dimensional array @c Y of length @c n.  Argument @c Y
            contains the vector @f$y@f$.

 @param[in,out] A an n by n symmetric packed matrix. On exit, @c A is
                overwritten by the specified part of the updated matrix.

 @return GBLA_SUCCESS
*/
GBLA_API int gbla_blas_dspr2 (double alpha,
			      const gbla_vector_t * X,
			      const gbla_vector_t * Y,
			      gbla_packed_matrix_t * A);

/** Rank-one update of a symmetric matrix: @f$A=\alpha*x*x^T+A@f$

 Perform the rank-one update of a real symmetric matrix:
 @f$A=\alpha*x*x^T+A@f$.

 @f$\alpha@f$ is a scalar, @f$x@f$ is vector with @f$n@f$ elements, and
 @f$A@f$ is an @f$n@f$ by @f$n@f$ symmetric matrix.


 @param[in] uplo specifies whether the upper or lower triangular part of the
                 matrix @c A is referenced:
                 - If <tt>uplo = 'U'</tt>, the upper-triangular part of @c A
                   is referenced.
                 - If <tt>uplo = 'L'</tt>, the lower-triangular part of @c A
                   is referenced.

 @param[in] alpha  the scalar @f$\alpha@f$.

 @param[in] X a one-dimensional array @c X. Array @c X contains the vector
            @f$x@f$.
            
 @param[in,out] A is a square symmetric @f$n@f$ by @f$n@f$ matrix. Argument @c
            uplo specifies which part of the matrix is used in the
            computation. On exit, @c A is overwritten; the specified part of
            the matrix @c A is overwritten by the corresponding part of the
            updated matrix.
  
 @return GBLA_SUCCESS.
 */
GBLA_API int  gbla_blas_dsyr (char uplo,
			      double alpha,
			      const gbla_vector_t * X,
			      gbla_matrix_t * A);

/** Rank-two update of a symmetric matrix: @f$A=\alpha*x*y^T+\alpha*y*x^T+A@f$.

 Perform the rank-two update of a real symmetric matrix:
 @f$A=\alpha*x*y^T+\alpha*y*x^T+A@f$.

 @f$\alpha@f$ is a scalar, @f$x@f$ and @f$y@f$ are vectors with @f$n@f$
 elements, and @f$A@f$ is an @f$n@f$ by @f$n@f$ symmetric matrix.

 @param[in] uplo specifies whether the upper or lower triangular part of the
                 matrix @c A is referenced:
                 - If <tt>uplo = 'U'</tt>, the upper-triangular part of @c A
                   is referenced.
                 - If <tt>uplo = 'L'</tt>, the lower-triangular part of @c A
                   is referenced.

 @param[in] alpha  the scalar @f$\alpha@f$.

 @param[in] X a one-dimensional array @c X of length @c n. Array @c X contains
            the vector @f$x@f$.

 @param[in] Y a one-dimensional array @c Y of length @c n. Array @c Y contains
            the vector @f$y@f$.

 @param[in,out] A is a square symmetric @f$n@f$ by @f$n@f$ matrix. Argument @c
            uplo specifies which part of the matrix is used in the
            computation. On exit, @c A is overwritten; the specified part of
            the matrix @c A is overwritten by the corresponding part of the
            updated matrix.

 @return GBLA_SUCCESS.
 */
GBLA_API int  gbla_blas_dsyr2 (char uplo,
			       double alpha,
			       const gbla_vector_t * X,
			       const gbla_vector_t * Y,
			       gbla_matrix_t * A);

/** Matrix-vector product for a triangular band matrix: @f$x=op(A)*x@f$

 Compute a matrix-vector product for a triangular band matrix or its
 transpose:
 @f{align*}
 x = & A*x \\
 \text{or} \\
 x = & A^T*x
 @f}

 @f$x@f$ is a vector with @f$n@f$ elements and @f$A@f$ is an @f$n@f$ by
 @f$n@f$ band matrix, with @f$k + 1@f$ diagonals. The band matrix is a unit or
 non-unit, upper- or lower- triangular matrix.

 @param[in] trans a character that specifies the operation to be performed:
            - If <tt>trans = 'N'</tt>, the operation is @f$x=A*x@f$. 
            - If <tt>trans = 'T'</tt>, the operation is @f$x=A^T*x@f$.
 
 @param[in] diag a character that specifies whether the matrix A is
            unit-triangular:
            - If <tt>diag = 'U'</tt>, A is a unit-triangular matrix.
            - If <tt>diag = 'N'</tt>, A is not a unit-triangular matrix.
            
 @param[in] A is a band matrix. If <tt>diag = 'U'</tt>, diagonal elements of
            the matrix are not referenced, but are assumed to be unity.

 @param[in,out] X On entry, a one-dimensional vector @c X of length @f$n@f$
                  Argument @c X contains the vector @f$x@f$. On exit, @c X is
                  overwritten with the transformed vector @f$x@f$.

 @return GBLA_SUCCESS
 */
GBLA_API int gbla_blas_dtbmv (char trans, char diag,
			      const gbla_band_matrix_t * A,
			      gbla_vector_t * X);


/** Solver of a system of linear equations with a triangular band matrix: @f$op(A)*x=b@f$.

 This function solve one of the following systems of linear equations for
 @f$x@f$:
 
 @f{align*}
 A*x =& b \\
 \text{or} \\
 A^T*x =& b
 @f}

 @f$b@f$ and @f$x@f$ are vectors with @f$n@f$ elements and @f$A@f$ is an
 @f$n@f$ by @f$n@f$ band matrix with @f$k+1@f$ diagonals. The matrix is a unit
 or non-unit, upper- or lower- triangular band matrix.

 The function does not perform checks for singularity or near singularity of
 the triangular matrix. The requirements for such a test depend on the
 application. If necessary, perform the test in your application program
 before calling the function.

 @param[in] trans a character that specifies the operation to be performed:
            - If <tt>trans = 'N'</tt>, the system is @f$A*x=b@f$. 
            - If <tt>trans = 'T'</tt>, the system is @f$A^T*x=b@f$.
            
 @param[in] diag a character that specifies whether the matrix A is
            unit-triangular:
            - If <tt>diag = 'U'</tt>, A is a unit-triangular matrix.
            - If <tt>diag = 'N'</tt>, A is not a unit-triangular matrix.
            
 @param[in] A is a band matrix. If <tt>diag = 'U'</tt>, diagonal elements of
            the matrix are not referenced, but are assumed to be unity.

 @param[in,out] X On entry, a one-dimensional vector @c X of length @c n.  The
                vector @c X contains the vector @f$b@f$. On exit, @c X is
                overwritten with the solution vector @f$x@f$.

 @return GBLA_SUCCESS
 */

GBLA_API int gbla_blas_dtbsv (char trans, char diag,
			      const gbla_band_matrix_t * A,
			      gbla_vector_t * X);

/** Matrix-vector product for a triangular matrix in packed form: @f$x=op(A)*x@f$.

 This function compute a matrix-vector product for a triangular
 matrix stored in packed form or its transpose:
 
 @f{align*}
 x = & A*x \\
 \text{or} \\
 x = & A^T*x
 @f}
 
 @f$x@f$ is a vector with @f$n@f$ elements and @f$A@f$ is an @f$n@f$ by
 @f$n@f$, unit or non-unit, upper- or lower-triangular matrix in packed form.

 @param[in] trans a character that specifies the operation to be performed:
            - If <tt>trans = 'N'</tt>, the operation is @f$x=A*x@f$. 
            - If <tt>trans = 'T'</tt>, the operation is @f$x=A^T*x@f$.


 @param[in] diag a character that specifies whether the matrix A is
            unit-triangular:
            - If <tt>diag = 'U'</tt>, A is a unit-triangular matrix.
            - If <tt>diag = 'N'</tt>, A is not a unit-triangular matrix.

 @param[in] A is a packed triangular matrix. If <tt>diag = 'U'</tt>, diagonal
            elements of the matrix are not referenced, but are assumed to be
            unity.

 @param[in,out] X On entry, a one-dimensional vector @c X of length @f$n@f$
                  Argument @c X contains the vector @f$x@f$. On exit, @c X is
                  overwritten with the transformed vector @f$x@f$.
 @return GBLA_SUCCESS.
 */
GBLA_API int gbla_blas_dtpmv (char trans, char diag,
			      const gbla_packed_matrix_t * A,
			      gbla_vector_t * X);

/** Solve a system of linear equations with a triangular matrix in packed form: @f$op(A)*x=b@f$.

 This function solve one of the following systems of linear equations for
 @f$x@f$:

 @f{align*}
 A*x =& b \\
 \text{or} \\
 A^T*x =& b
 @f}

 @f$b@f$ and @f$x@f$ are vectors with @f$n@f$ elements and @f$A@f$ is an n by
 n, unit or non-unit, upper- or lower-triangular matrix in packed form.

 The function does not perform checks for singularity or near singularity of
 the triangular matrix.  The requirements for such a test depend on the
 application.  If necessary, perform the test in your application pro- gram
 before calling this function.

 @param[in] trans a character that specifies the operation to be performed:
            - If <tt>trans = 'N'</tt>, the operation is @f$x=A*x@f$. 
            - If <tt>trans = 'T'</tt>, the operation is @f$x=A^T*x@f$.


 @param[in] diag a character that specifies whether the matrix A is
            unit-triangular:
            - If <tt>diag = 'U'</tt>, A is a unit-triangular matrix.
            - If <tt>diag = 'N'</tt>, A is not a unit-triangular matrix.

 @param[in] A is a packed triangular matrix. If <tt>diag = 'U'</tt>, diagonal
            elements of the matrix are not referenced, but are assumed to be
            unity.

 @param[in,out] X On entry, a one-dimensional vector @c X of length @c n.
                  Argument @c X contains the vector @f$b@f$.  On exit, @c X is
                  overwritten with the solution vector @f$x@f$.
 @return GBLA_SUCCESS.
 */
GBLA_API int gbla_blas_dtpsv (char trans, char diag,
			      const gbla_packed_matrix_t * A,
			      gbla_vector_t * X);

/** Matrix-vector product for a triangular matrix: @f$x=op(A)*x@f$.

 This function compute a matrix-vector product for a triangular
 matrix or its transpose: 

 @f{align*}
 x = & A*x \\
 \text{or} \\
 x = & A^T*x
 @f}
 
 @f$x@f$ is a vector with @f$n@f$ elements, and @f$A@f$ is an @f$n@f$ by
 @f$n@f$, unit or non-unit, upper- or lower-triangular matrix.

 @param[in] uplo a character that specifies whether the matrix @c A is an
             upper- or lower-triangular matrix:
             - If <tt>uplo =  'U'</tt>, @c A is an upper-triangular matrix.
             - If <tt>uplo =  'L'</tt>, @c A is an lower-triangular matrix.

 @param[in] trans a character that specifies the operation to be performed:
            - If <tt>trans = 'N'</tt>, the operation is @f$x=A*x@f$. 
            - If <tt>trans = 'T'</tt>, the operation is @f$x=A^T*x@f$.

  @param[in] diag a character that specifies whether the matrix A is
            unit-triangular:
            - If <tt>diag = 'U'</tt>, A is a unit-triangular matrix.
            - If <tt>diag = 'N'</tt>, A is not a unit-triangular matrix.

  @param[in] A a @c n by @c n general matrix. When <tt>uplo = 'U'</tt>, the
             leading @c n by @c n upper-triangular part of the matrix @c A
             must contain the upper-triangular matrix and the strictly
             lower-triangular part of @c A is not referenced. When <tt>uplo =
             'L'</tt>, the leading @c n by @c n lower-triangular part of the
             matrix @c A must contain the lower-triangular matrix and the
             strictly upper-triangular part of @c A is not referenced.  Note
             that when <tt>diag = 'U'</tt>, the diagonal elements of @c A are
             not referenced either, but are assumed to be unity. 

 @param[in,out] X On entry, a one-dimensional vector @c X of length @f$n@f$
                  Argument @c X contains the vector @f$x@f$. On exit, @c X is
                  overwritten with the transformed vector @f$x@f$.

 @return GBLA_SUCCESS
 */
GBLA_API int  gbla_blas_dtrmv (char uplo, char trans, char diag,
			       const gbla_matrix_t * A,
			       gbla_vector_t * X);

/** Solver of a system of linear equations with a triangular matrix: @f$op(A)*x=b@f$.

 This function solve one of the following systems of linear equations for
  @f$x@f$:

 @f{align*}
 A*x =& b \\
 \text{or} \\
 A^T*x =& b
 @f}

  @f$b@f$ and @f$x@f$ are vectors with @f$n@f$ elements and @f$A@f$ is an
  @f$n@f$ by @f$n@f$, unit or non-unit, upper- or lower-triangular matrix.

  The function does not perform checks for singularity or near singularity of
  the triangular matrix.  The requirements for such a test depend on the
  application.  If necessary, perform the test in your application program
  before calling this function.

  @param[in] uplo a character that specifies whether the matrix @c A is an
             upper- or lower-triangular matrix:
             - If <tt>uplo =  'U'</tt>, @c A is an upper-triangular matrix.
             - If <tt>uplo =  'L'</tt>, @c A is an lower-triangular matrix.

  @param[in] trans a character that specifies the operation to be performed:
            - If <tt>trans = 'N'</tt>, the operation is @f$x=A*x@f$. 
            - If <tt>trans = 'T'</tt>, the operation is @f$x=A^T*x@f$.

  @param[in] diag a character that specifies whether the matrix A is
            unit-triangular:
            - If <tt>diag = 'U'</tt>, A is a unit-triangular matrix.
            - If <tt>diag = 'N'</tt>, A is not a unit-triangular matrix.

  @param[in] A a @c n by @c n general matrix. When <tt>uplo = 'U'</tt>, the
             leading @c n by @c n upper-triangular part of the matrix @c A
             must contain the upper-triangular matrix and the strictly
             lower-triangular part of @c A is not referenced. When <tt>uplo =
             'L'</tt>, the leading @c n by @c n lower-triangular part of the
             matrix @c A must contain the lower-triangular matrix and the
             strictly upper-triangular part of @c A is not referenced.  Note
             that when <tt>diag = 'U'</tt>, the diagonal elements of @c A are
             not referenced either, but are assumed to be unity. 

 @param[in,out] X On entry, a one-dimensional vector @c X of length @c n.  The
                vector @c X contains the vector @f$b@f$. On exit, @c X is
                overwritten with the solution vector @f$x@f$.

 @return GBLA_SUCCESS
 */
GBLA_API int  gbla_blas_dtrsv (char uplo, char trans, char diag,
			       const gbla_matrix_t * A,
			       gbla_vector_t * X);
__GBLA_END_DECLS

#endif
