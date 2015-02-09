/* tol_bdat.h: Basic declarations and definitions of BDat Data Type.
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

/*! \class BDat
//  \brief Defining BDat basic class, its attributes an methods prototypes
//  \brief funcionality has been implemented in dat.cpp
*/

#ifndef TOL_BDAT_H
#define TOL_BDAT_H 1

#include <tol/tol_barray.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUNKNOWN '\0'
#define BKNOWN	 '\1'

#if defined(_MSC_VER) /* Microsoft Visual C++ */
# define IS_NAN(x)     _isnan(x)
# define IS_FINITE(x)  _finite(x)
#else
// gsl/gsl_sys.h
extern "C" {
  int gsl_isnan (const double x);
  int gsl_isinf (const double x);
  int gsl_finite (const double x);
}

# define IS_NAN(x)     gsl_isnan(x)
# define IS_FINITE(x)  gsl_finite(x)
#endif

class BDat;

BBool TOL_API AreKnown(const BDat& dat1, const BDat& dat2);
BBool TOL_API operator == (const BDat& dat1, const BDat& dat2);
BBool TOL_API operator != (const BDat& dat1, const BDat& dat2);
BBool TOL_API operator <  (const BDat& dat1, const BDat& dat2);
BBool TOL_API operator >  (const BDat& dat1, const BDat& dat2);
BBool TOL_API operator <= (const BDat& dat1, const BDat& dat2);
BBool TOL_API operator >= (const BDat& dat1, const BDat& dat2);

//--------------------------------------------------------------------
class TOL_API BDat

/*! Scalar real data representation. Infinite, complex and
 *  undeterminate vaues are represented as value called unknown
 */
