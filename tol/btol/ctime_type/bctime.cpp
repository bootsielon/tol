/* bctime.cpp - TOL Language. BCDate implementation.
 
   Copyright (C) 2003-2005 - Bayes Decision, SL
   author: Daniel Rus <danirus@tol-project.org>
 
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
#  include <win_tolinc.h>
#endif

#include <tol/tol_bctime.h>
#include <time.h>

#if defined(__USE_TC__)

//- class static members ---------------------------------------------

// start of Gregorian Calendar
const BCTime BCTime::begin_(BEG_YEAR, BEG_MONTH,  BEG_DAY);

// acceptable precision with 32bits
const BCTime BCTime::end_(END_YEAR, END_MONTH,  END_DAY);

/*! an unknown_ value will be returned as a result of an attempt to create
 *  a BCTime with (year,month,day) values out of limits set by begin_ and 
 *  end_ variables. */
BCTime BCTime::unknown_(0,1,1,0,0,0);

const std::string BCTime::editedGranul_[6] =
{ "SECONDS", "MINUTES", "HOURS", "DAYS", "MONTHS", "YEARS" };

//--------------------------------------------------------------------
static struct tai aSecond,  aMinute,  anHour,   aDay;

static bool init() {
    struct tai taiOp1, taiOp2;
    struct caltime ct1, ct2;

    ct1.date.year = 2000; ct1.date.month = 1; ct1.date.day = 2;
    ct1.hour      = 0;    ct1.minute     = 0; ct1.second   = 0;
    ct1.offset    = 0;
    ct2.date.year = 2000; ct2.date.month = 1; ct2.date.day = 1;
    ct2.hour      = 0;    ct2.minute     = 0; ct2.second   = 0;
    ct2.offset    = 0;
    caltime_tai(&ct1, &taiOp1);
    caltime_tai(&ct2, &taiOp2);
    
    //- aDay -----------------------
    tai_sub(&aDay, &taiOp1, &taiOp2);
    
    //- anHour ---------------------
    ct1.date.day = 1; ct1.hour = 1; 
    caltime_tai(&ct1, &taiOp1);
    tai_sub(&anHour, &taiOp1, &taiOp2);
    
    //- aMinute --------------------
    ct1.hour = 0; ct1.minute = 1; 
    caltime_tai(&ct1, &taiOp1);
    tai_sub(&aMinute, &taiOp1, &taiOp2);
    
    //- aSecond --------------------
    ct1.minute = 0; ct1.second = 1; 
    caltime_tai(&ct1, &taiOp1);
    tai_sub(&aSecond, &taiOp1, &taiOp2);
    
    return true;
};

const bool initiliazed = init();

//--------------------------------------------------------------------
/*! Default constructor\n
 *    Creates a Time Instant based on BCDate::Now()
 */
BCTime::BCTime() 
    : tai_(BCTime::Now().tai_), granul_(BCTime::Now().granul_)
{}

//--------------------------------------------------------------------
/*! Instantiate a BCTime object using variable number of parameters.
 * <li>Call it as: BCTime(yearL); to instantiate a BCTime that represents a 
 *     complete year (timebase YEARS)
 * <li>Call it as: BCTime(yearL, month); to instantiate a BCTime that 
 *     represents a complete month of given year (timebase MONTHS)
 * <li>Call it as: BCTime(yearL, month, day); to instantiate a BCTime that 
 *     represents a complete day of given month and year (timebase 
 *     DAYS)
 * <li>Call it as: BCTime(yearL, month, day, hour); to instantiate a BCTime
 *     that represents a complete hour of a given day, month and year
 *     (timebase HOURS)
 * <li>Call it as: BCTime(yearL, month, day, hour, minute); to instantiate 
 *     a BCTime that represents a complete minute of a given hour, day, 
 *     month and year (timebase MINUTES)
 * <li>Call it as: BCTime(yearL, month, day, hour, minute, second); to 
 *     instantiate a BCTime that represents a complete minute of a given 
 *     hour, day, month and year (timebase SECONDS)
 */
