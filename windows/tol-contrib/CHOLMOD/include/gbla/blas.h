/* blas.h : BLAS API
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

#ifndef __GBLA_BLAS_H__
#define __GBLA_BLAS_H__

/** @file blas.h

 @b BLAS Support

 The Basic Linear Algebra Subprograms (@b BLAS) define a set of fundamental
 operations on vectors and matrices which can be used to create optimized
 higher-level linear algebra functionality.

 
 There are three levels of BLAS operations,

 - <b>Level I</b>: Vector operations, e.g. @f$y = \alpha x + y@f$
 
 - <b>Level II</b>: Matrix-vector operations, e.g. @f$y = \alpha A x + \beta
   y@f$

 - <b>Level III</b>: Matrix-matrix operations, e.g. @f$C = \alpha A B + \beta
   C@f$


 Each routine has a name which specifies the operation, the type of matrices
 involved and their precisions. Some of the most common operations and their
 names are given below,

 - @b DOT : scalar product, @f$ x' y@f$ 
 - @b AXPY : vector sum, @f$ \alpha x + y @f$
 - @b MV : matrix-vector product, @f$ Ax @f$ 
 - @b SV : matrix-vector solve, @f$ A^{-1} x @f$ 
 - @b MM : matrix-matrix product, @f$ A B @f$ 
 - @b SM : matrix-matrix solve, @f$ A^{-1} B @f$ 

 The type of matrices are,

 - @b GE : general 
 - @b GB : general band 
 - @b SY :  symmetric 
 - @b SB :  symmetric band 
 - @b SP :  symmetric packed 
 - @b HE :  hermitian 
 - @b HB :  hermitian band 
 - @b HP :  hermitian packed 
 - @b TR :  triangular 
 - @b TB :  triangular band 
 - @b TP :  triangular packed 

 Each operation is defined for four precisions,

 - @b S : single real 
 - @b D : double real 
 - @b C : single complex 
 - @b Z : double complex 

 Thus, for example, the name @b SGEMM stands for "single-precision general
 matrix-matrix multiply" and @b ZGEMM stands for "double-precision complex
 matrix-matrix multiply".

 The library provides a low-level layer which is documented in blapi.h that
 match directly to the @b FORTRAN call and and a higher-level interface for
 operations on @b GBLA vectors and matrices. The level I, II and III are
 documented in blas1.h, blas2.h and blas3.h repectively.
 
 */

#include <gbla/blas1.h>
#include <gbla/blas2.h>
#include <gbla/blas3.h>

#endif
