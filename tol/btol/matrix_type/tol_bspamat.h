/* tol_bspamat.h: Definition of the class BMatrix.
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

#ifndef TOL_BSPAMAT_H
#define TOL_BSPAMAT_H 1

#include <tol/tol_matrix_previous.h>

template <class Any> struct TOL_API BCellRef { Any& x_; BInt i_; BInt j_; };

//--------------------------------------------------------------------
template <class Any> class  BSpaMatrix: public BMatrix<Any>

/*! Representation of symmetric matrices.
 */
//--------------------------------------------------------------------
{
public:
  // Constructors & Destructor:
  BSpaMatrix();
  BSpaMatrix(const BMatrix<Any>&    m);
  BSpaMatrix(const BSpaMatrix<Any>& m);
 ~BSpaMatrix();

  BMatrixStoreType StoreType() const { return(BMST_sparse_); }

  // Access & manipulation:
  const BArray< BInt > & NotNullRows() const
  { return(notNullRows_); }
  const BArray< BArray< BIndexed<Any> > >& Vectors() const
  { return(vectors_); }
  void Alloc(BInt r, BInt c);

  BMatrix<Any>& Copy(const BMatrix<Any>* m);
  BSpaMatrix<Any>& operator = (const  BMatrix	   <Any>& m);
  BSpaMatrix<Any>& operator = (const  BSpaMatrix   <Any>& m);
  BSpaMatrix<Any>& operator = (const  BSymMatrix   <Any>& m);
  BSpaMatrix<Any>& operator = (const  BLowTrMatrix <Any>& m);
  BSpaMatrix<Any>& operator = (const  BUpTrMatrix  <Any>& m);
  BSpaMatrix<Any>& operator = (const  BDiagMatrix  <Any>& m);

  BMatrix<Any>	operator * (const BMatrix    <Any>& m);
  BMatrix<Any> MBtProd(const BSpaMatrix <Any>& B);
  BMatrix<Any> MMtSqr();

  int NotNullCells() const { return(notNullCells_); }

/*
  // Operations:
  Any  Get	  (BInt i, BInt j) const;
  Any& operator() (BInt i, BInt j) const;

  BSpaMatrix<Any>& operator += (const BSpaMatrix<Any>& m1);
  BSpaMatrix<Any>& operator -= (const BSpaMatrix<Any>& m1);
  BSpaMatrix<Any>& operator *= (const Any& m1);
  BSpaMatrix<Any>& operator /= (const Any& m1);

  BSpaMatrix<Any>  operator +  (const  BSpaMatrix<Any>& m1)const;
  BSpaMatrix<Any>  operator -  (const  BSpaMatrix<Any>& m1)const;
  BSpaMatrix<Any>  operator *  (const Any& m1) const;
  BSpaMatrix<Any>  operator /  (const Any& m1) const;

  BSpaMatrix<Any> T() const; // Transpose
*/
protected:
//BBool trasposed_;
//BArray< BCell<Any> > cells_;
  int notNullCells_;
  BArray< BInt > notNullRows_;
  BArray< BArray< BIndexed<Any> > > vectors_;
};


#endif // TOL_BSPAMAT_H
