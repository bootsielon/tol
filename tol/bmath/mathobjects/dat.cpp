/* dat.cpp: Basic declarations and definitions of BDat Data Type.
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bcommon.h>
#include <tol/tol_bdat.h>
#include <tol/tol_bout.h>

#include <gsl/gsl_math.h>

#include <errno.h>
#include <float.h>
#include <limits>


#define POWFUNC    pow
#define FLOORFUNC  floor
#define FMODFUNC   fmod
#define CEILFUNC   ceil
#define SQRTFUNC   sqrt
#define LOG10FUNC  log10
#define LOGFUNC    log
#define EXPFUNC    exp
#define SINFUNC    sin
#define COSFUNC    cos
#define TANFUNC    tan
#define ASINFUNC   asin
#define ACOSFUNC   acos
#define ATANFUNC   atan

#ifndef LN_MAXDOUBLE
#define LN_MAXDOUBLE  7.0978E+2
#endif
#ifndef LN_MINDOUBLE
#define LN_MINDOUBLE  -7.0840E+2
#endif

//--------------------------------------------------------------------
// Static global class variable for BDat.
//--------------------------------------------------------------------
BTraceInit("dat.cpp");

BText& TOL_lc_numeric();

static double  posInf_;
static double  negInf_;

BReal BDat::nan_; 
BReal BDat::posInf_;
BReal BDat::negInf_;
BText BDat::format_      (StaticInit());
BText BDat::formatL_     (StaticInit());
BDat  BDat::unknown_     (StaticInit());
BDat  BDat::piNumber_    (StaticInit());
BDat  BDat::eNumber_     (StaticInit());
BDat  BDat::maxIter_     (StaticInit());
BDat  BDat::tolerance_   (StaticInit());
BDat  BDat::relTolerance_(StaticInit());
BDat  BDat::zero_        (StaticInit());

int    BData::count_;
double BData::kBytes_;

DefIsAlreadyInitilialized(BDat);

//--------------------------------------------------------------------
  bool BDat::InitializeClass()
//--------------------------------------------------------------------
{
  if(!IsAlreadyInitilialized(BDat)) 
  {
    BText::InitializeClass();
    nan_                 = GSL_NAN; 
    posInf_              = GSL_POSINF;
    negInf_              = GSL_NEGINF;
    BDat::nan_           = GSL_NAN; 
    BDat::posInf_        = GSL_POSINF;
    BDat::negInf_        = GSL_NEGINF;
    BDat::format_        = "%.16lg";
    BDat::formatL_       = "%.16Lg";
    BDat::unknown_       = BDat(GSL_NAN, false);
    BDat::piNumber_      = BDat(3.14159265358979323846, true);
    BDat::eNumber_       = BDat(2.71828182845904523536, true);
    BDat::maxIter_       = BDat(40, true);
    BDat::tolerance_     = BDat(1e-5, true);
    BDat::relTolerance_  = BDat(1e-5, true);
    BDat::zero_          = BDat(2.220446049250313081e-16 / 2, true); 
    BData::InitializeClass();
#   if(defined(__USE_TOL_IEEE__) && defined(NDEBUG))
    PrintBitsAllSpecialDouble();
#   endif
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BData::InitializeClass()
//--------------------------------------------------------------------
{
  BData::count_  = 0;
  BData::kBytes_ = 0.0;
  return(true);
}


/*
 * esto es para evitar que el compilador me optimize las expresiones
 * en tiempo de compilacion.
 *
 * /
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif


#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif
*/


//--------------------------------------------------------------------
BBool BDat::IsMultipleOf(BDat x) const

/*! Indicates if \x is multiple
 * \param x BDat to compare
 * \return Returns TRUE if this is multiple of \a x
 */
//--------------------------------------------------------------------
{
  if(IsUnknown()||x.IsUnknown())   { return(BFALSE); }
  BDat y = (*this)/x;
  BDat z = ::Floor(y);
  if(y.IsUnknown()||z.IsUnknown()) { return(BFALSE); }
  return(y==z);
}

//--------------------------------------------------------------------
BInt BDat::BinWrite(FILE* fil) const

/*! Appends this BDat to a binary file.
 * \param fil Binary file stream to write in
 * \return Return the number of items successfully writted. If an error
 *         occurs, or the end-of-file is reached, the return value is short
 *         item count (or cero)
 */
//--------------------------------------------------------------------
{
  return(fwrite(this, sizeof(BDat), 1, fil));
}


//--------------------------------------------------------------------
BInt BDat::BinRead(FILE* fil)

/*! Reads this BDat from a binary file.
 * \param fil Binary file stream to read in
 * \return Return the number of items successfully read. If an error occurs,
 *         or the end-of-file is reached, the return value is a short item
 *         count (or cero)
 */
