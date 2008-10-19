/* tol_bration.h: Class and functions header
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

#ifndef TOL_BRATION_H
#define TOL_BRATION_H 1

#include <tol/tol_bpolyn.h>


//--------------------------------------------------------------------
template <class Any>
class  BRational

/*! Generic rational functions representation.
 */
//--------------------------------------------------------------------
{
//protected:
//  static BRational<Any> unknown_;

private:
  BPolyn<Any> numerator_;
  BPolyn<Any> denominator_;

public:

  // Constructors and destructors:
  BRational ()
	 : numerator_  (BPolyn<Any>::Zero()),
	   denominator_(BPolyn<Any>::One()) { }

  BRational (const BRational<Any>& rat)
	: numerator_   (rat.Numerator()),
	  denominator_ (rat.Denominator()) { }

  BRational operator= (const BRational<Any>&  rat)
  {
    if(this!=&rat)
    {
	  numerator_  = rat.Numerator();
	    denominator_= rat.Denominator();
    }
	  return (*this);
  }

  BBool operator == (const BRational<Any>& rat) const
  {
    return((numerator_	 == rat.Numerator  ()) &&
	   (denominator_ == rat.Denominator()));
  }

  BBool operator != (const BRational<Any>& rat) const
  {
    return((numerator_	 != rat.Numerator  ()) ||
	   (denominator_ != rat.Denominator()));
  }

  static BRational<Any>& Unknown(); //{ return(unknown_); }

  BRational (const BPolyn<Any> & P, const BPolyn<Any> & Q=BPolyn<Any>(1,0))
  {
    numerator_=P;
    denominator_=Q;
  }

  // Access & Manipulation: inline
  BText	 ToBText(const BChar* var) const
  {
    return (BText("(")+numerator_.ToBText(var)	 + ") / (" +
		       denominator_.ToBText(var) + ")");
  }

  BText Name() const { return(ToBText("B")); }
  BText Dump() const { return(Name()); }
  BText Info() const { return(Name()); }

  BBool IsKnown() const {
      if(!(Numerator().IsKnown()) || !(Denominator().IsKnown()))
	  return BFALSE;
      else return BTRUE;
  }
  
  int BinWrite(FILE*) const { return 0; }
  int BinRead (FILE*)       { return 0; }

  const BPolyn<Any>& Numerator()   const	{ return (numerator_); }
  const BPolyn<Any>& Denominator() const	{ return (denominator_); }
  void	PutNumerator  (BPolyn<Any>& num) { numerator_  =num; }
  void	PutDenominator(BPolyn<Any>& den) { denominator_=den; }

  Any	Eval (const Any& x) const
  {
    return(Numerator  ().Eval(x) /
	   Denominator().Eval(x));
  }

  //Overloaded algebraic operators and functions :
  static BInt Compare(const BRational<Any>* r1, const BRational<Any>* r2)
  { 
    BInt cmp = BPolyn<Any>::Compare(&r1->Numerator(),&r2->Numerator());
    if(!cmp) 
    {
      cmp = BPolyn<Any>::Compare(&r1->Denominator(),&r2->Denominator());
    }
    return (cmp); 
  }

  //Overloaded algebraic operators and functions :
  BRational<Any>& operator += (const BRational<Any>&  rat);
  BRational<Any>& operator -= (const BRational<Any>&  rat);
  BRational<Any>& operator *= (const BRational<Any>&  rat);
  BRational<Any>& operator /= (const BRational<Any>&  rat);
  BRational<Any>& operator ^= (BInt Exp);

  BRational<Any>& operator +  (const BRational<Any>&  rat);
  BRational<Any>& operator -  (const BRational<Any>&  rat);
  BRational<Any>& operator *  (const BRational<Any>&  rat);
  BRational<Any>& operator /  (const BRational<Any>&  rat);
  BRational<Any>& operator ^  (BInt Exp);

  BPolyn<Any>	  Expand      (BInt degree, BBool aggregate);
  BPolyn<Any>	  operator %  (BInt degree);
  BPolyn<Any>	  ExactQuotient();
/*
  friend BRational<Any>	 operator - (const BRational<Any>&  rat);
  friend BRational<Any>	 operator / (const BPolyn<Any> & P,
					   const BPolyn<Any> & Q);
*/
};


//--------------------------------------------------------------------
// template-class implementation
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator += (const BRational<Any>&  rat)

/*! Sums "*this" with "rat", puts the result in "*this" and
 *  returns "*this".
 */
//--------------------------------------------------------------------
{
  BPolyn<Any> num =	Numerator()   * rat.Denominator() +
		    rat.Numerator()   *	    Denominator();
  BPolyn<Any> den =	Denominator() * rat.Denominator();
  numerator_   = num;
  denominator_ = den;
  return (*this);
}


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator -= (const BRational<Any>&  rat)

/*! Substracts "rat" from "*this" , puts the result in "*this" and
 *  returns "*this".
 */
//--------------------------------------------------------------------
{
  BPolyn<Any> num =	Numerator  () * rat.Denominator() -
		    rat.Numerator  () *	    Denominator();
  BPolyn<Any> den =	Denominator() * rat.Denominator();
  numerator_   = num;
  denominator_ = den;
  return (*this);
}


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator *= (const BRational<Any>&  rat)

/*! Multiplies "rat" and "*this" , puts the result in "*this" and
 *  returns "*this".
 */
//--------------------------------------------------------------------
{
    BPolyn<Any> num =	Numerator  () * rat.Numerator  ();
    BPolyn<Any> den =	Denominator() * rat.Denominator();
    numerator_   = num;
    denominator_ = den;
    return (*this);
}


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator /= (const BRational<Any>&  rat)

