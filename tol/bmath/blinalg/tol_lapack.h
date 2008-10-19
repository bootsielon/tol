/* tol_lapack.h: API of TOL for using LAPACK functions.
                  GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

#ifndef TOL_LAPACK_H
#define TOL_LAPACK_H 1

#include <tol/tol_blas.h>


/////////////////////////////////////////////////////////////////////////////
namespace TolLapack {
// BMatrix ports to C versions of LAPACK fortran functions when available or 
// directly to fortran ones in other case.
/////////////////////////////////////////////////////////////////////////////

//Does Choleski decomposition
int dpotrf(const enum CBLAS_UPLO Uplo, 
           const BMatrix<double>& S,
           BMatrix<double>& X);
//Does Choleski decomposition
int dpotrf(const enum CBLAS_UPLO Uplo, 
           const BMatrix<BDat>& S,
           BMatrix<BDat>& X);

//Computes the inverse of a real symmetric positive definite
int dpotri(const enum CBLAS_UPLO Uplo, 
           const BMatrix<double>& A,
           BMatrix<double>& X);
//Computes the inverse of a real symmetric positive definite
int dpotri(const enum CBLAS_UPLO Uplo, 
           const BMatrix<BDat>& A,
           BMatrix<BDat>& X);


};



#endif	// TOL_LAPACK_H