//--------------------------------------------------------------------
{
  return(fread(this, sizeof(BDat), 1, fil));
}


//--------------------------------------------------------------------
/*! Verifies if a nonreal number has been obtained after to
 *  make an operation on the \a value_ attribute
 * \param FloatExpression
 */
//--------------------------------------------------------------------
#define SAVEFPE1(FloatExpression) \
value_ = FloatExpression; \
return(*this);


//--------------------------------------------------------------------
/*! Verifies if a nonreal number has been obtained after to
 *  make an operation between a \a value_ attribute and a BDat
 * \param dat
 * \param FloatExpression
 */
//--------------------------------------------------------------------
#define SAVEFPE2(dat,FloatExpression)  \
  SAVEFPE1(FloatExpression)


//--------------------------------------------------------------------
// operators and mathematic functions implementation
//--------------------------------------------------------------------

BDat& BDat::operator +=(const BDat& dat)
{
  SAVEFPE2(dat,value_+dat.Value());
}

BDat& BDat::operator -=(const BDat& dat)
{
  SAVEFPE2(dat,value_-dat.Value());
}

BDat& BDat::operator *=(const BDat& dat)
{
  SAVEFPE2(dat,value_*dat.Value());
}

BDat& BDat::operator /=(const BDat& dat)
{
  if((value_==0)&&(dat.Value()==0))  { PutKnown(BUNKNOWN); return(*this); }
//if(dat.Value() ==  0) { PutKnown(BUNKNOWN); return(*this); }
  SAVEFPE2(dat,value_/dat.Value());
}

//--------------------------------------------------------------------
BDat& BDat::And(const BDat& dat)

/*! Make AND logic operation between this and \a dat
 * \param dat BDat with that it is compared
 * \return Return a BDat with the result of the operation
 */
//--------------------------------------------------------------------
{
  BBool k1 = IsKnown();
  if(k1 && !value_) 
  { 
    value_ = 0; 
    return(*this); 
  }
  BBool  k2 = dat.IsKnown();
  double v2 = dat.Value();
  if(k2 && !v2)
  { 
    value_ = 0; 
    if(!k1) { this->PutKnown(true); }
    return(*this); 
  }
  else if(!k1 || !k2)
  { 
    this->PutKnown(false);
    return(*this); 
  }
  else
  {
    value_ = value_ && v2; 
    return(*this);
  }
}

//--------------------------------------------------------------------
BDat& BDat::Or(const BDat& dat)

/*! Make OR logic operation between this and \a dat
 * \param dat BDat whit that it is compared
 * \return Return a BDat with the reslt of the operation
 */
//--------------------------------------------------------------------
{
  BBool k1 = IsKnown();
  if(k1 && value_) 
  { 
    value_ = 1; 
    return(*this); 
  }
  BBool  k2 = dat.IsKnown();
  double v2 = dat.Value();
  if(k2 && v2)
  { 
    value_ = 1; 
    if(!k1) { this->PutKnown(true); }
    return(*this); 
  }
  else if(!k1 || !k2)
  { 
    this->PutKnown(false);
    return(*this); 
  }
  else
  {
    value_ = value_ || v2; 
    return(*this);
  }
}

//--------------------------------------------------------------------
  BDat BDat::Compare(const BDat* dat1, const BDat* dat2) 
//--------------------------------------------------------------------
{
       if(dat1->Value() <  dat2->Value()) { return(-1); }
  else if(dat1->Value() >  dat2->Value()) { return( 1); }
  else if(dat1->Value() == dat2->Value()) { return( 0); }
  else if(dat1->Value() == posInf_      ) { return( 1); }
  else if(dat2->Value() == posInf_      ) { return(-1); }
  else if(dat1->Value() == negInf_      ) { return(-1); }
  else if(dat2->Value() == negInf_      ) { return( 1); }
  else                                    { return(nan_); }
}

//--------------------------------------------------------------------
  BInt DatCmp(const void* v1, const void* v2)

/*! Compare value the two BDat and it indicates as he is the greater
 *  one. It's used to sorting, so unknown result is not allowed
 * \param v1 First BDat to compare
 * \param v2 Second BDat to compare
 * \return Return 1 if v1 > v2, -1 if v2 > v1 and 0 if v1 = v2
 */
//--------------------------------------------------------------------
{
  BDat* dat1 = (BDat*)v1;
  BDat* dat2 = (BDat*)v2;
       if(dat1->Value() <  dat2->Value()) { return(-1); }
  else if(dat1->Value() >  dat2->Value()) { return( 1); }
  else if(dat1->Value() == dat2->Value()) { return( 0); }
  else if(dat1->Value() == posInf_      ) { return( 1); }
  else if(dat2->Value() == posInf_      ) { return(-1); }
  else if(dat1->Value() == negInf_      ) { return(-1); }
  else if(dat2->Value() == negInf_      ) { return( 1); }
  else                                    { return( 0); }
}

