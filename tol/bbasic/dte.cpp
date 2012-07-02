/* dte.h: BDate class behavior.
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

#include <tol/tol_bout.h>
#include <tol/tol_bdate.h>


//--------------------------------------------------------------------
// inicialization
//   Global variables (static in the class BDate).
//--------------------------------------------------------------------
BTraceInit("dte.cpp");

BDateFormat BDateFormat::defect_(StaticInit());
BDate  BDate::unknown_     (StaticInit());
BDate  BDate::begin_	     (StaticInit());
BDate  BDate::end_         (StaticInit());
BDate  BDate::defaultFirst_(StaticInit());
BDate  BDate::defaultLast_ (StaticInit());

static const char* ignored_ = "--";

const char* BDateFormat::shortMonth_[12][2] = 
{
  {"Jan", "Ene"},
  {"Feb", "Feb"},
  {"Mar", "Mar"},
  {"Apr", "Abr"},
  {"May", "May"},
  {"Jun", "Jun"},
  {"Jul", "Jul"},
  {"Aug", "Ago"},
  {"Sep", "Sep"},
  {"Oct", "Oct"},
  {"Nov", "Nov"},
  {"Dec", "Dic"}
};
const char* BDateFormat::longMonth_[12][2]=
{
  {"January",    "Enero"},
  {"February",   "Febrero"},
  {"March",      "Marzo"},
  {"April",      "Abril"},
  {"May",	       "Mayo"},
  {"June",       "Junio"},
  {"July",       "Julio"},
  {"August",     "Agosto"},
  {"September",  "Septiembre"},
  {"October",    "Octubre"},
  {"November",   "Noviembre"},
  {"December",   "Diciembre"}
};
const char* BDateFormat::shortWeekDay_[7][2]=
{
  {"Mon", "Lun"},
  {"Tue", "Mar"},
  {"Wed", "Mie"},
  {"Thu", "Jue"},
  {"Fri", "Vie"},
  {"Sat", "Sab"},
  {"Sun", "Dom"}
};
const char* BDateFormat::longWeekDay_[7][2] =
{
  {"Monday",	  "Lunes"},
  {"Tuesday",	  "Martes"},
  {"Wednesday", "Miercoles"},
  {"Thursday",	"Jueves"},
  {"Friday",	  "Viernes"},
  {"Saturday",	"Sabado"},
  {"Sunday",	  "Domingo"}
};

DefIsAlreadyInitilialized(BDate);
DefIsAlreadyInitilialized(BDateFormat);

//--------------------------------------------------------------------
  bool BDate::InitializeClass()
//--------------------------------------------------------------------
{
  if(!IsAlreadyInitilialized(BDate)) 
  {
    BText::InitializeClass();
    BDate::begin_ = BDate(1582, DECEMBER, 31);
    BDate::end_ = BDate(2500, JANUARY, 1);
    BDate::defaultFirst_ = BDate(DteToday().Year(), JANUARY,   1);
    BDate::defaultLast_  = BDate(DteToday().Year(), DECEMBER, 31);
    BDateFormat::InitializeClass();
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BDateFormat::InitializeClass()
//--------------------------------------------------------------------
{
  if(!IsAlreadyInitilialized(BDateFormat)) 
  {
    //ignored_ = "--";
    BDateFormat::defect_ = BDateFormat("%cy%Ym%md%d%uh%hi%is%s");
  }
  return(true);
}


//--------------------------------------------------------------------
   const char* BDateFormat::ShortMonth(BInt n, BInt id)

/*! Returns a text with the short name of the \a n-th month of the
 *  year in language specified by \a id.
 * \param n Number of month to return its name
 * \param id Identifier of language
 * \return Short month name if it exists or string ignored_ if not
 */  
//--------------------------------------------------------------------
{
    if((n>=1)&&(n<=12)) { return(shortMonth_[n-1][id]); }
    else	        { return(ignored_); }
}


//--------------------------------------------------------------------
const char* BDateFormat::LongMonth(BInt n, BInt id)

/*! Returns a text with the long name of the \a n-th month of the
 *  year in language specified by \a id.
 * \param n Number of month to return its name
 * \param id Identifier of language
 * \return Long month name if it exists or string ignored_ if not
 */
//--------------------------------------------------------------------
{
  if((n>=1)&&(n<=12)) { return(longMonth_[n-1][id]); }
  else		      { return(ignored_); }
}


//--------------------------------------------------------------------
const char* BDateFormat::ShortWeekDay(BInt n, BInt id)

/*! Returns a text with the short name of the \a n-th weekday of the
 *  week in language specified by \a id.
 * \param n Number of weekday to return its name
 * \param id Identifier of language
 * \return Short weekday day if it exists or string ignored_ if not
 */