//--------------------------------------------------------------------
{
private:
         BReal  value_;         //!< Store a number value
  static BDat   unknown_;       //!< Represent nonreal number
  static BDat   piNumber_;      //!< Define number PI
  static BDat   eNumber_;       //!< Define number E
  static BDat   maxIter_;       //!< Define maximun number of iterations
  static BDat   tolerance_;     //!< Define minimun tolerance
  static BDat   relTolerance_;  //!< Define minimun tolerance
  static BDat   zero_;          //!< Define number cero
  static BText  format_;        //!< Define short format
  static BText  formatL_;       //!< Define long format
  static BReal  posInf_;
  static BReal  negInf_;
  static BReal  nan_;
public:
  static bool InitializeClass();
  // Constructors and destructors: inline
  //! Does nothing for static members initialization
  BDat(const BStaticInit& unused) { }
  //! Create a new BDat
  BDat()
  : value_(nan_) {}
  //! Create a new BDat, initialized with the value \a val
  BDat(BReal val, bool known)
  : value_(val) { if(!known) { value_ = nan_; } }
  //! Create a new BDat, initialized with the value \a val
  BDat(BReal val)
  : value_(val) {}
  //! Creates a reply of a \a dat
  BDat(const BDat& dat)
  : value_(dat.Value()) {}
  //! Destructors by defect

  //! Appends this BDat to a binary file
  BInt BinWrite(FILE*) const;
  //! Reads this BDat from a binary file
  BInt BinRead (FILE*);

  //! The value of \a dat is assigned to an object of the BDat type
  BDat& operator = (const BDat& dat)
  {
    PutValue(dat.Value());
    return (*this);
  }

  //! Assigned \a val to attribute \value_ and it is indicated that the
  //! value is known
  BDat& operator = (BReal val)
  {
    PutValue(val);
    return (*this);
  }

  //! Returns double positive Infinite value
  static BReal PosInf()     { return(posInf_); }
  //! Returns double negative Infinite value
  static BReal NegInf()  { return(negInf_); }
  //! Returns double NaN value
  static BReal Nan()     { return(nan_); }
  //! Return \a unknown_ attribute
  static BDat Unknown ()      { return(unknown_); }
//static const BDat& Unknown ()      { return(unknown_=); }
  //! Return \a piNumber_ attribute
  static BDat Pi ()           { return(piNumber_); }
  //! Return \a eNumber_ attribute
  static BDat E ()            { return(eNumber_); }
  //! Return \a maxIter_ attribute
  static BDat& MaxIter ()      { return(maxIter_); }
  //! Return \a tolerance_ attribute
  static BDat& Tolerance ()    { return(tolerance_); }
  //! Return \a reltolerance_ attribute
  static BDat& RelTolerance () { return(relTolerance_); }
  //! Return \a zero_ attribute
  static BDat Zero ()         { return(zero_); }

  //! Saves the number of the committed error
  static void  Matherr (int type, char* name);

  //! Return \a value_ attribute with format \a format_ attribute
  BText Name()  const ;
  //! Return \a value_ attribute with format \a format_ attribute
  BText Dump()  const { return(Format("%.16lg")); }
  //! Return \a value_ attribute with format \a format_ attribute
  BText Info()  const { return(Format("%.16lg")); }
  //! Applied the \a format to the \a value_ attribute
  BText Format (const BText& format) const ;
  //! Return \a format_ attribute
  static const BText& RealFormat () { return(format_ ); }
  //! Return \a formatL_ attribute
  static const BText& RealFormatL() { return(formatL_); }
  //! Updates the format to apply
  static void PutRealFormat(const BText& f)
  /*! PURPUSE: Updates the \a format_ and \a formatL_ attributes with the
  //            value of \a f
  //  \param f New format
  //  \sa BText::Replace
  */
  {
    format_ = f;
    formatL_= f;
    formatL_.Replace('l','L');
  }

  //! Return \a value_ attribute
  const BReal& GetValue()  const { return(value_); }
  //! Return \a value_ attribute
  BReal& GetValue() { return(value_); }
  //! Return \a value_ attribute
  const BReal&  Value() const { return(value_); }
  //! Return \a true if is a valid number
  BBool  Known()     const { return(!IS_NAN(value_)); }
  //! Return \a true if is a valid number
  BBool  IsKnown()   const { return(!IS_NAN(value_)); }
  //! Return \a true if is not a valid number (NaN)
  BBool  IsUnknown() const { return(IS_NAN(value_)); }
  //! Return \a true if is +PosInf
  BBool  IsPosInf()  const { return(value_==posInf_); }
  //! Return \a true if is -PosInf
  BBool  IsNegInf()  const { return(value_==negInf_); }
  //! Return \a true if is a finite valid number
  BBool  IsFinite()  const { return(IS_FINITE(value_)); }

  //! Indicates if \a x is multiple of this
  BBool  IsMultipleOf (BDat x) const;
  //! Indicates if \a value_ of BDat is integer
  BBool  IsInteger() const { return(IsMultipleOf(1)); }
  //! Indicates if \a value_ of BDat is pair
  BBool  IsEven()	 const { return(IsMultipleOf(2)); }

  //! Updates with the values of BChar
  void PutValue (const BChar* name);
  //! Updates with the values of BText
  void PutValue (const BText& expr);    //dattext.cpp
  //! Updates with the values of BText and specified format
  void PutValue(const BText& name, const BText& format);
  //! Updates \a value_ attribute with \a x
  void   PutValue (BReal x) { value_ = x; }
  //! If !t sets value to NaN, else if value_ is NaN sets value to 0
  void   PutKnown (BBool t = BKNOWN)
  {
    if(!t) { value_ = nan_; }
    else if(IS_NAN(value_)) { value_=0; }
  }

  //Implementation

  /*! Compare the \a value_ the two BDat indicating as it as greater
   * \param dat1 First BDat to compare
   * \param dat2 Second BDat to compare
   * \return Return 1: \a dat1 > \a dat2, 0: \a dat1 = \a dat2,
   *               -1: \a dat1 < \a dat2, BUNKNOWN in other case
   */
  static BDat Compare(const BDat* dat1, const BDat* dat2);
  //

  //Overloaded algebraic operators and functions
  BDat& operator += (const BDat& dat);
  BDat& operator -= (const BDat& dat);
  BDat& operator *= (const BDat& dat);
  BDat& operator /= (const BDat& dat);
  BDat& operator ^= (const BDat& dat);
  //! Obtain the remaider
  BDat& Mod	    (const BDat& dat);
  //! Make AND logic opeeration
  BDat& And	    (const BDat& dat);
  //! Make OR logic operation
  BDat& Or	    (const BDat& dat);
  //! Indicates if this if greater that \a dat
  BDat& Gt	    (const BDat& dat);
  //! Indicates if this if minor that \a dat
  BDat& Lt	    (const BDat& dat);
  //! Indicates if this if equal that \a dat
  BDat& Eq	    (const BDat& dat);
  //! Indicates if this if greater or equal that \a dat
  BDat& Ge	    (const BDat& dat);
  //| Indicates if this if minor or equal that \a dat
  BDat& Le	    (const BDat& dat);
  //! Indicates if this if different that \a dat
  BDat& Ne	    (const BDat& dat);

  //! Change to the sign of the attribute \a value_
  BDat& ChangeSign ();
  //! Denied to the value of the attribute \a value_
  BDat& Not ();
  //! Obtain the absolute value
  BDat& Abs ();
  //! Indicates the sign of the \a value_
  BDat& Sign ();
  //! Obtain the round \a value_ to the nearest integer
  BDat& Round ();
  //! Obtain the round \a value_ down to the nearest integer
  BDat& Floor ();
  //! Obtain the round \a value_ up to the nearest integer
  BDat& Ceil ();
  //! Obtain the square of \a value_
  BDat& Sqrt ();
  //! Obtain the base-10 logarithm
  BDat& Log10 ();
  //! Obtain the natural logarithm
  BDat& Log ();
  //! Obtain the value of e raised to the power
  BDat& Exp ();
  //! Obtain the value of sine
  BDat& Sin ();
  //! Obtain the value of cosine
  BDat& Cos ();
  //! Obtain the value of tangent
  BDat& Tan ();
  //! Obtain the value of arc sine
  BDat& ASin ();
  //! Obtain the value of arc cosine
  BDat& ACos ();
  //! Obtain the value of arc tangent
  BDat& ATan ();
  //! Obtain the value of hyperbolic sine
  BDat& SinH ();
  //! Obtain the value of hyperbolic cosine
  BDat& CosH ();
  //! Obtain the value of hyperbolic tengent
  BDat& TanH ();
  //! Obtain the value of inverse hyperbolic sine
  BDat& ASinH ();
  //! Obtain the value of inverse hyperbolic cosine
  BDat& ACosH ();
  //! Obtain the value of inverse hyperbolic tangent
  BDat& ATanH ();

  //! Compare two BDat and indicates if first it is greater than the second
  static BBool IsGreaterThan (const BDat& dat1, const BDat& dat2)
  /*! PURPUSE: Compare two BDat and indicates if first it is greater than the
  //                 second
  //  \param dat1 First BDat
  //  \param dat2 Second BDat
  //  \return Return TRUE if \a dat1 is greater that \a dat2, FALSE isnot
  */
  { return ((dat1 >  dat2)); }

  //! Compare two BDat and indicates if first it is greater  or equal than
  //! the second
  static BBool IsGreaterOrEqualThan (const BDat& dat1, const BDat& dat2)
  /*! PURPUSE: Compare two BDat and indicates if first it is greater or equal
  //                 than the second
  //  \param dat1 First BDat
  //  \param dat2 Second BDat
  //  \return Return TRUE if \a dat1 is greater or equal that \a dat2,
  //  \return FALSE isnot
  */
  { return ((dat1 >= dat2)); }

  //! Compare two BDat and indicates if first it is minor than the second
  static BBool IsLessThan (const BDat& dat1, const BDat& dat2)
  /*! PURPUSE: Compare two BDat and indicates if first it is minor than the
  //                 second
  //  \param dat1 First BDat
  //  \param dat2 Second BDat
  //  \return Return TRUE if \a dat1 is minor that \a dat2, FALSE isnot
  */
  { return ((dat1 <  dat2)); }

  //! Compare two BDat and indicates if first it is minor or equal than the
  //! second
  static BBool IsLessOrEqualThan (const BDat& dat1, const BDat& dat2)
  /*! PURPUSE: Compare two BDat and indicates if first it is minor or equal
  //                 than the second
  //  \param dat1 First BDat
  //  \param dat2 Second BDat
  //  \return Return TRUE if \a dat1 is minor or equal that \a dat2,
  //  \return FALSE isnot
  */
  { return ((dat1 <= dat2)); }

  //! Compare two BDat and indicates if first it is different than the second
  static BBool IsNotEqualAs(const BDat& dat1, const BDat& dat2)
  /*! PURPUSE: Compare two BDat and indicates if first it is different than
  //                 the second
  //  \param dat1 First BDat
  //  \param dat2 Second BDat
  //  \return Return TRUE if \a dat1 is different that \a dat2, FALSE isnot
  */
  { return ((dat1 != dat2)); }

  //! Compare two BDat and indicates if first it is equal than the second
  static BBool IsEqualAs(const BDat& dat1, const BDat& dat2)
  /*! PURPUSE: Compare two BDat and indicates if first it is equal than the
  //                 second
  //  \param dat1 First BDat
  //  \param dat2 Second BDat
  //  \return Return TRUE if \a dat1 is equal that \a dat2, FALSE isnot
  */
  { return ((dat1 == dat2)); }


  // Internal types definitions
  //! Defines a type that is a pointer a function member of BDat that does
  //! not have arguments and returns a referncia to a BDat
  typedef BDat& (BDat::*MonaryOperator)();
  //! Defines a type that is a pointer a function member of BDat that have
  //! BDat & arguments and returns a referncia to a BDat
  typedef BDat& (BDat::*BinaryOperator)(const BDat&);

};


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------