//--------------------------------------------------------------------
BDat& BDat::Gt(const BDat& dat)

/*! Indicates if this if greater that \a dat
 * \param dat BDat whit that it is compared
 * \return Return TRUE if this is greater that \a dat, FALSE isnot
 */
//--------------------------------------------------------------------
{
       if(value_      <  dat.Value()) { value_ = 0; }
  else if(value_      >  dat.Value()) { value_ = 1; }
  else if(value_      == dat.Value()) { value_ = 0; }
  else if(dat.Value() == posInf_    ) { value_ = 0; }
  else if(value_      == negInf_    ) { value_ = 0; }
  else                                { value_ = nan_; }
  return(*this);
}

//--------------------------------------------------------------------
BDat& BDat::Lt(const BDat& dat)

/*! Indicates if this if minor that \a dat
 * \param dat BDat whit that it is compared
 * \return Return TRUE if this is minor that \a dat, FALSE isnot
 */
//--------------------------------------------------------------------
{
       if(value_      <  dat.Value()) { value_ = 1; }
  else if(value_      >  dat.Value()) { value_ = 0; }
  else if(value_      == dat.Value()) { value_ = 0; }
  else if(value_      == posInf_    ) { value_ = 0; }
  else if(dat.Value() == negInf_    ) { value_ = 0; }
  else                                { value_ = nan_; }
  return(*this);
}

//--------------------------------------------------------------------
BDat& BDat::Ge(const BDat& dat)

/*! Indicates if this if greater or equal that \a dat
 * \param dat
 * \return Return TRUE if this is greater or equal that \a dat, FALSE isnot
 */
//--------------------------------------------------------------------
{
       if(value_      <  dat.Value()) { value_ = 0; }
  else if(value_      >  dat.Value()) { value_ = 1; }
  else if(value_      == dat.Value()) { value_ = 1; }
  else if(value_      == posInf_    ) { value_ = 1; }
  else if(dat.Value() == negInf_    ) { value_ = 1; }
  else                                { value_ = nan_; }
  return(*this);
}

//--------------------------------------------------------------------
BDat& BDat::Le(const BDat& dat)

/*! Indicates if this if minor or equal that \a dat
 * \param dat BDat whit that it is compared
 * \return Return TRUE if this is minor or equal that \a dat, FALSE isnot
 */
//--------------------------------------------------------------------
{
       if(value_      <  dat.Value()) { value_ = 1; }
  else if(value_      >  dat.Value()) { value_ = 0; }
  else if(value_      == dat.Value()) { value_ = 1; }
  else if(dat.Value() == posInf_    ) { value_ = 1; }
  else if(value_      == negInf_    ) { value_ = 1; }
  else                                { value_ = nan_; }
  return(*this);
}

//--------------------------------------------------------------------
BDat& BDat::Eq(const BDat& dat)

/*! Indicates if this if equal that \a dat
 * \param dat BDat whit that it is compared
 * \return Return TRUE if this is equal that \a dat, FALSE isnot
 */
//--------------------------------------------------------------------
{
       if(value_ <  dat.Value()) { value_ = 0; }
  else if(value_ >  dat.Value()) { value_ = 0; }
  else if(value_ == dat.Value()) { value_ = 1; }
  else                           { value_ = nan_; }
  return(*this);
}

//--------------------------------------------------------------------
BDat& BDat::Ne(const BDat& dat)

/*! Indicates if this if different that \a dat
 * \param dat BDat whit that it is compared
 * \return Return TRUE if this is different that \a dat, FALSE isnot
 */
//--------------------------------------------------------------------
{
       if(value_ <  dat.Value()) { value_ = 1; }
  else if(value_ >  dat.Value()) { value_ = 1; }
  else if(value_ == dat.Value()) { value_ = 0; }
  else                           { value_ = nan_; }
  return(*this);
}


//--------------------------------------------------------------------
BDat& BDat::Not()

/*! Negate to the value of the attribute \a value_
 * \return Return a BDat with the \a value_ denied
 */
//--------------------------------------------------------------------
{
  SAVEFPE1(!value_);
}


//--------------------------------------------------------------------
BDat& BDat::Abs()

/*! Obtain the absolute value
 * \return Return the absolute value of the attribute \a value_
 */
//--------------------------------------------------------------------
{
  SAVEFPE1(fabs(value_));
}

//--------------------------------------------------------------------
BDat& BDat::Sign()

/*! Indicates the sign of the \a value_ attributte
 * \return Return  1: \a value_ > 0, 0: \a value_ = 0, -1: \a value_ < 0,
 *         \a value_: if is not know
 */
//--------------------------------------------------------------------
{
  if(!IsKnown())         { return(*this);    }
  else if((*this)==0)    { return(*this);    }
  else if((*this)< 0)    { return(*this=-1); }
  /*   if((*this)> 0) */ { return(*this=1);  }
}