BCTime::BCTime(long int y, int m, int d, int H, int M, int S) 
    throw(BException)
{
    bool error = false;
    std::stringstream EN_ss, ES_ss;
    struct caldate cd;
    struct caltime ct;

    cd.year = y;

    // checking MONTH
    if(m==0) {                                     // timebase = YEARS
	cd.month=1; cd.day=1; 
	ct.hour=0; ct.minute=0; ct.second=0; 
	granul_ = YEARS;
    } else if((m>0) && (m<13)) {                   // VALID month
	cd.month = m;
	// checking DAY
	if(d==0) {                                 // timebase = MONTHS
	    cd.day=1; 
	    ct.hour=0; ct.minute=0; ct.second=0; 
	    granul_ = MONTHS;
	} else if(BCTime::isValidYMD(y,m,d)) {     // VALID day
	    cd.day = d;
	    // checking HOUR
	    if(H==-1) {                            // timebase = DAYS
		ct.hour = 0; ct.minute = 0; ct.second = 0; 
		granul_ = DAYS;
	    } else if((H>=0) && (H<=23)) {         // VALID hour
		ct.hour = H;
		// checking MINUTE
		if(M==-1) {                        // timebase = HOURS
		    ct.minute = 0; ct.second = 0; 
		    granul_ = HOURS;
		} else if((M>=0) && (M<=59)) {     // VALID minute
		    ct.minute = M;
		    // checking SECOND
		    if(S==-1) {                    // timebase = MINUTES
			ct.second = 0; 
			granul_ = MINUTES;
		    } else if((S>=0) && (S<=59)) { // VALID second
			ct.second = S; 
			granul_ = SECONDS;
		    } else {
			error = true;
			EN_ss << "S=" << S << " -> Invalid second\n";
			ES_ss << "S=" << S << " -> Segundo inaceptable\n";
		    }
		} else {
		    error = true;
		    EN_ss << "M=" << M << " -> Invalid minute\n";
		    ES_ss << "M=" << M << " -> Minuto inaceptable\n";
		}
	    } else {
		error = true;
		EN_ss << "H=" << H << " -> Invalid hour\n"; 
		ES_ss << "H=" << H << " -> Hora inaceptable\n";
	}
	} else {
	    error = true;
	    EN_ss << "d=" << d << " -> Invalid day in year " << y
		  << " and month " << m << endl;
	    ES_ss << "d=" << d << " -> Día inaceptable en el año " << y
		  << " y mes " << m << endl;
	}
    } else {
	error = true;
	EN_ss << "m=" << m << " -> Invalid month\n";
	ES_ss << "m=" << m << " -> Mes inaceptable\n";
    }

    if(error) throw BException(EN_ss.str(), ES_ss.str());

    ct.date = cd;
    ct.offset = 0;
    caltime_tai(&ct, &tai_);
    
    if(isUnknown()) 
    {
	tai_    = BCTime::unknown_.tai_;
	granul_ = BCTime::unknown_.granul_;
    }
}

//--------------------------------------------------------------------
/*! Assignment Operator
 */
BCTime& BCTime::operator=(const BCTime& rhside)
{
    tai_    = rhside.tai_;
    granul_ = rhside.granul_;

    return *this;
}

//--------------------------------------------------------------------
/*! Appends (*this) BCTime to a binary file.
 * \param fil Binary file stream to write in
 */
int BCTime::BinWrite(FILE* fil) const
{
    char buf[TAI_PACK];
    char bux[TAI_PACK+1];
    memset(buf, '\0', TAI_PACK);
    memset(bux, '\0', TAI_PACK+1);

    struct tai t = tai_;
    tai_pack(buf, &t);
    memcpy(bux, buf, TAI_PACK);
    fwrite(&bux, 1, (sizeof(char)*(TAI_PACK+1)), fil);

    int granul = static_cast<int>(granul_);
    fwrite(&granul, 1, sizeof(int), fil);

    return (sizeof(uint64) + sizeof(int));
}


//--------------------------------------------------------------------
/*! Reads (*this) BCtime from a binary file.
 * \param fil Binary file stream to read from
 */
int BCTime::BinRead(FILE* fil)
{
    char buf[TAI_PACK];
    char bux[TAI_PACK+1];
    memset(buf, '\0', TAI_PACK);
    memset(bux, '\0', TAI_PACK+1);
    fread(&bux, 1, (sizeof(char)*(TAI_PACK+1)), fil);
    memcpy(buf, bux, TAI_PACK);
    tai_unpack(buf, &tai_);

    int granul;
    fread(&granul, 1, sizeof(int), fil);
    granul_ = static_cast<granularity>(granul);

    return(sizeof(uint64) + sizeof(int));
}


//--------------------------------------------------------------------
BCTime BCTime::getStartPointIn(granularity granul) const
{
    if     (granul> granul_) return BCTime::Unknown();
    else if(granul==granul_) return *this;

    BCTime start(*this);
    start.setGranul(granul);

    return start;
}

//--------------------------------------------------------------------
BCTime BCTime::getEndPointIn(granularity granul) const
{
    if     (granul> granul_) return BCTime::Unknown();
    else if(granul==granul_) return *this;

    BCTime end(*this);
    end.successor(1);
    end.setGranul(granul);
    end.successor(-1);

    return end;
}

//--------------------------------------------------------------------
/*! Returns the weekday value of this BCTime.\n
 *  Values are between 1 for Monday and 7 for Sunday.
 */
int BCTime::dayOfWeek() const
{
    int wd, yd;
    struct caltime ct;
    struct tai tai = tai_;
    caltime_utc(&ct, &tai, &wd, &yd);
    return ((wd==0)? 7 : wd);	
}

//--------------------------------------------------------------------
int BCTime::dayOfYear() const
{
    int wd, yd;
    struct caltime ct;
    struct tai tai = tai_;
    caltime_utc(&ct, &tai, &wd, &yd);
    return yd;
}

//--------------------------------------------------------------------
/*! Distance to the given tmi.\n
 * Both BCTimes, (this) and the given \a tmi, must have the same granularity.
 */
