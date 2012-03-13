/* dtealgeb.h: BDate class algebraic functions behavior.
               GNU/TOL Language.

   Copyright (C) 2003-2005 - Bayes Decision, SL (Spain [EU])

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

#include <math.h>

BTraceInit("dtealgeb.cpp");

static const BInt  dsByDay_  = 24*3600*100;
static const BReal dsScale_  = 1.0/dsByDay_;
static BDateFormat dateFormat_[] =
{
  BDateFormat("y%Ym%md%dh%hi%is%s"),
  BDateFormat("a%Ym%md%dh%hi%is%s"),
  BDateFormat("%Y/%m/%d:%h:%i:%s")
};



//--------------------------------------------------------------------
BDate ConstantDate(const BText& name)

/*! Returns a valid constant date for name.
 */
//--------------------------------------------------------------------
{
  BDate dte;
  if(name.Length()<3) { return(dte); }
       if(name(0)=='y') { dte =	 dateFormat_[0].TextToDate(name); }
  else if(name(0)=='a') { dte =	 dateFormat_[1].TextToDate(name); }
  else			{ dte =	 dateFormat_[2].TextToDate(name); }
  if(dte.IsTheEnd()) { dte = BDate::End(); }
  if(dte.IsTheBegin()) { dte = BDate::Begin(); }
  if(!dte.HasValue())
  {
    Error(name+I2(" is not a valid date"," no es una fecha válida"));
  }
  return(dte);
}


//--------------------------------------------------------------------
const BDateFormat& BDateFormat::operator= (const BText& format)

/*! Copies a date format
 */
//--------------------------------------------------------------------
{
  format_ = format;
  return(*this);
}


//--------------------------------------------------------------------
const BDateFormat& BDateFormat::operator= (const BDateFormat& format)

/*! Copies a date format
 */
//--------------------------------------------------------------------
{
  if(this!=&format)
  {
    format_ = format.Format();
  }
  return(*this);
}


//--------------------------------------------------------------------
BText BDateFormat::DateToText(const BDate& dte) const

/*! Takes a date and returns the corresponding formated text
 * \param dte BDate object to transform in formated text
 * \return Text object corresponding to BDate \a date
 */
//--------------------------------------------------------------------
{
  if(dte==BDate::Begin()) { return("TheBegin"); }
  if(dte==BDate::End())	  { return("TheEnd"  ); }
  if(!dte.HasValue())	  { return("Unknown" ); }

  bool uI, uH, uD, uM, uY, u, end;

  BText txt;
  txt.ReallocateBuffer(32);

  uI = (dte.Second()==0);
  uH = uI && (dte.Minute()==0);
  uD = uH && (dte.Hour()==0);
  uM = uD && (dte.Day()==1);
  uY = uM && (dte.Month()==1);
  u = false;
  end = false;
  
  BChar h [20]; sprintf(h , "%02d",  dte.Hour	());
  BChar mi[20]; sprintf(mi, "%02d",  dte.Minute());
  BChar s [20]; sprintf(s , "%05.2lf", dte.Second());
  for(BInt i=0; !(u&&end) && (i<format_.Length()); i++)
  {
    if(format_(i)=='%')
    {
      i++;
      if ( i >= format_.Length() ) break;
      switch(format_(i))
      {
	case 'u' : u = BTRUE;			  break;
	case 'c' : u = BFALSE;			  break;
	case 'Y' : txt += dte.TxtLongYear     (); end = uY; break;
	case 'y' : txt += dte.TxtShortYear    (); end = uY; break;
	case 'm' : txt += dte.TxtMonth	      (); end = uM; break;
	case 'n' : txt += dte.TxtShortMonth   (); end = uM; break;
	case 'N' : txt += dte.TxtLongMonth    (); end = uM; break;
	case 'd' : txt += dte.TxtDay	      (); end = uD; break;
	case 'w' : txt += dte.TxtShortWeekDay (); break;
	case 'W' : txt += dte.TxtLongWeekDay  (); break;
	case 'h' : txt += h			; end = uH; break;
	case 'i' : txt += mi			; end = uI; break;
	case 's' : txt += s			; break;
        case '%' : txt += "%"                   ; break;
	default	 : txt += BText("%")+format_(i) ; break;
      }
    }
    else
    {
      txt += format_(i);
    }
  }
  return(txt);
}


//--------------------------------------------------------------------
BDate BDateFormat::TextToDate(const BText& date) const

/*! Takes a formated text and returns the corresponding date with 
 *  the actual date format
 * \param date Formated text that represents a date
 * \return Date object corresponding to BText \a date
 */