//--------------------------------------------------------------------
BDat& BDat::ChangeSign ()

/*! Change to the sign of the attribute \a value_
 * \return Return a BDat with the \a value_ changing
 */
//--------------------------------------------------------------------
{
  SAVEFPE1(-value_);
}

//--------------------------------------------------------------------
BDat& BDat::Floor()

/*! Obtain the round \a value down to the nearest integer
 * \return Return the rounded integer value
 */
//--------------------------------------------------------------------
{
  SAVEFPE1(FLOORFUNC(value_));
}

//--------------------------------------------------------------------
BDat& BDat::Round()

/*! Round \a value_ to the nearest integer
 * \return Return the rounded integer value
 */
//--------------------------------------------------------------------
{
  if(IsKnown())
  {
    BReal x = value_;
    Floor();
    BReal y = x-value_;
    if(y>0.5) { value_ += 1; }
  }
  return(*this);
}

//--------------------------------------------------------------------
BDat& BDat::Ceil()

/*! Obtain the round \a value_ up to the nearest integer
 * \return Return the rounded integer value
 */
//--------------------------------------------------------------------
{
  SAVEFPE1(CEILFUNC(value_));
}

//--------------------------------------------------------------------
BDat& BDat::Mod(const BDat& dat)

/*! Obtain the remainder of dividing \a value_ by \a dat.value_
 * \param dat Element between which it is divided
 * \return Return value is this - n * \a dat, where n is the
 *         quotient of this / \a dat
 */
//--------------------------------------------------------------------
{
  SAVEFPE2(dat,FMODFUNC(value_,dat.Value()));
}

//--------------------------------------------------------------------
BDat& BDat::operator ^=(const BDat& dat)
//--------------------------------------------------------------------
{
  double v = dat.Value();
  if(IsUnknown()||dat.IsUnknown())    { return(*this=unknown_); }
  if((value_==0)&&(v==0))   { return(*this=1); }
  if((value_==0)&&(v<0) )   { return(*this=PosInf()); }
  if((value_==0)&&(v>0) )   { return(*this=0); }
  double v_ = floor(v);
  if(v==v_)
  {
    value_ = gsl_pow_int(value_,(int)v_);
  }
  else
  {
    value_ = pow(value_,dat.Value());
  }
/* */
  return(*this);
}

//--------------------------------------------------------------------
BDat& BDat::Sqrt()

/*! Obtain the square of \a value_
 * \return Return the non-negative square root of \a value_
 */
//--------------------------------------------------------------------
{
  if(value_ < 0) { PutKnown(BUNKNOWN); return(*this); }
  SAVEFPE1(SQRTFUNC(value_));
}

//--------------------------------------------------------------------
BDat& BDat::Log10()

/*! Obtain the base-10 logarithm
 * \return Return the value of base-10 logarithm of \a value_
 */
//--------------------------------------------------------------------
{
  if(value_ <=  0) { PutKnown(BUNKNOWN); return(*this); }
  SAVEFPE1(LOG10FUNC (value_));
}

//--------------------------------------------------------------------
BDat& BDat::Log()

/*! Obtain the natural logarithm
 * \return the value of natural logarithm of \a value_
 */
//--------------------------------------------------------------------
{
  if(value_ <=  0) { PutKnown(BUNKNOWN); return(*this); }
  SAVEFPE1(LOGFUNC (value_));
}


//--------------------------------------------------------------------
BDat& BDat::Exp()

/*! Obtain the value of e (the base of natural logarithms) raised to
 *  the power of \a value_
 * \return Return the value of e raised to the power of \a value_
 */
//--------------------------------------------------------------------
{
  if(value_ <=  LN_MINDOUBLE) { value_=0;        return(*this); }
  if(value_ >=  LN_MAXDOUBLE) { value_=PosInf(); return(*this); }
  SAVEFPE1(EXPFUNC(value_));
}


//--------------------------------------------------------------------
BInt Revolution(BReal& alfa)

/*! Obtains the number of complete loops to the circumference
 *  that gives \a alfa
 * \param alfa Number on that it operated
 * \return Return the number of complete loops to the circunference.
 * \return \a alfa return the angle between -PI/2 and PI/2
 */
//--------------------------------------------------------------------
{
  //  BReal x = alfa / (2*M_PI);
  BReal x = M_1_PI * alfa / 2;
  BInt  n = (BInt)x;
  alfa -= 2*M_PI*n;
  if(alfa> M_PI) { alfa -= 2*M_PI; }
  if(alfa<-M_PI) { alfa += 2*M_PI; }
  return(n);
};


//--------------------------------------------------------------------
BInt Quadrant(BReal alfa, BReal& beta)

