/* tol_bdate.h: BDate class declarations.
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

#ifndef TOL_BDATE_H
#define TOL_BDATE_H 1

#include <tol/tol_btext.h>


//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BMonth BInt
#define JANUARY	   1
#define FEBRUARY   2
#define MARCH	     3
#define APRIL	     4
#define MAY	       5
#define JUNE	     6
#define JULY	     7
#define AUGUST	   8
#define SEPTEMBER  9
#define OCTOBER	  10
#define NOVEMBER  11
#define DECEMBER  12

#define BWeekDay BInt
#define MONDAY	   1
#define TUESDAY	   2
#define WEDNESDAY  3
#define THURSDAY   4
#define FRIDAY	   5
#define SATURDAY   6
#define SUNDAY	   7


//--------------------------------------------------------------------
// constants
//--------------------------------------------------------------------
#define DayNumberZero 693595 // y1899m12d31 --> DayNumberOne y1900m01d01

//! Days in a 400 year cicle
#define DAYSCICLE400 (BInt) 146097
//! Days in a 100 year cicle
#define DAYSCICLE100 (BInt)  36524
//! Days in a 4 year cicle
#define DAYSCICLE4   (BInt)   1461
//! Days in a non leap year cicle
#define DAYSCICLE1   (BInt)    365


//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BDate;

/*!  \brief Class to help text formating for dates.
 *          Functionality has been implemented in dte.cpp and dtealgeb.cpp
 */
//--------------------------------------------------------------------
class TOL_API BDateFormat

/*! Class to help text formating for dates
 */
//--------------------------------------------------------------------
{
private:
  //! Date format
  BText	 format_;
  //! Default date format
  static BDateFormat defect_;
  //! Short month names
  static const char* shortMonth_   [12][2];
  //! Long month names
  static const char* longMonth_    [12][2];
  //! Short weekday names
  static const char* shortWeekDay_ [ 7][2];
  //! Long weekday names
  static const char* longWeekDay_  [ 7][2];

public:
  static bool InitializeClass();

  // Constructors and destructors: 
  //! Does nothing for static members initialization
  BDateFormat(const BStaticInit& unused): format_(unused) {}
  //! Creates a date format and fills it with a text \a format
  BDateFormat(const BText& format)
  : format_(format) {}
  //! Creates a date format and fills it with another date format \a format
  BDateFormat(const BDateFormat& format)
  : format_(format.Format()) {}
  //! Destroyes a date format
 ~BDateFormat() {}

  // Access and manipulation: inline
  //! Overwrites this operator
  const BDateFormat& operator= (const BDateFormat& format);
  const BDateFormat& operator= (const BText& format);
  //! Returns the actual format string
  const BText& Format() const { return(format_); }

  //! Takes a formated text and returns the corresponding date
  BDate TextToDate(const BText& txt) const;
  //! Takes a date and returns the corresponding formated text
  BText DateToText(const BDate& dte) const;

  //! Returns the default date format
  static BDateFormat& Defect() { return(defect_); }

  //! Returns the short name of the \a n-th month of the year in a given language
  static const char* ShortMonth(BInt n, BInt id=BText::Language());
  //! Returns the long name of the \a n-th month of the year in a given language
  static const char* LongMonth (BInt n, BInt id=BText::Language());
  //! Returns the short name of the \a n-th weekday in a given language
  static const char* ShortWeekDay  (BInt n, BInt id=BText::Language());
  //! Returns the long name of the \a n-th weekday in a given language
  static const char* LongWeekDay   (BInt n, BInt id=BText::Language());


};

/*!  \brief Dates representation.
 *          Functionality has been implemented in dte.cpp and dtealgeb.cpp
 */
//--------------------------------------------------------------------
class TOL_API BDate

/*! Dates (year, month, day) representation. This is the minimal unit
 *  for social timing and has a complex behavior in comparition with
 *  minor time units that have hexadecimal and decimal treatement.
 *  This class avails all needed functions to simplify time operations
 *  like Easter Sunday calculation for any year in past or future,
 *  number of days between two done dates ...
 */
