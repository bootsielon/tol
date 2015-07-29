/* tol_bcomplex.h: BComplex (z) class and functions header.
                   GNU/TOL Language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BCOMPLEX_H
#define TOL_BCOMPLEX_H 1

#include <tol/tol_bgencon.h>
#include <tol/tol_bpolyn.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define Ci BComplex::I()

struct tcomplex
{
  double x;
  double y;
};


//--------------------------------------------------------------------
class TOL_API BComplex

/*! Scalar real za representation. Infinite, complex and undeter-
 *  minate vaues are represented as value called unknown.
 */
//--------------------------------------------------------------------
{
private:
  BDat x_;
  BDat y_;
  BDat r_;
  BDat a_;
  BInt xr_;
  static BComplex  i_;
  static BComplex  unknown_;

  void	ToXYNC();
  void	ToRANC();

 private:
  //Overloaded algebraic operators and functions

public:

  // Constructors and destructors: inline
  BComplex() : x_(0), y_(0), r_(0), a_(0), xr_(6) {}
//BComplex(BDat x) { PutReal(x); }
//BComplex(BReal x) { PutReal(x); }
//BComplex(BInt x) { PutReal(x); }
  BComplex(BDat x, BDat y) : x_(x), y_(y), xr_(2)
  {
  //ToRA();
  }
  BComplex(tcomplex z) : x_(z.x), y_(z.y), xr_(2)
  {
  //ToRA();
  }
  BComplex(BDat r, BDat a, BInt xr) : r_(r), a_(Mod(a,2*BDat::Pi())), xr_(3)
  {
  //ToXY();
  }
 ~BComplex() {}

  void PutReal(BDat x);
  static BComplex  RC(BDat x) { BComplex z; z.PutReal(x); return(z); }
  static BComplex  XY(BDat x, BDat y) { return(BComplex(x,y)); }
  static BComplex  RA(BDat r, BDat a) { return(BComplex(r,a,3)); }

  // Access & Manipulation: inline
  static BComplex& I();
  static BComplex& Unknown();
  static BInt Compare(const BComplex* z1, const BComplex* z2) {
      BComplex* nc1 = (BComplex*) z1;
      BComplex* nc2 = (BComplex*) z2;
      return(( (*nc1) == (*nc2) ) ? 0 : 1);
  }

  BText Name() const;
  BText Dump() const { return(Name()); }
  BText Info() const { return(Name()); }

  int BinWrite(FILE*) const { return 0; }
  int BinRead (FILE*)       { return 0; }

  BBool IsXY() const { return(xr_%2==0); }
  BBool IsRA() const { return(xr_%3==0); }

  void	ToXY() const;
  void	ToRA() const;

  BDat X() const { ToXY(); return(x_); }
  BDat Y() const { ToXY(); return(y_); }
  BDat R() const { ToRA(); return(r_); }
  BDat A() const { ToRA(); return(a_); }

  BBool	    IsKnown  () const;
  BBool	    IsUnknown() const { return(!IsKnown());  }

  private:
   //Overloaded algebraic operators and functions
  friend BBool operator==(const BComplex& cmp1, const BComplex& cmp2)
  {
    return ((cmp1.x_==cmp2.x_) && (cmp1.y_==cmp2.y_));
  }

};



//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------


BBool operator != (BComplex z1, BComplex z2);
BComplex operator +  (BComplex z1, BComplex z2);
BComplex operator -  (BComplex z1, BComplex z2);
BComplex operator *  (BComplex z1, BComplex z2);
BComplex operator /  (BComplex z1, BComplex z2);
BComplex operator ^  (BComplex z1, BComplex z2);
BComplex operator -  (BComplex z);
BComplex Conj  (const BComplex& z);
BComplex Sqrt	     (BComplex z);
BComplex Log	     (BComplex z);
BComplex Exp	     (BComplex z);
BComplex Sin	     (BComplex);
BComplex Cos	     (BComplex);
BComplex Tan	     (BComplex);
BComplex ASin	     (BComplex);
BComplex ACos	     (BComplex);
BComplex ATan	     (BComplex);
BComplex SinH	     (BComplex);
BComplex CosH	     (BComplex);
BComplex TanH	     (BComplex);
BComplex ASinH	     (BComplex);
BComplex ACosH	     (BComplex);
BComplex ATanH	     (BComplex);


//--------------------------------------------------------------------
// inline functions
//--------------------------------------------------------------------
inline BText& operator<< (BText& txt, const BComplex& z)
{ return(txt << (z.Name())); }

inline ostream& operator<< (ostream& os, const BComplex& z)
{
    BText txt =  z.Name();
    return(os << txt.String());
}

inline BBool IsKnown(BComplex& z)
{ return(z.IsKnown()); }


inline BBool operator == (const BComplex& z1, const BDat& z2)
{ return(z1==BComplex(z2,0)); }
inline BBool operator != (const BComplex& z1, const BDat& z2)
{ return(z1!=BComplex(z2,0)); }
inline BComplex operator +  (const BComplex& z1, const BDat& z2)
{ return(z1+BComplex(z2,0)); }
inline BComplex operator -  (const BComplex& z1, const BDat& z2)
{ return(z1-BComplex(z2,0)); }
inline BComplex operator *  (const BComplex& z1, const BDat& z2)
{ return(z1*BComplex(z2,0)); }
inline BComplex operator /  (const BComplex& z1, const BDat& z2)
{ return(z1/BComplex(z2,0)); }
inline BComplex operator ^  (const BComplex& z1, const BDat& z2)
{ return(z1^BComplex(z2,0)); }

inline BBool operator == (const BDat& z1, const BComplex& z2)
{ return(BComplex(z1,0)==z2); }
inline BBool operator != (const BDat& z1, const BComplex& z2)
{ return(BComplex(z1,0)!=z2); }
inline BComplex operator +  (const BDat& z1, const BComplex& z2)
{ return(BComplex(z1,0)+z2); }
inline BComplex operator -  (const BDat& z1, const BComplex& z2)
{ return(BComplex(z1,0)-z2); }
inline BComplex operator *  (const BDat& z1, const BComplex& z2)
{ return(BComplex(z1,0)*z2); }
inline BComplex operator /  (const BDat& z1, const BComplex& z2)
{ return(BComplex(z1,0)/z2); }
inline BComplex operator ^  (const BDat& z1, const BComplex& z2)
{ return(BComplex(z1,0)^z2); }

int ComplexRootCmp(const void* v1, const void* v2);

void tol_gsl_poly_complex_solve(
  const BPolyn<BDat>& pol, 
  BArray<tcomplex>& row);

void tol_gsl_poly_complex_solve(
  const BPolyn<BDat>& pol, 
  BArray<BComplex>& row);

#endif // TOL_BCOMPLEX_H 
