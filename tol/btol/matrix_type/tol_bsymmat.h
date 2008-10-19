/* tol_bsymmat.h: Definition of the class BMatrix.
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

#ifndef TOL_BSYMAT_H
#define TOL_BSYMAT_H 1

#include <tol/tol_matrix_previous.h>

//--------------------------------------------------------------------
template <class Any> class  BSymMatrix: public BMatrix<Any>

/*! Representation of symmetric matrices.
 */
//--------------------------------------------------------------------
{
public:
  // Constructors & Destructor:
  BSymMatrix();
  BSymMatrix(BInt r):BMatrix<Any>() { Alloc(r); }
  BSymMatrix(const BSymMatrix<Any>& m);
 ~BSymMatrix();
  BMatrixStoreType StoreType() const { return(BMST_symmetric_); }

  // Access & manipulation:

  void Alloc   (BInt r);

  BMatrix<Any>& Copy(const BMatrix<Any>* m);
  BSymMatrix<Any>& operator = (const  BMatrix	   <Any>& m);
  BSymMatrix<Any>& operator = (const  BSymMatrix   <Any>& m);
  BSymMatrix<Any>& operator = (const  BLowTrMatrix <Any>& m);
  BSymMatrix<Any>& operator = (const  BUpTrMatrix  <Any>& m);
  BSymMatrix<Any>& operator = (const  BDiagMatrix  <Any>& m);

  // Operations:
  Any  Get	  (BInt i, BInt j) const;
  Any& operator() (BInt i, BInt j) const;

  BSymMatrix<Any>& operator += (const BSymMatrix<Any>& m1);
  BSymMatrix<Any>& operator -= (const BSymMatrix<Any>& m1);
  BSymMatrix<Any>& operator *= (const Any& m1);
  BSymMatrix<Any>& operator /= (const Any& m1);

  BSymMatrix<Any>  operator +  (const  BSymMatrix<Any>& m1)const;
  BSymMatrix<Any>  operator -  (const  BSymMatrix<Any>& m1)const;
  BSymMatrix<Any>  operator *  (const Any& m1) const;
  BSymMatrix<Any>  operator /  (const Any& m1) const;

  BSymMatrix<Any> T() const; // Transpose

/*
  friend void MMTSquare(const BMatrix<Any>& m,
			       BSymMatrix<Any>& sq);
  friend void MTMSquare(const BMatrix<Any>& m,
				BSymMatrix<Any>& sq);

  friend BBool Choleski(const BSymMatrix<Any>& a, BLowTrMatrix<Any>& l);
  friend BBool Inverse (const BSymMatrix<Any>& s, BSymMatrix  <Any>& inv);

  friend BBool SymSystemSolver(	     BMatrix<Any>&	    solution,
			       BLowTrMatrix<Any>& chol,
			 const BSymMatrix<Any>&	      m,
			 const BMatrix<Any>&	      d);

  friend BBool LeastSqrCholeski(BMatrix<Any>& c, BArray<Any>& d,
			  BArray<Any>&	x, BArray<Any>& r,
			  BLowTrMatrix<Any>& chol);
*/

};

#endif // TOL_BSYMMAT_H