int BCTime::distanceTo(const BCTime& tmi) const
{
    if(granul_==YEARS) return (tmi.year()-year());
    if(granul_==MONTHS) 
	return (((tmi.year() - year()) * 12) + (tmi.month()  - month()));

    struct tai dif, op1, op2;
    int sign = (tai_.x > tmi.getTai().x) ? -1 : 1;
    if(sign<0) { op1 = tai_; op2 = tmi.getTai(); }
    else       { op2 = tai_; op1 = tmi.getTai(); }

    tai_sub(&dif, &op1, &op2);
    if(granul_==DAYS)    return sign * (dif.x / aDay.x);
    if(granul_==HOURS)   return sign * (dif.x / anHour.x);
    if(granul_==MINUTES) return sign * (dif.x / aMinute.x);
    if(granul_==SECONDS) return sign * (dif.x / aSecond.x);
}

//--------------------------------------------------------------------
int BCTime::whichDayIs(int day) const
{
    if(day<0) // a negative day value returns starting from month's end 
	switch(month()) {
	    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		return (32 + day);
	    case 4: case 6: case 9: case 11:
		return (31 + day);
	    case 2:
		if(BCTime::isLeapYear(year()))
		    return (30 + day);
		else return (29 + day);
	}
    else return day;
}

//--------------------------------------------------------------------
/*! Calculates the next valid year, with the same values for month, day,
 * hour, minute and second, whenever they were eminent.\n
 * If it was in day 29th February, this method will return a date 4 years 
 * later, in the next leap year with a February 29th available.
 * \returns the amount of year incremented.
 */
int BCTime::nextYear()
{
    struct caltime ct = caltime();
    int nyears = 0;

    if((ct.date.month==2) && (ct.date.day==29)) 
	while(!isLeapYear(ct.date.year + (nyears+=4)));
    else nyears = 1;
    
    ct.date.year += nyears;
    ct.offset = 0;
    caltime_tai(&ct, &tai_);

    checkBoundaries();
    return nyears;
}

//--------------------------------------------------------------------
/*! Calculates the previous valid year, with the same values for month, day,
 * hour, minute and second, whenever they were outstanding.\n
 * If it was in day 29th February, this method will return a date 4 years 
 * before, in the previous leap year with a February 29th available.
 * \returns the amount of year decremented.
 */
int BCTime::prevYear()
{
    struct caltime ct = caltime();
    int nyears = 0;

    if((ct.date.month==2) && (ct.date.day==29))
	while(!isLeapYear(ct.date.year - (nyears+=4)));
    else nyears = 1;

    ct.date.year -= nyears;
    ct.offset = 0;
    caltime_tai(&ct, &tai_);

    checkBoundaries();
    return nyears;
}

//--------------------------------------------------------------------
/*! Calculates the next valid month, with the same values for year, day,
 * hour, minute and second, whenever they were outstanding.\n
 * The month can be incremented in more than one unit whenever the day was
 * the last of the month.
 * \returns the amount of months incremented.
 */
int BCTime::nextMonth()
{
    struct caltime ct = caltime();
    int nmonths = 1;

    if(ct.date.month==12)
    {
	ct.date.month = 1;
	ct.date.year += 1; 
    } 
    else if((ct.date.day==31) &&
	    ((ct.date.month==1) || (ct.date.month==3) || (ct.date.month==5) || 
	     (ct.date.month==8) || (ct.date.month==10)))
    {
	nmonths = 2;
	ct.date.month += 2;
    } 
    else if((ct.date.month==1) && 
	    ((ct.date.day==30) || 
	     ((ct.date.day==29) && !BCTime::isLeapYear(ct.date.year))))
    {
	nmonths = 2;
	ct.date.month += 2;
    }
    else 
	ct.date.month += 1;
    
    ct.offset = 0;
    caltime_tai(&ct, &tai_);

    checkBoundaries();
    return nmonths;
}

//--------------------------------------------------------------------
/*! Calculates the previous valid month, with the same values for year, day,
 * hour, minute and second, whenever they were outstanding.\n
 * The month can be decremented in more than one unit whenever the day was
 * the last of the month.
 * \returns the amount of months decremented.
 */
int BCTime::prevMonth()
{
    struct caltime ct = caltime();
    int nmonths = 1;

    if(ct.date.month==1)
    {
	ct.date.month = 12;
	ct.date.year -= 1; 
    } 
    else if((ct.date.day==31) &&
	    ((ct.date.month==3) || (ct.date.month==5)  || 
	     (ct.date.month==7) || (ct.date.month==10) ||
	     (ct.date.month==12)))
    {
	nmonths = 2;
	ct.date.month -= 2;
    } 
    else if((ct.date.month==3) && 
	    ((ct.date.day==30) || 
	     ((ct.date.day==29) && !BCTime::isLeapYear(ct.date.year))))
    {
	nmonths = 2;
	ct.date.month -= 2;
    }
    else
	ct.date.month -= 1;

    ct.offset = 0;
    caltime_tai(&ct, &tai_);

    checkBoundaries();
    return nmonths;
}

