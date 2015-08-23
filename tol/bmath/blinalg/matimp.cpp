/* matimp.cpp: Generic arithmetical functions
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

#include <tol/tol_matrix.h>
#include <tol/tol_bmatimp.h>
#include <tol/tol_lapack.h> 

BTraceInit("matimp.cpp");

void MatrixInitialize()
{
    DMat::Initialize();
    BMat::Initialize();
}
//template class BMatrix	    <BDat>;
template class BSymMatrix   <BDat>;
template class BLowTrMatrix <BDat>;
template class BUpTrMatrix  <BDat>;
template class BDiagMatrix  <BDat>;
template class BArray<BArray<BDat> >;

//template class BMatrix	    <double>;
template class BSymMatrix   <double>;
template class BLowTrMatrix <double>;
template class BUpTrMatrix  <double>;
template class BDiagMatrix  <double>;
template class BArray<BArray<double> >;

//--------------------------------------------------------------------
void BArrDouble2BDat(const BArray<double>& A, BArray<BDat>& B)
//--------------------------------------------------------------------
{
  B.AllocBuffer(A.Size());
  memcpy(B.GetBuffer(),
         A.Buffer(),
         A.Size()*sizeof(double));
}

//--------------------------------------------------------------------
void BArrBDat2Double(const BArray<BDat>& A, BArray<double>& B)
//--------------------------------------------------------------------
{
  B.AllocBuffer(A.Size());
  memcpy(B.GetBuffer(),
         A.Buffer(),
         A.Size()*sizeof(double));
}

//--------------------------------------------------------------------
void BMatDouble2BDat(const DMat& A, BMat& B)
//--------------------------------------------------------------------
{
  B.Alloc(A.Rows(),A.Columns());
  memcpy(B.GetData().GetBuffer(),
         A.Data().Buffer(),
         A.Data().Size()*sizeof(double));
}

//--------------------------------------------------------------------
void BMatBDat2Double(const BMat& A, DMat& B)
//--------------------------------------------------------------------
{
  B.Alloc(A.Rows(),A.Columns());
  memcpy(B.GetData().GetBuffer(),
         A.Data().Buffer(),
         A.Data().Size()*sizeof(double));
}


//--------------------------------------------------------------------
void InversePivots(const BArray<BInt>& pivot,
		         BArray<BInt>& pivotInv,
		         BInt          max)
//--------------------------------------------------------------------
{
    BInt i,  n = pivot.Size();
    // unused
    // BBool ok = BTRUE;
    pivotInv.Replicate(-1,max);
    
    for(i=0; i<n; i++)
    {
	pivotInv[pivot(i)] = i;
    }
}


//--------------------------------------------------------------------
void InversePivots(const BArray<BInt>& pivot,
		         BArray<BInt>& pivotInv)
//--------------------------------------------------------------------
{
    InversePivots(pivot,pivotInv,pivot.Size());
}



//--------------------------------------------------------------------
BBool Choleski(const BSymMatrix  <double>& a,
	                   BLowTrMatrix<double>& l)

/*! Constructs in t the Choleski's low triangular matrix
 *  corresponding to s.
 */
//--------------------------------------------------------------------
{
  BMatrix<double> A, L;
  A = a;
  int res = TolLapack::dpotrf(CblasLower,A,L);
  l = L;
  return(res==0);
}


//--------------------------------------------------------------------
BBool Inverse(const BSymMatrix<double>& s, BSymMatrix<double>& inv)
//--------------------------------------------------------------------
{
  BMatrix<double> A=s;
  BMatrix<double> L, Ai;
  int res1 = TolLapack::dpotrf(CblasLower,A,L);
  int res2 = TolLapack::dpotri(CblasLower,L,Ai);
  inv = Ai;
  return((res1 == 0)&&(res2 == 0));
}        

//--------------------------------------------------------------------
void Inverse(const BLowTrMatrix<double>& m, BLowTrMatrix<double>& inv)
//--------------------------------------------------------------------
{
  BMatrix<double> L  = m;
  BMatrix<double> Li;
  BMatrix<double> I = BDiagMatrix<double>(L.Rows(),1);
  TolBlas::dtrsm(CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, 
                 1.0, L, I, Li);
  inv = Li;
}


//--------------------------------------------------------------------
BBool Choleski(const BSymMatrix<BDat>& a,
	             BLowTrMatrix<BDat>& l)
//--------------------------------------------------------------------
{
  return(Choleski((const BSymMatrix<double>&)a,(BLowTrMatrix<double>&)l));
}  

//--------------------------------------------------------------------
BBool Inverse(const BSymMatrix<BDat>& s, BSymMatrix<BDat>& inv)
//--------------------------------------------------------------------
{
  return(Inverse((const BSymMatrix<double>&)s,(BSymMatrix<double>&)inv));
}        

//--------------------------------------------------------------------
void Inverse(const BLowTrMatrix<BDat>& m, BLowTrMatrix<BDat>& inv)
//--------------------------------------------------------------------
{
  return(Inverse((const BLowTrMatrix<double>&)m,(BLowTrMatrix<double>&)inv));
}  

