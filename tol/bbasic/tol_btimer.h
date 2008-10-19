/* tol_btime.h: Declaration of class BTimer.
                GNU/TOL Language.

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

#ifndef TOL_BTIMER_H
#define TOL_BTIMER_H 1

#include <tol/tol_bdate.h>
#ifdef __USE_TC__
#  include <tol/tol_bctime.h>
#endif

//--------------------------------------------------------------------
class TOL_API BTimer
//--------------------------------------------------------------------
{
 private:
    //! Processor time used by the program when object is created or reset.
    //! It's used as a timestamp
    clock_t created_;
    //! Name of process
    BText process_;

 public:
    // Creator
    //! Creates a timer of proccess \a process
    BTimer(const BText& process="");
    //! Timer destructor. Prints process time used since object was created
    ~BTimer();
    
    // Access and manipulation:
    /*! Returns processor time used (in clocks) since the moment this object
     * was created
     */
    clock_t Clk   () const;

    /*! Returns processor time used (in seconds) since the moment this object
     * was created
     */
    BInt  Sec   () const;

    /*! Returns processor time used (in mili-seconds) since the moment this object
     * was created
     */
    BInt  MSec   () const;

    /*! Returns processor time used (in text format) since the moment this
     * object was created
     */
    BText TimeTxt   () const;

    /*! Returns processor time used (in text format) since the moment this
     * object was created and the name of the process
     */
    BText Txt   () const;

    //! Puts actual clock time in created_
    void  Reset ();

    //! Transforms clocks to seconds
    static BReal ClockToSecond (clock_t  clk);
    //! Transforms clocks to miliseconds
    static BReal ClockToMiliSecond (clock_t  clk);

    //! Returns processor time used by program. It's returned in clocks
    static clock_t Clocks	     ();

    //! Returns processor time used by program. It's returned in seconds
    static BInt  Seconds	     ();

    //! Returns time \a t in localtime formated text
    static BText Text	     (time_t t = 0);

    //! Transforms data type time_t \a t to BDate type
    static BDate  TimeToDate(time_t t);
    static time_t DateToTime(const BDate& t);
#ifdef __USE_TC__
    static BCTime lastUpdate(time_t t);
#endif /* __USE_TC__ */

    //! Registers a process in the total time table
    static BInt  RegisterProcess(BChar* process,
				                         BInt&  callNumber,
				                         BReal& cpuTime);

    //! Prints the total time table
    static void  PrintProcess();
};


//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------

#define InitTotalTime(FUNCTION)					     \
  static BInt	_callNumber_	= 0;				     \
  static BReal	_cpuTime_	= 0;				     \
  static BChar* _process_	= FUNCTION;			     \
  static BReal	_processNumber_ =				     \
    BTimer::RegisterProcess(_process_,_callNumber_,_cpuTime_);	     \
  static BReal	_level_ = 0;					     \
  _level_++;							     \
  BTimer _time_;


#define SumPartialTime						     \
  _level_--;							     \
  if(_level_==0) { _cpuTime_+=_time_.Clk(); _callNumber_++; }

#endif // TOL_BTIMER_H