int TOL_API lc_format(double d, char* s);

BDat TOL_API operator +  (BInt n, const BDat& x);
BDat TOL_API operator -  (BInt n, const BDat& x);
BDat TOL_API operator *  (BInt n, const BDat& x);
BDat TOL_API operator /  (BInt n, const BDat& x);
BDat TOL_API operator ^  (BInt n, const BDat& x);

BDat TOL_API operator +  (const BDat& x, BInt n);
BDat TOL_API operator -  (const BDat& x, BInt n);
BDat TOL_API operator *  (const BDat& x, BInt n);
BDat TOL_API operator /  (const BDat& x, BInt n);
BDat TOL_API operator ^  (const BDat& x, BInt n);

BDat TOL_API operator +  (double r, const BDat& x);
BDat TOL_API operator -  (double r, const BDat& x);
BDat TOL_API operator *  (double r, const BDat& x);
BDat TOL_API operator /  (double r, const BDat& x);
BDat TOL_API operator ^  (double r, const BDat& x);

BDat TOL_API operator +  (long double r, const BDat& x);
BDat TOL_API operator -  (long double r, const BDat& x);
BDat TOL_API operator *  (long double r, const BDat& x);
BDat TOL_API operator /  (long double r, const BDat& x);
BDat TOL_API operator ^  (long double r, const BDat& x);