/*! Obtains the quadrant of the circumference that corresponds
 *  to \a alfa
 * \param alfa Number on that it operated
 * \param beta Returned the angle between -PI/2 and PI/2
 * \return Return the quadrant of the circunference
 */
//--------------------------------------------------------------------
{
  beta = alfa;
  Revolution(beta);
  if((beta>= 0)&&(beta<= M_PI_2))
  {
    return(1);
  }
  if((beta>= M_PI_2)&&(beta<= M_PI))
  {
    return(2);
  }
  if((beta>=-M_PI   )&&(beta<=-M_PI_2))
  {
    return(3);
  }
  //if((beta<=-PI/2)&&(beta>=0    ))
  return(4);
};

//--------------------------------------------------------------------
BDat& BDat::Sin()

/*! Obtain the sine of \a value_, where \a value_ os given in
 *  radians
 * \return Return a sine
 */
//--------------------------------------------------------------------
{
  SAVEFPE1(SINFUNC(value_));
}

//--------------------------------------------------------------------
BDat& BDat::Cos()

/*! PURPUSE: Obtain the cosine of \a value, where \a value_ is given in
//           radians
//  \return Return a object modified
*/
//--------------------------------------------------------------------
{
  SAVEFPE1(COSFUNC(value_));
}

//--------------------------------------------------------------------
BDat& BDat::Tan()

/*! Obtain the tangent of \a value_, where \a value_ is given in
 *  radians
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  if(IsKnown())
  {
    value_ = FMODFUNC(value_,2*M_PI);
  }
  SAVEFPE1(TANFUNC(value_));
}

//--------------------------------------------------------------------
BDat& BDat::ASin()

/*! Obtain the arc sine of \a value, that is the value whose sine is
 *  \a value
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  SAVEFPE1(ASINFUNC(value_));
}

//--------------------------------------------------------------------
BDat& BDat::ACos()

/*! Obtain the arc cosine of \a value_, that is the value whose
 *  cosine is \a value_
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  SAVEFPE1(ACOSFUNC(value_));
}

//--------------------------------------------------------------------
BDat& BDat::ATan()

/*! Obtain the arc tangent of \a value_, that is the value whose
 *  tangent is \a value_
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  SAVEFPE1(ATANFUNC(value_));
}

//--------------------------------------------------------------------
BDat& BDat::SinH()

/*! Obtain the hyperbolic sine of \a value_, which is defined
 *  mathematically as (exp(x) - exp(-x)) / 2
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  BDat x = ::Exp(*this);
  BDat y = ::Exp((*this)*(-1));
  return((*this)=(x-y)/2);
}

//--------------------------------------------------------------------
BDat& BDat::CosH()

/*! Obtain the hyperbolic cosine of \a value_, which is defined
 *  mathematically as (exp(x) + exp(-x)) / 2
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  BDat x = ::Exp(*this);
  BDat y = ::Exp((*this)*(-1));
  return((*this)=(x+y)/2);
}

//--------------------------------------------------------------------
BDat& BDat::TanH()

/*! Obtain the hyperbolic tangent of \a value_,  which is defined
 *  mathematically as sinh(x) / cosh(x).
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  if((*this)== PosInf()) { return((*this)= 1); }
  if((*this)==-PosInf()) { return((*this)=-1); }
  BDat x = ::Exp(*this);
  BDat y = ::Exp((*this)*(-1));
  return((*this)=(x-y)/(x+y));
}

//--------------------------------------------------------------------
BDat& BDat::ASinH()

/*! Obtain the inverse hyperbolic sine of \a value_, that is the
 *  value whose hyperbolic sine is \a value_
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  BDat x  = (*this);
  (*this) ^= 2;
  (*this) += 1;
  Sqrt();
  (*this)+=x;
  (*this).Log();
  return(*this);
}

//--------------------------------------------------------------------
BDat& BDat::ACosH()

/*! Obtain the inverse hyperbolic cosine of \a value_, that is the
 *  value whose hyperbolic cosine is \a value_
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  BDat x  = (*this);
  (*this) ^= 2;
  (*this) -= 1;
  Sqrt();
  (*this)+=x;
  (*this).Log();
  return(*this);
}

//--------------------------------------------------------------------
BDat& BDat::ATanH()

/*! Obtain the inverse hyperbolic tangent of \a value_, that is the
 *  value whose hyperbolic tangent is \a value_
 * \return Return a object modified
 */
//--------------------------------------------------------------------
{
  BDat x  = (*this);
  (*this) = (1.0+x)/(1.0-x);
  (*this).Log();
  (*this)/=2;
  return(*this);
}

BDat operator +(const BDat& dat1, const BDat& dat2)
{ return(BDat(dat1)+=dat2); }

BDat operator -(const BDat& dat1, const BDat& dat2)
{ return(BDat(dat1)-=dat2); }

