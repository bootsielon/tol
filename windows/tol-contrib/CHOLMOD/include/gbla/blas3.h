/* blas3.h : GBLA BLAS Level III API
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

#ifndef __GBLA_BLAS3_H__
#define __GBLA_BLAS3_H__

#include <gbla/matrix.h>

__GBLA_BEGIN_DECLS

/** @file blas3.h

 @b BLAS Level III API

 BLAS Level III include routines and functions, which perform matrix-matrix
 operations, e.g. @f$C = \alpha A B + \beta C@f$. The routines provided here
 are based on the vectors and matrix types. The corresponding low level
 routines are provided in blapi.h
*/

/** Matrix-matrix product and addition for a symmetric matrix: @f$C := \alpha *
  A*B + \beta * C@f$.

 These function compute a matrix-matrix product and addition for a real
 symmetric matrix:

 @f{align*}
 C :=& \alpha * A*B + \beta * C \\
 \text{or} \\
 C :=& \alpha * B*A + \beta * C
 @f}

 where @f$\alpha@f$ and @f$\beta@f$ are scalars, @f$A@f$ is the symmetric
 matrix, and @f$B@f$ and @f$C@f$ are @f$m@f$ by @f$n@f$ matrices.
  
 @param[in] side specifies whether the symmetric matrix @c A multiplies @c B
            on the left side or the right side:

            - If <tt>side = 'L'</tt>, the operation is @f$C := \alpha * A*B +
              \beta*C @f$. In this case @c A is a @c m by @c m symmetric
              matrix.

            - If <tt>side = 'R'</tt>, the operation is @f$C := \alpha * B*A +
              \beta *C @f$. In this case @c A is a @c n by @c n symmetric
              matrix.
 
 @param[in] uplo specifies whether the upper- or lower- triangular part of the
 symmetric matrix @c A is referenced:
 
            - If <tt>uplo = 'U'</tt>, the upper-triangular part of @c A is
              referenced.
            
            - If <tt>uplo = 'L'</tt>, the lower-triangular part of @c A is
              referenced.                     
 @param[in] alpha the scalar @f$ \alpha @f$.
 
 @param[in] A is an @c m by @c m (<tt>side='L'</tt>) or @c n by @c n
              (<tt>side='L'</tt>) symmetric matrix.
 
 @param[in] B is an @c m by @c n matrix.
 @param[in] beta the scalar @f$ \beta @f$
 @param[in,out] C is a @c m by @c n matrix.
 @return GBLA_SUCCESS
 */

GBLA_API int  gbla_blas_dsymm (char side,
                      char uplo,
                      double alpha,
                      const gbla_matrix_t * A,
                      const gbla_matrix_t * B,
                      double beta,
                      gbla_matrix_t * C);

/** Matrix-matrix product and addition: @f$ C  := \alpha*op(A)*op(B) +
  \beta*C @f$

 This function perform one of the matrix-matrix operations:

 @f{align*}
 C  :=& \alpha*A*B + \beta*C \\
 \text{or} \\
 C  :=& \alpha*A^T*B^T + \beta*C
 @f}

 where @f$\alpha@f$ and @f$\beta@f$ are scalars, and @f$A@f$, @f$B@f$ and
 @f$C@f$, with @f$op(A)@f$ an @f$m@f$ by @f$k@f$, @f$op(B)@f$ an @f$k@f$ by
 @f$n@f$ matrix and @f$C@f$ an @f$m@f$ by @f$n@f$ matrix.
 
 @param[in] transa specifies the form of @f$ op(A) @f$ used in the matrix
 multiplication:
                - If <tt>transa = 'N'</tt>, @f$ op(A)=A @f$
                - If <tt>transa  = 'T'</tt>, @f$ op(A)=A^T @f$     
 @param[in] transb specifies the form of @f$ op(B) @f$ used in the matrix
 multiplication:
            - If <tt>transb  = 'N'</tt>, @f$ op(B)=B @f$
            - If <tt>transb  = 'T'</tt>, @f$ op(B)=B^T @f$
 @param[in] alpha the scalar @f$ \alpha @f$.
 @param[in] A @f$op(A)@f$ is an @c m by @c k matrix.
 @param[in] B @f$op(B)@f$ is an @c k by @c n matrix.
 @param [in] beta the scalar @f$ \beta @f$
 @param [in,out] C a @c m by @c n matrix.
 @return GBLA_SUCCESS
 */
GBLA_API int  gbla_blas_dgemm (char transa,
                      char transb,
                      double alpha,
                      const gbla_matrix_t * A,
                      const gbla_matrix_t * B,
                      double beta,
                      gbla_matrix_t * C);