//--------------------------------------------------------------------
{
  if(date=="Unknown" )	 { return(BDate::Unknown()); }
  BDate dte;
  long int i, Y=0, y=-1, m=1, d=1, h=0, mi=0;
  BReal s = 0.0;
  BChar n[64]; n[0]='\0';
  BChar w[64]; w[0]='\0';
  const BChar* fmt = format_.String();
  const BChar* txt = date.String();

  BBool fail = BFALSE;
  for(i=0; /*!fail &&*/ fmt[0] && txt[0]; i++, fmt++)
  {
//  if(!(fail = (!!format_[i] != !!txt[0])))
    {
      if(fmt[0]=='%')
      {
	i++;
	fmt++;
	BInt nChar = 0;
	switch(fmt[0])
	{
	  case 'Y' : fail=!sscanf(txt, "%4ld%n", &Y,  &nChar); break;
	  case 'y' : fail=!sscanf(txt, "%2ld%n", &y,  &nChar); break;
	  case 'm' : fail=!sscanf(txt, "%2ld%n", &m,  &nChar); break;
	  case 'n' : fail=!sscanf(txt, "%3s%n",  n,  &nChar); break;
	  case 'N' : fail=!sscanf(txt, "%64s%n", n,  &nChar); break;
	  case 'd' : fail=!sscanf(txt, "%2ld%n", &d,  &nChar); break;
	  case 'w' : fail=!sscanf(txt, "%64s%n", w,  &nChar); break;
	  case 'W' : fail=!sscanf(txt, "%64s%n", w,  &nChar); break;
	  case 'h' : fail=!sscanf(txt, "%2ld%n", &h,  &nChar); break;
	  case 'i' : fail=!sscanf(txt, "%2ld%n", &mi, &nChar); break;
	  case 's' : fail=!sscanf(txt, "%lf%n", &s,  &nChar); break;
	  default  : fail=BTRUE; break;
	}
//	  if(!fail&&(nChar>0))
	{
	  if(nChar<=format_.Length()-i) { txt += nChar; }
	}
      }
      else
      {
	fail = (txt[0]!=fmt[0]);
	txt++;
      }
    }
  }
//if(!fail)
  {
    if((Y==0)&&(y>=0))
    {
      Y = ((y>20)?1900:2000) + y;
    }
    if(n[0])
    {
      n[3] = '\0';
      for(m=1; (m<=12)
            && strcasecmp(shortMonth_[m-1][0],n)
            && strcasecmp(shortMonth_[m-1][1],n); m++){}
      /* this must be an error and unknown must be returned */
      if(m==13) { m=1; }
    }
    dte.PutDate(Y,m,d,h,mi,s);
/*
    Std(BText("\nBDateFormat::TextToDate\t")+date+"\t"+format_+"\t"+
	      "("+Y+","+m+","+d+","+h+","+mi+","+s+") = " + dte +
	      "("+dte.HasValue()+")");
*/
  }
  return(dte);
}


//--------------------------------------------------------------------
BDate TextToDate(const BText& txt)

/*! Takes a formated text and returns the corresponding date for
 *  defect format.
 * \param txt sdafd
 * \sa BDateFormat::TextToDate
 */
//--------------------------------------------------------------------
{ return(BDateFormat::Defect().TextToDate(txt)); }



//--------------------------------------------------------------------
BText BDate::Name() const

/*! Returns the name of a date. It can be the text representation of
 *  a normal date, TheBegin (as the beggining of times), TheEnd (as 
 *  the end of times) or Unknown.
 */
//--------------------------------------------------------------------
{
  BText txt;
  if(HasValue())
  {
//  if(second_				 >0) { txt.Copy(second_,"s%lg"); }
//  if(second_+minute_			 >0) { txt = BText("i")+minute_+txt; }
//  if(second_+minute_+hour_		 >0) { txt = BText("h")+hour_+txt; }
//  if(second_+minute_+hour_+day_	 >1) { txt = BText("d")+day_+txt; }
//  if(second_+minute_+hour_+day_+month_ >2) { txt = BText("m")+month_+txt; }
//  txt = BText("y")+year_+txt;

    if(second_+minute_+hour_		 >0)
    {
      char theTime[64];
      BInt n = sprintf(theTime,"h%02di%02ds%06.3lf",hour_,minute_,second_);
      txt.Copy(theTime,n);
    }
    char theDate[64];
    BInt n = sprintf(theDate,"y%04dm%02dd%02d",year_,month_,day_);
    BText ymd;
    ymd.Copy(theDate,n);
    txt = ymd + txt;
  }
  else if(IsTheBegin())
  {
    txt = "TheBegin";
  }
  else if(IsTheEnd())
  {
    txt = "TheEnd";
  }
  else
  {
    txt = "Unknown";
  }
  return(txt);
}


//--------------------------------------------------------------------
BBool BDate::HasValue() const

/*! Validates if the object contains an valid date. To be a valid
 *  date, number of year must be between 1 and 9999. unknown_,
 *  begin_ and end_ dates haven't value.
 * \return Returns true if is a valid date of false if not
 */
//--------------------------------------------------------------------
{
  int by_ = begin_.Year();
  int ey_ = end_  .Year();

  return
  (
  //(year_   >=    1) &&
  //(year_   <=	9999) &&
    (year_   >   by_) &&
    (year_   < 	 ey_) &&
    (month_  >=	   1) &&
    (month_  <=	  12) &&
    (day_    >=    1) &&
    (day_    <=	  31) &&
    (hour_   >=	   0) &&
    (hour_   <	  24) &&
    (minute_ >=	   0) &&
    (minute_ <	  60) &&
    (second_ >=  	 0) &&
    (second_ <	  60) &&
    (day_    <= DteDaysInMonth(month_,year_))
  );
}


//--------------------------------------------------------------------
BText BDate::TxtLongYear() const
/*! Returns a text with the year as a decimal number including the
 *  century.
 */
//--------------------------------------------------------------------
{
    BText txt;
    txt.Copy(year_, "%04ld");
    return(txt);
}


//--------------------------------------------------------------------
BText BDate::TxtShortYear() const

/*! Return a text with the year as a decimal number without the
 *  century (range 00 to 99)
 */
//--------------------------------------------------------------------
{
    BText txt;
    txt.Copy(year_%100, "%02ld");
    return(txt);
}


//--------------------------------------------------------------------
BText BDate::TxtMonth() const
/*! Returns a text with the month as a decimal number
 *  (range 01 to 12)
 */
//--------------------------------------------------------------------
{
    BText txt;
    txt.Copy(month_, "%02ld");
    return(txt);
}


//--------------------------------------------------------------------
BText BDate::TxtShortMonth() const

