/* tol_blas.cpp: API of TOL for using BLAS functions.
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

#include <tol/tol_blas.h> 

BTraceInit("tol_blas.cpp");

#define b2dMat(M) ((BMatrix<double>&)(M))
#define cb2dMat(M) ((const BMatrix<double>&)(M))

/////////////////////////////////////////////////////////////////////////////
namespace TolBlas {
// BMatrix ports to C versions of BLAS fortran functions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
int dgemm(const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_TRANSPOSE TransB,
          const double alpha,
          const BMatrix<double>& A,
          const BMatrix<double>& B,
          const double beta,
          BMatrix<double>& C)
//Multiplies two generic matrices
// C = alfa*A * B  + beta*C if (TransA=CblasNoTrans  & TransB=CblasNoTrans)
// C = alfa*A'* B  + beta*C if (TransA=CblasTrans    & TransB=CblasNoTrans)
// C = alfa*A * B' + beta*C if (TransA=CblasNoTrans  & TransB=CblasTrans  )
// C = alfa*A'* B' + beta*C if (TransA=CblasTrans    & TransB=CblasTrans  )
//http://www.netlib.org/blas/dgemm.f
/////////////////////////////////////////////////////////////////////////////
{
  int M,N,K;
  if (TransA==CblasNoTrans) {
    M = A.Rows();
    K = A.Columns();
  } else {
    M = A.Columns();
    K = A.Rows();
  }
  N = TransB==CblasNoTrans ? B.Columns() : B.Rows();
  C.Alloc(M,N);
  cblas_dgemm
  (
    CblasRowMajor, TransA, TransB,
    M, N, K,
    alpha,
    A.Data().Buffer(), A.Columns(),
    B.Data().Buffer(), B.Columns(),
    beta,
    C.GetData().GetBuffer(), C.Columns()
  );
//VBR: Comprobar si hay error
  return(0);
};

/////////////////////////////////////////////////////////////////////////////
int dgemm(const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_TRANSPOSE TransB,
          const BDat alpha,
          const BMatrix<BDat>& A,
          const BMatrix<BDat>& B,
          const BDat beta,
          BMatrix<BDat>& C)
//Multiplies two generic matrices
// C = A*B
//http://www.netlib.org/blas/dtrsm.f
/////////////////////////////////////////////////////////////////////////////
{
  return(dgemm(TransA, TransB, alpha.Value(), cb2dMat(A), b2dMat(B), 
               beta.Value(),b2dMat(C)));
};


/////////////////////////////////////////////////////////////////////////////
int dsyrk(const enum CBLAS_UPLO Uplo,
          const enum CBLAS_TRANSPOSE Trans,
          const double alpha,
          const BMatrix<double>& A,
          const double beta,
          BMatrix<double>& C)
//Multiplies a generic matrix by its traspose or viceverse
// C = alfa*A * A'  + beta*C if (Trans=CblasNoTrans)
// C = alfa*A'* A   + beta*C if (Trans=CblasTrans)
//http://www.netlib.org/blas/dsyrk.f
/////////////////////////////////////////////////////////////////////////////
{
  int N,K;
  if (Trans==CblasNoTrans) {
    N = A.Rows();
    K = A.Columns();
  } else {
    N = A.Columns();
    K = A.Rows();
  }
  C.Alloc(N,N);
  cblas_dsyrk
  (
    CblasRowMajor, Uplo, Trans,
    N, K,
    alpha,
    A.Data().Buffer(), A.Columns(),
    beta,
    C.GetData().GetBuffer(), C.Columns()
  );
//VBR: Comprobar si hay error
  return(0);
};

/////////////////////////////////////////////////////////////////////////////
int dsyrk(const enum CBLAS_UPLO Uplo,
          const enum CBLAS_TRANSPOSE Trans,
          const BDat alpha,
          const BMatrix<BDat>& A,
          const BDat beta,
          BMatrix<BDat>& C)
//Multiplies a generic matrix by its traspose or viceverse
// C = alfa*A * A'  + beta*C if (Trans=CblasNoTrans)
// C = alfa*A'* A   + beta*C if (Trans=CblasTrans)
//http://www.netlib.org/blas/dsyrk.f
/////////////////////////////////////////////////////////////////////////////
{
  return(dsyrk(Uplo,Trans,alpha.Value(),cb2dMat(A),beta.Value(),b2dMat(C)));
}

/////////////////////////////////////////////////////////////////////////////
int dtrmm(const enum CBLAS_SIDE Side, 
          const enum CBLAS_UPLO Uplo, 
          const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_DIAG Diag,
          const double alpha,
          const BMatrix<double>& A, 
          const BMatrix<double>& B,
                BMatrix<double>& X)
//Multiplies a triangular matrix A by a generic one B
// A * B = X  if (Side=CblasLeft  & Trans=CblasNoTrans)
// B * A = X  if (Side=CblasRight & Trans=CblasNoTrans)
// A'* B = X  if (Side=CblasLeft  & Trans=CblasTrans)
// B * A'= X  if (Side=CblasRight & Trans=CblasTrans)
//http://www.netlib.org/blas/dtrmm.f
/////////////////////////////////////////////////////////////////////////////
{
  X = B;
  int M   = X.Rows   ();
  int N   = X.Columns();
  int lda = (Side==CblasLeft)?M:N;
  int ldb = N;
  cblas_dtrmm(CblasRowMajor,Side,Uplo,TransA,Diag,M,N,alpha, 
              A.Data   ().Buffer   (),lda,
              X.GetData().GetBuffer(),ldb);
//VBR: Comprobar si hay error
  return(0);
};

/////////////////////////////////////////////////////////////////////////////
int dtrmm(const enum CBLAS_SIDE Side, 
          const enum CBLAS_UPLO Uplo, 
          const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_DIAG Diag,
          const BDat alpha,
          const BMatrix<BDat>& A, 
          const BMatrix<BDat>& B,
                BMatrix<BDat>& X)
//Multiplies a triangular matrix A by a generic one B
// A * B = X  if (Side=CblasLeft  & Trans=CblasNoTrans)
// B * A = X  if (Side=CblasRight & Trans=CblasNoTrans)
// A'* B = X  if (Side=CblasLeft  & Trans=CblasTrans)
// B * A'= X  if (Side=CblasRight & Trans=CblasTrans)
//http://www.netlib.org/blas/dtrmm.f
/////////////////////////////////////////////////////////////////////////////
{
  return(dtrmm(Side,Uplo,TransA,Diag,alpha.Value(),
               cb2dMat(A),cb2dMat(B),b2dMat(X)));
};

/////////////////////////////////////////////////////////////////////////////
int dtrsm(const enum CBLAS_SIDE Side, 
          const enum CBLAS_UPLO Uplo, 
          const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_DIAG Diag,
          const double alpha,
          const BMatrix<double>& A, 
          const BMatrix<double>& B,
                BMatrix<double>& X)
//Solves triangular lineal systems
// A * X = B  if (Side=CblasLeft  & Trans=CblasNoTrans)
// X * A = B  if (Side=CblasRight & Trans=CblasNoTrans)
// A'* X = B  if (Side=CblasLeft  & Trans=CblasTrans)
// X * A'= B  if (Side=CblasRight & Trans=CblasTrans)
//http://www.netlib.org/blas/dtrsm.f
/////////////////////////////////////////////////////////////////////////////
{
  X = B;
  int M   = X.Rows   ();
  int N   = X.Columns();
  int lda = (Side==CblasLeft)?M:N;
  int ldb = N;
  cblas_dtrsm(CblasRowMajor,Side,Uplo,TransA,Diag,M,N,alpha, 
              A.Data   ().Buffer   (),lda,
              X.GetData().GetBuffer(),ldb);
//VBR: Comprobar si hay error
  return(0);
};

/////////////////////////////////////////////////////////////////////////////
int dtrsm(const enum CBLAS_SIDE Side, 
          const enum CBLAS_UPLO Uplo, 
          const enum CBLAS_TRANSPOSE TransA,
          const enum CBLAS_DIAG Diag,
          const BDat alpha,
          const BMatrix<BDat>& A, 
          const BMatrix<BDat>& B,
                BMatrix<BDat>& X)
//Solves triangular lineal systems
// A * X = B  if (Side=CblasLeft  & Trans=CblasNoTrans)
// X * A = B  if (Side=CblasRight & Trans=CblasNoTrans)
// A'* X = B  if (Side=CblasLeft  & Trans=CblasTrans)
// X * A'= B  if (Side=CblasRight & Trans=CblasTrans)
//http://www.netlib.org/blas/dtrsm.f
/////////////////////////////////////////////////////////////////////////////
{
  return(dtrsm(Side,Uplo,TransA,Diag,alpha.Value(),
               cb2dMat(A),cb2dMat(B),b2dMat(X)));
};

};