/*! Divides "*this" by "rat", puts the result in "*this" and
 *  returns "*this".
 */
//--------------------------------------------------------------------
{
    if (rat.Numerator()==BPolyn<Any>(0,0))
    {
//  Message("Intento de division por BPolyn cero.");
	return (*this);
    }
    BPolyn<Any> num =	Numerator  () * rat.Denominator();
    BPolyn<Any> den =	Denominator() * rat.Numerator  ();
    numerator_   = num;
    denominator_ = den;
    return (*this);
}


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator ^= (BInt exponent)

/*! Powers "*this" to exponent, puts the result in "*this" and
 *  returns "*this" if "*this" is not null.
 *  Else this function has no effect.
 */
//--------------------------------------------------------------------
{
    numerator_^=exponent;
    denominator_^=exponent;
    return (*this);
}


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator +  (const BRational<Any>&  rat)

/*! Returns a copy of "*this" += "rat". ( See operator += )
 */
//--------------------------------------------------------------------
{
    BRational<Any> FR=(*this);
    return FR+=rat;
}


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator -  (const BRational<Any>&  rat)

/*! Returns a copy of "*this" -= "rat". ( See operator -= )
 */
//--------------------------------------------------------------------
{
    BRational<Any> FR=(*this);
    return FR-=rat;
}


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator *  (const BRational<Any>&  rat)

/*! Returns a copy of "*this" *= "rat". ( See operator *= )
 */
//--------------------------------------------------------------------
{
    BRational<Any> FR=(*this);
    return FR*=rat;
}


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator /  (const BRational<Any>&  rat)

/*! Returns a copy of "*this" /= "rat". ( See operator /= )
 */
//--------------------------------------------------------------------
{
    BRational<Any> FR=(*this);
    return FR/=rat;
}


//--------------------------------------------------------------------
template<class Any>
BRational<Any>& BRational<Any>::operator ^ (BInt exponent)

/*! Returns a copy of "*this" ^= "exponent". ( See operator ^= )
 */
//--------------------------------------------------------------------
{
    BRational<Any> FR=(*this);
    return FR^=exponent;
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>	BRational<Any>::Expand (BInt degree, BBool aggregate)

/*! Returns the finite expansion of "*this" with degree "degree"
 */
//--------------------------------------------------------------------
{
  BPolyn<Any> E;
  BInt n;
  if((denominator_.Degree()==0) && (denominator_[0].Coef()!=0))
  {
    if(aggregate) 
    {	
      E = numerator_/denominator_[0].Coef();
    }
    else
    {
  	  	E.ReallocBuffer(degree+1);
	     for(n=0; n<E.Size(); n++) 
      {  
        E(n).PutDegree(n);
        E(n).PutCoef  (0);
      }
	     for(n=0; n<numerator_.Size(); n++) 
      {  
        E(numerator_(n).Degree()).PutCoef(numerator_(n).Coef());
      }
    }
  }
  else if((denominator_[0].Degree()==0) && (denominator_[0].Coef()!=0))
  {
	  	E.ReallocBuffer(degree+1);
	   BArray<Any> num, den;
	   numerator_.ToArray(num);
	   denominator_.ToArray(den);
	   for(n=0; n<E.Size(); n++)
	   {
	     E(n).PutDegree(n);
	     Any x0 = (n<num.Size())?num(n):Any(0);
	     E(n).PutCoef(x0);
	     BInt i = n;
	     if(denominator_.Degree()<n) { i = denominator_.Degree(); }
	     for (;i>0;i--)
	     { E(n).PutCoef(E(n).Coef() - E(n-i).Coef()*den(i)); }
	     E(n).PutCoef(E(n).Coef() / denominator_[0].Coef());
	   }
  }
  else
  {
    Warning(I2("Cannot apply polynomial expand to quotient",
               "No se puede aplicar la expansión polinomial al cociente")+
            " "+numerator_.Name()+"/"+denominator_.Name());
  }
  if(aggregate) { E.Aggregate(); }
  return E;
}


//------------------------------------------------------------
/*! Returns the finite expansion of "*this" with degree "degree"
 */
template <class Any>
BPolyn<Any> BRational<Any>::operator % (BInt degree) {
    return(Expand(degree,BTRUE));
}


//------------------------------------------------------------
template<class Any>
BPolyn<Any>  BRational<Any>::ExactQuotient() {
    BPolyn<Any> quo;
    if (denominator_!=Any(0)) {
	BMonome<Any> numMinMon = numerator_  [0] ^ (-1);
	BMonome<Any> denMinMon = denominator_[0] ^ (-1);
	BMonome<Any> difMon    = numerator_[0]*denMinMon;
	BPolyn<Any>  num       = numerator_  ; num.Product(numMinMon);
	BPolyn<Any>  den       = denominator_; den.Product(denMinMon);
	BRational<Any> rat	     = num / den;
	
	if (rat.Denominator().Degree()==0) {
	    quo = rat.Numerator();
	    quo.Product(difMon);
	} else {
	    BInt Gr=(rat.Numerator().Degree()-rat.Denominator().Degree());
	    if (Gr>=0) {
		quo = rat % Gr;
		quo.Product(difMon);
	    }
	}
    }
    
    return (quo);
}


template<class Any>
BRational<Any>	operator - (const BRational<Any> & P)
{
    BRational<Any> rat(-P.Numerator(), P.Denominator());
    return rat;
}


template<class Any>
BRational<Any>	operator /  (const BPolyn<Any> & P, const BPolyn<Any> & Q)
{
    BRational<Any> rat(P,Q);
    if (Q==Any(0))
    { Error(I2("Attempt to polynomial zero division",
	       "Intento de division por polinomio cero")); }
    return rat;
}

#endif // TOL_BRATION_H
