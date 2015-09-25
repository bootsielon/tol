/* tol_bmonome.h: BMonome (mon) class and functions header.
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

#ifndef TOL_BMONOME_H
#define TOL_BMONOME_H 1

#include <tol/tol_bgencon.h>

//--------------------------------------------------------------------
template <class Any>
class BMonome

/*! Generic monomial functions representation. "Any" must be a
 *  numerical class.
 */
//--------------------------------------------------------------------
{
private:
  Any  coef_;	   // The coeficient of the variable.
  BInt degree_;	   // The exponent of the variable.

public:
  // Constructors & destructors:
  BMonome  (const Any& coef=Any::Unknown(), BInt degree=0)
      : coef_(coef), degree_(degree) {}
  BMonome  (const BMonome<Any>& mon)
      : coef_(mon.Coef()), degree_(mon.Degree()) {}
  
  // Access & Manipulation: inline
  BInt	      Degree	() const       { return(degree_); }
  const Any&  Coef	() const       { return(coef_); }
  void	      PutDegree (BInt g)       { degree_=g; }
  void	      PutCoef	(const Any& c) { coef_ = c; }
  BText	      ToBText	(const BText& var="B") const ;

  // Miscelaneus internal operations:
  BMonome<Any>& operator =  (const BMonome<Any>&  mon);
  BMonome<Any>& operator *= (const BMonome<Any>&  mon);
  BMonome<Any>& operator += (const BMonome<Any>&  mon);
  BMonome<Any>& operator -= (const BMonome<Any>&  mon);
  BMonome<Any>& operator ^= (BInt exponent);
  BMonome<Any>	operator +  (const BMonome<Any>& mon);
  BMonome<Any>	operator -  (const BMonome<Any>& mon);
  BMonome<Any>	operator *  (const BMonome<Any>& mon);
  BMonome<Any>	operator ^  (BInt exponent);

  BMonome<Any>	operator -  ();

  // Miscelaneus static functions:
  static BBool AreAdditive(const BMonome<Any>& mon1,
			   const BMonome<Any>& mon2);

  static BBool HasEqualDegree(const BMonome<Any>& mon1,
			      const BMonome<Any>& mon2)
  { return (mon1.Degree()==mon2.Degree()); }

  static BInt BMonomeOrder(const BAny any1,
			   const BAny any2)
  {
    BInt d1 = ((const BMonome<Any>*)any1)->Degree();
    BInt d2 = ((const BMonome<Any>*)any2)->Degree();
    return((d1 < d2) ? -1 : ((d1 > d2) ? 1 : 0));
  }

  static BMonome<Any> Sum(const BMonome<Any>* mon, BInt from, BInt until);

  // static generators for special instances
  static const BMonome<Any>& X	  ();
  static const BMonome<Any>& One  ();
  static const BMonome<Any>& Zero ();
};


//--------------------------------------------------------------------
// template-class implementation
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
BMonome<Any>& BMonome<Any>::operator = (const BMonome<Any>&	mon )

/*! Destroies this, copies mon in *this and returns *this.
 */
//--------------------------------------------------------------------
{
    coef_=mon.Coef();
    degree_=mon.Degree();
    return (*this);
}


//--------------------------------------------------------------------
template <class Any>
BBool BMonome<Any>::AreAdditive(const BMonome<Any>& mon1,
				const BMonome<Any>& mon2)

/*! Returns BTRUE if mon1 and mon2 have the same degree or
 *  any of their coefficients is null. Else returns BFALSE.
 */
//--------------------------------------------------------------------
{
    return((mon1.Coef()==Any(0.0)) || (mon2.Coef()==Any(0.0)) ||
	   (mon1.Degree()==mon2.Degree()));
}


//--------------------------------------------------------------------
template <class Any>
BMonome<Any>& BMonome<Any>::operator += (const BMonome<Any> &mon)

/*! Sums *this with mon and puts the result in *this.
 *  This is only possible if *this and mon are additives.
 *  If the final result has null coeficient puts it null degree.
 *  Finally the function return *this.
 */