BDat operator *(const BDat& dat1, const BDat& dat2)
{ return(BDat(dat1)*=dat2); }

BDat operator /(const BDat& dat1, const BDat& dat2)
{ return(BDat(dat1)/=dat2); }

BDat  Mod(const BDat& dat1, const BDat& dat2)
{ return(BDat(dat1).Mod(dat2)); }

BDat operator ^(const BDat& dat1, const BDat& dat2)
{ return(BDat(dat1)^=dat2); }

BDat Pow       (const BDat& dat1, const BDat& dat2)
{ return(BDat(dat1)^=dat2); }

BBool operator &&(const BDat& dat1, const BDat& dat2)
{ return((BBool)(BDat(dat1).And(dat2)).Value()); }

BBool operator ||(const BDat& dat1, const BDat& dat2)
{ return((BBool)(BDat(dat1).Or(dat2)).Value()); }

BBool operator ==(const BDat& dat1, const BDat& dat2)
{ return((BBool)(BDat(dat1).Eq(dat2)).Value()); }

BBool operator !=(const BDat& dat1, const BDat& dat2)
{ return((BBool)(BDat(dat1).Ne(dat2)).Value()); }

BBool operator <=(const BDat& dat1, const BDat& dat2)
{ return((BBool)(BDat(dat1).Le(dat2)).Value()); }

BBool operator >=(const BDat& dat1, const BDat& dat2)
{ return((BBool)(BDat(dat1).Ge(dat2)).Value()); }

BBool operator <(const BDat& dat1, const BDat& dat2)
{ return((BBool)(BDat(dat1).Lt(dat2)).Value()); }

BBool operator >(const BDat& dat1, const BDat& dat2)
{ return((BBool)(BDat(dat1).Gt(dat2)).Value()); }

BDat  operator -(const BDat& dat) 
{ return(BDat(dat).ChangeSign()); }

BBool operator !(const BDat& dat) 
{ return((BBool)(BDat(dat).Not()).Value());}

BDat  Abs  (const BDat& dat) 
{ return(BDat(dat).Abs()); }

BDat  Sign  (const BDat& dat) 
{ return(BDat(dat).Sign()); }

BDat  Round  (const BDat& dat) 
{ return(BDat(dat).Round()); }

BDat  RoundD(const BDat& dat, const BDat& ndecs) 
{
  BDat newdat = dat * (10^(((BDat) ndecs).Floor()));
  newdat.Round();
  newdat = newdat / (10^(((BDat) ndecs).Floor()));
  return(BDat(newdat));
}

BDat  Floor  (const BDat& dat) { return(BDat(dat).Floor()); }
BDat  Ceil   (const BDat& dat) { return(BDat(dat).Ceil()); }
BDat  Sqrt   (const BDat& dat) { return(BDat(dat).Sqrt()); }
BDat  Log10  (const BDat& dat) { return(BDat(dat).Log10()); }
BDat  Log    (const BDat& dat) { return(BDat(dat).Log()); }
BDat  Exp    (const BDat& dat) { return(BDat(dat).Exp()); }
BDat  Sin    (const BDat& dat) { return(BDat(dat).Sin()); }
BDat  Cos    (const BDat& dat) { return(BDat(dat).Cos()); }
BDat  Tan    (const BDat& dat) { return(BDat(dat).Tan()); }
BDat  ASin   (const BDat& dat) { return(BDat(dat).ASin()); }
BDat  ACos   (const BDat& dat) { return(BDat(dat).ACos()); }
BDat  ATan   (const BDat& dat) { return(BDat(dat).ATan()); }
BDat  SinH   (const BDat& dat) { return(BDat(dat).SinH()); }
BDat  CosH   (const BDat& dat) { return(BDat(dat).CosH()); }
BDat  TanH   (const BDat& dat) { return(BDat(dat).TanH()); }
BDat  ASinH  (const BDat& dat) { return(BDat(dat).ASinH()); }
BDat  ACosH  (const BDat& dat) { return(BDat(dat).ACosH()); }
BDat  ATanH  (const BDat& dat) { return(BDat(dat).ATanH()); }


BDat operator + (BInt n, const BDat& x) { return(BDat(n)+x); }
BDat operator - (BInt n, const BDat& x) { return(BDat(n)-x); }
BDat operator * (BInt n, const BDat& x) { return(BDat(n)*x); }
BDat operator / (BInt n, const BDat& x) { return(BDat(n)/x); }
BDat operator ^ (BInt n, const BDat& x) { return(BDat(n)^x); }

BDat operator + (const BDat& x, BInt n) { return(x+BDat(n)); }
BDat operator - (const BDat& x, BInt n) { return(x-BDat(n)); }
BDat operator * (const BDat& x, BInt n) { return(x*BDat(n)); }
BDat operator / (const BDat& x, BInt n) { return(x/BDat(n)); }
BDat operator ^ (const BDat& x, BInt n) { return(x^BDat(n)); }