//--------------------------------------------------------------------
void BCTime::incDay()    { tai_add(&tai_,&tai_,&aDay);    checkBoundaries(); }
void BCTime::decDay()    { tai_sub(&tai_,&tai_,&aDay);    checkBoundaries(); }
void BCTime::incHour()   { tai_add(&tai_,&tai_,&anHour);  checkBoundaries(); }
void BCTime::decHour()   { tai_sub(&tai_,&tai_,&anHour);  checkBoundaries(); }
void BCTime::incMinute() { tai_add(&tai_,&tai_,&aMinute); checkBoundaries(); }
void BCTime::decMinute() { tai_sub(&tai_,&tai_,&aMinute); checkBoundaries(); }
void BCTime::incSecond() { tai_add(&tai_,&tai_,&aSecond); checkBoundaries(); }
void BCTime::decSecond() { tai_sub(&tai_,&tai_,&aSecond); checkBoundaries(); }

//--------------------------------------------------------------------
/*! Increments or decrements this BCTime using the parameters. Calculations
 * start always from seconds to years. It is important to note that
 * special dates like 29th of February, the 31th of some months, and leap 
 * years always do more than one incrementation in their values.\n
 * For example:\n
 * 1- given a CTime tmi(2000,1,31), a call like: tmi.add(0,1) does tmi to be 
 * y2000m3d31\n
 * 2- given a CTIme tmi(2000,2,29), a call like: tmi.add(1) does tmi to be
 * y2004m2d29\n
 */
void BCTime::add(long int y, int m, int d, int H, int M, int S)
{
    if(granul_<=SECONDS) for(int i=0; i<abs(S); i++) 
	(S>0) ? incSecond() : decSecond();

    if(granul_<=MINUTES) for(int i=0; i<abs(M); i++) 
	(M>0) ? incMinute() : decMinute();

    if(granul_<=HOURS) for(int i=0; i<abs(H); i++) 
	(H>0) ? incHour() : decHour();

    if(granul_<=DAYS) for(int i=0; i<abs(d); i++) 
	(d>0) ? incDay() : decDay();

    if(granul_<=MONTHS)	for(int i=0; i<abs(m); i++) 
	(m>0) ? nextMonth() : prevMonth();

    for(int i=0; i<abs(y); i++) 
	(y>0) ? nextYear() : prevYear();
}

//--------------------------------------------------------------------
void BCTime::successor(int n)
{
    switch(granul_)
    {
	case SECONDS: (n>0) ? incSecond() : decSecond(); break;
	case MINUTES: (n>0) ? incMinute() : decMinute(); break;
	case HOURS:   (n>0) ? incHour()   : decHour();   break;
	case DAYS:    (n>0) ? incDay()    : decDay();    break;
	case MONTHS:  (n>0) ? nextMonth() : prevMonth(); break;
	case YEARS:   (n>0) ? nextYear()  : prevYear();  break;
    }
}

//--------------------------------------------------------------------
void BCTime::nSuccessor(int n, granularity granul)
{
    granularity g = (granul!=UNK) ? granul : granul_;

    if(g==SECONDS) for(int i=0; i<abs(n); i++) 
	(n>0) ? incSecond() : decSecond();
    else if(g==MINUTES) for(int i=0; i<abs(n); i++) 
	(n>0) ? incMinute() : decMinute();
    else if(g<=HOURS) for(int i=0; i<abs(n); i++) 
	(n>0) ? incHour() : decHour();
    else if(g<=DAYS) for(int i=0; i<abs(n); i++) 
	(n>0) ? incDay() : decDay();
    else if(g<=MONTHS)	for(int i=0; i<abs(n); i++) 
	(n>0) ? nextMonth() : prevMonth();
    else for(int i=0; i<abs(n); i++) 
	(n>0) ? nextYear() : prevYear();
}

//--------------------------------------------------------------------
void BCTime::setGranul(granularity granul)
{
    struct caltime ct = caltime();
    switch(granul)
    {
	case YEARS:   ct.date.month = 1; 
	case MONTHS:  ct.date.day   = 1;
	case DAYS:    ct.hour       = 0; 
	case HOURS:   ct.minute     = 0; 
	case MINUTES: ct.second     = 0;
	case SECONDS: ct.offset     = 0;
    }
    caltime_tai(&ct, &tai_);
    granul_ = granul;
    checkBoundaries();
}

bool BCTime::incGranul()
{
    bool done = false;
    switch(granul_)
    {
	case YEARS:   break;
	case MONTHS:  setGranul(YEARS);   done = true; break;
	case DAYS:    setGranul(MONTHS);  done = true; break;
	case HOURS:   setGranul(DAYS);    done = true; break;
	case MINUTES: setGranul(HOURS);   done = true; break;
	case SECONDS: setGranul(MINUTES); done = true; break;
    }
    return done;
}

//--------------------------------------------------------------------
bool BCTime::includes(const BCTime &tmi) const
{
    BCTime tn(*this);
    tn.successor(1);
    BCTime pn(tmi);
    pn.successor(1);

    if(!(tai_less(&(tmi.tai_), &(tai_)   )) && 
       !(tai_less(&(tn.tai_) , &(pn.tai_))) )
	return true;
    else return false;
}

