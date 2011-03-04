/* timer.h: Constructors and destructor for BTime class and all derived
            classes. GNU/TOL Language.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

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

//static const char* bayesVersionId =

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_btimer.h>
#include <tol/tol_bout.h>


//--------------------------------------------------------------------
BReal BTimer::ClockToMiliSecond (clock_t clk)

/*! Transform clocks to seconds.
 * \param clk Clocks to be transformed
 * \return Seconds corresponding to \a clk clocks
 */
//--------------------------------------------------------------------
{
#if (defined(CLOCKS_PER_SEC))
  BReal tm = double(1000.0*clk)/double(CLOCKS_PER_SEC);
#else 
  BReal tm = double(1000.0*clk)/double(CLK_TCK);
#endif
return(tm);
}

//--------------------------------------------------------------------
    BReal BTimer::ClockToSecond (clock_t clk)

/*! Transform clocks to seconds.
		 * \param clk Clocks to be transformed
		 * \return Seconds corresponding to \a clk clocks
 */
//--------------------------------------------------------------------
{
#if (defined(CLOCKS_PER_SEC))
  BReal tm = double(clk)/double(CLOCKS_PER_SEC);
#else
  BReal tm = double(clk)/double(CLK_TCK);
#endif
  return(tm);
}


//--------------------------------------------------------------------
BTimer::BTimer(const BText& process)

/*! BTimer creator. Creates a timer of process \a process
 * \param process Name of process
 */
//--------------------------------------------------------------------
    : process_(process)
{
    if(process_.HasName())
    { Std(BText("\n")+Text()+" : "+I2("Begins ","\nEmpieza ")+process_); }
    Reset();
}


//--------------------------------------------------------------------
BTimer::~BTimer()

/*! BTimer destructor. Prints processor time used since the moment
 *  this object was created (or reset)
 */
//--------------------------------------------------------------------
{
    if(process_.HasName())
    { Std(Txt()); }
}


//--------------------------------------------------------------------
void	BTimer::Reset()

/*! Puts actual clock time (processor time used) in created_.
 */
//--------------------------------------------------------------------
{
  created_ = clock();
}


//--------------------------------------------------------------------
clock_t BTimer::Clk() const
/*! Returns proccesor time used since the moment this object was 
 *  created. It's returned  in clocks.
 */
//--------------------------------------------------------------------
{
    clock_t clk = clock();
    clock_t dif = clk-created_;
    return(dif);
}


//--------------------------------------------------------------------
BInt BTimer::Sec() const

/*! Returns proccesor time used since the moment this object was 
 *  created. It's returned in seconds.
 */
//--------------------------------------------------------------------
{
    return int(round(ClockToSecond(Clk())));
}

//--------------------------------------------------------------------
BInt BTimer::MSec() const

/*! Returns proccesor time used since the moment this object was 
 *  created. It's returned in seconds.
 */
//--------------------------------------------------------------------
{
    return int(round(ClockToMiliSecond(Clk())));
}


//--------------------------------------------------------------------
BText BTimer::Txt() const

/*! Returns proccesor time used since the moment this object was 
 *  created. It's returned in text format.
*/
//--------------------------------------------------------------------
{
  return(I2("\nTime ","\nTiempo ")+process_+" "+TimeTxt());
}

//--------------------------------------------------------------------
BText BTimer::TimeTxt() const

/*! Returns proccesor time used since the moment this object was 
 *  created. It's returned in text format.
*/
//--------------------------------------------------------------------
{
    double tm = BTimer::ClockToSecond(Clk());
    int    total_sec = (int)tm;
    BText txt;
    if(total_sec<60)
    {
      BText tmp;
      tmp.Copy(tm,"%.3lf");
      txt = tmp+I2(" seconds"," segundos");
    }
    else
    {
      //BInt  min, hou;
      ldiv_t min_sec, hour_min;

      min_sec = ldiv(total_sec, 60);
      hour_min = ldiv(min_sec.quot, 60);
      /*
      min = total_sec/60;
      sec = int(floor(clk));
      clk = clk - sec;
      sec = BInt(sec) % 60;
      sec = int(sec+clk);
      hou = min / 60;
      min = min % 60;*/
      BText h; 
      //if(hou>0) { h = hou+ I2(" hours "," horas "); }
      if (hour_min.quot>0)
        h.Copy(hour_min.quot) += I2(" hours "," horas ");
      BText m; 
      //if(min>0) { m = min+ I2(" minutes "," minutos "); }
      if (hour_min.rem>0)
        m.Copy(hour_min.rem) += I2(" minutes "," minutos ");
      BText s; 
      //if(sec>0) { s.Copy(sec,"%5.3lf") += I2(" seconds "," segundos "); }
      if (min_sec.rem>0)
        s.Copy(min_sec.rem+(tm-total_sec),"%.3lf") += I2(" seconds "," segundos ");
      txt = h+m+s;
    }
    return(txt);
}


//--------------------------------------------------------------------
clock_t BTimer::Clocks()

/*! Returns processor time used by program. It's returned in clocks.
 */
//--------------------------------------------------------------------
{
    return(clock());
}

//--------------------------------------------------------------------
BInt BTimer::Seconds()

/*! Returns processor time used by program. It's returned in seconds
 */
//--------------------------------------------------------------------
{
    return(BInt(ClockToSecond(Clocks())));
}


//--------------------------------------------------------------------
BText BTimer::Text(time_t t)

/*! Returns time \a t in localtime formated text. Example:<br>
 *  <pre>Sun Sep 16 01:03:52 1973\\n\0</pre>
 */
