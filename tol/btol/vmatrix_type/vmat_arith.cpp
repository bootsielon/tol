/* vmat_arith.cpp: BVMat arithmetic methods
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bvmat_impl.h>
#include <tol/tol_bpolyn.h>
#include <tol/tol_bratio.h>

#if defined( HAVE_CBLAS_H )
BEGIN_DECLS
#include <cblas.h>
END_DECLS
#else
#include <gsl/gsl_cblas.h>
#endif

////////////////////////////////////////////////////////////////////////////////
//Matrix algebra operators
////////////////////////////////////////////////////////////////////////////////


double fRR_sign (double x) { return(::Sign (BDat(x)).Value()); }
double fRR_sinh (double x) { return(::SinH (BDat(x)).Value()); }
double fRR_cosh (double x) { return(::CosH (BDat(x)).Value()); }
double fRR_tanh (double x) { return(::TanH (BDat(x)).Value()); }
double fRR_asinh(double x) { return(::ASinH(BDat(x)).Value()); }
double fRR_acosh(double x) { return(::ACosH(BDat(x)).Value()); }
double fRR_atanh(double x) { return(::ATanH(BDat(x)).Value()); } 
double fR2R_drop(double a, double b) { return((fabs(a)<=b)?0.0:a); }
double fR2R_sum (double a, double b) { return(a+b); }
double fR2R_rest(double a, double b) { return(a-b); }
double fR2R_prod(double a, double b) { return(a*b); }
double fR2R_quot(double a, double b) { return(a/b); } 

DefineMonary (Abs,        fabs,      true,  "Abs");
DefineMonary (Round,      round,     true,  "Round");
DefineMonary (Floor,      floor,     true,  "Floor");
DefineMonary (Ceil,       ceil,      true,  "Ceil");
DefineMonary (Sqrt,       sqrt,      true,  "Sqrt");
DefineMonary (Exp,        exp,       false, "Exp");
DefineMonary (Log,        log,       false, "Log");
DefineMonary (Log10,      log10,     false, "Log10");
DefineMonary (Sin,        sin,       true,  "Sin");
DefineMonary (Cos,        cos,       false, "Cos");
DefineMonary (Tan,        tan,       true,  "Tan");
DefineMonary (ASin,       asin,      true,  "ASin");
DefineMonary (ACos,       acos,      false, "ACos");
DefineMonary (ATan,       atan,      true,  "ATan");
DefineMonary (Sign,       fRR_sign,  true,  "Sign");
DefineMonary (SinH,       fRR_sinh,  true,  "SinH");
DefineMonary (CosH,       fRR_cosh,  false, "CosH");
DefineMonary (TanH,       fRR_tanh,  true,  "TanH");
DefineMonary (ASinH,      fRR_asinh, true,  "ASinH");
DefineMonary (ACosH,      fRR_acosh, false, "ACosH");
DefineMonary (ATanH,      fRR_atanh, true,  "ATanH");
DefineBinaryR(Sum,        fR2R_sum,  true,  false, "+ Real");
DefineBinaryR(Rest,       fR2R_rest, true,  false, "- Real");
DefineBinaryR(Prod,       fR2R_prod, true,  true,  "* Real");
DefineBinaryR(Quot,       fR2R_quot, false, false, "/ Real");
DefineBinary (WeightProd, fR2R_prod, true,  true,  "$*");
DefineBinary (WeightQuot, fR2R_quot, false, false, "$/");


////////////////////////////////////////////////////////////////////////////////
  int BVMat::T(const BVMat& A, BVMat& B)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  if(!A.CheckDefined("Tra")) { return(-1); }
  B.Delete();
  int i,j,k,r,c,n;
  double *x, *y;
  
  switch(A.code_) {
  case(ESC_blasRdense  ) :
    r = A.Columns();
    c = A.Rows();
    B.code_  = ESC_blasRdense;
    B.s_.blasRdense_ = CholmodAllocate_dense
      (r, c, r, CHOLMOD_REAL, common_);
    x = (double*)(A.s_.blasRdense_->x);
    y = (double*)(B.s_.blasRdense_->x);
    for (j=k=0; j<c; j++) 
    {
      for (i=0; i<r; i++, k++) 
      {
        y[k] = x[i*c+j];
      }
    }
    break;
  case(ESC_chlmRsparse ) :
    B.code_  = ESC_chlmRsparse;
    B.s_.chlmRsparse_=
      cholmod_transpose(A.s_.chlmRsparse_,1,common_);
    break;
  case(ESC_chlmRtriplet) :
    assert(cholmod_check_triplet(A.s_.chlmRtriplet_,common_));
    c = A.Columns();
    r = A.Rows();
    n = A.s_.chlmRtriplet_->nzmax;
    B.code_  = ESC_chlmRtriplet;
    B.s_.chlmRtriplet_=CholmodAllocate_triplet(c,r,n,0,CHOLMOD_REAL,common_);
    memcpy(B.s_.chlmRtriplet_->i,A.s_.chlmRtriplet_->j, n*sizeof(int));
    memcpy(B.s_.chlmRtriplet_->j,A.s_.chlmRtriplet_->i, n*sizeof(int));
    memcpy(B.s_.chlmRtriplet_->x,A.s_.chlmRtriplet_->x, n*sizeof(double));
    B.s_.chlmRtriplet_->nnz = A.s_.chlmRtriplet_->nnz;
    assert(cholmod_check_triplet(B.s_.chlmRtriplet_,common_));
    break;
  default:
    err_invalid_subtype("T {Transpose} ",A);
    return(-1); }
  return(0);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::T() const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  T(*this, aux);
  return(aux);
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::Drop(double drop)
//Importing method
////////////////////////////////////////////////////////////////////////////////
{
  int r, c, k, result=0;
  double* x;
  switch(code_) {
  case(ESC_blasRdense  ) : 
  {
    r = s_.blasRdense_->nrow;
    c = s_.blasRdense_->ncol;
    x = (double*)(s_.blasRdense_->x);
    for (k=0; k<r*c; k++) 
    {
      if(fabs(*x)<=drop) { *x = 0.0; }
    }    
    break;
  }
  case(ESC_chlmRsparse ) : 
  {
    cholmod_drop(drop,s_.chlmRsparse_, common_);
    CholmodSort(s_.chlmRsparse_,   common_);
    break;
  }
  case(ESC_chlmRtriplet) : 
  {
    BVMat sprs;
    cRt2cRs(*this,sprs);
    cholmod_drop(drop,sprs.s_.chlmRsparse_, common_);
    CholmodSort(sprs.s_.chlmRsparse_,   common_);
    cRs2cRt(sprs,*this);
    break; 
  }
  default: 
    err_invalid_subtype("Drop",*this); 
    result = -1; }
  return(result);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::Drop(const BVMat& A, double drop)
////////////////////////////////////////////////////////////////////////////////
{
  BVMat v;
  v.Copy(A);
  v.Drop(drop);
  return(v);
}

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::operator+ (double b) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  Sum(*this, b, aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::operator- (double b) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  Rest(*this, b, aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::operator* (double b) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  Prod(*this, b, aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::operator/ (double b) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  Quot(*this, b, aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::pow_nz(const BVMat& A, double b, BVMat& C)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  int i, nzmax = 0;
  double* x = NULL;
  C.Copy(A);
  C.StoredData(x, nzmax);
  for(i=0; i<nzmax; i++) { x[i]=pow(x[i],b); }
  return(0);
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::Pow(const BVMat& A, double b, BVMat& C)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  if(!A.CheckDefined("Power")) { return(-1); }
  if(b<=0)
  {
    switch(A.code_) {
    case(ESC_blasRdense) : 
      pow_nz(A, b, C);  break;
    default:
      err_invalid_subtype("^ (Real<=0)",A); 
      return(-1); }  
  }
  else
  {
    switch(A.code_) {
    case(ESC_blasRdense) : 
    case(ESC_chlmRtriplet) : 
    case(ESC_chlmRsparse) : 
      pow_nz(A, b, C);  break;
    default:
      err_invalid_subtype("^ (Real>0)",A); 
      return(-2); }  
  }
  return(0);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::Pow (double b) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  Pow(*this, b, aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::operator^ (double b) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  Pow(*this, b, aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::sum(const BVMat& A_, double b, const BVMat& B_, BVMat& C,
                 const char* fName)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  int result = 0;
  C.Delete();
  if(!A_.CheckDefined(fName)) { return(-1); }
  if(!B_.CheckDefined(fName)) { return(-1); }
  double a_[2] = { 1.0, 1.0 };
  double b_[2] = { b,   b   };
  int r  = A_.Rows();
  int c  = A_.Columns();
  int rB = B_.Rows();
  int cB = B_.Columns();
  if((r!=rB)||(c!=cB))
  {
    err_invalid_dimensions(fName,A_,B_);
    result = -1;
  }
  BVMat* A__=(BVMat*)&A_, *B__=(BVMat*)&B_;
  BVMat &A = *A__,  &B = *B__;
  if(result==0)
  {
    if(convertIfNeeded_all2bRd(A_,B_,A__,B__,fName,false)==0) 
    { 
      convertIfNeeded_all2cRs(A_,B_,A__,B__,fName,true);
    }
    A = *A__; B = *B__;
    if(A.code_!=B.code_)
    {
      err_invalid_subtypes(fName,A,B);
      result = -2;
    }
  }
  if(result==0)
  {
    int k, n;
    const double* Bx;
          double* Cx;
    switch(A.code_) {
    case(ESC_blasRdense  ) :
      C.Copy(A);
      n = A.s_.blasRdense_->nzmax;
      Bx = (const double*)B.s_.blasRdense_->x;
      Cx = (      double*)C.s_.blasRdense_->x;
      for(k=0; k<n; k++)
      {
        Cx[k] += b*Bx[k];
      };
      break;
    case(ESC_chlmRsparse) :
      C.code_ = ESC_chlmRsparse;
      C.s_.chlmRsparse_= cholmod_add
      (
        A.s_.chlmRsparse_, B.s_.chlmRsparse_,
        a_, b_, 1, 1, common_
      );
      break;
    default:
      err_invalid_subtype(fName,A); 
      result=-3; }
  }
  if(A__!=&A_) { delete A__; }
  if(B__!=&B_) { delete B__; }
  return(result);
};


////////////////////////////////////////////////////////////////////////////////
int BVMat::Sum(const BVMat& A, const BVMat& B, BVMat& C)
////////////////////////////////////////////////////////////////////////////////
{
  return(sum(A,1.0,B,C,"+"));
}

////////////////////////////////////////////////////////////////////////////////
int BVMat::Rest(const BVMat& A, const BVMat& B, BVMat& C)
////////////////////////////////////////////////////////////////////////////////
{
  return(sum(A,-1.0,B,C,"-"));
}
  
////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::operator+ (const BVMat& B) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  sum(*this,1.0,B,aux,"+");
  return(aux);
};


////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::operator- (const BVMat& B) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  sum(*this,-1.0,B,aux,"-");
  return(aux);

};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::bRd_bRd_prod(const BVMat& A, const BVMat& B, BVMat& C)
////////////////////////////////////////////////////////////////////////////////
{
  int rA = A.Rows();
  int rB = B.Rows();
  int rC = A.Rows();
  int M  = rA;
  int N  = B.Columns();
  int K  = rB;
  C.code_ = ESC_blasRdense;
  C.s_.blasRdense_=CholmodAllocate_dense
  ( A.s_.blasRdense_->nrow, B.s_.blasRdense_->ncol, 
    A.s_.blasRdense_->nrow, CHOLMOD_REAL, common_);
  cblas_dgemm
  (
    CblasColMajor, CblasNoTrans, CblasNoTrans,
    M, N, K,
    1.0,
    (const double*)A.s_.blasRdense_->x, rA,
    (const double*)B.s_.blasRdense_->x, rB,
    0.0,
    (double*)C.s_.blasRdense_->x, rC
  );
  return(0);   
};
  
////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_prod(const BVMat& A, const BVMat& B, BVMat& C)
////////////////////////////////////////////////////////////////////////////////
{
  static double alpha_one [2] = { 1.0,  1.0 };
  static double beta_zero [2] = { 0.0,  0.0 };
  C.code_ = ESC_blasRdense;
  C.s_.blasRdense_=CholmodAllocate_dense
  ( A.s_.chlmRsparse_->nrow, B.s_.blasRdense_->ncol, 
    A.s_.chlmRsparse_->nrow, CHOLMOD_REAL, common_);
  cholmod_sdmult
  (
    A.s_.chlmRsparse_, 
    0,      
    alpha_one,   
    beta_zero,    
    B.s_.blasRdense_,   
    C.s_.blasRdense_,   
    common_
  );
  return(0);   
};
  
////////////////////////////////////////////////////////////////////////////////
  int BVMat::bRd_cRs_prod(const BVMat& A, const BVMat& B, BVMat& C)
////////////////////////////////////////////////////////////////////////////////
{
  static double alpha_one [2] = { 1.0,  1.0 };
  static double beta_zero [2] = { 0.0,  0.0 };
  C.code_ = ESC_blasRdense;
  BVMat Ct, At = A.T();
  Ct.code_ = ESC_blasRdense;
  Ct.s_.blasRdense_=CholmodAllocate_dense
  ( B.s_.chlmRsparse_->ncol, A.s_.blasRdense_->nrow, 
    B.s_.chlmRsparse_->ncol, CHOLMOD_REAL, common_);
  cholmod_sdmult
  (
    B.s_.chlmRsparse_, 
    1,      
    alpha_one,   
    beta_zero,    
    At.s_.blasRdense_,   
    Ct.s_.blasRdense_,   
    common_
  );
  C = Ct.T();
  return(0);   
};
  
////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_cRs_prod(const BVMat& A, const BVMat& B, BVMat& C)
////////////////////////////////////////////////////////////////////////////////
{
  C.Delete();
  C.code_ = ESC_chlmRsparse;
  C.s_.chlmRsparse_ = cholmod_ssmult
  (
    A.s_.chlmRsparse_, /* left matrix to multiply */
    B.s_.chlmRsparse_, /* right matrix to multiply */
    0,          /* requested stype of C */
    1,         /* TRUE: do numerical values, FALSE: pattern only */
    1,         /* if TRUE then return C with sorted columns */
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",C.s_.chlmRsparse_);
  return(0);
};
  
