/* tol_butrimat.h: Definition of the class BUpTrMatrix.
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

#ifndef TOL_BUTRIMAT_H
#define TOL_BUTRIMAT_H 1

#include <tol/tol_matrix_previous.h>

//--------------------------------------------------------------------
template <class Any> class  BUpTrMatrix: public BMatrix<Any>

/*! Representation of upper triangular  matrices.
 */
//--------------------------------------------------------------------
{
  public:
  //Constructors & Destructor:
  BUpTrMatrix();
  BUpTrMatrix(BInt r): BMatrix<Any>() { Alloc(r); }
  void PutData(const BMatrix<Any>& m, BBool up=BTRUE);
  void PutData(const BSymMatrix<Any>& m);
  BUpTrMatrix(const BUpTrMatrix<Any>& m);
 ~BUpTrMatrix();
  BMatrixStoreType StoreType() const { return(BMST_uppTriang_); }

  // Access & manipulation:
  void Alloc   (BInt r);

  BMatrix<Any>& Copy(const BMatrix<Any>* m);
  BUpTrMatrix<Any>& operator = (const  BMatrix	    <Any>& m);
  BUpTrMatrix<Any>& operator = (const  BSymMatrix   <Any>& m);
  BUpTrMatrix<Any>& operator = (const  BLowTrMatrix <Any>& m);
  BUpTrMatrix<Any>& operator = (const  BUpTrMatrix  <Any>& m);
  BUpTrMatrix<Any>& operator = (const  BDiagMatrix  <Any>& m);


  // Operations:
  Any  Get	  (BInt i, BInt j) const;
  Any& operator() (BInt i, BInt j) const;

  //Operations:

  BLowTrMatrix<Any> T () const; // Transpose

  BUpTrMatrix<Any>& operator += (const BUpTrMatrix<Any>& m1);
  BUpTrMatrix<Any>& operator -= (const BUpTrMatrix<Any>& m1);
  BUpTrMatrix<Any>& operator *= (const Any& m1);
  BUpTrMatrix<Any>& operator /= (const Any& m1);
  BUpTrMatrix<Any>  operator +	(const BUpTrMatrix<Any>& m1);
  BUpTrMatrix<Any>  operator -	(const BUpTrMatrix<Any>& m1);
  BUpTrMatrix<Any>  operator *	(const Any& m1);
  BUpTrMatrix<Any>  operator /	(const Any& m1);
};

#endif // TOL_BUTRIMAT_H
