/* tol_bdiagmat.h: Definition of the class BDiagMatrix.
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

#ifndef TOL_BDIAGMAT_H
#define TOL_BDIAGMAT_H 1

#include <tol/tol_matrix_previous.h>

//--------------------------------------------------------------------
template <class Any> class  BDiagMatrix: public BMatrix<Any>

/*! Representation of diagonal matrices.
 */
//--------------------------------------------------------------------
{
  public:
  //Constructors & Destructor:
  BDiagMatrix();
  BDiagMatrix(BInt r, const Any& lambda);
  BDiagMatrix(const BDiagMatrix<Any>& m);
 ~BDiagMatrix();
  BMatrixStoreType StoreType() const { return(BMST_diagonal_); }

  //Access & manipulation:
  void Alloc(BInt r);

  BMatrix<Any>& Copy       (const  BMatrix	    <Any>* m);
  BDiagMatrix<Any>& operator = (const  BDiagMatrix  <Any>& m);
  BDiagMatrix<Any>& operator = (const  BMatrix	    <Any>& m);
  BDiagMatrix<Any>& operator = (const  BSymMatrix   <Any>& m);
  BDiagMatrix<Any>& operator = (const  BLowTrMatrix <Any>& m);
  BDiagMatrix<Any>& operator = (const  BUpTrMatrix  <Any>& m);

  Any  Get	  (BInt i, BInt j) const;
  Any& operator() (BInt i, BInt j) const;

  BDiagMatrix<Any>& PutColumnsNorm(const BMatrix<Any>& m);
  BDiagMatrix<Any>  GetColumnsNorm(const BMatrix<Any>& m);
  BDiagMatrix<Any>& PutColumnsMaxAbs(const BMatrix<Any>& m);
  BDiagMatrix<Any>  GetColumnsMaxAbs(const BMatrix<Any>& m);
  BDiagMatrix<Any>& PutDiag	  (const BMatrix<Any>& m, BInt d=0);
  BDiagMatrix<Any>  GetDiag	  (const BMatrix<Any>& m, BInt d=0);

  //Operations:

  BDiagMatrix<Any>& operator += (const BDiagMatrix<Any>& m1);
  BDiagMatrix<Any>& operator -= (const BDiagMatrix<Any>& m1);
  BDiagMatrix<Any>& operator *= (const BDiagMatrix<Any>& m1);
  BDiagMatrix<Any>& operator /= (const BDiagMatrix<Any>& m1);
  BDiagMatrix<Any>& operator += (const Any& x);
  BDiagMatrix<Any>& operator -= (const Any& x);
  BDiagMatrix<Any>& operator *= (const Any& x);
  BDiagMatrix<Any>& operator /= (const Any& x);
  BDiagMatrix<Any>& operator ^= (const Any& x);
  BDiagMatrix<Any>  operator +	(const BDiagMatrix<Any>& M) const;
  BDiagMatrix<Any>  operator -	(const BDiagMatrix<Any>& M) const;
  BDiagMatrix<Any>  operator *	(const BDiagMatrix<Any>& M) const;
  BDiagMatrix<Any>  operator /	(const BDiagMatrix<Any>& M) const;
  BDiagMatrix<Any>  operator +	(const Any& x) const;
  BDiagMatrix<Any>  operator -	(const Any& x) const;
  BDiagMatrix<Any>  operator *	(const Any& x) const;
  BDiagMatrix<Any>  operator /	(const Any& x) const;
  BDiagMatrix<Any>  operator ^	(const Any& x) const;
  BDiagMatrix<Any> I() const; // Inverse
  BDiagMatrix<Any> P(const Any chop) const; // Pseudoinverse
  BDiagMatrix<Any> T() const; // Transpose

  friend BDiagMatrix<Any> Sqrt	(const BDiagMatrix<Any>& m)
  {
    return(m^(Any(0.5)));
  }

  friend BDiagMatrix<Any> operator/(const Any& x, const BDiagMatrix<Any>& D)
  {
    return(D.I()*x);
  }

};

#endif // TOL_BDIAGMAT_H