//--------------------------------------------------------------------
{
    if(t<=0) { t = time(NIL); }
    BChar buf[100];
    strftime(buf,99,"%Y/%m/%d %H:%M:%S",localtime(&t));
    return(BText(buf));
}


//--------------------------------------------------------------------
BDate BTimer::TimeToDate(time_t t)

/*! Transforms data type time_t \a t to BDate type
 * \param t time_t Calendar time to be transformed. When interpreted as an
 *        absolute time value, it represents the number of seconds
 *        elapsed since 00:00:00 on January 1, 1970, Coordinated
 *        Universal Time (UTC).
 */
//--------------------------------------------------------------------
{
  BReal c = ClockToSecond(clock());
/*
struct tm {
   int tm_sec;
   int tm_min;
   int tm_hour;
   int tm_mday;
   int tm_mon;
   int tm_year;
   int tm_wday;
   int tm_yday;
   int tm_isdst;
*/
  struct tm* timeStruct = localtime(&t);
  if(!timeStruct) 
  { 
    return(BDate::Unknown());
  }
  else
  {
    BInt y = timeStruct->tm_year;
    if(y<1000)
    {
      if(y>50) { y=1900+y; }
      else     { y=2000+y; }
    }
    BDate dte
    (
      y,
      timeStruct->tm_mon+1,
      timeStruct->tm_mday,
      timeStruct->tm_hour,
      timeStruct->tm_min,
      timeStruct->tm_sec+(c-floor(c))
    );
    return(dte);
  }
};


//--------------------------------------------------------------------
  time_t BTimer::DateToTime(const BDate& date)

/*! Transforms data type time_t \a t to BDate type
 * \param t time_t Calendar time to be transformed. When interpreted as an
 *        absolute time value, it represents the number of seconds
 *        elapsed since 00:00:00 on January 1, 1970, Coordinated
 *        Universal Time (UTC).
 */
//--------------------------------------------------------------------
{
  struct tm timeStruct ;
  timeStruct.tm_sec   = (int)date.Second();
  timeStruct.tm_min   = date.Minute();
  timeStruct.tm_hour  = date.Hour  ();
  timeStruct.tm_mday  = date.Day   ();
  timeStruct.tm_mon   = date.Month ();
  timeStruct.tm_year  = date.Year  ()-1900;
  timeStruct.tm_isdst = 0;
  time_t t = mktime(&timeStruct);
  return(t);
};


#ifdef __USE_TC__
//--------------------------------------------------------------------
/* The same function as above but for Time Tol Type.
 * IMPORTANT: Remove the above function when Date Tol Type no longer exist.
 */
BCTime BTimer::lastUpdate(time_t t)
{
    BReal c = ClocksToSecond(clock());
    struct tm* timeStruct = localtime(&t);
    BCTime result(timeStruct->tm_year+1900, timeStruct->tm_mon+1, 
		  timeStruct->tm_mday,      timeStruct->tm_hour, 
		  timeStruct->tm_min,  
		  static_cast<int>(timeStruct->tm_sec+(c-floor(c))));
    
    return result;
};

#endif /* __USE_TC__ */

//--------------------------------------------------------------------
// Process's total time table
//--------------------------------------------------------------------
#define MAXPROCESSNUMBER 1024
static BTimer  totalProgramTime_;
static BInt    processNumber_ = -1;
static BInt*   callNumberRegister_ [MAXPROCESSNUMBER];
static BReal*  cpuTimeRegister_	   [MAXPROCESSNUMBER];
static BChar*  processRegister_	   [MAXPROCESSNUMBER];

//--------------------------------------------------------------------
BInt BTimer::RegisterProcess(BChar* process,
			     BInt&  callNumber,
			     BReal& cpuTime)
    
/*! Registers a process in the total time table
 * \param process Process to be registered
 * \param callNumber
 * \param cpuTime
 * \return Number of process registered
 */
//--------------------------------------------------------------------
{
    processNumber_++;
    if(processNumber_<MAXPROCESSNUMBER)
    {
	callNumberRegister_ [processNumber_] = &callNumber;
	cpuTimeRegister_	[processNumber_] = &cpuTime;
	processRegister_	[processNumber_] = process;
    }
    return(processNumber_);
}


//--------------------------------------------------------------------
void BTimer::PrintProcess()

/*! Prints the total time table
*/
//--------------------------------------------------------------------
{
  Std(Out() + "\n\nTotal Program Time = " + totalProgramTime_.Txt() +
"\n====================================================================================="
"\nNum   |Process                        |Called      |CPU        |Seconds  |Average    "
"\n=====================================================================================");
  for(BInt n=0; n<=processNumber_; n++)
  {
    BText  pro = processRegister_[n];
    double num = *(callNumberRegister_[n]);
    double cpu = *(cpuTimeRegister_[n]);
    double sec = BInt(BTimer::ClockToSecond(cpu));
    double avr = sec/num;
    BText    nPad;   nPad << n;	    nPad =   nPad. LeftPad( 4, ' ') + " |";
    BText  proPad; proPad << pro; proPad = proPad.RightPad(29, ' ') + " |";
    BText  numPad; numPad << num; numPad = numPad. LeftPad(10, ' ') + " |";
    BText  cpuPad; cpuPad << cpu; cpuPad = cpuPad. LeftPad( 9, ' ') + " |";
    BText  secPad; secPad << sec; secPad = secPad. LeftPad( 7, ' ') + " |";
    BText  avrPad; avrPad << avr; avrPad = avrPad. LeftPad( 9, ' ');
    Std(Out()+"\n"+nPad+proPad+numPad+cpuPad+secPad+avrPad);
  }
 Std(
"\n=====================================================================================");
}