BDat TOL_API operator +  (const BDat& x, double r);
BDat TOL_API operator -  (const BDat& x, double r);
BDat TOL_API operator *  (const BDat& x, double r);
BDat TOL_API operator /  (const BDat& x, double r);
BDat TOL_API operator ^  (const BDat& x, double r);

BDat TOL_API operator +  (const BDat& x, long double r);
BDat TOL_API operator -  (const BDat& x, long double r);
BDat TOL_API operator *  (const BDat& x, long double r);
BDat TOL_API operator /  (const BDat& x, long double r);
BDat TOL_API operator ^  (const BDat& x, long double r);

BDat TOL_API operator +  (const BDat&, const BDat&);
BDat TOL_API operator -  (const BDat&, const BDat&);
BDat TOL_API operator *  (const BDat&, const BDat&);
BDat TOL_API operator /  (const BDat&, const BDat&);
BDat TOL_API operator ^  (const BDat&, const BDat&);
BDat TOL_API Pow	  (const BDat&, const BDat&);
BDat TOL_API Mod	  (const BDat&, const BDat&);


BBool TOL_API operator && (const BDat&, const BDat&);
BBool TOL_API operator || (const BDat&, const BDat&);

BDat  TOL_API operator -  (const BDat&);
BBool TOL_API operator !  (const BDat&);