//--------------------------------------------------------------------
bool BCTime::isNormal() const
{
    struct caltime ct = caltime();

    // more less than begin_
    if(ct.date.year < BEG_YEAR) return false;
    if((ct.date.year == BEG_YEAR) && 
       (ct.date.month < BEG_MONTH)) return false;

    // greater than begin_
    if(ct.date.year > END_YEAR) return false;

    // the begin_
    if((ct.date.year  == BEG_YEAR) && 
       (ct.date.month == BEG_MONTH) && 
       (ct.date.day   == BEG_DAY)) return false;

    // the end_
    if((ct.date.year  == END_YEAR) && 
       (ct.date.month == END_MONTH) && 
       (ct.date.day   == END_DAY)) return false;

    // unknown_
    if((ct.date.year  == UNK_YEAR) && 
       (ct.date.month == UNK_MONTH) && 
       (ct.date.day   == UNK_DAY)) return false;

    return true;
}

//--------------------------------------------------------------------
bool BCTime::isUnknown() const
{
    struct caltime ct = caltime();
    
    // lower limit (1582,3,1 is begin_, before that is unknown_)
    if((ct.date.year < 1582) ||
       ((ct.date.year == 1582) && (ct.date.month < 3)))
	return true;
    // upper limit (4999999,12,31 is end_, after that is unknown_)
    if(ct.date.year > 4999999) 
	return true;

    return false;
}

//--------------------------------------------------------------------
bool BCTime::startsBefore(const BCTime &tmi) const
{
    return ((tai_less(&tai_, &(tmi.tai_))) == 1 ? true : false) ;
}

bool BCTime::startsBeforeEQ(const BCTime &tmi) const
{
    if(startsBefore(tmi)) return true;
    else return ((tai_less(&(tmi.tai_), &tai_)) != 1 ? true : false);
}


//--------------------------------------------------------------------
bool BCTime::endsAfter(const BCTime &tmi) const
{
    BCTime thisnext = (*this);
    thisnext.successor(1);
    BCTime tminext = tmi;
    tminext.successor(1);
    return ((tai_less(&(tminext.tai_), &(thisnext.tai_)))==1 ? true:false);
}

bool BCTime::endsAfterEQ(const BCTime &tmi) const
{
    if(endsAfter(tmi)) return true;
    else {
	BCTime thisnext = (*this);
	thisnext.successor(1);
	BCTime tminext = tmi;
	tminext.successor(1);
	return ((tai_less(&(thisnext.tai_), &(tminext.tai_)))!=1 ? true:false);
    }
}

//--------------------------------------------------------------------
/*! Returns a BCTime formatted into a BText object.
 * This method is used by Tol Kernel to obtain an internal std::string 
 * representation of BCTime.
 */
BText BCTime::Name() const 
{
    std::string edited;
    edited << static_cast<const BCTime&>(*this);
    return BText(edited.c_str());
}

//--------------------------------------------------------------------
std::string BCTime::format(char *format) const
{
    const struct tm cstm = getTM();
    int size = strlen(format) + 20;
    char str[1024];
    strftime(str, size, format, &cstm);
    return str;    
}

//--------------------------------------------------------------------
/*! Returns a std::string with short month name based
 * current locale configuration.
 */
std::string BCTime::shortMonth() const {
    const struct tm cstm = getTM();
    char str[10];
    strftime(str, 10, "%b", &cstm);
    return str;
}
//--------------------------------------------------------------------
/*! Returns a std::string with long month name based
 * on current locale configuration.
 */
std::string BCTime::longMonth() const {
    const struct tm cstm = getTM();
    char str[11];
    strftime(str, 11, "%B", &cstm);
    return str;
}

//--------------------------------------------------------------------
/*! Returns a std::string with short weekday name based 
 * on current locale configuration.
 */
std::string BCTime::shortWeekDay() const {
    const struct tm cstm = getTM();
    char str[11];
    strftime(str, 11, "%a", &cstm);
    return str;
}

//--------------------------------------------------------------------
/*! Returns a std::string with long weekday name based
 * on current locale configuration.
 */
std::string BCTime::longWeekDay() const {
    const struct tm cstm = getTM();
    char str[11];
    strftime(str, 11, "%A", &cstm);
    return str;
}

//--------------------------------------------------------------------
BCTime BCTime::fillNew(const char* strCTime, const char* format)
{
    struct tm cstm;
    cstm.tm_year =  0; cstm.tm_mon =  0; cstm.tm_mday =  0; 
    cstm.tm_hour = -1; cstm.tm_min = -1; cstm.tm_sec  = -1;

#ifdef WIN32
    printf("\nINTERNAL ERROR: NOT IMPLEMENTED METHOD IN WINDOWS BCTime::fillNew, due to strptime function not exists");
#else
    strptime(strCTime, format, &cstm);
#endif

    BCTime ret(cstm.tm_year, cstm.tm_mon+1, cstm.tm_mday, 
	       cstm.tm_hour, cstm.tm_min,   cstm.tm_sec);

    return ret;
}

//--- static method --------------------------------------------------
/*! Returns a new BCTime object. This method receives the next parameters: 
 * <li>A granularity (\a g), used for the new BCTime returned, 
 * <li>The type of default values to use when a BCTime parameter is 
 * omited (\a point), that could be LV (Low Values), doing month and day =1
 * and hour, minute and seconds =0. Or could be HV (High Values) doing
 *  month=12, day=31, hour=23, and minutes=seconds=59
 * <li>A number (\a num) telling how many BCTime parameters follow.</li>
 * \sa putHValues, putLValues
 */
