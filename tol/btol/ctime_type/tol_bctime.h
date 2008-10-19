/* tol_bctime.h - TOL Language. BTime class declarations
 
   Copyright (C) 2003-2005 - Bayes Decision, SL (Spain [EU])
   Author: Daniel Rus <danirus@tol-project.org>
 
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

#ifndef BCTIME_H
#define BCTIME_H 1

#include <string>
#include <sstream>
#include <stdarg.h>

#include <tai/caldate.h>
#include <tai/caltime.h>
#include <tai/tai.h>

#include <tol/tol_btext.h>
#include <tol/tol_bexc.h>

#if defined(__USE_TC__)

enum granularity { SECONDS=1, MINUTES, HOURS, DAYS, MONTHS, YEARS, UNK };
typedef enum granularity granularity;

enum timePoint { LV=-1, HV=1 }; // LV=Low_Values, HV=High_Values
typedef enum timePoint TimePoint;

const long int BEG_YEAR   = 1582;
const int      BEG_MONTH  = 3;
const int      BEG_DAY    = 1;  // Monday (wd=1)
const int      BEG_HOUR   = 0;
const int      BEG_MINUTE = 0;
const int      BEG_SECOND = 0;
const long int END_YEAR   = 4999999;
const int      END_MONTH  = 12;
const int      END_DAY    = 31; // Friday (wd=5)
const int      END_HOUR   = 23;
const int      END_MINUTE = 59;
const int      END_SECOND = 59;
const long int UNK_YEAR   = 0;
const int      UNK_MONTH  = 1;
const int      UNK_DAY    = 1;

const std::string TIME_FMT = "y%ym%md%dh%Hi%Ms%S";

//--------------------------------------------------------------------
class TOL_API BCTime
{
 private:
    struct tai tai_;
    granularity granul_;

    static const BCTime begin_;        //!< The beginning of times
    static const BCTime end_;          //!< The end of times
    static BCTime unknown_;
    static const std::string editedGranul_[6];

 public:
    //--------------------------------------------------
    // Constructors and Destructor, operator=

    //! Default constructor, creates a BCTime with the current time value
    BCTime();

    BCTime(long int y, int m=0, int d=0,
	  int H=-1, int M=-1, int S=-1) throw(BException);

    BCTime(const BCTime& btmi) 
	: tai_(btmi.tai_), granul_(btmi.granul_) 
	{}
    BCTime(struct tai tai, granularity granul)
	: tai_(tai), granul_(granul)
	{}
    BCTime(const BCTime& btmi, granularity granul) 
	: tai_(btmi.tai_)
	{
	    setGranul(granul);
	}

    ~BCTime() {};

    BCTime& operator=(const BCTime& rhside);

    //--------------------------------------------------
    // public methods

    //! Appends (*this) BCTime to a binary file
    int BinWrite(FILE*) const;
    //! Reads (*this) BCTime from a binary file
    int BinRead (FILE*);

    BCTime getStartPointIn(granularity granul) const;
    BCTime getEndPointIn(granularity granul) const;

    struct tai  getTai()    const { return tai_; }
    granularity getGranul() const { return granul_; }

    long int julianDay() const { return caldate_mjd(&(caltime().date)); } 
    long int year()   const { return caltime().date.year;  }
    int      month()  const { return caltime().date.month; }
    int      day()    const { return caltime().date.day;   }
    int      hour()   const { return caltime().hour;       }
    int      minute() const { return caltime().minute;     }
    int      second() const { return caltime().second;     }

    int dayOfWeek() const;
    int dayOfYear() const;
    int distanceTo(const BCTime& tmi) const;
    int whichDayIs(int day) const; // used with negative values for day

    int nextYear();   int prevYear();
    int nextMonth();  int prevMonth();

    void incDay();    void decDay();
    void incHour();   void decHour();
    void incMinute(); void decMinute();
    void incSecond(); void decSecond();

    void add(long int y, int m, int d, int H, int M, int S);
    void successor(int n);
    void nSuccessor(int n, granularity granul=UNK);

    void setGranul(granularity granul);
    bool incGranul(); 

    bool includes(const BCTime& tmi) const;
    bool isNormal() const;
    bool isUnknown() const;
    bool startsBefore(const BCTime &tmi) const;
    bool endsAfter(const BCTime &tmi) const;
    bool startsBeforeEQ(const BCTime &tmi) const;
    bool endsAfterEQ(const BCTime &tmi) const;

    // Next methods are required by "Time" Tol Data Type implementation
    BText Name() const;
    BText Dump() const { return Name(); }
    BText Info() const { return Name(); }

    std::string format(char *format) const;

    std::string shortMonth() const;
    std::string longMonth() const;
    std::string shortWeekDay() const;
    std::string longWeekDay()  const;

    //--------------------------------------------------
    // public static methods

    static BCTime fillNew(const char* strCTime, const char* format);
    static BCTime fillNew(granularity g, TimePoint point, int num, ...);
    static BCTime fillNew(granularity g, TimePoint point, const BCTime& tmi);

    static bool isLeapYear(long int year) {
	return (((year % 4)==0) && 
		(((year % 100)!=0) || ((year % 400)==0))
	    ) ? true : false;
    }

    static int Compare(const BCTime* btmi1, const BCTime* btmi2) {
	if     (*btmi1 > *btmi2) return  1;
	else if(*btmi1 < *btmi2) return -1;
	return 0;
    }

    static BCTime Now() { 
	struct tai tai; 
	tai_now(&(tai)); 
	return BCTime(tai, SECONDS); 
    }

    static BCTime Easter(long int year) {
	struct caldate cd;
	cd.year = year;
	caldate_easter(&(cd));
	BCTime tmi(cd.year, cd.month, cd.day);
	return tmi;
    }

    static const BCTime& Begin()   { return begin_;   }
    static const BCTime& End()     { return end_;     }
    static       BCTime& Unknown() { return unknown_; }

    static std::string editedGranul(granularity granul)
	{ return BCTime::editedGranul_[granul-1]; }
    
 private:
    void checkBoundaries();
    void fillValues(int num, TimePoint point, va_list argptr);

    struct caltime caltime() const 
	{
	    int wd, yd;
	    struct caltime ct;
	    struct tai tai = tai_;
	    caltime_utc(&ct, &tai, &wd, &yd);
	    return ct;	
	}

    struct tm getTM() const
	{
	    int wd, yd;
	    struct caltime ct;
	    struct tai tai = tai_;
	    struct tm stm;
	    caltime_utc(&ct, &tai, &wd, &yd);   
	    stm.tm_sec  = ct.second;
	    stm.tm_min  = ct.minute;
	    stm.tm_hour = ct.hour;
	    stm.tm_mday = ct.date.day;
	    stm.tm_mon  = ct.date.month-1;
	    stm.tm_year = ct.date.year;
	    stm.tm_wday = wd;
	    stm.tm_yday = yd;
	    return stm;
	}

    //--------------------------------------------------
    // private static methods

    static bool isValidYMD(long int y, int m, int d);
    static bool isValidMD(int m, int d);

 public:
    //--------------------------------------------------
    // friend functions
    friend std::ostream& operator<<(std::ostream& os, const BCTime& tmi);

    friend std::string& operator<<(std::string &out_str, const BCTime& tmi);
    friend std::string& operator>>(std::string &in_str, BCTime& tmi);

    friend bool operator==(const BCTime& lh, const BCTime& rh);
    friend bool operator!=(const BCTime& lh, const BCTime& rh);
    friend bool operator> (const BCTime& lh, const BCTime& rh);
    friend bool operator>=(const BCTime& lh, const BCTime& rh);
    friend bool operator< (const BCTime& lh, const BCTime& rh);
    friend bool operator<=(const BCTime& lh, const BCTime& rh);

    friend BText  operator+ (const BText& txt, const BCTime& tmi);
    friend BText& operator+=(      BText& txt, const BCTime& tmi);
};

#endif /* __USE_TC__ */
#endif /* TOL_BCTIME_H */