//! Calculate the absolute value
BDat TOL_API Abs(const BDat&);
//! Indicates the sign of the \a value_
BDat TOL_API Sign(const BDat&);
//! Calculate the round \a value_ down to the nearest integer
BDat TOL_API Round	(const BDat&);
BDat TOL_API RoundD     (const BDat&, const BDat&);
//! Calculate the round \a value_ down to the nearest integer
BDat TOL_API Floor	(const BDat&);
//! Calculate the round \a value_ up to the nearest integer
BDat TOL_API Ceil(const BDat&);
//! Calculate the remaider of \a value_
BDat TOL_API Mod(const BDat&);
//! Calculate the square of \a value_
BDat TOL_API Sqrt(const BDat&);
//! Calculate the base-10 logarithm
BDat TOL_API Log10(const BDat&);
//! Calculate the natural logarithm
BDat TOL_API Log(const BDat&);
//! Calculate the value of e raised to the power
BDat TOL_API Exp(const BDat&);
//! Calculate the value of sine
BDat TOL_API Sin(const BDat&);
//! Calculate the value of cosine
BDat TOL_API Cos(const BDat&);
//! Calculate the value of tangent
BDat TOL_API Tan(const BDat&);
//! Calculate the value of arc sine
BDat TOL_API ASin(const BDat&);
//! Calculate the value of arc cosine
BDat TOL_API ACos(const BDat&);
//! Calculate the value of arc tangent
BDat TOL_API ATan(const BDat&);
//! Calculate the value of hyperbolic sine
BDat TOL_API SinH(const BDat&);
//! Calculate the value of hyperbolic cosine
BDat TOL_API CosH(const BDat&);
//! Calculate the value of hyperbolic tangent
BDat TOL_API TanH(const BDat&);
//! Calculate the value of inverse hyperbolic sine
BDat TOL_API ASinH(const BDat&);
//! Calculate the value of inverse hyperbolic cosine
BDat TOL_API ACosH(const BDat&);
//! Calculate the value of inverse hyperbolic tangen
BDat TOL_API ATanH(const BDat&);

//! Obtain the Frobenius norm without unknown values
BDat TOL_API FrobeniusNormU(const BArray<BDat>&);
//! Obtain the Frobenius norm even with unknown values
BDat TOL_API FrobeniusNorm(const BArray<BDat>&);
//! Show the \a value_ of the elements of the BArray in the output indicate
ostream TOL_API & operator<< (ostream& os, const BArray<BDat>& dat);
//! Build a string with the \a value_ of all the elements of \a arr
BText TOL_API Name (const BArray<BDat>& arr);
//! Compare value the two BDat
BInt TOL_API DatCmp(const void* v1, const void* v2);



//--------------------------------------------------------------------
// inline functions
//--------------------------------------------------------------------

inline BDat And (const BDat& dat1, const BDat& dat2) { BDat x=dat1; return(x.And(dat2)); }
inline BDat Or  (const BDat& dat1, const BDat& dat2) { BDat x=dat1; return(x.Or(dat2)); }
inline BDat Not (const BDat& dat)                    { BDat x=dat;  return(x.Not()); }
inline BDat Eq (const BDat& dat1, const BDat& dat2) { BDat x=dat1; return(x.Eq(dat2)); }
inline BDat Ne (const BDat& dat1, const BDat& dat2) { BDat x=dat1; return(x.Ne(dat2)); }
inline BDat Lt (const BDat& dat1, const BDat& dat2) { BDat x=dat1; return(x.Lt(dat2)); }
inline BDat Gt (const BDat& dat1, const BDat& dat2) { BDat x=dat1; return(x.Gt(dat2)); }
inline BDat Le (const BDat& dat1, const BDat& dat2) { BDat x=dat1; return(x.Le(dat2)); }
inline BDat Ge (const BDat& dat1, const BDat& dat2) { BDat x=dat1; return(x.Ge(dat2)); }