BCTime BCTime::fillNew(granularity g, TimePoint point, int num, ...)
{
    va_list argptr;
    va_start(argptr, num);
    BCTime tmi; // created after next condition
    switch(g) {
	case YEARS:   tmi.granul_ = YEARS;   break;
	case MONTHS:  tmi.granul_ = MONTHS;  break;
	case DAYS:    tmi.granul_ = DAYS;    break;
	case HOURS:   tmi.granul_ = HOURS;   break;
	case MINUTES: tmi.granul_ = MINUTES; break;
	case SECONDS: tmi.granul_ = SECONDS;
    }

    tmi.fillValues(num, point, argptr);
    return tmi;
}

//--------------------------------------------------------------------
BCTime BCTime::fillNew(granularity g, TimePoint point, const BCTime& tmi)
{
    switch(tmi.getGranul()) 
    {
	case YEARS: 
	    return BCTime::fillNew(g, point, 1, tmi.year());
	case MONTHS: 
	    return BCTime::fillNew(g, point, 2, tmi.year(), tmi.month());
	case DAYS: 
	    return BCTime::fillNew(g, point, 3, tmi.year(), tmi.month(), 
				  tmi.day());
	case HOURS: 
	    return BCTime::fillNew(g, point, 4, tmi.year(), tmi.month(), 
				  tmi.day(),tmi.hour());
	case MINUTES: 
	    return BCTime::fillNew(g, point, 5, tmi.year(), tmi.month(), 
				  tmi.day(), tmi.hour(), tmi.minute());
	case SECONDS: 
	    return BCTime::fillNew(g, point, 6, tmi.year(), tmi.month(), 
				  tmi.day(), tmi.hour(), tmi.minute(),
				   tmi.second());
    }
}

//--------------------------------------------------------------------
/*! All BCTime public not-const methods must call this method before
 * return or exit from their own scopes. Operations done inside methods 
 * can have an out of boundaries BCTime as a result. In order to protect
 * that result, this method will replace its \a tai_ attribute with
 * the corresponding boundary.
 */
void BCTime::checkBoundaries()
{
    if((*this)<=BCTime::Begin()) (*this) = BCTime::Begin();
    if((*this)>=BCTime::End())   (*this) = BCTime::End();
}

//--------------------------------------------------------------------
/*! Accepts a variable list of date arguments in \a argptr parameter. 
 * \a num parameter specify the number of elements to extract from argptr.
 * \a point parameter specify what kind of values must be used to fill the 
 * new BCTime. Can be LV (that fills with month=day=1, 
 * hour=minute=second=0) or HV (that fills with month=12, day=32,
 * hour=23, minute=second=59).
 * Returns a new BCTime object with defaults values in attributes not givens 
 * as arguments.
 * At least year must be given. Default values appliyed for the other 
 * not given attributes will be 1 for month and day, and 0 for hour, minute,
 * and second.
 * \sa fillNew
 */
void BCTime::fillValues(int num, TimePoint point, va_list argptr)
{
    struct caltime ct;
    struct caldate cd;
    cd.month=1;
    cd.day=1;
    ct.hour=0;
    ct.minute=0;
    ct.second=0;

    if(point==HV)
	switch(granul_)	{
	    case MONTHS:
		cd.month=12; 
		break;
	    case DAYS:
		cd.month=12; cd.day=31; 
		break;
	    case HOURS:
		cd.month=12; cd.day=31; ct.hour=23; 
		break;
	    case MINUTES:
		cd.month=12; cd.day=31; 
		ct.hour=23; ct.minute=59;	
		break;
	    case SECONDS: 
		cd.month=12; cd.day=31; 
		ct.hour=23; ct.minute=59; ct.second=59;
	}

    for(int i=1; num>=i; i++)
    {
	switch(i) {
	    case 1: { cd.year   = va_arg(argptr, long int); break; }
	    case 2: { cd.month  = va_arg(argptr, int);      break; }
	    case 3: { cd.day    = va_arg(argptr, int);      break; }
	    case 4: { ct.hour   = va_arg(argptr, int);      break; }
	    case 5: { ct.minute = va_arg(argptr, int);      break; }
	    case 6: { ct.second = va_arg(argptr, int);      break; }
	}
    }

    if(point==HV)
	switch(granul_)	{	
	    case SECONDS: if(ct.second == -1) ct.second = 59;
	    case MINUTES: if(ct.minute == -1) ct.minute = 59;
	    case HOURS:   if(ct.hour   == -1) ct.hour = 23;
	    case DAYS:    //if(cd.day    == -1) cd.day = 31;
	    case MONTHS:  if(cd.month  == 0) cd.month = 12;
	}
    else if(point==LV)
	switch(granul_)	{
	    case SECONDS: if(ct.second==-1) ct.second = 0;
	    case MINUTES: if(ct.minute==-1) ct.minute = 0;
	    case HOURS:   if(ct.hour==-1)   ct.hour = 0;
	    case DAYS:    if(cd.day==0)     cd.day = 1;	
	    case MONTHS:  if(cd.month==0)   cd.month = 1;
	}

    if(cd.day<0) // a negative day value returns starting from month's end 
	switch(cd.month) {
	    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		cd.day = 32 + cd.day;
		break;
	    case 4: case 6: case 9: case 11:
		cd.day = 31 + cd.day;
		break;
	    case 2:
		if(BCTime::isLeapYear(cd.year))
		    cd.day = 30 + cd.day;
		else cd.day = 29 + cd.day;
	}
    
    /* a granul less than MONTHS has been required, and an specific day
     * was not given (num<3) -> Possible adjustment in day value.
     */
    if((point==HV) && (granul_<MONTHS) && ((num<3) || (cd.day==0)))
//(cd.day==31))
	switch(cd.month) {
	    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		cd.day = 31;
		break;
	    case 4: case 6: case 9: case 11:
		cd.day = 30;
		break;
	    case 2:
		if(BCTime::isLeapYear(cd.year))
		    cd.day = 29;
		else cd.day = 28;
	}
    
    if((granul_<MONTHS) && (num>2) && 
       (!BCTime::isValidYMD(cd.year, cd.month, cd.day)))
    {
	if(BCTime::isValidMD(cd.month, cd.day))
	    while(!BCTime::isValidYMD(cd.year, cd.month, cd.day))
		if(point==LV) 
		    cd.year++;
		else cd.year--;
	else
	    while(!BCTime::isValidMD(cd.month, cd.day))
		if(point==LV) 
		    cd.month++;
		else cd.month--;
    }

    ct.date = cd;
    ct.offset = 0;
    caltime_tai(&ct, &tai_);
    
//    if(isUnknown()) 
//    {
//	tai_    = BCTime::unknown_.tai_;
//	granul_ = BCTime::unknown_.granul_;
//    }

    va_end(argptr);
    checkBoundaries();
}