#ifdef __USE_PRISM_STRASSEN__
/** Matrix-matrix strassen product and addition: @f$ C  := \alpha*op(A)*op(B) +
  \beta*C @f$

 This function perform one of the matrix-matrix operations:

 @f{align*}
 C  :=& \alpha*A*B + \beta*C \\
 \text{or} \\
 C  :=& \alpha*A^T*B^T + \beta*C
 @f}
 
 where @f$\alpha@f$ and @f$\beta@f$ are scalars, and @f$A@f$, @f$B@f$ and
 @f$C@f$, with @f$op(A)@f$ an @f$m@f$ by @f$k@f$, @f$op(B)@f$ an @f$k@f$ by
 @f$n@f$ matrix and @f$C@f$ an @f$m@f$ by @f$n@f$ matrix. The matrix product
 use a Winograd's variant of Strassen's algorithm for multiplying matrices of
 arbitrary size. The advantage of usign a Strassen-based algorithm is that,
 for square matrices, it has a scaling of @f$O(n^{lg(7)})@f$ instead of the
 conventional algorithm in ::gbla_blas_dgemm which scale as @f$O(n^3)@f$. \sa
 http://www.mcs.anl.gov/Projects/PRISM/lib/software.html
 
 @param[in] transa specifies the form of @f$ op(A) @f$ used in the matrix
 multiplication:
                - If <tt>transa = 'N'</tt>, @f$ op(A)=A @f$
                - If <tt>transa  = 'T'</tt>, @f$ op(A)=A^T @f$     
 @param[in] transb specifies the form of @f$ op(B) @f$ used in the matrix
 multiplication:
            - If <tt>transb  = 'N'</tt>, @f$ op(B)=B @f$
            - If <tt>transb  = 'T'</tt>, @f$ op(B)=B^T @f$
 @param[in] alpha the scalar @f$ \alpha @f$.
 @param[in] A @f$op(A)@f$ is an @c m by @c k matrix.
 @param[in] B @f$op(B)@f$ is an @c k by @c n matrix.
 @param [in] beta the scalar @f$ \beta @f$
 @param [in,out] C a @c m by @c n matrix.
 @return GBLA_SUCCESS
 */
GBLA_API int  gbla_blas_dgefmm (char transa,
                      char transb,
                      double alpha,
                      const gbla_matrix_t * A,
                      const gbla_matrix_t * B,
                      double beta,
                      gbla_matrix_t * C);
#endif

/** Perform a rank-2k update of a symmetric matrix.

 This function perform a rank-2k matrix-matrix operation using symmetric
 matrices. The operation is defined as:

 @f{align*}
 C := & \alpha * A*B^T + \alpha * B*A^T + \beta * C \\
 \text{or} \\
 C := & \alpha * A^T*B + \alpha * B^T*A + \beta * C
 @f}

 where @f$\alpha@f$ and @f$\beta@f$ are scalars, @f$C@f$ is an @f$n@f$ by
 @f$n@f$ symmetric matrix and @f$A@f$ and @f$B@f$ are @f$n@f$ by @f$k@f$
 matrices in the first case and @f$k@f$ by @f$n@f$ matrices in the second
 case.

 @param[in] uplo can take the values:
            - 'U' only the upper triangular part of C is to be referenced.
            - 'L' only the lower triangular part of C is to be referenced.
            
 @param[in] trans can take the values:
            - 'N' @f$ C := \alpha * A*B^T + \alpha * B*A^T + \beta * C @f$
            - 'T' @f$ C := \alpha * A^T*B + \alpha * B^T*A + \beta * C @f$
            
 @param[in] alpha the scalar @f$ \alpha @f$
 
 @param[in] A @c n by @c k matrix when <tt>trans='N'</tt> or @c k by @c n
              when <tt>trans='T'</tt>.
 
 @param[in] B @c n by @c k matrix when <tt>trans='N'</tt> or @c k by @c n
            when <tt>trans='T'</tt>.
 
 @param[in] beta the scalar @f$ \beta @f$
 
 @param[in,out] C is an @c n by @c n symmetric matrix
 @return GBLA_SUCCESS no argument value and dimension check is performed.
 */
GBLA_API int  gbla_blas_dsyr2k(char uplo,
                      char trans,
                      double alpha,
                      const  gbla_matrix_t * A,
                      const  gbla_matrix_t * B,
                      double beta,
                      gbla_matrix_t * C);

/** Perform a rank-n update of a symmetric matrix.

 This function perform a matrix-matrix operation using symmetric
 matrices. The operation is defined as:

 @f{align*}
 C := & \alpha * A*A' + \beta * C \\
 \text{or} \\
 C := & \alpha * A'*A + \beta * C
 @f}
 
 @param[in] uplo can take the values:
            - @c 'U' only the upper triangular part of @c C is to be
              referenced.
            
            - @c 'L' only the lower triangular part of @c C is to be
              referenced.
            
 @param[in] trans can take the values:
            - @c 'N' @f$ C := \alpha * A*A' + \beta * C @f$
            - @c 'T' @f$ C := \alpha * A'*A + \beta * C @f$
            
 @param[in] alpha the scalar @f$ \alpha @f$
 
 @param[in] A @c n by @c k matrix when <tt>trans='U'</tt> or @c k by @c n
 when <tt>trans='L'</tt>.

 @param[in] beta the scalar @f$ \beta @f$
 @param[in,out] C is an @c n by @c n symmetric matrix
  
 */
