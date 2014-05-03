/* tol_bmatrixgen.h: Definition of the class BMatrixGen.
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

#ifndef TOL_BMATRIXGEN_H
#define TOL_BMATRIXGEN_H 1

#include <tol/tol_matrix_previous.h>
#include <tol/tol_bindex.h>
#include <tol/tol_barith.h>

//--------------------------------------------------------------------
template <class Any> class BMatrixGen

/*! Representation of matrices with generic type elements.
 */
//--------------------------------------------------------------------
{
public:
  //Constructors & Destructor:
  BMatrixGen();
  BMatrixGen(BInt r, BInt c, const BArray<BArray<Any> >& d);
  BMatrixGen(BInt r, BInt c, const Any* buffer=NIL);
  BMatrixGen(const BMatrixGen<Any>& m);
  virtual ~BMatrixGen();
  virtual  BMatrixStoreType StoreType() const { return(BMST_dynamic_); }
  //Internal manipulation
          void Alloc   (BInt r, BInt c);
  virtual void Alloc   (BInt r)		{ Alloc(r,r); }

  virtual BMatrixGen<Any>& Copy(const BMatrixGen<Any>* m);
  BMatrixGen<Any>& operator = (const  BMatrixGen<Any>& m);

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

  BMatrixGen<Any> Sub(BInt i0, BInt j0, BInt rows, BInt columns) const;
  BMatrixGen<Any> SafeSub(BInt i0, BInt j0, BInt rows, BInt columns) const;
  BMatrixGen<Any> SubRow(BInt row) const
  {
    return(Sub(row,0,1,Columns()));
  }
  BMatrixGen<Any> SubCol(BInt col) const
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
  BMatrixGen<Any> T() const;   // Transpose


  BMatrixGen<Any>& operator += (const  BMatrixGen<Any>& m1);
  BMatrixGen<Any>& operator -= (const  BMatrixGen<Any>& m1);
  BMatrixGen<Any>& operator += (const Any& m1);
  BMatrixGen<Any>& operator -= (const Any& m1);
  BMatrixGen<Any>& operator *= (const Any& m1);
  BMatrixGen<Any>	operator -  () const;
  BMatrixGen<Any>	operator +  (const  BMatrixGen<Any>& m1) const;
  BMatrixGen<Any>	operator -  (const  BMatrixGen<Any>& m1) const;
  BMatrixGen<Any>	operator +  (const Any& m1) const;
  BMatrixGen<Any>	operator -  (const Any& m1) const;
  BMatrixGen<Any>	operator *  (const Any& m1) const;
  void	Reverse();
  BMatrixGen<Any>	PermutateRows	(const BArray<BInt>& perm);
  BMatrixGen<Any>	PermutateColumns(const BArray<BInt>& perm);
  Any Determinant() const;

  static BMatrixGen<Any>& Unknown();
  static BDat Compare(const BMatrixGen<Any>*, const BMatrixGen<Any>*) {
      return(0);
  }

  static Any& NullValue ();
  static void Initialize();

protected:
  BArray<BInt> firstOfRow_;
  BArray<Any> data_;
  Any* buffer_;
  BInt* firstBuffer_;
  BInt rows_, columns_;

};

#endif // TOL_BMATRIXGEN_H