/*! Returns a text with the abbreviated month name
 */
//--------------------------------------------------------------------
{
    return(BDateFormat::ShortMonth(month_));
}



//--------------------------------------------------------------------
BText BDate::TxtLongMonth() const

/*! Returns a text with the full month name
 */
//--------------------------------------------------------------------
{
    return(BDateFormat::LongMonth(month_));
}



//--------------------------------------------------------------------
BText BDate::TxtDay() const

/*! Returns a text with the day of the month as a decimal number
 *  (range 01 to 31)
 */
//--------------------------------------------------------------------
{
    BText txt;
    txt.Copy(day_, "%02ld");
    return(txt);
}


//--------------------------------------------------------------------
BText BDate::TxtShortWeekDay() const

/*! Returns a text with the abbreviated weekday name
 */
//--------------------------------------------------------------------
{
    return(BDateFormat::ShortWeekDay(WeekDay()));
}


//--------------------------------------------------------------------
BText BDate::TxtLongWeekDay() const

/*! Returns a text with the full weekday name
 */
//--------------------------------------------------------------------
{
    return(BDateFormat::LongWeekDay(WeekDay()));
}


//--------------------------------------------------------------------
BInt BDate::DaysRestInYear() const

/*! Returns the number of days that rest until the year's end.
 * <br><pre>
 * ALGORITHM:
 *   Initialite rest with the number of days pass until the actual day.
 *   For all following month:
 *     Acumulate in rest the number of day of the the month.</pre>
 */
//--------------------------------------------------------------------
{
    BInt rest, mesTraverse;
    
    rest = DteDaysInMonth(month_, year_) - day_;
    for (mesTraverse = month_+1; mesTraverse <= DECEMBER; mesTraverse++)
    { rest += DteDaysInMonth((BMonth) mesTraverse, year_); }
    
    return (rest);
}


//--------------------------------------------------------------------
BInt BDate::DaysPassInYear() const

/*! Returns the number of days that have been passed in this year.
 */
//--------------------------------------------------------------------
{
    return (DteDaysInYear(year_)-DaysRestInYear()+1);
}


//--------------------------------------------------------------------
BReal BDate::Index() const
//--------------------------------------------------------------------
{ 
  int d = DayIndex();
  if(d== -300000) { return(d); }
  else if(d<=-115782) { return(-115782); }
  else if(d>= 219147) { return( 219147); }
  return(d+Fraction()); 
}

//--------------------------------------------------------------------
BInt BDate::DayIndex() const

/*! Returns the index of a date as the amount of days pass from the
 *  predefined day number zero.
 */
//--------------------------------------------------------------------
{
  BInt days;
  BInt rest, i;
//if(!HasValue()) { return(year_*366); }
  if(!day_) { 
//  Std(BText("\nTRACE DayIndex for unknown date\n"));
    return(-300000); 
  }
  if(*this<=begin_) 
  { 
  //Std(BText("\nTRACE DayIndex for begin date\n"));
    return(-115782); 
  }
  if(*this>=end_  ) 
  { 
//  Std(BText("\nTRACE DayIndex for end date\n"));
    return( 219147); 
  }

  days = (year_ / 400) * DAYSCICLE400;
  rest = year_ % 400;
  days += (rest / 100) * DAYSCICLE100;
  rest = rest % 100;
  days += (rest / 4) * DAYSCICLE4;
  rest = rest % 4;
  if(rest==0)
  {
    for(i=(BInt)month_; i<=12; i++)
    { days-=DteDaysInMonth((BMonth)i,year_); }
  }
  else
  {
    for(i = 1; i<rest; i++)
    { days+=DteDaysInYear(year_-i); }
    for(i = 1; i<(BInt)month_; i++)
    { days+=DteDaysInMonth((BMonth)i,year_); }
  }
  days += day_;
  days -= DayNumberZero;
//TRZ(BDate(*this));
//TRZ(BDat(days));
  return(days);
}


//--------------------------------------------------------------------
void BDate::PutDayIndex(BInt index)

/*! Sets the date corresponding to an index (amount of days pass from 
 *  the predefined day number zero)
 * \param index Integer value that indicates the number of days pass from the
 *        predefined day number zero.
 */
//--------------------------------------------------------------------
{
	double idx = index;
  if(index == -300000) { *this = unknown_; return; }
  if(index <= -115782) { *this = begin_; return; }
  if(index >=  219147) { *this = end_; return; }
  BInt c, y, y4, d=index+DayNumberZero;
  BMonth m = JANUARY;
  BInt daysInMonth;
  if(d<1) { d = 1; }
  c = d / DAYSCICLE400;
  y = c*400;
  d -= c*DAYSCICLE400;
  if (d==0)
  {
    m=12;
    d=31;
  }
  else if (d==DAYSCICLE400-1)
  {
    m=12;
    d=30;
    y+=400;
  }
  else
  {
    if (d)
    {
      c = d / DAYSCICLE100;
      y += c * 100;
      d -= c*DAYSCICLE100;
      if (d)
      {
	c = d / DAYSCICLE4;
	y += c * 4;
	d -= c*DAYSCICLE4;
	if (d)
	{
	  y4 = d / DAYSCICLE1;
	  y += y4;
	  d  = d % DAYSCICLE1;
	  if((y4==4)&&(d==0))
	  {
	    d=365;
	    y--;
	  }
	}
      }
    }
    if(d)
    {
      y++;
      while((m < DECEMBER) && ((daysInMonth = DteDaysInMonth(m, y))<d))
      {
	d-= daysInMonth;
	m++;
      }
    }
    else
    {
      m=12;
      d=31;
    }
  }
  year_	 = y;
  month_ = m;
  day_	 = d;
}


