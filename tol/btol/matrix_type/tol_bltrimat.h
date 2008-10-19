/* tol_bltrimat.h: Definition of the class BLowTrMatrix.
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

#ifndef TOL_BLTRIMAT_H
#define TOL_BLTRIMAT_H 1

#include <tol/tol_matrix_previous.h>

//--------------------------------------------------------------------
template <class Any> class  BLowTrMatrix: public BMatrix<Any>

/*! Representation of lower triangular matrices.
 */
//--------------------------------------------------------------------
{
public:
  // Constructors & Destructor:
  BLowTrMatrix();
  BLowTrMatrix(BInt r): BMatrix<Any>() { Alloc(r); }
  BLowTrMatrix(const BLowTrMatrix<Any>& m);
 ~BLowTrMatrix();

  BMatrixStoreType StoreType() const { return(BMST_lowTriang_); }
  // Access & manipulation:
  void Alloc   (BInt r);


  // Operations:
  Any  Get	  (BInt i, BInt j) const;
  Any& operator() (BInt i, BInt j) const;

  BMatrix<Any>& Copy(const BMatrix<Any>* m);
  BLowTrMatrix<Any>& operator = (const	BMatrix	     <Any>& m);
  BLowTrMatrix<Any>& operator = (const	BSymMatrix   <Any>& m);
  BLowTrMatrix<Any>& operator = (const	BLowTrMatrix <Any>& m);
  BLowTrMatrix<Any>& operator = (const	BUpTrMatrix  <Any>& m);
  BLowTrMatrix<Any>& operator = (const	BDiagMatrix  <Any>& m);


  // Operations:
  BUpTrMatrix<Any> T() const; // Transpose
  void MTMSquare(BSymMatrix<Any>& sq) const;

  BLowTrMatrix<Any>& operator += (const BLowTrMatrix<Any>& m1);
  BLowTrMatrix<Any>& operator -= (const BLowTrMatrix<Any>& m1);
  BLowTrMatrix<Any>& operator *= (const Any& m1);
  BLowTrMatrix<Any>& operator /= (const Any& m1);
  BLowTrMatrix<Any>  operator +	 (const	 BLowTrMatrix<Any>& m1);
  BLowTrMatrix<Any>  operator -	 (const	 BLowTrMatrix<Any>& m1);
  BLowTrMatrix<Any>  operator *	 (const Any& m1);
  BLowTrMatrix<Any>  operator /	 (const Any& m1);

};

#endif // TOL_BLTRIMAT_H