BDat operator + (double r, const BDat& x) { return(BDat(r)+x); }
BDat operator - (double r, const BDat& x) { return(BDat(r)-x); }
BDat operator * (double r, const BDat& x) { return(BDat(r)*x); }
BDat operator / (double r, const BDat& x) { return(BDat(r)/x); }
BDat operator ^ (double r, const BDat& x) { return(BDat(r)^x); }

BDat operator + (const BDat& x, double r) { return(x+BDat(r)); }
BDat operator - (const BDat& x, double r) { return(x-BDat(r)); }
BDat operator * (const BDat& x, double r) { return(x*BDat(r)); }
BDat operator / (const BDat& x, double r) { return(x/BDat(r)); }
BDat operator ^ (const BDat& x, double r) { return(x^BDat(r)); }

BDat operator + (long double r, const BDat& x) { return(BDat(r)+x); }
BDat operator - (long double r, const BDat& x) { return(BDat(r)-x); }
BDat operator * (long double r, const BDat& x) { return(BDat(r)*x); }
BDat operator / (long double r, const BDat& x) { return(BDat(r)/x); }
BDat operator ^ (long double r, const BDat& x) { return(BDat(r)^x); }

BDat operator + (const BDat& x, long double r) { return(x+BDat(r)); }
BDat operator - (const BDat& x, long double r) { return(x-BDat(r)); }
BDat operator * (const BDat& x, long double r) { return(x*BDat(r)); }
BDat operator / (const BDat& x, long double r) { return(x/BDat(r)); }
BDat operator ^ (const BDat& x, long double r) { return(x^BDat(r)); }

//--------------------------------------------------------------------
BDat FrobeniusNormU(const BArray<BDat>& arr )
    
/*! Obtain the Frobenius norm of a vector, it's to say, the
 *  second order's moment of its elements.
 * \param arr Array from whith it is obtained the FrobeniusNorm
 * \return Returns the Frobenius norm of a vector
 * \sa BArray::Size
 * If any unknown value is found in the array the result is unknown  
 * Only if all values are known the result is known
 */
//--------------------------------------------------------------------
{
  BDat norm=0;
  for(BInt i=0; i<arr.Size(); i++)
  {
    norm += (arr[i]*arr[i]);
  }
  return(Sqrt(norm/arr.Size()));
}

//--------------------------------------------------------------------
BDat FrobeniusNorm(const BArray<BDat>& arr )
    
/*! Obtain the Frobenius norm of a vector, it's to say, the
 *  second order's moment of its elements.
 * \param arr Array from whith it is obtained the FrobeniusNorm
 * \return Returns the Frobenius norm of a vector
 * \sa BArray::Size
 * If any known value is found in the array the result is known.
 * Only if all values are unknown the result is unknown
 */
//--------------------------------------------------------------------
{
  BDat norm=0;
  BInt num =0;
  for(BInt i=0; i<arr.Size(); i++)
  {
    if(norm.IsKnown())
    {
      norm += (arr[i]*arr[i]);
      num++;
    }
  }
  return(Sqrt(norm/num));
}


//--------------------------------------------------------------------
   int markThousands(char* out, const char* in) 
/* Add thousands character when needed */
//--------------------------------------------------------------------
{
  char* lconf = setlocale( LC_NUMERIC, 0);
  struct lconv *lcPtr = localeconv();
  char thousands_char = '\0';
  int grouping = 0;
  char tmp[80];

  /* Which is the thousands separator specified in current LC_NUMERIC.
   * Should be '.' if LC_NUMERIC is "Spanish" or something like "es_ES"
   */
  if(lcPtr->thousands_sep) 
  {
    thousands_char = lcPtr->thousands_sep[0];
    if(!thousands_char &&
       (strstr(lconf, "Spanish") || strstr(lconf, "es_ES")))
      { thousands_char = '.'; }
  } 
  
  // exit without changes => there is no thousands character in LC_NUMERIC
  if(!thousands_char) 
  {
    strcpy(out, in);
    return -1;
  }
  
  if(lcPtr->grouping && *lcPtr->grouping && 
    (*lcPtr->grouping>='0' && *lcPtr->grouping<='9'))
  {
    grouping = atoi(lcPtr->grouping);
  }
  else
  {
    grouping = 3;
  }
  
  int len   = strlen(in)-1;
  int i     = len;
  int j     = 0;
  int count = 0;

  // First, copy until decimal_point and...
  while(in[i]!=lcPtr->decimal_point[0]) 
  {
    tmp[j++] = in[i--];
  }
  tmp[j++] = in[i--]; // ...also copy decimal_point

  // Add a thousand's character each 'grouping' digits
  while(i>=0) 
  {
    tmp[j++] = in[i--];
    count++;
    if(((count % grouping) == 0) && (i>=0))
    {
      tmp[j++] = thousands_char;
    }
  }

  // Write to 'out' pointer parameter...
  for(i=0; j>=0; i++) out[i] = tmp[--j];
  out[i-1] = '\0'; // ...and add a null character termination
  return 0;
}