//--------------------------------------------------------------------
{

private:

#ifdef SHORT_BDATE_MODE_
  //! Number of year, normally in the range 0 to 9999
  short	 year_;
  //! The month of the year, in the range 1 to 12
  unsigned char month_;
  //! The day of the month, in the range 1 to 31
  unsigned char	 day_;
  //! The number of hours past midnight, in the range 0 to 23
  unsigned char	 hour_;
  //! The number of minutes after the hour, in the range 0 to 59
  unsigned char	 minute_;
  //! The number of seconds after the minute
  float	 second_;
#else
  //! Number of year, normally in the range 0 to 9999
  BInt	 year_;
  //! The month of the year, in the range 1 to 12
  BMonth month_;
  //! The day of the month, in the range 1 to 31
  BInt	 day_;
  //! The number of hours past midnight, in the range 0 to 23
  BInt	 hour_;
  //! The number of minutes after the hour, in the range 0 to 59
  BInt	 minute_;
  //! The number of seconds after the minute
  BReal	 second_;
#endif

  // Global class constants
  //! The beggining of times
  static BDate begin_;
  //! The end of times
  static BDate end_;
  //! An unknown date
  static BDate unknown_;
  //! The default first date
  static BDate defaultFirst_;
  //! The default las date
  static BDate defaultLast_;

public:
  static bool InitializeClass();
  // Constructors and destructors: 
  //! Does nothing for static members initialization
  BDate(const BStaticInit& unused) { }
  //! Creates a date and initializes it with default values
  BDate();
  //! Creates a date and initializes it with a given date object
  BDate(const BDate& dte);
  //! Creates a date and initializes it with given values
  BDate(BInt y, BMonth m, BInt d=1,
	BInt h=0, BInt mi=0, BReal s=0.0);
  //! Destroyes a date
 ~BDate();

 //! Appends this BDate to a binary file
  BInt BinWrite(FILE*) const;
 //! Reads this BDate from a binary file
  BInt BinRead (FILE*);

  // Virtual functions redefinition: inline
  //! Returns a text with the year as a decimal number including the century
  BText TxtLongYear	() const;
  //! Returns a text with the year as a decimal number without the century
  BText TxtShortYear	() const;
  //! Returns a text with the month as a decimal number
  BText TxtMonth	() const;
  //! Returns a text with the abbreviated month name
  BText TxtShortMonth	() const;
  //! Returns a text with the full month name
  BText TxtLongMonth	() const;
  //! Returns a text with the day of the month as a decimal number
  BText TxtDay		() const;
  //! Returns a text with the abbreviated weekday name
  BText TxtShortWeekDay () const;
  //! Returns a text with the full weekday name
  BText TxtLongWeekDay	() const;
  //! Returns the name of the actual date
  BText Name		() const;
  //! Returns the contens of the actual date
  BText Dump		() const { return(Name()); }
  //! Returns information about BDate object
  BText Info		() const { return(Name()); }
  //! Returns a text with actual date formatted according to a given format
  BText GetText(const BDateFormat& format = BDateFormat::Defect()) const
  { return(format.DateToText(*this)); }

  // Access and manipulation: inline
  //! Returns the year of this date
  BInt	 Year()	      const { return(year_);  }
  //! Returns the number of month of this date
  BMonth Month()      const { return(month_); }
  //! Returns the number of day of this date
  BInt	 Day()	      const { return(day_);   }
  //! Returns the number of hours of this date
  BInt	 Hour()	      const { return(hour_); }
  //! Returns the number of minutes of this date
  BInt	 Minute()     const { return(minute_); }
  //! Returns the number of seconds of this date
  BReal	 Second()     const { return(second_); }
  //! Returns the fraction of day for this date
  BReal	 Fraction()   const;

  //! Returns if the object is the begin date
  BBool	 IsTheBegin() const { return(day_ && (year_<=begin_.Year())); }
  //! Returns if the object is the end date
  BBool	 IsTheEnd()   const { return(day_ && (year_>=end_  .Year())); }
  //! Returns if the object is the unknown date
  BBool	 IsUnknown()   const { return(day_==0); }
  //! Returns if the object contains a valid date or not
  BBool	 HasValue()   const;

  //! Sets year for the actual date
  void	PutYear	    (BInt    year  ) { year_	= year;	  }
  //! Sets month for the actual date
  void	PutMonth    (BMonth  month ) { month_	= month;  }
  //! Sets day for the actual date
  void	PutDay	    (BInt    day   ) { day_	= day;	  }
  //! Sets hour for the actual date
  void	PutHour	    (BInt    hour  ) { hour_	= hour;	  }
  //! Sets minute for the actual date
  void	PutMinute   (BInt    minute) { minute_	= minute; }
  //! Sets second to the actual date
  void	PutSecond   (BReal   second) { second_	= second; }
  //! Sets hours, minutes and seconds corresponding to a given day fraction
  void	PutFraction (BReal   fraction);
  //! Sets the date corresponding to an index (amount of days pass from the 
  //! predefined day number zero)
  void	PutIndex    (BReal   index);
  //! Sets the date corresponding to an index (amount of days pass from the 
  //! predefined day number zero)
  void	PutDayIndex (BInt    index);
  //! Sets date attributes corresponding to the given parameters (year, month, day, ...)
  void	PutDate(BInt y, BMonth m=JANUARY, BInt d=1,
		BInt h=0, BInt mi=0, BReal s=0.0)
  {
    year_=y;
    month_=m;
    day_=d;
    hour_=h;
    minute_=mi;
    second_=s;
  }
  //! Sets date attributes from a Text \a txt (containig a date) and a DateFormat \a format
  void	PutDate(const BText& txt,
		const BDateFormat& format = BDateFormat::Defect())
  { (*this) = format.TextToDate(txt); }
  //! Puts object attributes corresponding with a hash value
  void	PutHash(BReal hash);
  // Access and manipulation to global class constants: inline

  //! Returns the beggining of times date object
  static BDate& Begin()	       { return(begin_); }
  //! Returns the end of times date object
  static BDate& End()	       { return(end_); }
  //! Returns the unknown date object
  static BDate& Unknown()      { return(unknown_); }
  //! Returns the default first date object
  static BDate& DefaultFirst() { return(defaultFirst_); }
  //! Returns the default last date object
  static BDate& DefaultLast () { return(defaultLast_ ); }

  //! Changes the default first date for time series calculating
  static void	PutDefaultFirst(const BDate& f);
  //! Changes the default last date for time series calculating
  static void	PutDefaultLast (const BDate& l);
  
  /*! Compares two dates given as parameters
   * \param dte1 BDate object to compare
   * \param dte2 BDate object to compare
   * \return Value indicating the lexicographical relation between the dates.
   */
  static BInt Compare(const BDate* dte1, const BDate* dte2) {
      BReal dif = dte1->Hash() - dte2->Hash();
      return dif>0 ? 1 : (dif<0 ? -1 : 0);
  }

  // Support to algebraical operations: dtealgeb.cpp
  //! Returns the number of days that rest until the year's end
  BInt	DaysRestInYear () const;
  //! Returns the number of days that have been passed in this year
  BInt	DaysPassInYear () const;
  //! Returns the index of the actual date as the amount of days from the
  //! predefined day number zero
  BInt	DayIndex       () const;
  //! Returns the index of the actual date as the amount of hours from the
  //! predefined day number zero
  BInt	HourIndex      () const { return(DayIndex()*24+Hour()); }
  //! Returns the index of the actual date as the amount of minutes from the
  //! predefined day number zero
  BInt	MinuteIndex    () const { return(HourIndex()*60+Minute()); }
  //! Returns the index of the actual date as the amount of seconds from the
  //!  predefined day number zero
  BReal SecondIndex    () const { return(MinuteIndex()*60+Second()); }
  //! Returns the index of the actual date as the amount of days (and fraction
  //! of day) from the predefined day number zero
  BReal Index	       () const { return(DayIndex()+Fraction()); }
  //! Returns a hashing real value of this date
  BReal Hash	       () const;

  //! If s>0 increments s seconds. If s<0 decrements s seconds.
  void IncSecond (BReal s=1);
  //! Pass the date to the \a m-th following minute
  void IncMinute (BInt	m=1);
  //! Pass the date to the \a h-th following hour.
  void IncHour	 (BInt	h=1);
  //! Pass the date to the \a d-th following day.
  void IncDay	 (BInt	d=1);
  //! Pass the date to the \a w-th following week.
  void IncWeek	 (BInt	w=1);
  //! Pass the date to the \a m-th following month.
  void IncMonth	 (BInt	m=1);
  //! Pass the date to the \a y-th following year.
  void IncYear	 (BInt	y=1);
  //! Increments (or decrements) the base \a base of the actual date
  void	IncBase	  (BInt	 n=1, BReal base=1);
  //! Checks if the actual date has base \a base
  BBool HasBase(BReal base) const;

  //! Returns the date object corresponding to a given day index
  friend BDate  DayIndexToDte(BInt  i);
  //! Returns the date object corresponding to a given day index
  friend BDate  IndexToDte   (BReal i);
  //! Returns the date object corresponding to a given hash value
  friend BDate  HashToDte    (BReal h);

  // Miscellaneous functions: dtemisc.cpp
  //! Returns the weekday of the actual date
  BWeekDay     WeekDay()	 const;

  // Operators: dteoper.cpp
  BDate& operator=  (const BDate&);

  BDate& operator+= (BReal x) { PutIndex(Index()+x); return(*this); }
  BDate& operator-= (BReal x) { PutIndex(Index()-x); return(*this); }

  BDate	 operator + (BReal x) const { return(BDate(*this)+=x); }
  BDate	 operator - (BReal x) const { return(BDate(*this)-=x); }

  BReal	 operator - (const BDate& d) const { return(Index()-d.Index()); }

  //! Returns the difference in years between two dates
  friend BInt DifInYears(BDate d1, BDate d2)
  {
    return(d2.Year()-d1.Year());
  }
  //! Returns the difference in months between two dates
  friend BInt DifInMonths(BDate d1, BDate d2)
  {
    return(12*DifInYears(d1,d2)+(d2.Month()-d1.Month()));
  }
  //! Returns the difference in weeks between two dates
  friend BInt DifInWeeks(BDate d1, BDate d2)
  {
    return(DifInDays(d1,d2)/7);
  }
  //! Returns the difference in days between two dates
  friend BInt DifInDays(BDate d1, BDate d2)
  {
    return(d2.DayIndex()-d1.DayIndex());
  }
  //! Returns the difference in hours between two dates
  friend BInt DifInHours(BDate d1, BDate d2)
  {
    return(d2.HourIndex()-d1.HourIndex());
  }
  //! Returns the difference in minutes between two dates
  friend BInt DifInMinutes(BDate d1, BDate d2)
  {
    return(d2.MinuteIndex()-d1.MinuteIndex());
  }
  //! Returns the difference in seconds between two dates
  friend BReal DifInSeconds(BDate d1, BDate d2)
  {
    return(d2.SecondIndex()-d1.SecondIndex());
  }


  // Relational operators: dteoper.cpp

  BBool	 operator<  (const BDate&) const;
  BBool	 operator<= (const BDate&) const;
  BBool	 operator>= (const BDate&) const;
  BBool	 operator>  (const BDate&) const;
  BBool	 operator== (const BDate&) const;
  BBool	 operator!= (const BDate&) const;

};

