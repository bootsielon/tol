/* tol_bpolyn.h: BPolyn (pol): Class and functions header
                 of GNU/TOL language.

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

#ifndef TOL_BPOLYN_H
#define TOL_BPOLYN_H 1

#include <tol/tol_bmonome.h>
#include <boost/math/common_factor_rt.hpp>
#include <limits>

//--------------------------------------------------------------------
template <class Any>
class BPolyn : public BArray<BMonome<Any> >

/*! Generic polynomial functions representation.
 */
//--------------------------------------------------------------------
{
protected:
 

public:
  // Constructors and destructors:

  BPolyn()
  : BArray< BMonome<Any> >() {}

  BPolyn(BInt size, const BMonome<Any>* buf=NIL)
  : BArray< BMonome<Any> >(size, buf) { Aggregate(); }

  BPolyn(const BMonome<Any>& mon)
  : BArray< BMonome<Any> >(1, &mon) { }

  BPolyn(const BArray< BMonome<Any> >& arr)
  : BArray< BMonome<Any> >(arr.Size(), arr.Buffer())
  { Aggregate(); }

  BPolyn(const BPolyn<Any>& pol)
  : BArray< BMonome<Any> >(pol.Size(), pol.Buffer()) {}

  static BPolyn<Any>& Unknown();

  //-----------------------------------------------------------------
  // Access & Manipulation: inline

  void ToArray(BArray<Any>& arr) const
  {
    arr.Replicate(0,Degree()+1);

    for(BInt i=0; i<this->Size(); i++)
    {
      arr((*this)(i).Degree()) = (*this)(i).Coef();
    }
  }

  const Any& Coef (BInt degree) const;
  void PutCoef (BInt degree,const Any& coef);
  BText ToBText (const BText& B="B", const BText& F="F") const;
  BText Name() const { return(ToBText()); }
  BText Dump() const { return(Name()); }
  BText Info() const { return(Name()); }
  BInt Degree() const { return((!this->size_)?std::numeric_limits<int>::min():(*this)(this->size_-1).Degree()); }
  BInt Period() const;
  BBool IsNull() const { return((Degree()==0) && (Coef(0)==Any(0.0)));}
  Any Eval (const Any& x) const;
  BPolyn<Any> Derivate () const;
  BPolyn<Any> Integrate() const;
 
  int BinWrite(FILE*) const { return 0; }
  int BinRead (FILE*)       { return 0; }

  // Miscelaneus member operations:
  void Sort()  { BArray< BMonome<Any> >::Sort(BMonome<Any>::BMonomeOrder); }
  BInt Find(BInt searched) const
  {
    return(FindSorted(BMonome<Any>(0.0,searched),BMonome<Any>::BMonomeOrder));
  }

  void Aggregate();
  void ChangeBF(const BPolyn<Any>& pol);
  void InverseRoots(const BPolyn<Any>& pol);

  BBool IsKnown() const {
      for(int i=0; i<this->Size(); i++)
      if(!(*this)(i).Coef().IsKnown())
          return BFALSE;
      return BTRUE;
  }
 
  BPolyn<Any>& Product       (const BMonome<Any>& mon);
  BPolyn<Any>& Product       (const Any& x)
  { return (Product(BMonome<Any>(x,0))); }

  //Overloaded algebraic operators and functions :
  static BInt Compare(const BPolyn<Any>* pol1, const BPolyn<Any>* pol2)
//      { return((*pol1<*pol2)?-1:(*pol1>*pol2)?1:0); }
      { return (*pol1==*pol2)? 0 : 1 ; }
 
  BBool operator == (const BPolyn<Any>& pol) const;
  BBool operator == (const Any& any) const
      { return ((Degree()==0)&&(Coef(0)==any)); }
  BBool operator != (const BPolyn<Any>& pol) const
      { return (!((*this)==pol)); }
  BBool operator != (const Any& any) const
      { return (!((*this)==any)); }
  BBool operator < (const BPolyn<Any>& pol) const
      { return(Degree()<pol.Degree()); }
  BBool operator > (const BPolyn<Any>& pol) const
      { return(Degree()>pol.Degree()); }
  BBool operator <= (const BPolyn<Any>& pol) const
      { return(Degree()<=pol.Degree()); }
  BBool operator >= (const BPolyn<Any>& pol) const
      { return(Degree()>=pol.Degree()); }
 
  BPolyn<Any>& operator =  (const BPolyn<Any>& pol);
  BPolyn<Any>& operator =  (const BMonome<Any>& mon);
  BPolyn<Any>& operator =  (const Any& any);

  BPolyn<Any>& operator += (const BPolyn<Any>& pol);
  BPolyn<Any>& operator -= (const BPolyn<Any>& pol);
  BPolyn<Any>& operator *= (const BPolyn<Any>& pol);
  BPolyn<Any>& operator /= (const Any& any);
  BPolyn<Any>& operator ^= (BInt exponent);

  BPolyn<Any>  operator +  (const BPolyn<Any>& pol) const;
  BPolyn<Any>  operator -  (const BPolyn<Any>& pol) const;
  BPolyn<Any>  operator *  (const BPolyn<Any>& pol) const;
  BPolyn<Any>  operator /  (const Any& any) const;
  BPolyn<Any>  operator ^  (BInt exponent) const;

  friend BPolyn<Any>
  operator - (const BPolyn<Any>& pol)
  { return (BPolyn<Any>(pol).Product(-1)); }

  friend BPolyn<Any>
  operator + (const Any& any, const BPolyn<Any>& pol)
  {
    BPolyn<Any> pol1=pol;
    BPolyn<Any> pol2;
    pol2=any;
    return (pol1+= pol2);
  }

  friend BPolyn<Any>
  operator - (const Any& any, const BPolyn<Any>& pol)
  {
    BPolyn<Any> pol1=pol;
    BPolyn<Any> pol2;
    pol2=any;
    pol1.Product(-1);
    return (pol1+=pol2);
  }

  friend BPolyn<Any>
  operator * (const Any& any, const BPolyn<Any>& pol)
  {
    BPolyn<Any> pol1=pol;
    return (pol1.Product(any));
  }

  //static generators for special instances
  static const BPolyn<Any>& X ();
  static const BPolyn<Any>& B ();
  static const BPolyn<Any>& F ();
  static const BPolyn<Any>& One ();
  static const BPolyn<Any>& Zero ();

};