//--------------------------------------------------------------------
{
    if(AreAdditive(*this, mon)) { coef_+=mon.Coef(); }
    if(!Degree())		{ PutDegree(mon.Degree()); }
    if(coef_==Any(0.0))	        { degree_=0; }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BMonome<Any>& BMonome<Any>::operator -= (const BMonome<Any> &mon)

/*! Substracts mon from *this and puts the result in *this.
 *  This is only possible if *this and mon are additives.
 *  If the final result has null coeficient puts it null degree.
 *  Finally the function return *this.
 */
//--------------------------------------------------------------------
{
    if(AreAdditive(*this, mon)) { coef_+=mon.Coef(); }
    if(!Degree())		{ PutDegree(mon.Degree()); }
    if(coef_==Any(0.0))	        { degree_=0; }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BMonome<Any>& BMonome<Any>::operator *= (const BMonome<Any> &mon)

/*! Multiplies *this with mon, puts the result in *this.
 *  If the final result has null coeficient puts it null degree.
 *  Finally the function return *this.
 */
//--------------------------------------------------------------------
{
    degree_+=mon.Degree();
    coef_*=mon.Coef();
    if(coef_==Any(0.0)) { degree_=0; }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BMonome<Any>& BMonome<Any>::operator ^= (BInt exponent)

/*! Powers *this to exponent, puts the result in *this and returns
 *  *this if *this is not null. Else this function has no effect.
 */
//--------------------------------------------------------------------
{
    if(coef_==Any(0.0)) { degree_=0; }
    else
    {
	degree_ *= exponent;
	coef_   ^= exponent;
    }
    return(*this);
}



//--------------------------------------------------------------------
template <class Any> BMonome<Any> BMonome<Any>::operator - ()

/*! Returns a copy of mon with the sign changed.
 */
//--------------------------------------------------------------------
{
    BMonome<Any> mon1;
    mon1.PutCoef(-Coef());
    mon1.PutDegree(Degree());
    return(mon1);
}


//--------------------------------------------------------------------
template <class Any>
BMonome<Any> BMonome<Any>::operator + ( const BMonome<Any> &mon)

/*! Returns a copy of *this += mon (see operator +=).
 */
//--------------------------------------------------------------------
{
    return(BMonome<Any>(*this)+=mon);
}


//--------------------------------------------------------------------
template <class Any>
BMonome<Any> BMonome<Any>::operator - ( const BMonome<Any> &mon)

/*! Returns a copy of *this -= mon (see operator -=).
 */
//--------------------------------------------------------------------
{
    return(BMonome<Any>(*this)-=mon);
}


//--------------------------------------------------------------------
template <class Any>
BMonome<Any> BMonome<Any>::operator * ( const BMonome<Any> &mon)

/*! Returns a copy of *this *= mon (see operator *=).
 */
//--------------------------------------------------------------------
{
    return(BMonome<Any>(*this)*=mon);
}


//--------------------------------------------------------------------
template <class Any>
BMonome<Any> BMonome<Any>::operator ^  (BInt exponent)

/*! Returns a copy of *this ^= exponent (see operator ^=).
 */
//--------------------------------------------------------------------
{
    return (BMonome<Any>(*this)^=exponent);
}


//--------------------------------------------------------------------
template <class Any>
BMonome<Any> BMonome<Any>::Sum(const BMonome<Any>* buf, BInt from,BInt until)

/*! Sums the elements of buf with indexes between from and until.
 */
//--------------------------------------------------------------------
{
    BMonome<Any> sum = buf[from];
    for(BInt i=from+1; i<=until; i++)
    { sum += buf[i]; }
    return (sum);
}


//--------------------------------------------------------------------
template <class Any>
const BMonome<Any>& BMonome<Any>::X ()

/*! Returns the identity monome 1*x^1.
 */
//--------------------------------------------------------------------
{
    static BMonome<Any> x_(1,1);
    return(x_);
}


//--------------------------------------------------------------------
template <class Any>
const BMonome<Any>& BMonome<Any>::One ()

/*! Returns the unitary nomome 1*x^0.
 */
//--------------------------------------------------------------------
{
    static BMonome<Any> one_(1,0);
    return(one_);
}


//--------------------------------------------------------------------
template <class Any>
const BMonome<Any>& BMonome<Any>::Zero ()

/*! Returns the null nomome 0*x^0.
 */
//--------------------------------------------------------------------
{
    static BMonome<Any> zero_(Any(0.0),0);
    return(zero_);
}


//--------------------------------------------------------------------
template <class Any>
BText BMonome<Any>::ToBText(const BText& var) const

/*! Creates a BText containing a monomial expression in var
 *  of *this.
 */
//--------------------------------------------------------------------
{
    BText txt;
    Any  a = Abs(Coef());
    Any  s = Sign(Coef());
    if(s>=Any(0))	   { txt = "+"; }
    else		   { txt = "-"; }
    if (Degree()==0)       { txt += a.Name(); }
    else 
    {
	if(a!=Any(1.0)) { txt += a.Name() + "*"; }
	txt += var;
	if(Degree()>1)
	{
	    BChar degree[30];
	    sprintf(degree, "%d", Degree());
	    txt << "^" << degree;
	}
    }
    return(txt);
}

#endif // TOL_BMONOME_H
