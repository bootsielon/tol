/* mfastpro.cpp: Generic arithmetical functions.
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

#include <tol/tol_bmfstpro.h>
#include <tol/tol_matrix.h>
#include <tol/tol_blas.h> 

BTraceInit("mfastpro.cpp");

//VBR: This three lines should be deleted
static double _log2 = log(2.0);
static BDat StrassenCutoff_ = 12800;
BDat& StrassenCutoff() { return(StrassenCutoff_); }

/////////////////////////////////////////////////////////////////////////////
int MatProd(const BMatrix<double>& A_, 
            const BMatrix<double>& B_,
                  BMatrix<double>& C)
/////////////////////////////////////////////////////////////////////////////
{
  if(A_.Columns()!=B_.Rows()) { return(false); }
  if(!A_.Columns() || !B_.Columns() || !A_.Rows() || !B_.Rows()) 
  { 
    C.Alloc(0,0);
    return(true); 
  }
  BMatrixStoreType ast = A_.StoreType();
  BMatrixStoreType bst = B_.StoreType();
  BMatrix<double> A_dense;
  BMatrix<double> B_dense;
  const BMatrix<double>& A = (ast==BMST_dense_)?A_:(A_dense=A_);
  const BMatrix<double>& B = (bst==BMST_dense_)?B_:(B_dense=B_);
  bool storeOk = true;
  int res = 0;
  if((ast==BMST_dense_)&&(bst==BMST_dense_))
  {
    res = TolBlas::dgemm(CblasNoTrans,CblasNoTrans,1.0,A,B,0.0,C);
  }
  else if((ast!=BMST_dense_)&&(bst==BMST_dense_)) switch(ast)
  {
    case BMST_diagonal_: res = TolBlas::dtrmm
     (CblasLeft,CblasUpper,CblasNoTrans,CblasUnit,1.0,A,B,C); break;
    case BMST_lowTriang_: res = TolBlas::dtrmm
     (CblasLeft,CblasLower,CblasNoTrans,CblasNonUnit,1.0,A,B,C); break;
    case BMST_uppTriang_: res = TolBlas::dtrmm
     (CblasLeft,CblasUpper,CblasNoTrans,CblasNonUnit,1.0,A,B,C); break;
    case BMST_symmetric_: res = TolBlas::dgemm
     (CblasNoTrans,CblasNoTrans,1.0,A,B,0.0,C); break;
    default:
      storeOk = false;
  }
  else switch(bst)
  {
    case BMST_diagonal_: res = TolBlas::dtrmm
     (CblasRight,CblasUpper,CblasNoTrans,CblasUnit,1.0,B,A,C); break;
    case BMST_lowTriang_: res = TolBlas::dtrmm
     (CblasRight,CblasLower,CblasNoTrans,CblasNonUnit,1.0,B,A,C); break;
    case BMST_uppTriang_: res = TolBlas::dtrmm
     (CblasRight,CblasUpper,CblasNoTrans,CblasNonUnit,1.0,B,A,C); break;
    case BMST_symmetric_: res = TolBlas::dgemm
     (CblasNoTrans,CblasNoTrans,1.0,A,B,0.0,C); break;
    default:
      storeOk = false;
  }
  if(!storeOk)
  {
    Error(I2("Undefined product for matrix store types",
             "Producto indefinido para los tipos de almacenamiento de "
             "matriz"));
  }
  return((res==0)&&storeOk);
}


/////////////////////////////////////////////////////////////////////////////
BMatrix<double> 
operator *(const BMatrix<double>& A, const BMatrix<double>& B)
/////////////////////////////////////////////////////////////////////////////
{
  BMatrix<double> C;
  MatProd(A,B,C);
  return(C);
}

/////////////////////////////////////////////////////////////////////////////
int MtMSqr(const BMatrix<double>& A_, BSymMatrix<double>& S)
/////////////////////////////////////////////////////////////////////////////
{
  BMatrixStoreType ast = A_.StoreType();
  BMatrix<double> A_dense;
  const BMatrix<double>& A = (ast==BMST_dense_)?A_:(A_dense=A_);
  bool storeOk = true;
  BMatrix<double> S_;
  int res = TolBlas::dsyrk(CblasLower,CblasTrans,1.0,A,0.0,S_);
  S = S_;
  return(res);
}


/////////////////////////////////////////////////////////////////////////////
BSymMatrix<double> MtMSqr(const BMatrix<double>& A)
/////////////////////////////////////////////////////////////////////////////
{
  BSymMatrix<double> S;
  MtMSqr(A,S);
  return(S);
}


/////////////////////////////////////////////////////////////////////////////
int MatProd(const BMatrix<BDat>& A, 
            const BMatrix<BDat>& B,
                  BMatrix<BDat>& C)
/////////////////////////////////////////////////////////////////////////////
{
  return(MatProd((const BMatrix<double>&)A,
                 (const BMatrix<double>&)B,
                       (BMatrix<double>&)C));
};

/////////////////////////////////////////////////////////////////////////////
BMatrix<BDat> 
operator *(const BMatrix<BDat>& A, const BMatrix<BDat>& B)
/////////////////////////////////////////////////////////////////////////////
{
  BMatrix<BDat> C;
  MatProd((const BMatrix<double>&)A,
          (const BMatrix<double>&)B,
                (BMatrix<double>&)C);
  return(C);
}



/////////////////////////////////////////////////////////////////////////////
int MtMSqr(const BMatrix<BDat>& A, BSymMatrix<BDat>& S)
/////////////////////////////////////////////////////////////////////////////
{
  return(MtMSqr((const BMatrix<double>&)A, (BSymMatrix<double>&)S));
};

/////////////////////////////////////////////////////////////////////////////
BSymMatrix<BDat> MtMSqr(const BMatrix<BDat>& A)
/////////////////////////////////////////////////////////////////////////////
{
  BSymMatrix<BDat> S;
  MtMSqr((const BMatrix<double>&)A,(BSymMatrix<double>&)S);
  return(S);
}