GBLA_API int gbla_blas_dsyrk (char uplo,
                     char trans,
                     double alpha,
                     const gbla_matrix_t * A,
                     double beta,
                     gbla_matrix_t * C);

/** Computes a scalar-matrix-matrix product (one matrix operand is
triangular).

 This function perform a matrix-matrix operation using triangular
 matrices. The operation is defined as:
 
 @f{align*}
 B :=& \alpha * op(A)*B \\
 \text{or} \\
 B :=& \alpha * B * op(A)
 @f}

 @param[in] side specify whether @f$op(A)@f$ multiplies @c B from the left or
            right in the operation as follows:
 
            - If <tt>side := 'L'</tt> @f$ B := \alpha * op(A)*B @f$
            - If <tt>side := 'R'</tt> @f$ B := \alpha * B * op(A) @f$

 @param[in] uplo specify whether the matrix @c A is an upper or lower
            triangular matrix as follows:
 
            - If <tt>uplo=='U'</tt> matrix @c A is an upper triangular matrix.
            - If <tt>uplo=='L'</tt> matrix @c A is a lower triangular matrix.

 @param[in] trans specify the form of @f$ op(A) @f$ to be used in the matrix
            multiplication as follows:
 
            - If <tt>trans=='N'</tt> @f$ op(A) = A @f$
            - If <tt>trans=='T'</tt> @f$ op(A) = A^T @f$

 @param[in] diag specify whether or not @c A is unit triangular as follows:
 
           - If <tt>diag=='U'</tt> matrix @c A is assumed to be unit
             triangular.
           
           - If <tt>diag=='N'</tt> matrix @c A is not assumed to be unit
             triangular.

 @param[in] alpha is the scalar @f$ \alpha @f$
 
 @param[in] A is a unit or non-unit, upper or lower triangular
            matrix. @f$op(A)@f$ is one of @f$ op(A) = A @f$ or @f$ op(A) = A'
            @f$. A is @c m by @c m if <tt>side='L'</tt> or @c n by @c n if
            <tt>side='R'</tt>.

 @param[in,out] B is a @c m by @c n matrix.
 @return GBLA_SUCCESS
 */
GBLA_API int  gbla_blas_dtrmm (char side,
                      char uplo, char trans,
                      char diag,
                      double alpha,
                      const gbla_matrix_t * A,
                      gbla_matrix_t * B);

/** Solves a matrix equation (one matrix operand is triangular).
 
 This function perform a matrix-matrix operation using triangular
 matrices. The operation is defined as:

 @f{align*}
 op(A)*X =& \alpha *B \\
 \text{or} \\
 X*op(A) =& \alpha *B
 @f}

 @param[in] side specify whether @f$op(A)@f$ multiplies @f$B@f$ from the left
            or right in the operation as follows:
            
            - If <tt>side = 'L'</tt> @f$ op(A)*X = \alpha *B @f$
            - If <tt>side = 'R'</tt> @f$ X*op(A) = \alpha *B @f$

 @param[in] uplo specify whether the matrix @c A is an upper or lower
            trinagular matrix as follows:
            
            - If <tt>uplo = 'U'</tt> matrix @c A is an upper triangular matrix.
            - If <tt>uplo = 'L'</tt> matrix @c A is a lower triangular matrix.

 @param[in] trans specify the form of @f$ op(A) @f$ to be used in the matrix
            multiplication as follows:
            
            - If <tt>trans = 'N'</tt> @f$ op(A) = A @f$
            - If <tt>trans = 'T'</tt> @f$ op(A) = A^T @f$

 @param[in] diag specify whether or not @c A is unit triangular as follows:
 
           - If <tt>diag = 'U'</tt> matrix A is assumed to be unit triangular.
           
           - If <tt>diag = 'N'</tt> matrix A is not assumed to be unit
             triangular.

 @param[in] alpha is the scalar @f$ \alpha @f$
 
 @param[in] A is a unit or non-unit, upper or lower triangular
            matrix. @f$op(A)@f$ is one of @f$ op(A) = A @f$ or @f$ op(A) = A^T
            @f$. A is @c m by @c m if <tt>side='L'</tt> or @c n by @c n if
            <tt>side='R'</tt>.

 @param[in,out] B is a @c m by @c n matrix. On exit it is overwritten by the
                solution matrix @c X.
                
 @return GBLA_SUCCESS
 */
GBLA_API int  gbla_blas_dtrsm (char side,
                      char uplo, char trans,
                      char diag,
                      double alpha,
                      const gbla_matrix_t * A,
                      gbla_matrix_t * B);

__GBLA_END_DECLS

#endif