//--------------------------------------------------------------------
void BDate::PutIndex(BReal index)

/*! Sets the date corresponding to an index (amount of days pass from 
 *  the predefined day number zero)
 * \param index Real value that indicates the number of days (and fraction of
 *        day) pass from the predefined day number zero.
 */
//--------------------------------------------------------------------
{
	double idx = index;
  if(index == -300000) { *this = unknown_; return; }
  if(index <= -115782) { *this = begin_; return; }
  if(index >=  219147) { *this = end_; return; }
	double frac = modf(floor(index*dsByDay_)/dsByDay_, &idx);

	PutDayIndex(int(idx));
	PutFraction(frac);
}


//--------------------------------------------------------------------
BDate IndexToDte(BReal index)

/*! Returns the date object corresponding to a given day index.
 * \param index Real value that indicates the number of days (and fraction
 *        of day) pass from the predefined day number zero.
 */
//--------------------------------------------------------------------
{
    BDate d;
    d.PutIndex(index);
    return(d);
}


//--------------------------------------------------------------------
BDate DayIndexToDte(BInt index)

/*! Returns the date object corresponding to a given day index
 *  \a index.
 * \param index Integer value that indicates the number of days pass from the
 *        predefined day number zero.
 */
//--------------------------------------------------------------------
{
    BDate d;
    d.PutDayIndex(index);
    return(d);
}


//--------------------------------------------------------------------
BDate HashToDte(BReal hash)

/*! Returns the date object corresponding to a given hash value
 *          \a hash.
 * \param hash Hash value of a date
 */
//--------------------------------------------------------------------
{
    BDate d;
    d.PutHash(hash);
    return(d);
}



//--------------------------------------------------------------------
BWeekDay BDate::WeekDay() const

/*! Returns the weekday of the actual date.
 * <br>
 * NOTE: The algorithm is based in an specific day on which its weekday is
 *	 known.
 * <br>
 * ALGORITHM:<ul>
 *   <li>Calculates diference as number of weekdays beetwen the weekday of the
 *   date and the weekday of the fixed date.
 *   <li>When diference is greather than 7 returns 7 - diference.
 *   <li>When diference is less than 1 returns 7 + diference.
 *   <li>Otherwise returns diference.</ul>
 */
//--------------------------------------------------------------------
{
            BInt     diference;
    static  BDate    fixedDate_ = BDate(1995,MARCH,27);
    static  BWeekDay fixedWeekDay_=MONDAY;
    
    diference = fixedWeekDay_ +
	((DayIndex() - fixedDate_.DayIndex()) % 7);
    
    if (diference > 7)      { return((BWeekDay) (7 - diference)); }
    else if (diference < 1) { return((BWeekDay) (7 + diference)); }
    else		    { return((BWeekDay) diference);	  }
}


//--------------------------------------------------------------------
BInt DteDaysInMonth(BMonth m, BInt y)

/*! Returns the number of days in the month \a m of the year \a y.
 * \param m Month
 * \param y Year
 */
