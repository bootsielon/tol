/* tol_blas.h: API of TOL for using BLAS functions.
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

#ifndef TOL_BLAS_H
#define TOL_BLAS_H 1

#include <tol/tol_bcommon.h> 

BEGIN_DECLS
#include <cblas.h>
END_DECLS

#include <tol/tol_bdat.h>
#include <tol/tol_matrix.h>

extern "C" {
  void cblas_xerbla(int p, const char *rout, const char *form, ...);
}

/////////////////////////////////////////////////////////////////////////////
namespace TolBlas {
// BMatrix ports to C versions of BLAS fortran functions
/////////////////////////////////////////////////////////////////////////////

//Multiplies two generic matrices
int dgemm(const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_TRANSPOSE TransB,
          const double alpha,
          const BMatrix<double>& A,
          const BMatrix<double>& B,
          const double beta,
          BMatrix<double>& C);
//Multiplies two generic matrices
int dgemm(const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_TRANSPOSE TransB,
          const BDat alpha,
          const BMatrix<BDat>& A,
          const BMatrix<BDat>& B,
          const BDat beta,
          BMatrix<BDat>& C);

//Multiplies a generic matrix by its traspose or viceverse
int dsyrk(const enum CBLAS_UPLO Uplo,
          const enum CBLAS_TRANSPOSE Trans,
          const double alpha,
          const BMatrix<double>& A,
          const double beta,
          BMatrix<double>& C);

//Multiplies a generic matrix by its traspose or viceverse
int dsyrk(const enum CBLAS_UPLO Uplo,
          const enum CBLAS_TRANSPOSE Trans,
          const BDat alpha,
          const BMatrix<BDat>& A,
          const BDat beta,
          BMatrix<BDat>& C);

//Multiplies a triangular matrix A by a generic one B
int dtrmm(const enum CBLAS_SIDE Side, 
          const enum CBLAS_UPLO Uplo, 
          const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_DIAG Diag,
          const double alpha,
          const BMatrix<double>& A, 
          const BMatrix<double>& B,
                BMatrix<double>& X);
//Multiplies a triangular matrix A by a generic one B
int dtrmm(const enum CBLAS_SIDE Side, 
          const enum CBLAS_UPLO Uplo, 
          const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_DIAG Diag,
          const BDat alpha,
          const BMatrix<BDat>& A, 
          const BMatrix<BDat>& B,
                BMatrix<BDat>& X);

//Solves triangular lineal systems
int dtrsm(const enum CBLAS_SIDE Side, 
          const enum CBLAS_UPLO Uplo, 
          const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_DIAG Diag,
          const double alpha,
          const BMatrix<double>& A, 
          const BMatrix<double>& B,
                BMatrix<double>& X);

//Solves triangular lineal systems
int dtrsm(const enum CBLAS_SIDE Side, 
          const enum CBLAS_UPLO Uplo, 
          const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_DIAG Diag,
          const BDat alpha,
          const BMatrix<BDat>& A, 
          const BMatrix<BDat>& B,
                BMatrix<BDat>& X);

};

#endif	// TOL_BLAS_H