//--- static method --------------------------------------------------
/*! Returns true if year, month and day given as arguments represents
 * a valid date. Method checks if a given day is valid in a given month,  
 * and verify also if a given year is a leap year.
 */
bool BCTime::isValidYMD(long int y, int m, int d) 
{
    if((d>=0) && (d<=31) && 
       ((m==1) || (m==3) || (m==5) || (m==7) || (m==8) || (m==10) || (m==12)))
	return true;
    if((d>=0) && (d<=30) && ((m==4) || (m==6) || (m==9) || (m==11)))
	return true;
    if((d>=0) && (d<29) && (m==2))
	return true;
    if((d>=0) && (d<30) && (m==2) && BCTime::isLeapYear(y))
	return true;

    return false;
}

bool BCTime::isValidMD(int m, int d) 
{
    if((d>=0) && (d<=31) && 
       ((m==1) || (m==3) || (m==5) || (m==7) || (m==8) || (m==10) || (m==12)))
	return true;
    if((d>=0) && (d<=30) && ((m==4) || (m==6) || (m==9) || (m==11)))
	return true;
    if((d>=0) && (d<30) && (m==2))
	return true;

    return false;
}


//--- friend function ------------------------------------------------
/*! Sending a representation of BCTime object to an ostream. Very 
 * useful in operations like: cout << aBCTime << "\n";
 */
std::ostream& operator<<(std::ostream& os, const BCTime& tmi)
{
    if(tmi.getGranul() == YEARS) os << "y" << tmi.year();
    else if(tmi.getGranul() == MONTHS)
	os << "y" << tmi.year() << "m" << tmi.month();
    else if(tmi.getGranul() == DAYS)
	os << "y" << tmi.year() << "m" << tmi.month() << "d" << tmi.day();
    else if(tmi.getGranul() == HOURS)
	os << "y" << tmi.year() << "m" << tmi.month() << "d" << tmi.day()  
	   << "h" << tmi.hour();
    else if(tmi.getGranul() == MINUTES)
	os << "y" << tmi.year() << "m" << tmi.month() << "d" << tmi.day()  
	   << "h" << tmi.hour() << "i" << tmi.minute();
    else if(tmi.getGranul() == SECONDS)
	os << "y" << tmi.year() << "m" << tmi.month()  << "d" << tmi.day()
	   << "h" << tmi.hour() << "i" << tmi.minute() << "s" << tmi.second();

    return (os);
}

//--------------------------------------------------------------------
/*! Takes an instant of time and returns the corresponding formated 
 * std::string, using \a format_ attribute.
 * \param aBCTime object to transform in formated std::string
 * \return std::string object corresponding to BCDate \a aBCTime
 */
