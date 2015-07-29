/* tol_lapack.cpp: API of TOL for using LAPACK functions.
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
#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_lapack.h> 

BTraceInit("tol_lapack.cpp");

#define b2dMat(M) ((BMatrix<double>&)(M))
#define cb2dMat(M) ((const BMatrix<double>&)(M))

/////////////////////////////////////////////////////////////////////////////
namespace TolLapack {
// BMatrix ports to C versions of LAPACK fortran functions when available or 
// directly to fortran ones in other case.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
static void replicateTriang(BMatrix<double>& X, 
                            const enum CBLAS_UPLO Uplo,
                            bool symm)
/////////////////////////////////////////////////////////////////////////////
{
  int i, j, n=X.Rows();
  if(Uplo==CblasLower)
  {
    for(i=0; i<n; i++) for(j=i+1; j<n; j++) X(i,j)=symm?X(j,i):0;
  }
  else //if(Uplo==CblasUpper)
  {
    for(i=0; i<n; i++) for(j=0; j<i; j++) X(i,j)=symm?X(j,i):0;
  }  
};

/////////////////////////////////////////////////////////////////////////////
int dpotrf(const enum CBLAS_UPLO Uplo, 
           const BMatrix<double>& S,
           BMatrix<double>& X)
// If Uplo==CblasLower does lower triangular Choleski decomposition
// S = X*X'; 
// If Uplo==CblasUpper does upper triangular Choleski decomposition
// S = X'*X;
//http://www.netlib.org/lapack/double/dpotrf.f
/////////////////////////////////////////////////////////////////////////////
{
  X = S; 
  if(!X.GetData().Size()) { return(0); } 
  int n = S.Columns();
  double* X_ = X.GetData().GetBuffer();
  int res = clapack_dpotrf( CblasRowMajor, LAPACK_UPLO(Uplo), n, X_, n );
  replicateTriang(X,Uplo,false);
  return(res);
};

/////////////////////////////////////////////////////////////////////////////
int dpotrf(const enum CBLAS_UPLO Uplo, 
           const BMatrix<BDat>& S,
           BMatrix<BDat>& X)
// If Uplo==CblasLower does lower triangular Choleski decomposition
// S = X*X'; 
// If Uplo==CblasUpper does upper triangular Choleski decomposition
// S = X'*X;
//http://www.netlib.org/lapack/double/dpotrf.f
/////////////////////////////////////////////////////////////////////////////
{
  return(dpotrf(Uplo, cb2dMat(S), b2dMat(X)));
};


/////////////////////////////////////////////////////////////////////////////
int dpotri(const enum CBLAS_UPLO Uplo, 
           const BMatrix<double>& A,
           BMatrix<double>& X)
//Computes the inverse of a real symmetric positive definite given a Choleski 
//decomposition
// X = (A *A')^-1 If Uplo==CblasLower 
// X = (A'*A )^-1 If Uplo==CblasUpper 
//http://www.netlib.org/lapack/double/dpotri.f
/////////////////////////////////////////////////////////////////////////////
{
  X = A; 
  if(!X.GetData().Size()) { return(0); } 
  int n = A.Columns();
  double* X_ = X.GetData().GetBuffer();
  int res = clapack_dpotri( CblasRowMajor, LAPACK_UPLO(Uplo), n, X_, n );
  replicateTriang(X,Uplo,true);
  return(res);
};
/////////////////////////////////////////////////////////////////////////////
int dpotri(const enum CBLAS_UPLO Uplo, 
           const BMatrix<BDat>& A,
           BMatrix<BDat>& X)
//Computes the inverse of a real symmetric positive definite given a Choleski 
//decomposition
// X = (A *A')^-1 If Uplo==CblasLower 
// X = (A'*A )^-1 If Uplo==CblasUpper 
//http://www.netlib.org/lapack/double/dpotri.f
/////////////////////////////////////////////////////////////////////////////
{
  return(dpotri(Uplo, cb2dMat(A), b2dMat(X)));
};

};


