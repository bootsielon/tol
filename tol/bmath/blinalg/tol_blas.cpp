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

#include <stdarg.h>
#include <stdio.h>

#include <tol/tol_blas.h> 
#include <tol/tol_bout.h> 

BTraceInit("tol_blas.cpp");

#define b2dMat(M) ((BMatrix<double>&)(M))
#define cb2dMat(M) ((const BMatrix<double>&)(M))

void cblas_xerbla(int p, const char *rout, const char *form, ...)
{
char buffer[1024];
va_list argptr;

va_start(argptr, form);
sprintf(buffer, "[XERBLA] Parameter %d to routine %s was incorrect\n", p, rout);
Error(buffer);
vsnprintf(buffer, 1023, form, argptr);
Error(buffer);
va_end(argptr);
}

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
  int Ar = A.Rows();
  int Br = B.Rows();
  int Cr = C.Rows();
  int Ac = A.Columns();
  int Bc = B.Columns();
  int Cc = C.Columns();
  if (TransA==CblasNoTrans) {
    M = Ar;
    K = Ac;
  } else {
    M = Ac;
    K = Ar;
  }
  N = TransB==CblasNoTrans ? Bc : Br;
  if(beta==0.0)
  {
    C.Alloc(M,N);
    C.SetAllValuesTo(0);
  }
  if(N&&K&&M)
  {
    const double* A_ = A.Data().Buffer();
    const double* B_ = B.Data().Buffer();
    double* C_ = C.GetData().GetBuffer();
    cblas_dgemm
    (
      CblasRowMajor, TransA, TransB,
      M, N, K,
      alpha,
      A_, Ac,
      B_, Bc,
      beta,
      C_, N
    );
  }
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
//http://www.netlib.org/blas/dgemm.f
/////////////////////////////////////////////////////////////////////////////
{
  if( (TransA==CblasNoTrans && TransB==CblasNoTrans && A.Columns()==B.Rows   ()) ||
      (TransA==CblasNoTrans && TransB==CblasTrans   && A.Columns()==B.Columns()) ||
      (TransA==CblasTrans   && TransB==CblasNoTrans && A.Rows   ()==B.Rows   ()) ||
      (TransA==CblasTrans   && TransB==CblasTrans   && A.Rows   ()==B.Columns()) )
  {
    return(dgemm(TransA, TransB, alpha.Value(), cb2dMat(A), b2dMat(B), 
                 beta.Value(),b2dMat(C)));
  }
  else
  {
    Error(BText("[dgemm] Cannot multiply matrices with dimensions ")+
    "("+(TransA==CblasNoTrans?A.Rows():A.Columns())+"x"+(TransA==CblasNoTrans?A.Columns():A.Rows())+") * "
    "("+(TransB==CblasNoTrans?B.Rows():B.Columns())+"x"+(TransB==CblasNoTrans?B.Columns():B.Rows())+")\n");
    return(-1);
  }
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
  int Ar = A.Rows();
  int Cr = C.Rows();
  int Ac = A.Columns();
  int Cc = C.Columns();
  if (Trans==CblasNoTrans) {
    N = Ar;
    K = Ac;
  } else {
    N = Ac;
    K = Ar;
  }
  if(beta==0.0)
  {
    Cc = N;
    C.Alloc(N,N);
    C.SetAllValuesTo(0);
  }
  if(N&&K)
  {
    cblas_dsyrk
    (
      CblasRowMajor, Uplo, Trans,
      N, K,
      alpha,
      A.Data().Buffer(), Ac,
      beta,
      C.GetData().GetBuffer(), Cc
    );
  }
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
  if(N&&M&&(
    (Side==CblasLeft  && TransA==CblasNoTrans && A.Columns()==M) ||
    (Side==CblasRight && TransA==CblasNoTrans && A.Rows   ()==N) ||
    (Side==CblasLeft  && TransA==CblasTrans   && A.Rows   ()==M) ||
    (Side==CblasRight && TransA==CblasTrans   && A.Columns()==N) ))
  {
    cblas_dtrmm(CblasRowMajor,Side,Uplo,TransA,Diag,M,N,alpha, 
                A.Data   ().Buffer   (),lda,
                X.GetData().GetBuffer(),ldb);
    return(0);
  }
  else
  {
    if(Side==CblasLeft)
    {
      Error(BText("[dtrmm] Cannot multiply matrices with dimensions ")+
      "("+(TransA==CblasNoTrans?A.Rows():A.Columns())+"x"+(TransA==CblasNoTrans?A.Columns():A.Rows())+") * "
      "("+B.Rows()+"x"+B.Columns()+")\n");
    }
    else
    {
      Error(BText("[dtrmm] Cannot multiply matrices with dimensions ")+
      "("+B.Rows()+"x"+B.Columns()+") * "
      "("+(TransA==CblasNoTrans?A.Rows():A.Columns())+"x"+(TransA==CblasNoTrans?A.Columns():A.Rows())+")\n");
    }
    return(-1);
  }
//VBR: Comprobar si hay error
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
  if(N&&M&&(
     //(Side==CblasLeft  && TransA==CblasNoTrans && A.Columns()==M) ||
     (Side==CblasLeft  && TransA==CblasNoTrans && A.Rows()==M) ||
     //(Side==CblasRight && TransA==CblasNoTrans && A.Rows   ()==N) ||
     (Side==CblasRight && TransA==CblasNoTrans && A.Columns   ()==N) ||
     //(Side==CblasLeft  && TransA==CblasTrans   && A.Rows   ()==M) ||
     (Side==CblasLeft  && TransA==CblasTrans   && A.Columns   ()==M) ||
     //(Side==CblasRight && TransA==CblasTrans   && A.Columns()==N) ))
     (Side==CblasRight && TransA==CblasTrans   && A.Rows()==N) ))
  {
    cblas_dtrsm(CblasRowMajor,Side,Uplo,TransA,Diag,M,N,alpha, 
                A.Data   ().Buffer   (),lda,
                X.GetData().GetBuffer(),ldb);
    return(0);
  }
  else
  {
    if(Side==CblasLeft)
    {
      Error(BText("[dtrmm] Cannot multiply matrices with dimensions ")+
      "("+(TransA==CblasNoTrans?A.Rows():A.Columns())+"x"+(TransA==CblasNoTrans?A.Columns():A.Rows())+") * "
      "("+B.Rows()+"x"+B.Columns()+")\n");
    }
    else
    {
      Error(BText("[dtrmm] Cannot multiply matrices with dimensions ")+
      "("+B.Rows()+"x"+B.Columns()+") * "
      "("+(TransA==CblasNoTrans?A.Rows():A.Columns())+"x"+(TransA==CblasNoTrans?A.Columns():A.Rows())+")\n");
    }
    return(-1);
  }
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


