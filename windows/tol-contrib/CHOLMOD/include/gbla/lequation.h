/* lequation.h
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

/** @file lequation.h 
  Linear Equations.
  The API for solving systems of linear equations and performing a number of
  related computational tasks.
  
  Routines are supported for systems of equations with the following types of
  matrices:

   - general
   - banded
   - symmetric or Hermitian positive-definite (both full and packed storage)
   - symmetric or Hermitian positive-definite banded
   - symmetric or Hermitian indefinite (both full and packed storage)
   - symmetric or Hermitian indefinite banded
   - triangular (both full and packed storage)
   - triangular banded
   - tridiagonal.

  For each of the above matrix types, the API includes routines for performing
  the following computations:

   - factoring the matrix (except for triangular matrices)
   - equilibrating the matrix
   - solving a system of linear equations
   - estimating the condition number of a matrix
   - refining the solution of linear equations and computing its error bounds
   - inverting the matrix.  
*/

#ifndef __GBLA_LEQUATION_H__
#define __GBLA_LEQUATION_H__

#include <gbla/matrix.h>

__GBLA_BEGIN_DECLS

/** LU Factorization of a general m by n matrix.

 The routine forms the LU factorization of a general m by n matrix A as @f[A =
 PLU@f] where P is a permutation matrix, L is lower triangular with unit
 diagonal elements (lower trapezoidal if m > n) and U is upper triangular
 (upper trapezoidal if m < n). Usually A is square (m = n), and both L and U
 are triangular. The routine uses partial pivoting, with row interchanges. It
 is based on the @b LAPACK routine @b @em dgetrf.

 @param[in,out] A the matrix to factorize. Overwritten by L and U. The unit
                diagonal elements of L are not stored.
                
 @param[out] P integer array, dimension (@f$min(M,N)@f$). The pivot indices;
             for @f$1\leq i\leq min(M,N)@f$, row i of the matrix was
             interchanged with row @f$P(i)@f$.
 
 @return
            - = 0:  successful exit.
            - < 0:  illegal arguments.
            - > 0:   if  INFO = i, U(i,i) is exactly zero. The factorization
            has been completed, but the factor U is exactly  singular,  and
            division  by zero will occur if it is used to solve a system of
            equations.
 */
GBLA_API int gbla_LU_matrix(gbla_matrix_t * A, int * P);

/* Routines for Solving Systems of Lunear Equations */

/** Solution of an LU-factored square matrix, with multiples right-hand sides.
  
 This function solve a system of linear equations @f$A * X = B@f$ or @f$A' * X
 = B@f$ with a general N-by-N matrix A using the LU factorization computed by
 gbla_LU_matrix. It is based on the @b LAPACK routine @b @em dgetrs.

 @param[in] trans Specifies the form of the system of equations:
 - = 'N':  @f$A * X = B@f$  (No transpose)
 - = 'T':  @f$A'* X = B@f$  (Transpose)
 @param[in] LU the factored matrix. Contains the factors L and U from the
            factorization @f$A = P*L*U@f$ as computed by gbla_LU_matrix.
            
 @param[in] P integer array, dimension(N). Is the pivot indices from
            gbla_LU_matrix; for @f$1\leq i\leq N@f$, row i of the matrix was
            interchanged with row P(i).

 @param[in,out] B B On entry, the N-by-M matrix of right hand side matrix B.
                On exit, if successful, the N-by-M solution matrix X.

  @return
            - = 0:  successful exit.
            - < 0:  illegal arguments. 
 */
GBLA_API int gbla_LU_matrix_solve(char trans, gbla_matrix_t * LU, int * P,
                         gbla_matrix_t * B);

/* Driver Routines */

/** Solution to the system of linear equations.
  
  Computes the solution to the system of linear equations with a square matrix
  A and multiple right-hand sides B.
 
  The LU decomposition with partial pivoting and row interchanges is used to
  factor A as A = P * L * U, where P is a permutation matrix, L is unit lower
  triangular, and U is upper triangular.  The factored form of A is then used
  to solve the system of equations A * X = B. It is based on the @b LAPACK
  routine @b @em dgesv.

  @param[in] LU N-by-N coefficient matrix
  @param[in] B N-by-M matrix of right hand sides.
  @param[out] X N-by-M solution matrix
  @return
            - = 0:  successful exit.
            - < 0:  illegal arguments.
            - > 0: i, U(i,i) is exactly zero.  The factorization has been
              completed, but the factor U is exactly singular, so the solution
              could not be computed.

 */
GBLA_API int gbla_solve_linear(gbla_matrix_t *LU, gbla_matrix_t *B, gbla_matrix_t *X);

/** Inplace solution to the system of linear equations.
  
  Computes the solution to the system of linear equations with a square matrix
  A and multiple right-hand sides B.
 
  The LU decomposition with partial pivoting and row interchanges is used to
  factor A as
               A = P * L * U,
  where P is a permutation matrix, L is unit lower triangular, and U is upper
  triangular.  The factored form of A is then used to solve the system of
  equations A * X = B.

  On exit, A contains the factors L and U from the factorization A = P*L*U;
  the unit diagonal elements of L are not stored.  It is based on the @b
  LAPACK routine @b @em dgesv.

  @param[in,out] A On entry, the N-by-N coefficient matrix A.  On exit, the
                 factors L and U from the factorization A = P*L*U; the unit
                 diagonal elements of L are not stored.

  @param[in,out] B On entry, the N-by-M matrix of right hand side matrix B.
                 On exit, if successful, the N-by-M solution matrix X.

  @param[out] P int array of dimension (N)is the pivot indices that define the
              permutation matrix P; row i of the matrix was interchanged with
              row P[i].

  @return
            - = 0:  successful exit.
            - < 0:  illegal arguments.
            - > 0: i, U(i,i) is exactly zero.  The factorization has been
              completed, but the factor U is exactly singular, so the solution
              could not be computed.
 */

GBLA_API int gbla_solve_linear_inplace(gbla_matrix_t *A, gbla_matrix_t *B, int *P);

__GBLA_END_DECLS

#endif
