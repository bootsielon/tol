/* qr_factor.h
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

/** @file qr_factor.h
  Orthogonal factorizations.
    
  The API for performing the @f$QR@f$ and @f$LQ@f$ factorization of
  matrices.

  <b>QR Factorization.</b> Assume that @f$A@f$ is an @f$m@f$ by @f$n@f$ matrix
  to be factored. If @f$m\geq n@f$, the @f$QR@f$ factorization is given by

  @f{equation*}
  A = Q \begin{pmatrix} R \\ 0 \end{pmatrix}
    = \begin{pmatrix} Q_1 & Q_2 \end{pmatrix}\begin{pmatrix} R \\ 0 \end{pmatrix}
  @f}
  
  where @f$R@f$ is an @f$n@f$ by @f$n@f$ upper triangular matrix with real
  diagonal elements, and @f$Q@f$ is an @f$m@f$ by @f$m@f$ orthogonal (or
  unitary) matrix.

  You can use the @f$QR@f$ factorization for solving the following
  least-squares problem: @f$min\| Ax-b\|_2@f$ where @f$A@f$ is a full-rank
  @f$m@f$ by @f$n@f$ matrix (@f$m\geq n@f$). After factoring the matrix,
  compute the solution @f$x@f$ by solving @f$Rx=Q_1^Tb@f$.

  If @f$m<n@f$, the @f$QR@f$ factorization is given by

  @f{equation*}
  A = QR = Q\begin{pmatrix} R_1 & R_2 \end{pmatrix}
  @f}

  where @f$R@f$ is trapezoidal, @f$R_1@f$ is upper triangular and @f$R_2@f$ is
  rectangular.

  The routines do not form the matrix @f$Q@f$ explicitly. Instead, @f$Q@f$ is
  represented as a product of @f$min(m,n)@f$ <em>elementary
  reflectors</em>. Routines are provided to work with @f$Q@f$ in this
  representation.

  <b>LQ Factorization.</b> LQ factorization of an @f$m@f$ by @f$n@f$ matrix
  @f$A@f$ is as follows. If @f$m\leq n@f$, 

   @f{equation*}
   A = \begin{pmatrix} L & 0 \end{pmatrix}Q
     = \begin{pmatrix} L & 0 \end{pmatrix}\begin{pmatrix} Q_1 \\ Q_2 \end{pmatrix}
     = L Q_1
   @f}

   where @f$L@f$ is an @f$m@f$ by @f$m@f$ lower triangular matrix with real
   diagonal elements, and @f$Q@f$ is an @f$n@f$ by @f$n@f$ orthogonal (or
   unitary) matrix.

   If @f$m> n@f$, the @f$LQ@f$ factorization is

   @f{equation*}
   A = LQ = \begin{pmatrix} L_1 \\ L_2 \end{pmatrix}Q
   @f}

   where @f$L_1@f$ is an @f$n@f$ by @f$n@f$ lower triangular matrix, @f$L_2@f$
   is rectangular, and @f$Q@f$ is an @f$n@f$ by @f$n@f$ orthogonal (or
   unitary) matrix.

   You can use the @f$LQ@f$ factorization to find the minimun-norm solution of
   and underdetermined system of linear equations @f$Ax=b@f$ where @f$A@f$ is
   an @f$m@f$ by @f$n@f$ matrix (@f$m<n@f$). After factoring the matrix,
   compute the solution @f$x@f$ as follows: solve @f$Ly=b@f$, and then compute
   @f$x=Q_1^Ty@f$.
 */

#ifndef __GBLA_QR_FACTOR_H__
#define __GBLA_QR_FACTOR_H__

#include <gbla/matrix.h>

__GBLA_BEGIN_DECLS

/** QR factor structure.
 */
typedef struct 
{
  /** QR packed form */
  gbla_matrix_t A;
  /** Additional information on matrix @f$Q@f$*/
  double *tau;
  /** Internal workspace */
  double *work;
  /** size of internal workspace */
  int lwork;
  /** internal flags */
  int flags;
} gbla_QR_factor_t;

/** QR factor initialization.
 */
GBLA_API gbla_QR_factor_t *gbla_QR_factor_init(gbla_QR_factor_t *QR,
					       gbla_matrix_t *A,
					       int override);

/** QR factor finalization.
 */
GBLA_API void gbla_QR_factor_free(gbla_QR_factor_t *QR);

/** Solving @f$QR*X=B@f$.
 */
GBLA_API int gbla_QR_solve(gbla_QR_factor_t *QR,
			   gbla_matrix_t *X, const gbla_matrix_t *B);

/** Solving @f$QR*X=B@f$ (inplace).
 */
GBLA_API int gbla_QR_solve_inplace(gbla_QR_factor_t *QR,
				   gbla_matrix_t *X);

/** Compute @f$Q@f$ factor.
 */
GBLA_API int gbla_QR_unpack_Q(gbla_QR_factor_t *QR,
			      gbla_matrix_t *Q);

/** Apply matrix @f$Q@f$.
 */
GBLA_API int gbla_QR_apply(gbla_QR_factor_t *QR, gbla_matrix_t *C,
			   char side, char trans);

__GBLA_END_DECLS

#endif
