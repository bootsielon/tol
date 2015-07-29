/* vmat_chol.cpp: BVMat Choleski related methods
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
#include <tol/tol_lapack.h>

////////////////////////////////////////////////////////////////////////////////
  void BVMat::bRd_choFac_X(const BVMat& X, BVMat& L,
                           bool& isOk, bool& isNotPosDef)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  int i, j, res;
  int n = X.s_.blasRdense_->nrow;
  double* y = (double*)X.s_.blasRdense_->x;
  L.BlasRDense(n,n);
  double* x = (double*)L.s_.blasRdense_->x;
  memcpy(x, y, n*n*sizeof(double));
  res = clapack_dpotrf( CblasColMajor, LAPACK_UPLO(CblasLower), n, x, n );
  for(i=0; i<n; i++) for(j=i+1; j<n; j++) x[j*n+i] = 0;
  isOk = (res==0);
  isNotPosDef = (res>0);
}
  
////////////////////////////////////////////////////////////////////////////////
  void BVMat::bRd_choFac_XtX(const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  bRd_choFac_X(X.MtMSqr(),L,isOk,isNotPosDef);
}
  
////////////////////////////////////////////////////////////////////////////////
  void BVMat::bRd_choFac_XXt(const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  bRd_choFac_X(X.MMtSqr(),L,isOk,isNotPosDef);
}
  
////////////////////////////////////////////////////////////////////////////////
  void BVMat::cRs_choFac_X(const BVMat& X_, BVMat& L, 
                           bool& isOk, bool& isNotPosDef)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  int res;
  BVMat X;
  X.Copy(X_);
  L.code_ = ESC_chlmRfactor;
  L.s_.chlmRfactor_ = cholmod_analyze(X.s_.chlmRsparse_,common_);
  res = cholmod_factorize(X.s_.chlmRsparse_,L.s_.chlmRfactor_,common_);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Factor",X.s_.chlmRsparse_)
  
  isNotPosDef = (common_->status == CHOLMOD_NOT_POSDEF)||
                (L.s_.chlmRfactor_->minor<L.s_.chlmRfactor_->n);
  cholmod_change_factor
  (
    CHOLMOD_REAL,
    1, 
    L.s_.chlmRfactor_->is_super, 
    0, 
    0, 
    L.s_.chlmRfactor_, 
    common_
  );
  int check = L.Check();
//isOk = (res!=0) && !isNotPosDef;
  isOk = check && !isNotPosDef;
}
  
////////////////////////////////////////////////////////////////////////////////
  void BVMat::cRs_choFac_XtX(const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  cRs_choFac_X(X.T(),L,isOk,isNotPosDef);
}

////////////////////////////////////////////////////////////////////////////////
  void BVMat::cRs_choFac_XXt(const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  cRs_choFac_X(X,L,isOk,isNotPosDef);
}
 
////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiFactor(const BVMat& X_, BVMat& L, 
                            ECholFacOri ori,  
                            bool checkSymmetric,
                            bool forceNaturalOrder,
                            bool showError)
////////////////////////////////////////////////////////////////////////////////
{
  const char* fName = "CholeskiFactor";
  if(!X_.CheckDefined(fName)) { return(-1); }
  if(!showError)
  {
#if defined( CHOLMOD_MAIN_VERSION ) && ( CHOLMOD_MAIN_VERSION >= 3 )
#else
    common_->print_function = NULL;
#endif
    common_->error_handler  = NULL;
  }
  bool old_force = force_natural_order(forceNaturalOrder);
  BVMat* X__;
  convertIfNeeded_cRt2cRs(X_,X__,fName);
  BVMat &X = *X__;
  int r = X.Rows();
  int c = X.Columns();
  int result = 0;
  if((ori==ECFO_X)&&(r!=c))
  {
    if(showError)
    err_cannot_apply(fName,I2("non square","no cuadrada"),X);
    result = -1;
  }
  else if((ori==ECFO_XtX)&&(r<c))
  {
    if(showError)
    err_cannot_apply(fName,
      I2("row deficient to build",
         "no tiene bastantes filas para construir")+ " S = X' X",X);
    result =-2;
  }
  else if((ori==ECFO_XXt)&&(r>c))
  {
    if(showError)
    err_cannot_apply(fName,
      I2("column deficient to build",
         "no tiene bastantes columnas para construir")+" S = X X'",X);
    result =-3;
  }
  else
  {
    if(ori!=ECFO_X) { checkSymmetric=false; }
    bool isNotSymm = (checkSymmetric)?!X.IsSymmetric():false;
    bool isOk = true;
    bool isNotPosDef=false;
    if(isNotSymm)
    {
      if(showError)
      err_cannot_apply(fName, I2("non symmetric","no simetrica"),X);
      result = -4;
    }
    else
    {
      if(checkSymmetric && !isNotSymm) { ((BVMat&)X).CompactSymmetric(false); }
      const StrCholFac* cholFac = FindCholFac(X.code_, ori);
      if(cholFac)
      {
        (*cholFac->fun)(X, L, isOk, isNotPosDef);
      }
      else
      {
        err_invalid_subtype(fName,X); 
        isOk = false;
      }
      if(!isOk)
      {
        if(isNotPosDef)
        {
          if(showError)
          err_cannot_apply(fName,  
           I2("non positive definite", "no definida positiva"),X);
          L.Delete();
          result = -5;
        }
        else
        {
          result = -6;
        }
      } 
    }
  }
  force_natural_order(old_force);
  if(!showError)
  {
#if defined( CHOLMOD_MAIN_VERSION ) && ( CHOLMOD_MAIN_VERSION >= 3 )
#else
    common_->print_function = cholmod_print_function;
#endif
    common_->error_handler  = cholmod_error_handler;
  }
  if(X__!=&X_) { delete X__; }
  return(0);
}
  
////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiFactor(const BVMat& X, BVMat& L,
                            const BText& oriName, 
                            bool checkSymmetric,
                            bool forceNaturalOrder,
                            bool showError)
////////////////////////////////////////////////////////////////////////////////
{
  const char* fName = "CholeskiFactor";
  ECholFacOri ori = FindCFacName(oriName);
  if(ori==ECFO_none)
  {
    err_cannot_apply(fName,
    I2("Invalid origen identifier",
       "Identificador de origen inválido")+" "+oriName+" "+
    I2("should be one of these",
       "debería ser uno de estos")+"\n"+CFacNames()+"\n",
    L);
    return(-3);
  }
  return(CholeskiFactor(X,L,ori,checkSymmetric,forceNaturalOrder,showError));
}
  
////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::CholeskiFactor(ECholFacOri ori, 
                              bool checkSymmetric,
                              bool forceNaturalOrder,
                              bool showError)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  CholeskiFactor(*this, aux, ori, checkSymmetric, forceNaturalOrder, showError);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::CholeskiFactor(const BText& oriName, 
                              bool checkSymmetric,
                              bool forceNaturalOrder,
                              bool showError)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  CholeskiFactor(*this, aux, oriName, checkSymmetric,forceNaturalOrder,showError);
  return(aux);
}
  
////////////////////////////////////////////////////////////////////////////////
  int BVMat::bRd_bRd_cholSolLLt(const BVMat& L, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x = b;
  int M = x.Rows   ();
  int N = x.Columns();

  cblas_dtrsm(CblasColMajor,CblasLeft,CblasLower,CblasNoTrans,CblasNonUnit,
              M,N,1.0, 
              (const double*)(L.s_.blasRdense_->x),M,
              (      double*)(x.s_.blasRdense_->x),M);
  cblas_dtrsm(CblasColMajor,CblasLeft,CblasLower,CblasTrans,CblasNonUnit,
              M,N,1.0, 
              (const double*)(L.s_.blasRdense_->x),M,
              (      double*)(x.s_.blasRdense_->x),M);
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::bRd_bRd_cholSolLt(const BVMat& L, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x = b;
  int M = x.Rows   ();
  int N = x.Columns();
  cblas_dtrsm(CblasColMajor,CblasLeft,CblasLower,CblasTrans,CblasNonUnit,
              M,N,1.0, 
              (const double*)(L.s_.blasRdense_->x),M,
              (      double*)(x.s_.blasRdense_->x),M);
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::bRd_bRd_cholSolL(const BVMat& L, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x = b;
  int M = x.Rows   ();
  int N = x.Columns();
  cblas_dtrsm(CblasColMajor,CblasLeft,CblasLower,CblasNoTrans,CblasNonUnit,
              M,N,1.0, 
              (const double*)(L.s_.blasRdense_->x),M,
              (      double*)(x.s_.blasRdense_->x),M);
  return(0);    
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::bRd_bRd_cholSolId(const BVMat& L, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  //Dense BLAS has not permutation implemented, so identity is taken
  x = b;
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_cholSolPtLLtP(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_blasRdense;
  x.s_.blasRdense_ = cholmod_solve
  (
    CHOLMOD_A,
    factor.s_.chlmRfactor_,
    b.s_.blasRdense_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Blas.R.Dense",x.s_.blasRdense_);
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_cholSolLLt(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_blasRdense;
  x.s_.blasRdense_ = cholmod_solve
  (
    CHOLMOD_LDLt,
    factor.s_.chlmRfactor_,
    b.s_.blasRdense_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Blas.R.Dense",x.s_.blasRdense_);
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_cholSolPtL(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  cRs_bRd_cholSolPt(factor, b,   aux);
  cRs_bRd_cholSolL (factor, aux, x  );
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_cholSolLtP(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  cRs_bRd_cholSolLt(factor, b,   aux);
  cRs_bRd_cholSolP (factor, aux, x  );
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_cholSolL(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_blasRdense;
  x.s_.blasRdense_ = cholmod_solve
  (
    CHOLMOD_L,
    factor.s_.chlmRfactor_,
    b.s_.blasRdense_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Blas.R.Dense",x.s_.blasRdense_);
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_cholSolLt(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_blasRdense;
  x.s_.blasRdense_ = cholmod_solve
  (
    CHOLMOD_Lt,
    factor.s_.chlmRfactor_,
    b.s_.blasRdense_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Blas.R.Dense",x.s_.blasRdense_);
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_cholSolP(const BVMat& L, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_blasRdense;
  x.s_.blasRdense_ = cholmod_solve
  (
    CHOLMOD_Pt,
    L.s_.chlmRfactor_,
    b.s_.blasRdense_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Blas.R.Dense",x.s_.blasRdense_);
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_cholSolPt(const BVMat& L, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_blasRdense;
  x.s_.blasRdense_ = cholmod_solve
  (
    CHOLMOD_P,
    L.s_.chlmRfactor_,
    b.s_.blasRdense_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Blas.R.Dense",x.s_.blasRdense_);
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_cRs_cholSolPtLLtP(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_chlmRsparse;
  cholmod_sparse * b_ = b.s_.chlmRsparse_;
  if(b_->stype!=0)
  {
    b_ = CholmodCopy(b.s_.chlmRsparse_, 0, 1, common_);
  }
  x.s_.chlmRsparse_ = cholmod_spsolve
  (
    CHOLMOD_A,
    factor.s_.chlmRfactor_,
    b_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",x.s_.chlmRsparse_);
  if(b_ != b.s_.chlmRsparse_)
  {
    CholmodFree_sparse(&b_, common_); 
  }
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_cRs_cholSolLLt(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_chlmRsparse;
  cholmod_sparse * b_ = b.s_.chlmRsparse_;
  if(b_->stype!=0)
  {
    b_ = CholmodCopy(b.s_.chlmRsparse_, 0, 1, common_);
  }
  x.s_.chlmRsparse_= cholmod_spsolve
  (
    CHOLMOD_LDLt,
    factor.s_.chlmRfactor_,
    b_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",x.s_.chlmRsparse_);
  if(b_ != b.s_.chlmRsparse_)
  {
    CholmodFree_sparse(&b_, common_); 
  }
  return(0);      
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_cRs_cholSolPtL(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_chlmRsparse;
  cholmod_sparse * b_ = b.s_.chlmRsparse_;
  if(b_->stype!=0)
  {
    b_ = CholmodCopy(b.s_.chlmRsparse_, 0, 1, common_);
  }
  cholmod_sparse* aux = cholmod_spsolve
  (
    CHOLMOD_P,
    factor.s_.chlmRfactor_,
    b_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",aux);
  if(b_ != b.s_.chlmRsparse_)
  {
    CholmodFree_sparse(&b_, common_); 
  }
  x.s_.chlmRsparse_= cholmod_spsolve
  (
    CHOLMOD_L,
    factor.s_.chlmRfactor_,
    aux,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",x.s_.chlmRsparse_);
  CholmodFree_sparse(&aux, common_); 
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_cRs_cholSolLtP(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_chlmRsparse;
  cholmod_sparse * b_ = b.s_.chlmRsparse_;
  if(b_->stype!=0)
  {
    b_ = CholmodCopy(b.s_.chlmRsparse_, 0, 1, common_);
  }
  cholmod_sparse* aux = cholmod_spsolve
  (
    CHOLMOD_Lt,
    factor.s_.chlmRfactor_,
    b_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",aux);
  if(b_ != b.s_.chlmRsparse_)
  {
    CholmodFree_sparse(&b_, common_); 
  }
  x.s_.chlmRsparse_= cholmod_spsolve
  (
    CHOLMOD_Pt,
    factor.s_.chlmRfactor_,
    aux,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",x.s_.chlmRsparse_);
  CholmodFree_sparse(&aux, common_); 
  return(0);    
};
  
////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_cRs_cholSolLt(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_chlmRsparse;
  cholmod_sparse * b_ = b.s_.chlmRsparse_;
  if(b_->stype!=0)
  {
    b_ = CholmodCopy(b.s_.chlmRsparse_, 0, 1, common_);
  }
  x.s_.chlmRsparse_ = cholmod_spsolve
  (
    CHOLMOD_Lt,
    factor.s_.chlmRfactor_,
    b_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",x.s_.chlmRsparse_);
  if(b_ != b.s_.chlmRsparse_)
  {
    CholmodFree_sparse(&b_, common_); 
  }
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_cRs_cholSolL(const BVMat& factor, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_chlmRsparse;
  cholmod_sparse * b_ = b.s_.chlmRsparse_;
  if(b_->stype!=0)
  {
    b_ = CholmodCopy(b.s_.chlmRsparse_, 0, 1, common_);
  }
  x.s_.chlmRsparse_ = cholmod_spsolve
  (
    CHOLMOD_L,
    factor.s_.chlmRfactor_,
    b_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",x.s_.chlmRsparse_);
  if(b_ != b.s_.chlmRsparse_)
  {
    CholmodFree_sparse(&b_, common_); 
  }
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_cRs_cholSolP(const BVMat& L, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_chlmRsparse;
  cholmod_sparse * b_ = b.s_.chlmRsparse_;
  if(b_->stype!=0)
  {
    b_ = CholmodCopy(b.s_.chlmRsparse_, 0, 1, common_);
  }
  x.s_.chlmRsparse_ = cholmod_spsolve
  (
    CHOLMOD_Pt,
    L.s_.chlmRfactor_,
    b_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",x.s_.chlmRsparse_);
  if(b_ != b.s_.chlmRsparse_)
  {
    CholmodFree_sparse(&b_, common_); 
  }
  return(0);    
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_cRs_cholSolPt(const BVMat& L, const BVMat& b, BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  x.Delete();
  x.code_ = ESC_chlmRsparse;
  cholmod_sparse * b_ = b.s_.chlmRsparse_;
  if(b_->stype!=0)
  {
    b_ = CholmodCopy(b.s_.chlmRsparse_, 0, 1, common_);
  }
  x.s_.chlmRsparse_ = cholmod_spsolve
  (
    CHOLMOD_P,
    L.s_.chlmRfactor_,
    b_,
    common_
  );
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",x.s_.chlmRsparse_);
  if(b_ != b.s_.chlmRsparse_)
  {
    CholmodFree_sparse(&b_, common_); 
  }
  return(0);    
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiSolve (const BVMat& L, const BVMat& B_, 
                            BVMat& X, ECholSolSys sys)
////////////////////////////////////////////////////////////////////////////////
{
  if(!L .CheckDefined("CholeskiSolve")) { return(-1); }
  if(!B_.CheckDefined("CholeskiSolve")) { return(-1); }
  int result = 0;
  BVMat *B__=NULL;
  if(L.code_==ESC_blasRdense)
  {
    convertIfNeeded_all2bRd(B_,B__,"CholeskiSolve");
  }
  else if(L.code_==ESC_chlmRfactor)
  {
    convertIfNeeded_all2cRs(B_,B__,"CholeskiSolve");
  }
  else
  {
    err_invalid_subtype("CholeskiSolve",L);
    result = -2;
    return(result);
  }
  BVMat &B = *B__;
  const StrCholSol* cholSol = FindCholSol(L.code_, B.code_, sys);
  if(cholSol)
  {
    int c = L.Columns();
    if(c!=B.Rows())
    {
      err_invalid_dimensions("CholeskiSolve",L,B);
      result = -1;
    }
    else
    {
      (*cholSol->fun)(L, B, X);
    }
  }
  else
  {
    err_invalid_subtypes("CholeskiSolve",L,B);
    result = -2;
  }
  if(B__!=&B_) { delete B__; }
  return(result);
}
  
////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiSolve (const BVMat& L, const BVMat& B, 
                            BVMat& X, const BText& sysName)
////////////////////////////////////////////////////////////////////////////////
{
  int result = 0;
  ECholSolSys sys = FindCSysName(sysName);
  if(sys==ECSS_none)
  {
    err_cannot_apply("CholeskiSolve",
    I2("Invalid system identifier",
       "Identificador de sistema inválido")+" "+sysName+" "+
    I2("should be one of these",
       "debería ser uno de estos")+"\n"+CSysNames()+"\n",
    L);
    result = -3;
  }
  else
  {
    result = CholeskiSolve(L,B,X,sys);
  }
  return(result);
}
  

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::CholeskiSolve(const BVMat& B, ECholSolSys sys) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  CholeskiSolve(*this,B,aux,sys);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::CholeskiSolve(const BVMat& B, const BText& sysName) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  CholeskiSolve(*this,B,aux,sysName);
  return(aux);
}

////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiInv (const BVMat& X, BVMat& Xi)
////////////////////////////////////////////////////////////////////////////////
{
  if(!X.CheckDefined("CholeskiInv")) { return(-1); }
  BVMat I;
  int r = X.Rows();
  int c = X.Columns();
  if(c!=r)
  {
    err_cannot_apply("CholeskiInv",I2("non square","no cuadrada"),X);
    return(-1);
  }
  switch(X.code_) {
  case(ESC_blasRdense  ) :
    I.Eye(X.code_, r);
    bRd_bRd_cholSolLLt(X, I, Xi);
    break;
  case(ESC_chlmRfactor ) :
    I.Eye(X.code_, r);
    cRs_cRs_cholSolLLt(X, I, Xi);
    break;
  default:
    err_cannot_apply("CholeskiInv","",X); 
    return(-2); }
  return(0);
}
    
////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::CholeskiInv() const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  CholeskiInv(*this, aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiMinRes (const BVMat& X, const BVMat& b, BVMat& a)
//Find b as minimum residuals solution to linear regression
// a = X*b + e; e ~ N(0,s); s>0
////////////////////////////////////////////////////////////////////////////////
{
  BVMat L;
  if(CholeskiFactor(X, L, BVMat::ECFO_XtX, false, false, true)) { return(-1); }
  BVMat Xtb = X.T()*b;
  if(CholeskiSolve(L,Xtb,a, BVMat::ECSS_PtLLtP)) { return(-2); }
  return(0);
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiUpdate (const BVMat& L, const BVMat& C_, bool upDown)
////////////////////////////////////////////////////////////////////////////////
{
  if(!L .CheckDefined("CholeskiUpdate")) { return(-1); }
  if(!C_.CheckDefined("CholeskiUpdate")) { return(-1); }
  int result = 0;
  BVMat *C__=NULL;
  if(L.code_==ESC_chlmRfactor)
  {
    convertIfNeeded_all2cRs(C_,C__,"CholeskiUpdate");
  }
  else
  {
    err_invalid_subtype("CholeskiUpdate",L);
    result = -2;
    return(result);
  }
  BVMat &C = *C__;
  cholmod_factor* l = L.s_.chlmRfactor_;
  cholmod_sparse* c = cholmod_submatrix(
    C.s_.chlmRsparse_, (int*)l->Perm, l->n, NULL, -1, 1, 1, common_);
  result = cholmod_updown(upDown, c, l, common_);
  bool isNotPosDef = (common_->status == CHOLMOD_NOT_POSDEF)||
                (L.s_.chlmRfactor_->minor<L.s_.chlmRfactor_->n);
  if(isNotPosDef)
  {
    result = -1;
    err_cannot_apply("CholeskiUpdate","",L); 
  }
  else
  {
    CholmodFree_sparse(&c,common_);
    
  }
  if(C__!=&C_) { delete C__; }
  return(result);
}
   

////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiUpdate (const BVMat& C_, bool upDown)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  return(CholeskiUpdate(*this,C_,upDown));
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiAddRow (const BVMat& L, const BVMat& R_, int rowPos)
////////////////////////////////////////////////////////////////////////////////
{
  if(!L .CheckDefined("CholeskiAddRow")) { return(-1); }
  if(!R_.CheckDefined("CholeskiAddRow")) { return(-1); }
  int result = 0;
  BVMat *R__=NULL;
  if(L.code_==ESC_chlmRfactor)
  {
    convertIfNeeded_all2cRs(R_,R__,"CholeskiAddRow");
  }
  else
  {
    err_invalid_subtype("CholeskiAddRow",L);
    result = -2;
    return(result);
  }
  BVMat &R = *R__;
  result = cholmod_rowadd(rowPos, R.s_.chlmRsparse_, L.s_.chlmRfactor_, common_);
  if(R__!=&R_) { delete R__; }
  return(result);
}
   

////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiAddRow (const BVMat& R_, int rowPos)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  return(CholeskiAddRow(*this,R_,rowPos));
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiDelRow (const BVMat& L, const BVMat& R_, int rowPos)
////////////////////////////////////////////////////////////////////////////////
{
  if(!L .CheckDefined("CholeskiDelRow")) { return(-1); }
  if(!R_.CheckDefined("CholeskiDelRow")) { return(-1); }
  int result = 0;
  BVMat *R__=NULL;
  if(L.code_==ESC_chlmRfactor)
  {
    convertIfNeeded_all2cRs(R_,R__,"CholeskiDelRow");
  }
  else
  {
    err_invalid_subtype("CholeskiDelRow",L);
    result = -2;
    return(result);
  }
  BVMat &R = *R__;
  result = cholmod_rowdel(rowPos, R.s_.chlmRsparse_, L.s_.chlmRfactor_, common_);
  if(R__!=&R_) { delete R__; }
  return(result);
}
   

////////////////////////////////////////////////////////////////////////////////
  int BVMat::CholeskiDelRow (const BVMat& R_, int rowPos)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  return(CholeskiDelRow(*this,R_,rowPos));
};