inline BInt DateCmp(const void* v1, const void* v2)
{
  const BDate* dte1 = (BDate*)v1;
  const BDate* dte2 = (BDate*)v2;
  BReal dif = dte1->Hash() - dte2->Hash();
  return dif>0 ? 1 : (dif<0 ? -1 : 0);
}

/* ARM-style name-injection of friend declaration not ANSI/C++
   standard.*/
//! Returns the date object corresponding to a given day index
BDate  DayIndexToDte(BInt  i);
//! Returns the date object corresponding to a given day index
BDate  IndexToDte   (BReal i);
//! Returns the date object corresponding to a given hash value
BDate  HashToDte    (BReal h);

//--------------------------------------------------------------------
// external funcition
//--------------------------------------------------------------------
//! Compares two dates given as parameters
//TOL_API BInt Compare(const BDate* dte1, const BDate* dte2);
//! Returns today date
TOL_API BDate DteToday();
//! Returns todays date and now time
TOL_API BDate DteNow();
//! Returns the date of the Easter's Sunday in the year given as parameter
TOL_API BDate DteEasterSunday(BInt year);
//! Returns the number of days in the month \a m of the year \a y
TOL_API BInt DteDaysInMonth(BMonth month, BInt year);
//! Takes a formated text and returns the corresponding date for defect format
TOL_API BDate TextToDate(const BText& txt);
//! Returns a valid constant date for name
TOL_API BDate ConstantDate(const BText& name);
TOL_API BText& operator<< (BText& txt, const BDate& dte);
TOL_API BText& operator+= (BText& txt, const BDate& dte);
TOL_API BText operator+ (const BText& txt, const BDate& dte);
TOL_API ostream& operator<< (ostream& os, const BDate& dte);
//! Returns if a year is a leap year
TOL_API BBool DteYearIsLeap(BInt A);
//! Returns the number of days of a given year
TOL_API BInt DteDaysInYear(BInt y);
//! Returns the month after a given one
TOL_API BMonth DteNextMonth(BMonth m);
//! Returns the month before a given one
TOL_API BMonth DtePrevMonth(BMonth m);

#endif // BDATE