////////////////////////////////////////////////////////////////////////////////
int BVMat::Prod(const BVMat& A_, const BVMat& B_, BVMat& C)
////////////////////////////////////////////////////////////////////////////////
{
  if(!A_.CheckDefined("*")) { return(-1); }
  if(!B_.CheckDefined("*")) { return(-1); }
  BVMat* A__, *B__;
  convertIfNeeded_cRt2cRs(A_,B_,A__,B__,"*",true);
  BVMat &A = *A__,  &B = *B__;
  const StrProduct* product = FindProduct(A.code_, B.code_);
  int result = 0;
  if(product)
  {
    int c = A.Columns();
    if(c!=B.Rows())
    {
      err_invalid_dimensions("*",A,B);
      result = -1;
    }
    else
    {
      (*product->fun)(A, B, C);
    }
  }
  else
  {
    err_invalid_subtypes("*",A,B);
    result = -2;
  }
  if(A__!=&A_) { delete A__; }
  if(B__!=&B_) { delete B__; }
  return(result);  
}

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::operator* (const BVMat& B) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  Prod(*this, B, aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::KroneckerProd(const BVMat& A, const BVMat& B, BVMat& C)
////////////////////////////////////////////////////////////////////////////////
{
  static const char* fName = "KroneckerProd";
  if(!A.CheckDefined(fName)) { return(-1); }
  if(!B.CheckDefined(fName)) { return(-1); }
  int rA = A.Rows(); 
  int cA = A.Columns(); 
  int rB = B.Rows(); 
  int cB = B.Columns(); 
  int rC = rA*rB; 
  int cC = cA*cB;
  if((A.code_ == ESC_blasRdense) && (B.code_ == ESC_blasRdense))
  {
    int iC, jC, iA, jA, iB, jB; 
    C.BlasRDense(rC,cC);
    double* xA0 =(double*)A.s_.blasRdense_->x;
    double* xB0 =(double*)B.s_.blasRdense_->x;
    double* xC0 =(double*)C.s_.blasRdense_->x;
    double* xA = xA0;
    double* xB;
    double* xC;

    for(jA=0; jA<cA; jA++)
    {
      for(iA=0; iA<rA; iA++, xA++)
      {
        iC = iA*rB;
        for(jB=0; jB<cB; jB++)
        { 
          jC = jA*cB + jB;
          xC = xC0 + (jC*rC+iC);
          xB = xB0 + (jB*rB);
          for(iB=0; iB<rB; iB++, xC++, xB++)
          {
            (*xC) = (*xA)*(*xB);  
          }
        }
      }
    }
    return(0);
  } 
  if((A.code_ == ESC_chlmRtriplet) && (B.code_ == ESC_chlmRtriplet))
  {
    int nA = A.s_.chlmRtriplet_->nnz;
    int* iA = (int*)A.s_.chlmRtriplet_->i;
    int* jA = (int*)A.s_.chlmRtriplet_->j;
    double* xA = (double*)A.s_.chlmRtriplet_->x;
    int nB = B.s_.chlmRtriplet_->nnz;
    int* iB = (int*)B.s_.chlmRtriplet_->i;
    int* jB = (int*)B.s_.chlmRtriplet_->j;
    double* xB = (double*)B.s_.chlmRtriplet_->x;
    int nC = nA*nB;
    C.ChlmRTriplet(rC, cC, nC);
    int* iC = (int*)C.s_.chlmRtriplet_->i;
    int* jC = (int*)C.s_.chlmRtriplet_->j;
    double* xC = (double*)C.s_.chlmRtriplet_->x;
    int kA, kB;
    double x;
    C.s_.chlmRtriplet_->nnz = 0;
    for(kA=0; kA<nA; kA++, iA++, jA++, xA++)
    {
      iB = (int*)B.s_.chlmRtriplet_->i;
      jB = (int*)B.s_.chlmRtriplet_->j;
      xB = (double*)B.s_.chlmRtriplet_->x;
      for(kB=0; kB<nB; kB++, iB++, jB++, xB++)
      {
        x=(*xA)*(*xB);
        if(x)
        {
          (*iC) = (*iA)*rB+(*iB);
          (*jC) = (*jA)*cB+(*jB);
          (*xC) = x;
          iC++;
          jC++;
          xC++;
          C.s_.chlmRtriplet_->nnz++;
        }
      }
    }
    return(0);
  }
  else 
  {
    BVMat A_, B_, C_;
    A_.Convert(A,ESC_chlmRtriplet);
    B_.Convert(B,ESC_chlmRtriplet);
    int err = BVMat::KroneckerProd(A_,B_,C_);
    if(!err)
    {
      C.Convert(C_,ESC_chlmRsparse);
    }
    return(err);
  } 
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::MtMSqr(const BVMat& A, BVMat& B)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  if(!A.CheckDefined("MtMSqr")) { return(-1); }
  int i, j;
  int r = A.Rows();
  int c = A.Columns();  
  if(!r || !c) 
  {
    B=A;
    return(0);
  }
  double * x;
  BVMat sp, tr;
  int *fset;
  switch(A.code_) {
  case(ESC_blasRdense  ) :
    B.code_ = ESC_blasRdense;
    B.s_.blasRdense_ = CholmodAllocate_dense(c, c, c, CHOLMOD_REAL, common_);
    x = (double*)B.s_.blasRdense_->x;
    cblas_dsyrk
    (
      CblasColMajor, CblasLower, CblasTrans, 
      c, r, 1.0, (const double*)A.s_.blasRdense_->x, r, 0.0, x, c
    );
    for(i=0; i<c; i++) { for(j=0; j<i; j++) { x[i*c+j] = x[j*c+i]; } }
    break;
  case(ESC_chlmRtriplet ) :
    sp.Convert(A,ESC_chlmRsparse);
    MtMSqr(sp, B);
    break;
  case(ESC_chlmRsparse ) :
    B.code_ = ESC_chlmRsparse;
    T(A, tr);
    if(A.s_.chlmRsparse_->stype==0)
    {
      fset = new int[r];
      for(j=0; j<r; j++) { fset[j] = j; }
      B.s_.chlmRsparse_ = cholmod_aat(tr.s_.chlmRsparse_,fset,r,1,common_);
      delete [] fset;
    }
    else 
    {
      cRs_cRs_prod(tr,A,sp); 
      B.s_.chlmRsparse_ = CholmodCopy(sp.s_.chlmRsparse_,-1,1,common_);
    }
    break;
  default:
    err_invalid_subtype("MtMSqr",A); 
    return(-1);
  }
  return(0);
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::MMtSqr(const BVMat& A, BVMat& B)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  if(!A.CheckDefined("MMtSqr")) { return(-1); }
  return(MtMSqr(A.T(),B));
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::MtMSqr() const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  MtMSqr(*this, aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::MMtSqr() const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux1 = T();
  BVMat aux2;
  MtMSqr(aux1, aux2);
  return(aux2);
};


//--------------------------------------------------------------------
double BVMat::GetBackwardValue(const BPolyn  <BDat>& P,
                               const double* X0, int n0, 
                               const double* X,  int n, 
                               int i, int k)
//--------------------------------------------------------------------
{
  double xk = 0;
  i-=P[k].Degree();
  if(i>=0) 
  { 
    assert(i<n);
    xk = X[i];
  }
  else     
  { 
    assert(n0+i>=0);
    xk = X0[n0+i];
  }
  return(P[k].Coef().Value() * xk);
}


//--------------------------------------------------------------------
 double BVMat::GetForwardValue(const BPolyn  <BDat>& P,
                               const double* X0, int n0, 
                               const double* X,  int n, 
                               int i, int k)
//--------------------------------------------------------------------
{
  double xk = 0;
  BMonome<BDat>& Pk = P[k];
  int d = Pk.Degree();
  double c = Pk.Coef().Value();
  int id = i+d;
  if(id<n) 
  { 
    assert(id>=0);
    xk = X[id];
/*  Std(BText("  GetForwardValue ")+
      "\tn0= "+n0+
      "\tn= "+n+
      "\ti= "+i+
      "\tk= "+k+
      "\td= "+d+
      "\tc= "+c+
      "\tid= "+id+
      "\txk= X["+id+"]="+xk+
      "\tc*xk= "+(c * xk)+
      "\n"); */
  }
  else	   
  { 
    assert(id-n<n0);
    xk = X0[id-n];
/*  Std(BText("  GetForwardValue ")+
      "\tn0= "+n0+
      "\tn= "+n+
      "\ti= "+i+
      "\tk= "+k+
      "\td= "+d+
      "\tc= "+c+
      "\tid= "+id+
      "\txk= X0["+(id-n)+"]="+xk+
      "\tc*xk= "+(c * xk)+
      "\n"); */
  }
  return(c * xk);
}

//--------------------------------------------------------------------
void BVMat::DifEq(const BRational <BDat>& R,
                  const BVMat& X0,
                  const BVMat& X,
                  const BVMat& Y0,
                        BVMat& Y_,
                        double minSparse,
                        double chop)
//--------------------------------------------------------------------
{
  const char* fName = "DifEq";
  int i,j,k;
  const BPolyn<BDat>& num = R.Numerator();
  const BPolyn<BDat>& den = R.Denominator();
  int c = X.Columns();
  int r = X.Rows();
  int p = num.Degree();
  int q = den.Degree();
  Y_.Delete();
  if(c!=X0.Columns())
  {
    BVMat::err_cannot_apply(fName, 
      BText(" (X has ")+c+" columns but X0 has "+X0.Columns()+").", X0); 
    return;
  }
  if(c!=Y0.Columns())
  {
    BVMat::err_cannot_apply(fName, 
      BText(" (X has ")+c+" columns but Y0 has "+Y0.Columns()+").", Y0); 
    return;
  }
  if(p>X0.Rows())
  {
    BVMat::err_cannot_apply(fName, 
      BText(" (Numerator degree p=")+p+" and X0 have not enouth rows).", X0); 
    return;
  }
  if(q >Y0.Rows())
  {
    BVMat::err_cannot_apply(fName, 
      BText(" (Denominator degree q=")+q+" and Y0 have not enouth rows).", Y0); 
    return;
  }
  if(!c || (!p&&!q)) { Y_ = X; return; }
  BVMat* Y = &Y_;
  if(X.code_==ESC_chlmRsparse) { Y = new BVMat; }
  Y->BlasRDense(r,c);
  BVMatColMajIter x0_(X0, fName);
  BVMatColMajIter x_ (X,  fName);
  BVMatColMajIter y0_(Y0, fName);
  BArray<double> _x0, _x, _y0;
  double *x0=NULL, *x=NULL, *y0=NULL, *y=NULL;
  double *yij= ((double*)Y->s_.blasRdense_->x);
  int nnz = 0;
  for(j=0; j<c; j++)
  {
    if(p) { x0_.GetCol(j,_x0); x0 = _x0.GetBuffer(); }
            x_ .GetCol(j,_x ); x  = _x .GetBuffer();
    if(q) { y0_.GetCol(j,_y0); y0 = _y0.GetBuffer(); }
    y = yij;
	  for(i=0; i<r; i++)
	  {
	    *yij = 0.0;
	    for(k=0; k<num.Size(); k++)
	    {
	      *yij += GetBackwardValue(num,x0,p,x,r,i,k);
	    }
	    for(k=1; k<den.Size(); k++)
	    {
	      *yij -= GetBackwardValue(den,y0,q,y,r,i,k);
	    }
      if(fabs(*yij)<chop) { *yij=0.0; }
      if(fabs(*yij)>0.0)  { nnz++; }
      yij++;
	  }
  }
  if(X.code_==ESC_chlmRsparse) 
  {
    if(double(nnz)<=minSparse*double(r*c))
    {
      Y_.Convert(*Y,ESC_chlmRsparse);
    }
    else
    {
      Y_.Copy(*Y);
    }
    delete Y;
  }
}

//--------------------------------------------------------------------
void BVMat::BackDifEq(const BRational <BDat>& R,
                      const BVMat& X0,
                      const BVMat& X,
                      const BVMat& Y0,
                            BVMat& Y_,
                            double minSparse,
                            double chop)
//--------------------------------------------------------------------
{
  const char* fName = "BackDifEq";
  const BPolyn<BDat>& num = R.Numerator();
  const BPolyn<BDat>& den = R.Denominator();
  int i,j,k;
  int c = X.Columns();
  int r = X.Rows();
  int p = num.Degree();
  int q = den.Degree();
  Y_.Delete();
  if(c!=X0.Columns())
  {
    BVMat::err_cannot_apply(fName, 
      BText(" (X has ")+c+" columns but X0 has "+X0.Columns()+").", X0); 
    return;
  }
  if(c!=Y0.Columns())
  {
    BVMat::err_cannot_apply(fName, 
      BText(" (X has ")+c+" columns but Y0 has "+Y0.Columns()+").", Y0); 
    return;
  }
  if(p>X0.Rows())
  {
    BVMat::err_cannot_apply(fName, 
      BText(" (Numerator degree p=")+p+" and X0 have not enouth rows).", X0); 
    return;
  }
  if(q >Y0.Rows())
  {
    BVMat::err_cannot_apply(fName, 
      BText(" (Denominator degree q=")+q+" and Y0 have not enouth rows).", Y0); 
    return;
  }
  if(!c || (!p&&!q)) { Y_ = X; return; }
  BVMat* Y = &Y_;
  if(X.code_==ESC_chlmRsparse) { Y = new BVMat; }
  Y->BlasRDense(r,c);
  BVMatColMajIter x0_(X0, fName);
  BVMatColMajIter x_ (X,  fName);
  BVMatColMajIter y0_(Y0, fName);
  BArray<double> _x0, _x, _y0;
  double *x0=NULL, *x=NULL, *y0=NULL, *y=NULL;
  double *yij= ((double*)Y->s_.blasRdense_->x)+(r*c-1);
  int nnz = 0;
  for(j=c-1; j>=0; j--)
  {
    if(p) { x0_.GetCol(j,_x0); x0 = _x0.GetBuffer(); }
            x_ .GetCol(j,_x ); x  = _x .GetBuffer();
    if(q) { y0_.GetCol(j,_y0); y0 = _y0.GetBuffer(); }
    y = yij-r+1;
   	for(i=r-1; i>=0; i--)
	  {
	    *yij = 0.0;
    //Std(BText("BackDifEq AR\n"));
	    for(k=0; k<num.Size(); k++)
	    {
		    *yij += GetForwardValue(num,x0,p,x,r,i,k);
	    }
    //Std(BText("BackDifEq MA\n"));
	    for(k=1; k<den.Size(); k++)
	    {
		    *yij -= GetForwardValue(den,y0,q,y,r,i,k);
	    }
      if(fabs(*yij)<chop) { *yij=0.0; }
      if(fabs(*yij)>0.0)  { nnz++; }
    //Std(BText("y[")+i+"]="+y[i]+"\n");
      yij--; 
	  }
  }
  if(X.code_==ESC_chlmRsparse) 
  {
    if(double(nnz)<=minSparse*double(r*c))
    {
      Y_.Convert(*Y,ESC_chlmRsparse);
    }
    else
    {
      Y_.Copy(*Y);
    }
    delete Y;
  }
}

//--------------------------------------------------------------------
void BVMat::DifEq(const BRational <BDat>& R,
                  const BVMat& X,
                        BVMat& Y_,
                        double minSparse,
                        double chop)
//--------------------------------------------------------------------
{
  const BPolyn<BDat>& num = R.Numerator();
  const BPolyn<BDat>& den = R.Denominator();
  int c = X.Columns();
  int p = num.Degree();
  int q = den.Degree();
  BVMat X0, Y0;
  X0.Zeros(p, c, ESC_blasRdense);
  Y0.Zeros(q, c, ESC_blasRdense);
  BVMat::DifEq(R, X0, X, Y0, Y_, minSparse, chop);
}

//--------------------------------------------------------------------
void BVMat::BackDifEq(const BRational <BDat>& R,
                  const BVMat& X,
                        BVMat& Y_,
                        double minSparse,
                        double chop)
//--------------------------------------------------------------------
{
  const BPolyn<BDat>& num = R.Numerator();
  const BPolyn<BDat>& den = R.Denominator();
  int c = X.Columns();
  int p = num.Degree();
  int q = den.Degree();
  BVMat X0, Y0;
  X0.Zeros(p, c, ESC_blasRdense);
  Y0.Zeros(q, c, ESC_blasRdense);
  BVMat::BackDifEq(R, X0, X, Y0, Y_, minSparse, chop);
}