//! Sends the \a value to BText
inline BText& operator<< (BText& txt, const BDat& dat)
{ return(txt << (dat.Name())); }

inline BText& operator+= (BText& txt, const BDat& dat)
{ return(txt += (dat.Name())); }

inline BText operator+ (const BText& txt, const BDat& dat)
{ return(txt + (dat.Name())); }

//! Sends the \a value to output \a os
inline ostream& operator<< (ostream& os, const BDat& dat)
{
  BText txt =  dat.Name();
  return(os << txt.String());
}

//! Return \a true if dat is a valid number
inline BBool IsKnown(const BDat& dat)
{ return(dat.IsKnown()); }


//! Transform the \a value_ of \a d to text with format
inline BText TxtFmt(const BDat& d, const BChar* format = "%.16lg")

/*! Transform the \a value_ of \a d to text with format
 * \param d BDat from which obtain the value
 * \param format Format to apply, default "%.16lg"
 * \return Return a text with the attribute \a value_
 */
//--------------------------------------------------------------------
{
    BText txt;
    txt += d.Format(format);
    return(txt);
}


template class TOL_API BArray<BDat>;

//--------------------------------------------------------------------
  class TOL_API BData : public BArray<BDat>
//--------------------------------------------------------------------
{
private:
  static int    count_;
  static double kBytes_;

public:
  static bool InitializeClass();
  //! Does nothing for static members initialization
  BData(const BStaticInit& unused) { }

  BData(int len=0) : BArray<BDat>(len)
  {
    count_++;
    kBytes_ += MaxSize()*sizeof(BDat)/1024.0;
  };
  BData(const BArray<BDat>& data) : BArray<BDat>(0)
  {
    count_++;
    Copy(data.Size(), data.Buffer()); 
    kBytes_ += MaxSize()*sizeof(BDat)/1024.0;
  };
  BData(const BData& data) : BArray<BDat>(0)
  {
    count_++;
    Copy(data.Size(), data.Buffer()); 
    kBytes_ += MaxSize()*sizeof(BDat)/1024.0;
  };
 ~BData()
  {
    count_--;
    kBytes_ -= MaxSize()*sizeof(BDat)/1024.0;
  }
  BData& operator=(const BArray<BDat>& data)
  {
    Copy(data.Size(), data.Buffer());
    return(*this);
  }
  BData& operator=(const BData& data)
  {
    Copy(data.Size(), data.Buffer());
    return(*this);
  }
  void ReallocBuffer(int length)
  {
    double oldBytes = MaxSize()*sizeof(BDat);
    BArray<BDat>::ReallocBuffer(length);
    double bytes = MaxSize()*sizeof(BDat);
    kBytes_ += double(bytes-oldBytes)/1024.0;
  }
  void AllocBuffer(int length)
  {
    double oldBytes = MaxSize()*sizeof(BDat);
    BArray<BDat>::AllocBuffer(length);
    double bytes = MaxSize()*sizeof(BDat);
    kBytes_ += double(bytes-oldBytes)/1024.0;
  }
  void DeleteBuffer()
  {
    double oldBytes = MaxSize()*sizeof(BDat);
    BArray<BDat>::DeleteBuffer();
    double bytes = MaxSize()*sizeof(BDat);
    kBytes_ += double(bytes-oldBytes)/1024.0;
  }
  static int    Count () { return(count_ ); }
  static double KBytes() { return(kBytes_); }

  };

#endif // TOL_BDAT_H