//--------------------------------------------------------------------
{
  if((n>=1)&&(n<= 7)) { return(shortWeekDay_[n-1][id]); }
  else		      { return(ignored_); }
}


//--------------------------------------------------------------------
const char* BDateFormat::LongWeekDay(BInt n, BInt id)

/*! Returns a text with the long name of the \a n-th weekday of the
 *  week in language specified by \a id.
 * \param n Number of weekday to return its name
 * \param id Identifier of language
 * \return Long weekday day if it exists or string ignored_ if not
 */
//--------------------------------------------------------------------
{
  if((n>=1)&&(n<= 7)) { return(longWeekDay_[n-1][id]); }
  else		      { return(ignored_); }
}


//--------------------------------------------------------------------
void BDate::PutDefaultFirst(const BDate& f)

/*! Changes the default first date for time series calculating
 * \param f New first date
 */
//--------------------------------------------------------------------
{
  BDate::defaultFirst_ = f;
}


//--------------------------------------------------------------------
void   BDate::PutDefaultLast (const BDate& l)

/*! Changes the default last date for time series calculating
 * \param l New last date
 */
//--------------------------------------------------------------------
{
    BDate::defaultLast_  = l;
}

//#define TRACE_COMPARE
BInt BDate::Compare(const BDate* dte1, const BDate* dte2) {
#ifdef TRACE_COMPARE
  std::cout << "Compare: " <<  std::endl;
  std::cout << "Hash(dte1) = " <<  dte1->Hash() << std::endl;
  std::cout << "Hash(dte2) = " <<  dte2->Hash() << std::endl;    
#endif
  BReal h1 = dte1->Hash();
  BReal h2 = dte2->Hash();
  BReal dif = h1 - h2;
#ifdef TRACE_COMPARE
  std::cout << "dif: " << dif << std::endl;
  std::cout << "h1 < h2: " << (h1<h2) << std::endl;
#endif
  BReal dif0 = fabs( dif );
  return dif < -1.0e-7 ? -1 : ( dif0 > 1.0e-7 ? 1 : 0 );
}

//--------------------------------------------------------------------
BDate::BDate()

/*! Creates a date and initializes it with default values that defines
 *  the unknown date
 */
//--------------------------------------------------------------------
    :  year_(0), month_(0), day_(0), hour_(0), minute_(0), second_(0.0)
{
}


//--------------------------------------------------------------------
BDate::BDate(const BDate& dte)

/*! Creates a date and initializes it with date \a dte
 * \param dte Date to initialize new date object
 */
//--------------------------------------------------------------------
    :  year_(dte.Year()), month_(dte.Month()), day_(dte.Day()),
       hour_(dte.Hour()), minute_(dte.Minute()), second_(dte.Second())
{
}


//--------------------------------------------------------------------
BDate::BDate(BInt y, BMonth m, BInt d, BInt h, BInt mi, BReal s)

/*! Creates a date and initializes it with given values
 * \param y  Year of date
 * \param m  Month of date
 * \param d  Day of date
 * \param h  Hours of date
 * \param mi Minutes of date
 * \param s  Seconds of date
 */
//--------------------------------------------------------------------
    : year_(y), month_(m), day_(d), hour_(h), minute_(mi), second_(s)
{
}


//--------------------------------------------------------------------
BDate::~BDate()

/*! Destroyes a date.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BInt BDate::BinWrite(FILE* fil) const

/*! Appends this BDate to a binary file.
 * \param fil Binary file stream to write in
 */
//--------------------------------------------------------------------
{
    fwrite(&year_,   1, sizeof(BInt),  fil);
    fwrite(&month_,  1, sizeof(BInt),  fil);
    fwrite(&day_,    1, sizeof(BInt),  fil);
    fwrite(&hour_,   1, sizeof(BInt),  fil);
    fwrite(&minute_, 1, sizeof(BInt),  fil);
    fwrite(&second_, 1, sizeof(BReal), fil);
    return(5*sizeof(BInt)+sizeof(BReal));
}


//--------------------------------------------------------------------
BInt BDate::BinRead(FILE* fil)

/*! Reads this BDate from a binary file.
 * \param fil Binary file stream to read from
 */
//--------------------------------------------------------------------
{
    fread(&year_,   1, sizeof(BInt),  fil);
    fread(&month_,  1, sizeof(BInt),  fil);
    fread(&day_,    1, sizeof(BInt),  fil);
    fread(&hour_,   1, sizeof(BInt),  fil);
    fread(&minute_, 1, sizeof(BInt),  fil);
    fread(&second_, 1, sizeof(BReal), fil);
    return(5*sizeof(BInt)+sizeof(BReal));
}