//--------------------------------------------------------------------
// template-class implementation
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>& BPolyn<Any>::operator= (const BPolyn<Any>& pol)

/*! Destroies the actual polynomial, copies pol to "*this" and
 *  returns "*this"
 */
//--------------------------------------------------------------------
{
    this->DeleteBuffer();
    Copy(pol.Size(), pol.Buffer());
    return (*this);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>& BPolyn<Any>::operator= (const BMonome<Any>& mon)

/*! Destroies the actual polynomial, copies mon to "*this" and
 *  returns "*this"
 */
//--------------------------------------------------------------------
{
    this->DeleteBuffer();
    Replicate(mon);
    return (*this);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>& BPolyn<Any>::operator= (const Any& x)

/*! Destroies the actual polynomial, copies x to "*this" and
 *  returns "*this"
 */
//--------------------------------------------------------------------
{
    return (*this=BMonome<Any>(x,0));
}


//--------------------------------------------------------------------
template <class Any>
Any BPolyn<Any>::Eval (const Any& x) const

/*! Evaluates the correspondient polinomial function
 */
//--------------------------------------------------------------------
{
    BInt n  = this->Size();
    BInt i;
    Any  y = 0;
    
    for(i=n-1; i>=0; i--)
    {
    BInt deg = (*this)(i).Degree();
    y += (*this)(i).Coef();
    if(i>0) { deg -= (*this)(i-1).Degree(); }
    y *= x^deg;
    }
    return(y);
}


//--------------------------------------------------------------------
template <class Any>
void BPolyn<Any>::Aggregate()

/*! Clean the buffer of null monomes and sums all that are additive.
 */
//--------------------------------------------------------------------
{
  BInt n,m;
  BBool ag;
  do
  {
    ag = BFALSE;
    for(n=1; n<this->Size(); n++)
    {
      if((*this)(n).Degree()<(*this)(n-1).Degree())
      {
        Sort();
        Aggregate();
        return;
      }
      if(((*this)(n).Degree()==(*this)(n-1).Degree())&&
         (*this)(n-1).Coef())
      {
        ag = BTRUE;
        (*this)(n).PutCoef((*this)(n).Coef()+(*this)(n-1).Coef());
        (*this)(n-1).PutDegree(0);
        (*this)(n-1).PutCoef  (0);
      }
    }
  } while(ag);
  BPolyn<Any> p;
  p.ReallocBuffer(this->Size());
  for(n=m=0; n<this->Size(); n++)
  {
    if((*this)(n).Coef()!=Any(0.0))
    {
      p(m++) = (*this)(n);
    }
  }
  if(m==0)
  {
    p.ReallocBuffer(1);
    p(0) = BMonome<Any>(0.0,0);
  }
  else
  {
    p.ReallocBuffer(m);
  }
  (*this) = p;
}


//--------------------------------------------------------------------
template <class Any>
void BPolyn<Any>::ChangeBF(const BPolyn<Any>& pol)
//--------------------------------------------------------------------
{
    ReallocBuffer(pol.Size());
    for(BInt n=0; n<this->Size(); n++)
    {
    (*this)(n) = pol(this->Size()-1-n);
    (*this)(n).PutDegree(-((*this)(n).Degree()));
    }
}

//--------------------------------------------------------------------
template <class Any>
void BPolyn<Any>::InverseRoots(const BPolyn<Any>& pol)
//--------------------------------------------------------------------
{
  int s = pol.Size();
  int n = pol.Degree();
  int i, j, deg;
  Any coef;
  this->ReallocBuffer(s);
  for(i=0; i<s; i++)
  {
    j = s-1-i;
    deg  = pol(i).Degree();
    coef = pol(i).Coef();
    (*this)(j).PutDegree(n-deg);
    (*this)(j).PutCoef(coef);
  }
}


//--------------------------------------------------------------------
template <class Any>
BInt BPolyn<Any>::Period() const
//--------------------------------------------------------------------
{
  int s = this->Size();
  int i;
  int deg;
  int gcd = (*this)(0).Degree();
  for(i=1; i<s; i++)
  {
    deg = (*this)(i).Degree();
    if(deg) { gcd = boost::math::gcd(gcd,deg); }
  }
  return(gcd);
}


//--------------------------------------------------------------------
template <class Any>
BBool   BPolyn<Any>::operator == (const BPolyn<Any>& pol) const

/*! Returns BTRUE if "*this" and "pol" are identical. Else returns
 *  BFALSE
 */
//--------------------------------------------------------------------
{
    BBool isEqual=BTRUE;
    
    if(this->Size()==pol.Size())
    {
    for(BInt i=0; (i<this->Size()) && isEqual; i++)
    {
        isEqual = ((*this)(i).Coef()   == pol(i).Coef()) &&
        ((*this)(i).Degree() == pol(i).Degree());
    }
    }
    else { isEqual=BFALSE; }
    
    return(isEqual);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>&     BPolyn<Any>::Product(const BMonome<Any>& mon)

/*! Multiplies all the monomes in "*this" with "mon" and returns
 *  "*this"
 */
//--------------------------------------------------------------------
{
    for(BInt i=0; i<this->Size(); i++)
    {
        (*this)(i).PutDegree( (*this)(i).Degree() + mon.Degree());
        (*this)(i).PutCoef  ( (*this)(i).Coef()   * mon.Coef());
    }
    return (*this);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>& BPolyn<Any>::operator += (const BPolyn<Any>& pol)

/*! Sums "*this" with "pol", puts the result in "*this" and
 *  returns "*this".
 */
//--------------------------------------------------------------------
{
    AutoConcat(pol);
    Sort();
    Aggregate();
    return (*this);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>& BPolyn<Any>::operator -= (const BPolyn<Any>& pol)

/*! Substracts "pol" from "*this" , puts the result in "*this" and
 *  returns "*this".
 */
//--------------------------------------------------------------------
{
    return ((*this)+=BPolyn<Any>(pol).Product(-1));
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>& BPolyn<Any>::operator *= (const BPolyn<Any>& pol)

/*! Multiplies "pol" and "*this" , puts the result in "*this" and
 *  returns "*this".
 */
//--------------------------------------------------------------------
{
    BInt i,j,oldSize=this->Size();

    ReallocBuffer(this->Size()*pol.Size());
    for(j=pol.Size()-1; j>=0; j--)
    {
    for(i=oldSize-1; i>=0; i--)
    { (*this)(j*oldSize+i)=pol(j)*(*this)(i); }
    }
    Sort();
    Aggregate();
    return (*this);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>& BPolyn<Any>::operator /= (const Any& any)

/*! Multiplies "pol" and "*this" , puts the result in "*this" and
 *  returns "*this".
 */
//--------------------------------------------------------------------
{
    if(any==Any(0.0))
    {
    Error(I2("Polynomial division by zero",
         "Division de polinomio por cero"));
    }
    return (Product(1/any));
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>& BPolyn<Any>::operator ^= (BInt exponent)

/*! Powers "*this" to exponent, puts the result in "*this" and
 *  returns "*this" if "*this" is not null.
 *  Else this function has no effect.
 */
//--------------------------------------------------------------------
{
    if (!IsNull())
    {
    if (exponent>1)
    {
        BPolyn<Any> pol=(*this);
        while ( exponent-- > 1 )
        {    (*this)*=pol; }
    }
    else if (exponent==0)
    { (*this)=Any(1.0); }
    else if (exponent!=1)
    {
        Error(I2("Negative exponent in polynomial power",
             "Exponente negativo en potencia de polinomios"));
    }
    }
    return (*this);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any>    BPolyn<Any>::operator + (const BPolyn<Any>& pol) const

/*! Returns a copy of "*this" += "pol". ( See operator += )
 */
//--------------------------------------------------------------------
{
    return (BPolyn<Any>(*this)+=pol);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any> BPolyn<Any>::operator - (const BPolyn<Any>& pol) const

/*! Returns a copy of "*this" -= "pol". ( See operator -= )
 */
//--------------------------------------------------------------------
{
    return (BPolyn<Any>(*this)-=pol);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any> BPolyn<Any>::operator * (const BPolyn<Any>& pol) const

/*! Returns a copy of "*this" *= "pol". ( See operator *= )
 */
//--------------------------------------------------------------------
{
    return (BPolyn<Any>(*this)*=pol);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any> BPolyn<Any>::operator / (const Any& any) const

/*! Returns a copy of "*this" /= "any". ( See operator *= )
 */
//--------------------------------------------------------------------
{
  return ((BPolyn<Any>(*this)).Product(1/any));
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any> BPolyn<Any>::operator ^ (BInt exponent) const

/*! Returns a copy of "*this" ^= "exponent". ( See operator ^= )
 */
//--------------------------------------------------------------------
{
    return (BPolyn<Any>(*this)^=exponent);
}


//--------------------------------------------------------------------
template <class Any>
const BPolyn<Any>& BPolyn<Any>::X ()

/*! Returns the identity polynomial 1*x^1.
 */
//--------------------------------------------------------------------
{
    static BPolyn<Any> x_(BMonome<Any>::X());
    return (x_);
}


//--------------------------------------------------------------------
template <class Any>
const BPolyn<Any>& BPolyn<Any>::B ()

/*! Returns the identity polynomial 1*x^1.
 */
//--------------------------------------------------------------------
{
    static BPolyn<Any> b_(BMonome<Any>(1,1));
    return (b_);
}


//--------------------------------------------------------------------
template <class Any>
const BPolyn<Any>& BPolyn<Any>::F ()

/*! Returns the identity polynomial 1*x^(-1).
 */
//--------------------------------------------------------------------
{
    static BPolyn<Any> f_(BMonome<Any>(1,-1));
    return (f_);
}


//--------------------------------------------------------------------
template <class Any>
const BPolyn<Any>& BPolyn<Any>::One ()

/*! Returns the unitary polynomial 1*x^0.
 */
//--------------------------------------------------------------------
{
    static BPolyn<Any> o_(BMonome<Any>::One());
    return (o_);
}


//--------------------------------------------------------------------
template <class Any>
const BPolyn<Any>& BPolyn<Any>::Zero ()

/*! Returns the null polynomial 0*x^0.
 */
//--------------------------------------------------------------------
{
    static BPolyn<Any> z_(BMonome<Any>::Zero());
    return (z_);
}


//------------------------------------------------------------
/*! Returns the coefficient of the monome of degree "degree".
 */
template <class Any>
const Any& BPolyn<Any>::Coef(BInt degree) const
{
  static Any zero(0);
  BInt d = Find(degree);
  return ( (d<0) ? zero : (*this)(d).Coef() );
}

//------------------------------------------------------------
/*! Changes the coefficient of the monome of degree "degree".
 */
template <class Any>
void BPolyn<Any>::PutCoef(BInt degree, const Any& coef)
{
  BInt d = Find(degree);
//Std(BText("\ndegree = ")+degree+"; d="+d+"; (*this)(d).Coef()="+(*this)(d).Coef()+"; coef="+coef);
  if((d>=0)&&(d<this->size_))
  {
    (*this)(d).PutCoef(coef);
  //Std(BText(" -> ")+(*this)(d).Coef());
  }
}


//------------------------------------------------------------
/*! Creates a BText containing a polynomial expression in var B
 *  of *this(NOTE F = 1/B).
 */
template <class Any>
BText BPolyn<Any>::ToBText(const BText& B, const BText& F) const {
    BText txt=NIL;
    for(BInt i=0; i<this->Size(); i++)
    {
    BMonome<Any> mon = (*this)(i);
    if((B!=F) && (mon.Degree()<0))
    {
        mon.PutDegree(-mon.Degree());
        txt += mon.ToBText(F);
    }
    else
    { txt += mon.ToBText(B); }
    }
    if(txt.Get(0)=='+')
    {
      txt.PutChar(0,' ');
      txt.Compact();
    }
    return(txt);
}


//--------------------------------------------------------------------
template <class Any>
BPolyn<Any> BPolyn<Any>::Derivate () const

/*! Calculates the derivate of a polinomial.
 */
//--------------------------------------------------------------------
{
    BPolyn<Any> p = *this;
    BInt i;
    for(i=0; i<this->Size(); i++)
    {
    if(p(i).Degree()==0)
    {
        p(i).PutCoef(0);
    }
    else
    {
        p(i).PutCoef(p(i).Coef()*p(i).Degree());
        p(i).PutDegree(p(i).Degree()-1);
    }
    }
//Std(BText("\nDerivate of ")+Name()+" is "+p.Name());
    p.Aggregate();
    return(p);
}

//--------------------------------------------------------------------
template <class Any>
BPolyn<Any> BPolyn<Any>::Integrate() const

/*! Calculates the integrate of a polinomial.
 */
//--------------------------------------------------------------------
{
    BPolyn<Any> p = *this;
    BInt i;
    for(i=0; i<this->Size(); i++)
    {
    if(p(i).Degree()==-1)
    {
        Error(I2("Cannot integrate a polynomial with terms in x^-1 "
             "as another polynomial.",
             "No se puede integrar un polinomio con téminos en "
             "x^-1 como otro polinomio"));
        p = Unknown();
    }
    else
    {
        p(i).PutCoef(p(i).Coef()/(1+p(i).Degree()));
        p(i).PutDegree(p(i).Degree()+1);
    }
    }
    return(p);
}

#endif // TOL_BPOLYN_H