std::string& operator<<(std::string& out_str, const BCTime& tmi)
{
    int i=0;
    bool enough = true;
    std::stringstream ss;

    if     (BCTime::Begin().includes(tmi))   ss << "TheBegin";
    else if(BCTime::End().includes(tmi))     ss << "TheEnd";
    else if(BCTime::Unknown().includes(tmi)) ss << "Unknown"; 
    else enough = false;
    
    while(i<TIME_FMT.length() && !enough) 
    {
	if(TIME_FMT.at(i)=='%') 
	{
	    i++;
	    switch(TIME_FMT.at(i)) {
		case 'y' : 
		{
		    ss << abs(tmi.year());
		    if(tmi.getGranul()==YEARS) enough = true;
		    break;
		}
		case 'm' : 
		{
		    if(tmi.month()<10) ss << 0;
		    ss << tmi.month();
		    if(tmi.getGranul()==MONTHS) enough = true;
		    break;
		}
		case 'd' : 
		{
		    if(tmi.day()<10) ss << 0;
		    ss << tmi.day();
		    if(tmi.getGranul()==DAYS) enough = true;
		    break;
		}
		case 'H' : 
		{
		    if(tmi.hour()<10) ss << 0;
		    ss << tmi.hour();
		    if(tmi.getGranul()==HOURS) enough = true;
		    break;
		}
		case 'M' : 
		{
		    if(tmi.minute()<10) ss << 0;
		    ss << tmi.minute();
		    if(tmi.getGranul()==MINUTES) enough = true;
		    break;
		}
		case 'S' : 
		{
		    if(tmi.second()<10) ss << 0;
		    ss << tmi.second();
		    if(tmi.getGranul()==SECONDS) enough = true;
		}
	    }
	} else {
	    ss << TIME_FMT.at(i);
	}
	i++;
    }

    return (out_str = ss.str());
}

//--- friend function ------------------------------------------------
/*! Takes a formated text and returns the corresponding date with 
 *  the actual date format
 * \param date Formated text that represents a date
 * \return Date object corresponding to BText \a date
 */
std::string& operator>>(std::string& in_str, BCTime& tmi)
{
    if(in_str=="unknown") {
	tmi = BCTime::Unknown();
	return in_str;
    }

    int i=0, y=-1, m=0, d=0, H=-1, M=-1, S=-1;
    const char* fmt = TIME_FMT.c_str();
    const char* txt = in_str.c_str();
    bool error = false;

    int yearSign = 1;
    if(txt[0]=='-') { txt++; yearSign = -1; }

    while(fmt[0] && txt[0] && !error) 
    {
	if(fmt[0]=='%') 
	{
	    i++;
	    fmt++;
	    int nChar = 0;
	    switch(fmt[0])
	    {
		case 'y' : sscanf(txt, "%ld%n", &y, &nChar); break;
		case 'm' : sscanf(txt, "%ld%n", &m, &nChar); break;
		case 'd' : sscanf(txt, "%ld%n", &d, &nChar); break;
		case 'H' : sscanf(txt, "%ld%n", &H, &nChar); break;
		case 'M' : sscanf(txt, "%ld%n", &M, &nChar); break;
		case 'S' : sscanf(txt, "%ld%n", &S, &nChar); break;
	    }
	    if(nChar <= (TIME_FMT.length()-i)) txt += nChar;
	    fmt++;
	    i++;
	} 
	else {
	    while(fmt && txt && (fmt[0]==txt[0])) { fmt++; i++; txt++; }
	    if(fmt && (fmt[0]!='%')) error = true;
	}
    }

    if(error) tmi = BCTime::Unknown();
    else tmi = BCTime((yearSign * y), m, d, H, M, S);

    return in_str;
}

//--- friend function ------------------------------------------------
bool operator==(const BCTime& lh, const BCTime& rh)
{
    if((!tai_less(&(lh.tai_),&(rh.tai_))) && 
       (!tai_less(&(rh.tai_),&(lh.tai_))) && 
       lh.getGranul()==rh.getGranul())
	return true;
    else return false;
}

//--- friend function ------------------------------------------------
bool operator!=(const BCTime& lh, const BCTime& rh)
{
    return !(lh==rh);
}

//--- friend function ------------------------------------------------
bool operator>(const BCTime& lh, const BCTime& rh)
{
    if( // (lh.tai_ > rh.tai_):
	(tai_less(&(rh.tai_),&(lh.tai_))) ||    
	
	// (lh.tai_ == rh.tai_) && (lh.granul > rh.granul)
	((!tai_less(&(lh.tai_),&(rh.tai_))) &&
	 (!tai_less(&(rh.tai_),&(lh.tai_))) &&
	 lh.getGranul()>rh.getGranul())
	
	) return true;
    else  return false;
}

//--- friend function ------------------------------------------------
bool operator>=(const BCTime& lh, const BCTime& rh)
{
    if((lh>rh) || (lh==rh)) 
	return true;
    else return false;
}

//--- friend function ------------------------------------------------
bool operator<(const BCTime& lh, const BCTime& rh)
{
    if( // (lh.tai_ > rh.tai_):
	(tai_less(&(lh.tai_),&(rh.tai_))) ||    
	
	// (lh.tai_ == rh.tai_) && (lh.granul > rh.granul)
	((!tai_less(&(lh.tai_),&(rh.tai_))) &&
	 (!tai_less(&(rh.tai_),&(lh.tai_))) &&
	 lh.getGranul()<rh.getGranul())
	
	) return true;
    else  return false;
}

//--- friend function ------------------------------------------------
bool operator<=(const BCTime& lh, const BCTime& rh)
{
    if((lh<rh) || (lh==rh)) 
	return true;
    else return false;
}

//--- friend function ------------------------------------------------
BText operator+(const BText& txt, const BCTime& tmi)
{ 
    return(txt + tmi.Name()); 
}

//--- friend function ------------------------------------------------
BText& operator+=(BText& txt, const BCTime& tmi)
{ 
    return(txt += tmi.Name()); 
}

#endif /* __USE_TC__ */