//--------------------------------------------------------------------
{
  static int dm_[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
  if(m==FEBRUARY)
  {
    return(DteYearIsLeap(y) ? 29 : 28);
  }
  else
  {
    return(dm_[m]);
  }
}


//--------------------------------------------------------------------
BDate DteToday()

/*! Returns todays date.
 */
//--------------------------------------------------------------------
{
  time_t sec = time(NIL);
  struct tm* locTime = localtime(&sec);

  if (locTime)
  {
    BMonth month = (BMonth)(locTime->tm_mon+1); //localtime()returns0-11months
    BInt year = locTime->tm_year;   // localtime() returns 99 year
	 year = (year < 1900) ? 1900+year : year;
    return(BDate(year, month, locTime->tm_mday));
  }
  else
  {
    return(BDate(0, JANUARY, 0));
  }
}


//--------------------------------------------------------------------
BDate DteNow()

/*! Returns todays date and now time.
 */
//--------------------------------------------------------------------
{
  time_t sec = time(NIL);
  struct tm* locTime = localtime(&sec);
  if (locTime)
  {
    BMonth month = (BMonth)(locTime->tm_mon+1); //localtime()returns0-11months
    BInt year = locTime->tm_year;   // localtime() returns 99 year
	 year = (year < 1900) ? 1900+year : year;
    return(BDate(year, month, locTime->tm_mday,
		 locTime->tm_hour, locTime->tm_min, locTime->tm_sec));
  }
  else
  {
    return(BDate(0, JANUARY, 0));
  }
}

//--------------------------------------------------------------------
BDate DteNowGmt()

/*! Returns todays date and now time.
 */
//--------------------------------------------------------------------
{
  time_t sec = time(NIL);
  struct tm* gmTime = gmtime(&sec);
  if (gmTime)
  {
    BMonth month = (BMonth)(gmTime->tm_mon+1); //localtime()returns0-11months
    BInt year = gmTime->tm_year;   // localtime() returns 99 year
	 year = (year < 1900) ? 1900+year : year;
    return(BDate(year, month, gmTime->tm_mday,
		 gmTime->tm_hour, gmTime->tm_min, gmTime->tm_sec));
  }
  else
  {
    return(BDate(0, JANUARY, 0));
  }
}


//--------------------------------------------------------------------
BDate DteEasterSunday(BInt year)

/*! PURPOSE: Returns the date of the Easter's Sunday in the year.
// <br>
// ALGORITHM:<ul>
//   <li>Exceptions:<ul>
//    <li> are Easter sundays April 18 in years 1954, 2049 and 2106.
//    <li> are Easter sundays April 19 in years 1609, 1981, 2076 and 2133.</ul>
//   <li>Normal proceeding:<ul>
//   <li> calculates the full moon day in the following table, where [0..18]
//     is the rest of the division year / 19:</ul></ul>
// <br><pre>
//		   Julian      1583-1699       1700-1899       1900-2199
//		  Calendar
//	       0     5a		  12a		  13a		  14a
//	       1    25m		   1a		   2a		   3a
//	       2    13a		  21m		  22m		  23m
//	       3     2a		   9a		  10a		  11a
//	       4    22m		  29m		  30m		  31m
//	       5    10a		  17a		  18a		  19a
//	       6     30m	   6a		   7a		   8a
//	       7    18a		  26m		  27m		  28m
//	       8     7a		  14a		  15a		  16a
//	       9    27m		   3a		   4a		   5a
//	      10    15a		  23m		  24m		  25m
//	      11     4a		  11a		  12a		  13a
//	      12    24m		  31m		   1a		   2a
//	      13    12a		  19a		  21m		  22m
//	      14     1a		   8a		   9a		  10a
//	      15    21m		  28m		  29m		  30m
//	      16     9a		  16a		  17a		  18a
//	      17    29m		   5a		   6a		   7a
//	      18    17a		  25m		  26m		  27m
// </pre>
//     calculates and returns the sunday following the full moon day.
*/
//--------------------------------------------------------------------
{
  if(year<=BDate::Begin().Year()) { return(BDate::Begin()); }
  if(year>=BDate::End  ().Year()) { return(BDate::End  ()); }
  if ((year==1954) || (year==2049) || (year==2106))
  { return(BDate(year,APRIL,18));
  }
  else if ((year==1609) || (year==1981) || (year==2076) || (year==2133))
  { return(BDate(year,APRIL,19));
  }
  else
  {
    BInt rest = year % 19;
    BDate easter;
    easter.PutYear(year);

    if (year < 1583)		      // Julian calendar
    {
      switch (rest)
      {
	case 0:	 easter.PutMonth(APRIL); easter.PutDay(5);  break;
	case 1:	 easter.PutMonth(MARCH); easter.PutDay(25); break;
	case 2:	 easter.PutMonth(APRIL); easter.PutDay(13); break;
	case 3:	 easter.PutMonth(APRIL); easter.PutDay(2);  break;
	case 4:	 easter.PutMonth(MARCH); easter.PutDay(22); break;
	case 5:	 easter.PutMonth(APRIL); easter.PutDay(10); break;
	case 6:	 easter.PutMonth(MARCH); easter.PutDay(30); break;
	case 7:	 easter.PutMonth(APRIL); easter.PutDay(18); break;
	case 8:	 easter.PutMonth(APRIL); easter.PutDay(7);  break;
	case 9:	 easter.PutMonth(MARCH); easter.PutDay(27); break;
	case 10: easter.PutMonth(APRIL); easter.PutDay(15); break;
	case 11: easter.PutMonth(APRIL); easter.PutDay(4);  break;
	case 12: easter.PutMonth(MARCH); easter.PutDay(24); break;
	case 13: easter.PutMonth(APRIL); easter.PutDay(12); break;
	case 14: easter.PutMonth(APRIL); easter.PutDay(1);  break;
	case 15: easter.PutMonth(MARCH); easter.PutDay(21); break;
	case 16: easter.PutMonth(APRIL); easter.PutDay(9);  break;
	case 17: easter.PutMonth(MARCH); easter.PutDay(29); break;
	case 18: easter.PutMonth(APRIL); easter.PutDay(17); break;
      }
    }
    else if (year <= 1699)	       // 1583 - 1699 period
    {
      switch (rest)
      {
	case 0:	 easter.PutMonth(APRIL); easter.PutDay(12); break;
	case 1:	 easter.PutMonth(APRIL); easter.PutDay(1);  break;
	case 2:	 easter.PutMonth(MARCH); easter.PutDay(21); break;
	case 3:	 easter.PutMonth(APRIL); easter.PutDay(9);  break;
	case 4:	 easter.PutMonth(MARCH); easter.PutDay(29); break;
	case 5:	 easter.PutMonth(APRIL); easter.PutDay(17); break;
	case 6:	 easter.PutMonth(APRIL); easter.PutDay(6);  break;
	case 7:	 easter.PutMonth(MARCH); easter.PutDay(26); break;
	case 8:	 easter.PutMonth(APRIL); easter.PutDay(14); break;
	case 9:	 easter.PutMonth(APRIL); easter.PutDay(3);  break;
	case 10: easter.PutMonth(MARCH); easter.PutDay(23); break;
	case 11: easter.PutMonth(APRIL); easter.PutDay(11); break;
	case 12: easter.PutMonth(MARCH); easter.PutDay(31); break;
	case 13: easter.PutMonth(APRIL); easter.PutDay(19); break;
	case 14: easter.PutMonth(APRIL); easter.PutDay(8);  break;
	case 15: easter.PutMonth(MARCH); easter.PutDay(28); break;
	case 16: easter.PutMonth(APRIL); easter.PutDay(16); break;
	case 17: easter.PutMonth(APRIL); easter.PutDay(5);  break;
	case 18: easter.PutMonth(MARCH); easter.PutDay(25); break;
      }
    }
    else if (year <= 1899)	       // 1700 - 1899 period
    {
      switch (rest)
      {
	case 0:	 easter.PutMonth(APRIL); easter.PutDay(13); break;
	case 1:	 easter.PutMonth(APRIL); easter.PutDay(2);  break;
	case 2:	 easter.PutMonth(MARCH); easter.PutDay(22); break;
	case 3:	 easter.PutMonth(APRIL); easter.PutDay(10); break;
	case 4:	 easter.PutMonth(MARCH); easter.PutDay(30); break;
	case 5:	 easter.PutMonth(APRIL); easter.PutDay(18); break;
	case 6:	 easter.PutMonth(APRIL); easter.PutDay(7);  break;
	case 7:	 easter.PutMonth(MARCH); easter.PutDay(27); break;
	case 8:	 easter.PutMonth(APRIL); easter.PutDay(15); break;
	case 9:	 easter.PutMonth(APRIL); easter.PutDay(4);  break;
	case 10: easter.PutMonth(MARCH); easter.PutDay(24); break;
	case 11: easter.PutMonth(APRIL); easter.PutDay(12); break;
	case 12: easter.PutMonth(APRIL); easter.PutDay(1);  break;
	case 13: easter.PutMonth(MARCH); easter.PutDay(21); break;
	case 14: easter.PutMonth(APRIL); easter.PutDay(9);  break;
	case 15: easter.PutMonth(MARCH); easter.PutDay(29); break;
	case 16: easter.PutMonth(APRIL); easter.PutDay(17); break;
	case 17: easter.PutMonth(APRIL); easter.PutDay(6);  break;
	case 18: easter.PutMonth(MARCH); easter.PutDay(26); break;
      }
    }
    else if (year <=2199)	      // 1900 - 2199 period
    {
      switch (rest)
      {
	case 0:	 easter.PutMonth(APRIL); easter.PutDay(14); break;
	case 1:	 easter.PutMonth(APRIL); easter.PutDay(3);  break;
	case 2:	 easter.PutMonth(MARCH); easter.PutDay(23); break;
	case 3:	 easter.PutMonth(APRIL); easter.PutDay(11); break;
	case 4:	 easter.PutMonth(MARCH); easter.PutDay(31); break;
	case 5:	 easter.PutMonth(APRIL); easter.PutDay(19); break;
	case 6:	 easter.PutMonth(APRIL); easter.PutDay(8);  break;
	case 7:	 easter.PutMonth(MARCH); easter.PutDay(28); break;
	case 8:	 easter.PutMonth(APRIL); easter.PutDay(16); break;
	case 9:	 easter.PutMonth(APRIL); easter.PutDay(5);  break;
	case 10: easter.PutMonth(MARCH); easter.PutDay(25); break;
	case 11: easter.PutMonth(APRIL); easter.PutDay(13); break;
	case 12: easter.PutMonth(APRIL); easter.PutDay(2);  break;
	case 13: easter.PutMonth(MARCH); easter.PutDay(22); break;
	case 14: easter.PutMonth(APRIL); easter.PutDay(10); break;
	case 15: easter.PutMonth(MARCH); easter.PutDay(30); break;
	case 16: easter.PutMonth(APRIL); easter.PutDay(18); break;
	case 17: easter.PutMonth(APRIL); easter.PutDay(7);  break;
	case 18: easter.PutMonth(MARCH); easter.PutDay(27); break;
      }
    }
    else if (year > 2199)	      // default
    {
      switch (rest)
      {
	case 0:	 easter.PutMonth(APRIL); easter.PutDay(14); break;
	case 1:	 easter.PutMonth(APRIL); easter.PutDay(3);  break;
	case 2:	 easter.PutMonth(MARCH); easter.PutDay(23); break;
	case 3:	 easter.PutMonth(APRIL); easter.PutDay(11); break;
	case 4:	 easter.PutMonth(MARCH); easter.PutDay(31); break;
	case 5:	 easter.PutMonth(APRIL); easter.PutDay(19); break;
	case 6:	 easter.PutMonth(APRIL); easter.PutDay(8);  break;
	case 7:	 easter.PutMonth(MARCH); easter.PutDay(28); break;
	case 8:	 easter.PutMonth(APRIL); easter.PutDay(16); break;
	case 9:	 easter.PutMonth(APRIL); easter.PutDay(5);  break;
	case 10: easter.PutMonth(MARCH); easter.PutDay(25); break;
	case 11: easter.PutMonth(APRIL); easter.PutDay(13); break;
	case 12: easter.PutMonth(APRIL); easter.PutDay(2);  break;
	case 13: easter.PutMonth(MARCH); easter.PutDay(22); break;
	case 14: easter.PutMonth(APRIL); easter.PutDay(10); break;
	case 15: easter.PutMonth(MARCH); easter.PutDay(30); break;
	case 16: easter.PutMonth(APRIL); easter.PutDay(18); break;
	case 17: easter.PutMonth(APRIL); easter.PutDay(7);  break;
	case 18: easter.PutMonth(MARCH); easter.PutDay(27); break;
      }
    }
    easter.IncDay();
    while (!easter.IsTheEnd() && (easter.WeekDay() != SUNDAY)) 
    { easter.IncDay(); }
    return(easter);
  }
}

//--------------------------------------------------------------------
BReal BDate::Fraction()const

/*! Returns the fraction of day for this date.
 */
//--------------------------------------------------------------------
{
    BReal f = second_/(24*3600);
    f+=(BReal)minute_/(24*60);
    f+=(BReal)hour_/24;
    return(f);
}

//--------------------------------------------------------------------
void BDate::PutFraction (BReal fraction)

/*! Puts the hour, minute and second corresponding to the given
 *  day fraction \a fraction.
 * \param fraction Day fraction
 */
//--------------------------------------------------------------------
{
    if(fraction>0)
    {
	BInt fr = int(round(dsByDay_*fraction));
	hour_ = 24*fr/dsByDay_;
	fr -= hour_*dsByDay_/24;
	minute_ = 24*60*fr/dsByDay_;
	fr -= minute_*dsByDay_/(24*60);
	second_ = BReal(24*3600*fr)/dsByDay_;
    }
    else
    {
	hour_ = minute_ = 0;
	second_ = 0.0;
    }
}


//--------------------------------------------------------------------
BReal BDate::Hash()const

/*! Returns a hashing real value of this date.
 */
//--------------------------------------------------------------------
{
  if(!IsUnknown())
  {
    double dayHash = fabs(double(year_))*10000.0+double(month_)*100.0+double(day_);
    if(year_<0) { dayHash = -dayHash; }
    BReal f = Fraction();
    BReal h = dayHash + f;
    //  Std(BText("\nHASH(")+Name()+") = "+h+" = "+dayHash+" + "+f);
    return(h);
  }
  else
  {
    return(0);
  }
}


//--------------------------------------------------------------------
void BDate::PutHash(BReal hash)

/*! Puts the year, month, day and fraction corresponding with a hash
 *  value.<br>
 *  EXAMPLES:<br>
 *       19980112 -> (year:1998, month:JANUARY, day:12)
 * \param hash Hash value of date
 */
//--------------------------------------------------------------------
{
  double y = (double)(hash/10000.0);
  if(hash<0) { hash = -hash; }
  PutYear((int)y);
  hash -= 10000*(int)(fabs(y));
  PutMonth((BMonth)(hash/100.0));
  hash -= 100*Month();
  PutDay((BInt)hash);
  PutFraction(hash-(BInt)hash);
}


//--------------------------------------------------------------------
void BDate::IncSecond(BReal s)
       
/*! If s>0 increments \a s seconds. If s<0 decrements \a s seconds.
 * \param s Seconds to increment/decrement
 */
//--------------------------------------------------------------------
{
  if(s)
  {
    BReal ss = s+second_;
    BReal sec;
    BInt min;

    sec = fmod(ss,60);
    if ( sec < 0.0 )
      sec += 60.0;
    min = ((long int)(ss-sec))/60;
    IncMinute(min);
    second_ = sec;
    /*BInt  s  = (ss>0)?1:-1;
    IncMinute(s*Div(s*ss,60));
    second_ = Mod(ss,60);*/
//  PutIndex(Index()+s/(24.0*60.0));
  }
}


//--------------------------------------------------------------------
void BDate::IncMinute(BInt m)

/*! Pass the date to the \a m-th following minute.
 */
//--------------------------------------------------------------------
{
  if(m)
  {
    BInt mm = m+minute_;
    BInt hour, min;

    min = mm % 60;
    if ( min < 0 )
      min += 60;
    hour = (mm-min)/60;
    IncHour(hour);
    minute_ = min;
    /*BInt s  = (mm>0)?1:-1;
    IncHour(s*Div(s*mm,60));
    minute_ = Mod(mm,60);*/
//  PutIndex(Index()+BReal(m)/(24.0*60.0));
  }
}


//--------------------------------------------------------------------
void BDate::IncHour(BInt h)

/*! Pass the date to the \a h-th following hour.
 */
//--------------------------------------------------------------------
{
  if(h)
  {
    BInt hh = h+hour_;
    BInt day, hour;

    hour = hh % 24;
    if ( hour < 0 )
      hour += 24;
    day = (hh-hour)/24;
    IncDay(day);
    hour_ = hour;

    /*BInt s  = (hh>0)?1:-1;
    IncDay(s*Div(s*hh,24));
    hour_ = Mod(hh,24);*/
//  PutIndex(Index()+BReal(h)/24.0);
  }
}


//--------------------------------------------------------------------
void BDate::IncDay(BInt d)

/*!  Pass the date to the \a d-th following day.
 *
 * NOTE: Both employed methods are equivalents but the first executes around
 *	 of 50 times while the second runs once.
 */
//--------------------------------------------------------------------
{
  if(HasValue())
  {
/*
    if((d>=1)&&(d<=50))
    {
      for(BInt i=1; i<=d; i++)
      {
	if(day_<27) { day_++; }
	else if(day_<DteDaysInMonth(month_,year_)) { day_++; }
	else
	{
	  day_ = 1;
	  if(month_<DECEMBER) { month_++; }
	  else
	  {
	    month_ = JANUARY;
	    year_++;
	  }
	}
      }
    }
    else if((d<=-1)&&(d>=-50))
    {
      for(BInt i=1; i<=-d; i++)
      {
	if(day_>1) { day_--; }
	else
	{
	  if(month_>JANUARY) { month_--; }
	  else
	  {
	    month_ = DECEMBER;
	    year_--;
	  }
	  day_ = DteDaysInMonth(month_,year_);
	}
      }
    }
    else */ if(d!=0)
    {
      PutDayIndex(DayIndex() + d);
    }
  }
}


//--------------------------------------------------------------------
void BDate::IncWeek(BInt w)

/*! Pass the date to the \a w-th following week.
 */
//--------------------------------------------------------------------
{
    BInt wd = WeekDay();
    if(wd!=MONDAY)
    {
	if     (w>0) { IncDay(8-wd); w--; }
	else if(w<0) { IncDay(1-wd); w++; }
    }
    IncDay(w*7);
}


//--------------------------------------------------------------------
void BDate::IncMonth(BInt m)

/*! Pass the date to the \a m-th next month.
 */
//--------------------------------------------------------------------
{
  BInt y= m/12;
  year_+=y;
  m=m%12;
  if(m>0)
  {
    for(;m>0;m--)
    {
      if (month_ == DECEMBER) { month_ = JANUARY; year_++; }
      else		      { month_ = month_+1; }
    }
  }
  else if(m<0)
  {
    for(;m<0;m++)
    {
      if (month_ == JANUARY) { month_ = DECEMBER; year_--; }
      else		     { month_ = month_-1; }
    }
  }
  day_ = 1;
}


//--------------------------------------------------------------------
void BDate::IncYear(BInt y)

/*! Pass the date to the \a y-th next year
 */
//--------------------------------------------------------------------
{
  if(month_!=JANUARY)
  {
	 if(y>0) { IncMonth(13-month_); y--; }
    else if(y<0) { IncMonth( 1-month_); y++; }
  }
  day_	 = 1;
  month_ = JANUARY;
  year_+=y;
}


//--------------------------------------------------------------------
void BDate::IncBase(BInt  n, BReal base)

/*! Increments (or decrements if n<0) the base \a base of the actual date.
 */
//--------------------------------------------------------------------
{
  if(HasValue())
  {
    BReal i  = Index();
//  Std(BText("\n")+Name()+".IncBase("+n+", "+base+")");
//  Std(BText(" index =")+i);
    BReal fr = (i-BInt(i));
    if(fr>0)
    {
	   if(n>0) { fr = floor(fr/base)*base; }
      else if(n<0) { fr = ceil (fr/base)*base; }
      i += n*base;
    }
    else
    {
      i += n*base;
    }
//  Std(BText(" -> ")+i);
    PutIndex(i);
//  Std(BText(" -> ")+Name()+"\n");
  }
}


//--------------------------------------------------------------------
BBool BDate::HasBase(BReal base) const

/*! Checks if the actual date has base \a base
 */
//--------------------------------------------------------------------
{
  if(HasValue())
  {
    BReal i = Index(); i-=BInt(i); i/=base;
    BReal f = fabs(i-round(i));
    BBool hasBase = f<dsScale_;
//  Std(BText("\n")+Name()+".HasBase("+base+", "+i+", "+f+", "+dsScale_+")="+
//	hasBase+"\n");
    return(hasBase);
  }
  else
  {
    return(BFALSE);
  }
}


//--------------------------------------------------------------------
BDate& BDate::operator= (const BDate& dte)

/*! Copies the contents of a date in another one.
 */
//--------------------------------------------------------------------
{
  year_ = dte.Year();
  month_ = dte.Month();
  day_ = dte.Day();
  hour_ = dte.Hour();
  minute_ = dte.Minute();
  second_ = dte.Second();

  return (*this);
}


//--------------------------------------------------------------------
BBool BDate::operator< (const BDate& dte) const

/*! Overloading of this relational operator.
 */
//--------------------------------------------------------------------
  { return (Hash()<dte.Hash()); }


//--------------------------------------------------------------------
BBool BDate::operator<= (const BDate& dte) const

/*! Overloading of this relational operator.
 */
//--------------------------------------------------------------------
{ return (Hash()<=dte.Hash()); }


//--------------------------------------------------------------------
BBool BDate::operator> (const BDate& dte) const

/*! Overloading of this relational operator.
 */
//--------------------------------------------------------------------
{ return (Hash()>dte.Hash()); }


//--------------------------------------------------------------------
BBool BDate::operator>= (const BDate& dte) const

/*! Overloading of this relational operator.
 */
//--------------------------------------------------------------------
{ return (Hash()>=dte.Hash()); }


//--------------------------------------------------------------------
BBool BDate::operator== (const BDate& dte) const

/*! Overloading of this relational operator.
 */
//--------------------------------------------------------------------
{ return (Hash()==dte.Hash()); }


//--------------------------------------------------------------------
BBool BDate::operator!= (const BDate& dte) const

/*! Overloading of this relational operator.
 */
//--------------------------------------------------------------------
{ return (Hash()!=dte.Hash()); }

//--------------------------------------------------------------------
BText& operator<< (BText& txt, const BDate& dte)
//--------------------------------------------------------------------
{ return(txt << dte.Name()); }

//--------------------------------------------------------------------
BText& operator+= (BText& txt, const BDate& dte)
//--------------------------------------------------------------------
{ return(txt += dte.Name()); }

//--------------------------------------------------------------------
BText operator+ (const BText& txt, const BDate& dte)
//--------------------------------------------------------------------
{ return(txt + dte.Name()); }

//--------------------------------------------------------------------
ostream& operator<< (ostream& os, const BDate& dte)
//--------------------------------------------------------------------
{ return(os << dte.Name().String()); }


//--------------------------------------------------------------------
BBool DteYearIsLeap(BInt A)

/*! Validates if a given year \a A is a leap year or not
 * \param A Year to be validated
 * \return Returns true if year is leap
 */
//--------------------------------------------------------------------
{
    BBool ok =  !(A%4);
    if(ok)
    {
	ok = A%100 || !(A%400);
    }
    return(ok);
}

//--------------------------------------------------------------------
BInt DteDaysInYear(BInt y)

/*! Returns the number of days of a given year \a y
 * \param y Year
 */
//--------------------------------------------------------------------
{ return(DteYearIsLeap(y) ? 366 : 365); }

//--------------------------------------------------------------------
BMonth DteNextMonth(BMonth m)

/*! Returns the month after a given one \a m
 * \param m Month
 */
//--------------------------------------------------------------------
{ return((m==DECEMBER) ? JANUARY : (BMonth) ((int) m+1)); }

//--------------------------------------------------------------------
BMonth DtePrevMonth(BMonth m)

/*! Returns the month before a given one \a m
 * \param m Month
 */
//--------------------------------------------------------------------
{ return((m==JANUARY) ? DECEMBER : (BMonth) ((int) m-1)); }
