/* tol_bmatrix.h: Definition of the class BMatrix.
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

#ifndef TOL_BMATRIX_H
#define TOL_BMATRIX_H 1

#include <tol/tol_matrix_previous.h>
#include <tol/tol_bindex.h>
#include <tol/tol_barith.h>

template <class Any> class BUpTrMatrix;
template <class Any> class BDiagMatrix;


//--------------------------------------------------------------------
template <class Any> class BMatrix

/*! Representation of matrices with generic type elements.
 */
//--------------------------------------------------------------------
{
public:
  //Constructors & Destructor:
  BMatrix();
  BMatrix(BInt r, BInt c, const BArray<BArray<Any> >& d);
  BMatrix(BInt r, BInt c, const Any* buffer=NIL);
  BMatrix(const BMatrix<Any>& m);
  virtual ~BMatrix();
  virtual  BMatrixStoreType StoreType() const { return(BMST_dense_); }
  //Internal manipulation
          void Alloc   (BInt r, BInt c);
  virtual void Alloc   (BInt r)		{ Alloc(r,r); }

  virtual BMatrix<Any>& Copy(const BMatrix<Any>* m);
  BMatrix<Any>& operator = (const  BMatrix	<Any>& m);
  BMatrix<Any>& operator = (const  BSymMatrix	<Any>& m);
  BMatrix<Any>& operator = (const  BLowTrMatrix <Any>& m);
  BMatrix<Any>& operator = (const  BUpTrMatrix	<Any>& m);
  BMatrix<Any>& operator = (const  BDiagMatrix	<Any>& m);

  void SetAllValuesTo(const Any& x);

  //Access & manipulation:
  BInt Rows   () const { return(rows_   );}
  BInt Columns() const { return(columns_);}

  virtual Any  Get	  (BInt i, BInt j) const
  {
  //BMAT_ENSURE_DIM(i,j);
    if((i<0)||(j<0)||(i>=rows_)||(j>=columns_))
    { return(BArray<Any>::Overflow()); }
    else
    { return(buffer_[firstBuffer_[i]+j]); }
  }

  virtual Any& operator() (BInt i, BInt j) const
  {
    BMAT_ENSURE_DIM(i,j);
    return(buffer_[firstBuffer_[i]+j]);
  }

  void EnsureDimensions(BInt i, BInt j) const;

  const BArray<Any >& Data   () const { return(data_); }
        BArray<Any >& GetData()       { return(data_); }

  BText MatToText() const { return(Name()); }
  BText Name () const;
  BText Dump () const { return(Name()); }
  BText Info () const { return(BText("")+Rows()+"x"+Columns()); }

  BBool IsKnown() const;

  BMatrix<Any> Sub(BInt i0, BInt j0, BInt rows, BInt columns) const;
  BMatrix<Any> SafeSub(BInt i0, BInt j0, BInt rows, BInt columns) const;
  BMatrix<Any> SubRow(BInt row) const
  {
    return(Sub(row,0,1,Columns()));
  }
  BMatrix<Any> SubCol(BInt col) const
  {
    return(Sub(0,col,Rows(),1));
  }

  int BinWrite(FILE*) const { return 0; }
  int BinRead (FILE*)       { return 0; };

  //Operations:
  Any  Max	    () const;
  Any  Min	    () const;
  Any  MaxAbsNorm   () const;
  Any  FrobeniusNorm() const;
  Any  NotNull	    () const;
  void NormalizeRows();
  void NormalizeRows(BMatrix<Any>& m);

  BMatrix<Any> T() const;   // Transpose


  BMatrix<Any>& operator += (const  BMatrix<Any>& m1);
  BMatrix<Any>& operator -= (const  BMatrix<Any>& m1);
  BMatrix<Any>& operator += (const Any& m1);
  BMatrix<Any>& operator -= (const Any& m1);
  BMatrix<Any>& operator *= (const Any& m1);
  BMatrix<Any>& operator /= (const Any& m1);
  BMatrix<Any>	operator -  () const;
  BMatrix<Any>	operator +  (const  BMatrix<Any>& m1) const;
  BMatrix<Any>	operator -  (const  BMatrix<Any>& m1) const;
  BMatrix<Any>	operator +  (const Any& m1) const;
  BMatrix<Any>	operator -  (const Any& m1) const;
  BMatrix<Any>	operator *  (const Any& m1) const;
  BMatrix<Any>	operator /  (const Any& m1) const;

  Any Determinant() const;

  BInt	GaussReduction (BInt firstColumns);
  BBool Solve	       (const BMatrix<Any>& x, BMatrix<Any>& y);
  BInt	LinCombSpace   (BMatrix<Any>& equations);
  BBool Inverse	       (BMatrix<Any>& X);

  void	Reverse();
  BMatrix<Any>	PermutateRows	(const BArray<BInt>& perm);
  BMatrix<Any>	PermutateColumns(const BArray<BInt>& perm);

  static BMatrix<Any>& Unknown();
  static BDat Compare(const BMatrix<Any>*, const BMatrix<Any>*) {
      return(0);
  }
/*
  friend void  MatrixMult(const	 BMatrix<Any>& A,
				 const	BMatrix<Any>& B,
					BMatrix<Any>& C) ;

  friend BMatrix<Any>  ConcatRows   (const BMatrix<Any>& m1,
					    const BMatrix<Any>& m2);
  friend BMatrix<Any>  ConcatColumns(const BMatrix<Any>& m1,
					    const BMatrix<Any>& m2);

  friend BMatrix<Any>  operator <<  (const BMatrix<Any>& m1,
					    const BMatrix<Any>& m2);

  friend BMatrix<Any>  operator |  (const BMatrix<Any>& m1,
					   const BMatrix<Any>& m2);

  friend BBool	LeastSqrCholeski(BMatrix<Any>& c,
					BArray <Any>& d,
					BArray <Any>& x,
					BArray <Any>& r);

  friend BBool	LeastSqrGivens(BMatrix<Any>& c,
				      BArray <Any>& d,
				      BArray <Any>& x,
				      BArray <Any>& r);

  friend BBool	HouseholderTransformation(BArray <Any>& t,
						 BMatrix<Any>& c);
  friend void	LeastSqrHouseholder(const BArray <Any>& t,
					   const BMatrix<Any>& c,
					   const BArray <Any>& d,
						 BArray <Any>& x,
						 BArray <Any>& r);

  friend ostream&  operator << (ostream& os, const BMatrix<Any>& mat);
  friend void  Std(const BMatrix<Any>& mat);
*/
  BInt GaussReduction (BInt firstColumns, BInt** reducted);
  static Any& NullValue ();
  static void Initialize();

protected:
  BArray<BInt> firstOfRow_;
  BArray<Any> data_;
  Any* buffer_;
  BInt* firstBuffer_;
  BInt rows_, columns_;

};



#endif // TOL_BMATRIX_H