//--------------------------------------------------------------------
int lc_format(double d, char* s)
//--------------------------------------------------------------------
{
  char format_in[80], format_out[80], str_in[80];
  setlocale( LC_NUMERIC, TOL_lc_numeric());
  strcpy(format_in, BDat::RealFormat());
  int i,j;
  for(i=0, j=0; format_in[i]!='\0'; i++)
  {
    if(format_in[i]!='\'') 
    {
      format_out[j++] = format_in[i];
    }
  }
  format_out[j++] = '\0';
  int ecode = sprintf(str_in, format_out, d);
  markThousands(s, str_in);
  setlocale( LC_NUMERIC, "C");
  return ecode;
}


//--------------------------------------------------------------------
BText BDat::Format(const BText& format) const

/*! Applied the BDat in a done text format.
 * \param format Format thet is applied
 * \return Returns the \a value_ of BDat in a done \a format, if it is not
 *         known, return "?"
 * \sa BText::Copy, BText::String
 */
//--------------------------------------------------------------------
{
  BText txt, bformat_out;
  char format_in[80], format_out[80], str_in[1024], str_out[1024];
  if(Known())
  {
    int i,j;
    setlocale( LC_NUMERIC, TOL_lc_numeric());
    strcpy(format_in, format.String());
    for(i=0, j=0; format_in[i]!='\0'; i++)
    {
      if(format_in[i]!='\'') 
      { format_out[j++] = format_in[i]; }
    }
    format_out[j++] = '\0';
    bformat_out = format_out;
    //txt.Copy(Value(), format.String());
    txt.Copy(Value(), bformat_out);
    strcpy(str_in, txt.Buffer());
    markThousands(str_out, str_in);
    txt = str_out;
    setlocale( LC_NUMERIC, "C");
    //  if(Value()==floor(Value())) { txt.Copy(BInt(Value())); }
  }
  else { txt="?"; }
  return(txt);
}


//--------------------------------------------------------------------
BText BDat::Name() const

/*! Returns the BDat in text format.
 * \return Returns the \a value_ attribute in text format
 */
//--------------------------------------------------------------------
{
  return(Format(format_));
}


//--------------------------------------------------------------------
void BDat::PutValue(const BChar* name)

/*! Puts the real value of name in this or unknown if name is not
 *  a valid real number
 * \param name Value that is assigned to BDat
 */
//--------------------------------------------------------------------
{
  BText txt = name;
  PutValue(txt);
}


//--------------------------------------------------------------------
void BDat::PutValue(const BText& name)

/*! Puts the real value of name in this or unknown if name is not
 *  a valid real number
 * \param name Value thet is assigned to BDat
 */
//--------------------------------------------------------------------
{
  static BChar* endptr;
  if(!name.String() || !(name[0]) || (name=="?")) { PutKnown(BUNKNOWN); }
//else if(name=="inf") { PutValue(BDat::PosInf()); }
  else 
  {
    BReal value;
    value = strtod(name, &endptr);
    BBool isNotValid = (endptr)[0];
    if(isNotValid) { PutKnown(BUNKNOWN); }
    else
    {
      PutValue(value);
    }
  }
}

//--------------------------------------------------------------------
void BDat::PutValue(const BText& name, const BText& format)

/*! Puts the real value of name in this or unknown if name is not
 *  a valid real number
 * \param name Value thet is assigned to BDat
 */
//--------------------------------------------------------------------
{
  value_ = nan_;
  int ret = sscanf(name.String(),format.String(),&value_);
  if(ret!=1)
  {
    value_ = nan_;
  }
}

//--------------------------------------------------------------------
ostream& operator<< (ostream& os, const BArray<BDat>& arr)

/*! Redefines the operator << to show the elements of a BArray
 *  by the output that it indicated in \a os
 * \param os Indicated output
 * \param arr BArray to show
 * \return Return the \a value_ of elements of the \a arr
 */
//--------------------------------------------------------------------
{
  for(BInt i=0; i<arr.Size(); i++) { os << arr[i] << "\n"; }
  return(os);
}


//--------------------------------------------------------------------
BText Name (const BArray<BDat>& arr)

/*! Build a string with the value of all the elements of the array
 * \param arr Array from whith it is obtain the name
 * \return Return a string with the names
 * \sa BArray::Size
 */
//--------------------------------------------------------------------/
{
  BText txt;
  for(BInt i=0; i<arr.Size(); i++) { txt += arr[i]; txt += "\n"; }
  return(txt);
}

