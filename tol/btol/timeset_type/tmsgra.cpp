/* tmsgra.cpp: Time Set Grammar's functions
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

#include <tol/tol_btmsgra.h>
#include <tol/tol_btmsgrav.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bsys.h>

template <>
BArray<BCacheInfo> BTmsStaticCached<BUserTimeSet,  BI_VOID>::cacheInfo_( 1);
template <>
BArray<BCacheInfo> BTmsStaticCached<BUserTimeSet,  BI_ALL>::cacheInfo_( 1);
template <>
BArray<BCacheInfo> BTmsStaticCached<BUserTimeSet,  BI_EASTER>::cacheInfo_( 1);
template <>
BArray<BCacheInfo> BTmsStaticCached<BTmsTemporary, BI_WEEKDAY>::cacheInfo_( 7);
template <>
BArray<BCacheInfo> BTmsStaticCached<BTmsTemporary, BI_MONTHDAY>::cacheInfo_(31);
template <>
BArray<BCacheInfo> BTmsStaticCached<BTmsTemporary, BI_MONTH>::cacheInfo_(12);
template <>
BArray<BCacheInfo> BTmsStaticCached<BTmsTemporary, BI_HOUR>::cacheInfo_(24);
template <>
BArray<BCacheInfo> BTmsStaticCached<BTmsTemporary, BI_MINUTE>::cacheInfo_(60);
template <>
BArray<BCacheInfo> BTmsStaticCached<BTmsTemporary, BI_SECOND>::cacheInfo_(60);

//--------------------------------------------------------------------
   static bool InitAllCache()
//--------------------------------------------------------------------
{
  static BTimeSet** base_ = NULL;
  if(base_) { return(false); }
  base_ = new BTimeSet*[196];
  int k, n = 0;
  base_[n++] = new BTmsAllDays;
  base_[n++] = new BTmsEaster;
  for(k=1; k<= 7; k++) { base_[n++] = new BTmsWeekDay (k); }
  for(k=1; k<=31; k++) { base_[n++] = new BTmsMonthDay(k); }
  for(k=1; k<=12; k++) { base_[n++] = new BTmsMonth   (k); }
  for(k=0; k< 24; k++) { base_[n++] = new BTmsHour    (k); }
  for(k=0; k< 60; k++) { base_[n++] = new BTmsMinute  (k); }
  for(k=0; k< 60; k++) { base_[n++] = new BTmsSecond  (k); }

//for(n=0; n<196; n++) { base_[n]->GetHashBetween(); }
  return(true);
}


#define EnsureLimitsSuccessor(dte)                                      \
  if(!dte.HasValue()) { return(dte); }           

#define EnsureLimitsPredecessor(dte)                                    \
  if(!dte.HasValue()) { return(dte); }           

//--------------------------------------------------------------------
template<>
BDat BGraObject<BTimeSet>::Compare(const BSyntaxObject* obj1,
                                   const BSyntaxObject* obj2)
//--------------------------------------------------------------------
{
    BText name1 = obj1->Name();
    BText name2 = obj2->Name();
    return((name1<name2)?-1:(name1>name2)?1:0);
}


//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
//BBool ForzeLinkerTmsGraContinuous();

BBool ForzeLinkerTmsGraVarious();

static BBool forzeLinkers_ = //ForzeLinkerTmsGraContinuous() &&
			     ForzeLinkerTmsGraVarious();



//--------------------------------------------------------------------
// Static variables
//--------------------------------------------------------------------
BTraceInit("tmsgra.cpp");
template<>
BGrammar* BGraObject<BTimeSet>::ownGrammar_ = NIL;

static bool initAllCache_ = InitAllCache();


//--------------------------------------------------------------------
BInt SelectAxisDates(BUserTimeSet* axisTms,
		     BUserTimeSet* serTms,
		     const BDate&  first,
		     BInt	      max,
		     BChar*	      select)
    
/*! Returns an array of BBool with BTRUE for each date of serTms
 *  that's the first no great date in serTms of a date in axisTms.
 *  This is an usefull functions for charting.
 */
//--------------------------------------------------------------------
{
  BInt num = 0;
  if(!serTms || !axisTms)
  {
    for(BInt n=0; n<max; n++) { select[n] = 'y'; }
  }
  else
  {
    BDate dte = first;
    BDate axisDte = axisTms->Successor(dte);
    num++;
    select[0] = 'y';
    for(BInt n=1; n<max-1; n++)
    {
      dte = serTms->Successor(dte);
      if(dte<axisDte)
      {
	select[n] = 'n';
      }
      else
      {
	num++;
	select[n] = 'y';
	axisDte	  = axisTms->Successor(dte);
      }
//    Std(BText("\nAxis date = ") + axisDte +
//		  "Series date = " + dte + " : " + select[n]);
    }
    select[max-1] = 'y';
    num++;
  }
  return(num);
}


//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BGraObject<BTimeSet>::InitInstances()
{
    BTraceInit("BUserTimeSet::InitInstances");
    BTmsVoid*	 void_	        = new BTmsVoid;
    BTmsAllDays* allDays_     = new BTmsAllDays("C");
    BTmsAllDays* allDaysEsp_  = new BTmsAllDays("Diario");
    BTmsAllDays* allDaysEng_  = new BTmsAllDays("Daily");
    BTmsEaster*	 easterEng_   = new BTmsEaster("Easter");
    BTmsEaster*	 easterEsp_   = new BTmsEaster("Pascua");
    OwnGrammar()->PutDefect(void_);
}


//--------------------------------------------------------------------
/*! Returns a valid constant time set for name.
 */
template<>
BSyntaxObject* BGraObject<BTimeSet>::FindConstant (const BText&)
{ return(NIL); }


//--------------------------------------------------------------------
/*! Returns a valid time set for obj.
 */
template<>
BSyntaxObject* BGraObject<BTimeSet>::Casting(BSyntaxObject* obj)
{
    if(!obj)			       { return(NIL); }
    if(obj->Grammar()==OwnGrammar())   { return(obj); }
    if(obj->Grammar()->Name()=="Date") { return(new BTmsOneDay(NCons(obj))); }
    return(NIL);
}

//--------------------------------------------------------------------
BSyntaxObject* BUserTimeSet::CopyContens()
//--------------------------------------------------------------------
{
  return(OwnGrammar()->New("",this));
}

//--------------------------------------------------------------------
// Algebraic temporary class declarations.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// BTmsAllDays

BBool BTmsAllDays::Includes(const BDate& dte) const
{
  if(!dte.HasValue()) { return(false); }
  return(!dte.Fraction());
}

BDate BTmsAllDays::Successor(const BDate& dte) const
{
  EnsureLimitsSuccessor(dte);
  BDate d = dte;
  d.PutFraction(0);
  d.IncDay(1);
  assert(d>=dte);
  return(d);
}

BDate BTmsAllDays::Predecessor(const BDate& dte) const
{
  EnsureLimitsPredecessor(dte);
  BDate d = dte;
  d.PutFraction(0);
  if(!dte.Fraction()) { d.IncDay(-1); }
  assert(d<=dte);
  return(d);
}

BDate BTmsAllDays::Next(const BDate& dte, BInt nth) const
{
  EnsureLimitsSuccessor(dte);
  if(nth<0)  { return(Prev(dte,-nth)); }
  if(nth==0) { return(dte.Fraction()?BDate::Unknown():dte); }
  if(!dte.HasValue()) { return(dte); }
  BDate d = dte;
  d.PutFraction(0);
  d.IncDay(nth);
  return(d);
}

BDate BTmsAllDays::Prev(const BDate& dte, BInt nth) const
{
  EnsureLimitsPredecessor(dte);
  if(nth<0)  { return(Next(dte,-nth)); }
  if(nth==0) { return(dte.Fraction()?BDate::Unknown():dte); }
  if(!dte.HasValue()) { return(dte); }
  BDate d = dte;
  d.PutFraction(0);
  if(dte.Fraction()>0) { nth--; } 
  d.IncDay(-nth);
  return(d);
}

BInt	BTmsAllDays::Difference(const BDate& dte1, const BDate& dte2) const
{
  return(DifInDays(dte1,dte2));
}


// End of BTmsAllDays

//--------------------------------------------------------------------
BTmsEaster::BTmsEaster(const BText& name)
//--------------------------------------------------------------------
: BTmsStaticCached<BUserTimeSet, BI_EASTER>(name,
   I2("The time set of all on Sundays of Easter.",
      "El conjunto temporal de todos los domingos de Pascua."))
{
}

//--------------------------------------------------------------------
  BDate BTmsEaster::CalcInf() const 
//--------------------------------------------------------------------
{ return(BDate::Begin()); }
//{ return(BDate(1583,4,10)); }


//--------------------------------------------------------------------
BBool BTmsEaster::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(!dte.HasValue()) { return(false); }
  return(dte == DteEasterSunday(dte.Year()));
}

//--------------------------------------------------------------------
BDate BTmsEaster::Successor(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
  BDate easterDay = DteEasterSunday(dte.Year());
  BDate d;
  if (dte < easterDay) { d=easterDay; }
  else	               { d=DteEasterSunday(dte.Year()+1); }
  assert(d>=dte);
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsEaster::Predecessor(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
  BDate easterDay = DteEasterSunday(dte.Year());
  BDate d;
  if (dte > easterDay)     { d=easterDay; }
  else		                 { d=DteEasterSunday(dte.Year()-1); }
  assert(d<=dte);
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsEaster::Next(const BDate& dte, int nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
  if(nth<0)  { return(Prev(dte,-nth)); }
  if(nth==0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  BDate d = Successor(dte);
  if(nth>1) 
  { 
    int y = d.Year()+nth-1;
    d = DteEasterSunday(y);
  }
  assert(d>=dte);
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsEaster::Prev(const BDate& dte, int nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
  if(nth<0)  { return(Next(dte,-nth)); }
  if(nth==0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  BDate d = Predecessor(dte);
  if(nth>1) 
  {
    if(d <= Inf()) { return(BDate::Begin()); }
    int y = d.Year()-nth+1;
    if(y < Inf().Year())   { return(BDate::Begin()); }
    d = DteEasterSunday(y);
  }
  assert(d<=dte);
  return(d);
}

//--------------------------------------------------------------------
BInt	BTmsEaster::Difference(const BDate& dte1, const BDate& dte2) const
//--------------------------------------------------------------------
{
  return(BTimeSet::Difference(dte1,dte2));
//BDate d1 = FirstNoLess (dte1);
//BDate d2 = FirstNoGreat(dte2);
//return(d2.Year()-d1.Year());
}


//--------------------------------------------------------------------
DefExtOpr(1, BTmsWeekDay, "WD", 1, 1, "Real",
  "(Real n)",
  I2("Returns the time set of all dates that are the teenth day "
     "of the week. Parameter n should be in a range between 1 (Monday) and "
     "7 (Sunday).",
     "Devuelve el conjunto temporal de todas las fechas que son el enésimo "
     "día de la semana. El parámetro n tiene que estar entre los valores "
      "1 (Lunes) y 7 (Domingo)."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsWeekDay::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{ 
  if(!dte.HasValue()) { return(false); }
  return(!dte.Fraction() && dte.WeekDay() == weekDay_); 
}
//--------------------------------------------------------------------
  BDate BTmsWeekDay::CalcInf() const 
//--------------------------------------------------------------------
{ 
{ return(BDate::Begin()); }
//return(BDate(1583,1,2)+weekDay_); 
}

//--------------------------------------------------------------------
BDate BTmsWeekDay::Successor(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
  BDate d = dte; 
  d.PutFraction(0);
  int w = d.WeekDay();
  d+=(7+weekDay_-w)%7 + 7*(weekDay_==w);
//Std(BText("\nBTmsWeekDay::Successor(")+dte+")="+d);
  assert(d>=dte);
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsWeekDay::Predecessor(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
  BDate d = dte; 
  d.PutFraction(0);
  if(dte.Fraction()) { d.IncDay(1); }
  int w = d.WeekDay();
  d-= (7+w-weekDay_)%7 + 7*((w==weekDay_));
//Std(BText("\nBTmsWeekDay::Predecessor(")+dte+")="+d);
  assert(d<=dte);
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsWeekDay::Next(const BDate& dte, int nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
  if(nth<0)  { return(Prev(dte,-nth)); }
  if(nth==0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  BDate d = Successor(dte); 
  if(nth>1) { d+= 7*(nth-1); }
//Std(BText("\nBTmsWeekDay::Next(")+dte+","+nth+")="+d);
  assert(d>=dte);
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsWeekDay::Prev(const BDate& dte, int nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
  if(nth<0)  { return(Next(dte,-nth)); }
  if(nth==0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  int w = dte.WeekDay();
  BDate d = Predecessor(dte); 
  if(nth>1) { d-= 7*(nth-1); }
  if(d < Inf()) { return(BDate::Begin()); }

//Std(BText("\nBTmsWeekDay::Prev(")+dte+","+nth+")="+d);
  assert(d<=dte);
  return(d);
}


//--------------------------------------------------------------------
BInt	BTmsWeekDay::Difference(const BDate& dte1, const BDate& dte2) const
//--------------------------------------------------------------------
{
  return(BTimeSet::Difference(dte1,dte2));
//BDate d1 = FirstNoLess (dte1);
//BDate d2 = FirstNoGreat(dte2);
//return(DifInDays(d1, d2)/7);

}


//--------------------------------------------------------------------
DefExtOpr(1, BTmsMonthDay, "D", 1, 1, "Real",
  "(Real n)",
  I2("Returns the time set of all the dates that are the umpteenth day of "
     "the month.",
     "Devuelve el conjunto temporal de todas las fechas que son el enésimo "
     "día del mes."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsMonthDay::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{ 
  if(!dte.HasValue()) { return(false); }
  return(!dte.Fraction() && dte.Day() == monthDay_); 
}
//--------------------------------------------------------------------
  BDate BTmsMonthDay::CalcInf() const 
//--------------------------------------------------------------------
{ 
{ return(BDate::Begin()); }
//return(BDate(1583,1,monthDay_)); 
}

//--------------------------------------------------------------------
BDate BTmsMonthDay::Successor(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
  BDate suc(dte);
  suc.PutFraction(0);
  if (dte.Day() >= monthDay_) { suc.IncMonth(1); }
  while (DteDaysInMonth(suc.Month(), suc.Year()) < monthDay_)
  { suc.IncMonth(1); }
  suc.PutDay(monthDay_);
  assert(suc>=dte);
  return(suc);
}

//--------------------------------------------------------------------
BDate BTmsMonthDay::Predecessor(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
  BDate pre = dte;
  pre.PutFraction(0);
  if((dte.Day() <  monthDay_) ||
     (dte.Day() == monthDay_) && !dte.Fraction()) { pre.IncMonth(-1); }
  while (DteDaysInMonth(pre.Month(), pre.Year()) < monthDay_)
  { pre.IncMonth(-1); }
  pre.PutDay(monthDay_);
  assert(pre<=dte);
  return(pre);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTmsMonth, "M", 1, 1, "Real",
  "(Real n)",
  I2("Returns the time set of all the dates of the umpteenth month of the "
     "year.",
     "Devuelve el conjunto temporal de todas las fechas del enésimo mes del "
     "año."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsMonth::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{ 
  if(!dte.HasValue()) { return(false); }
  return(!dte.Fraction() && dte.Month() == month_); 
}
//--------------------------------------------------------------------
  BDate BTmsMonth::CalcInf() const 
//--------------------------------------------------------------------
{ 
{ return(BDate::Begin()); }
//return(BDate(1583,month_,1)); 
}

//--------------------------------------------------------------------
BDate BTmsMonth::Successor(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
  BDate suc(dte);
  suc.PutFraction(0);
  if (suc.Month() < month_) { suc.PutDate(dte.Year(), month_, 1); }
  else if (suc.Month() == month_)
  {
  	suc.IncDay();
	  if (suc.Month() != month_) { suc.PutDate(dte.Year()+1, month_, 1); }
  }
  else { suc.PutDate(dte.Year()+1, month_, 1); }
  assert(suc>=dte);
  return(suc);
}

//--------------------------------------------------------------------
BDate BTmsMonth::Predecessor(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
  BDate pre = dte;
  pre.PutFraction(0);
  if (pre.Month() < month_)
  { pre.PutDate(dte.Year()-1,month_,DteDaysInMonth(month_,dte.Year()-1)); }
  else if (pre.Month() == month_)
  {
    if(!dte.Fraction()) { pre.IncDay(-1); }
    if (pre.Month() != month_)
    { pre.PutDate(dte.Year()-1,month_,DteDaysInMonth(month_,dte.Year()-1)); }
  }
  else // (pre.Month() > month)
  { pre.PutDate(dte.Year(),month_,DteDaysInMonth(month_,dte.Year())); }
  assert(pre<=dte);
  return(pre);
}

//--------------------------------------------------------------------
DeclareTimeSetEvaluator(BTmsHour);
//--------------------------------------------------------------------
DefExtOpr(1, BTmsHour, "H", 1, 1, "Real",
  I2("(Real hour)",
     "(Real hora)"),
  I2("Returns the time set of all the dates in the given hour.",
     "Devuelve el conjunto temporal de todas las fechas en la hora dada."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsHour::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(!dte.HasValue()) { return(false); }
  return((dte.Hour()==hour_)&&(dte.Minute()==0)&&(dte.Second()==0));
}

//--------------------------------------------------------------------
BDate BTmsHour::Successor   (const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
    BDate d = dte;
    d.PutMinute(0);
    d.PutSecond(0);
    if(d.Hour()>=hour_) { d.IncDay(1); }
    d.PutHour(hour_);
  assert(d>=dte);
    return(d);
}

//--------------------------------------------------------------------
BDate BTmsHour::Predecessor (const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
    BDate d = dte;
    d.PutMinute(0);
    d.PutSecond(0);
    if(d.Hour()<=hour_) { d.IncDay(-1); }
    d.PutHour(hour_);
  assert(d<=dte);
    return(d);
}

//--------------------------------------------------------------------
BDate BTmsHour::Next(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
  if(nth == 0) { return((Includes(dte))?dte:BDate::Unknown()); }
  if(!dte.HasValue()) { return(dte); }
  if(nth<0) { return(Prev(dte,-nth)); }
  BDate d = Successor(dte);
  d.IncDay(nth-1);
  assert(d>=dte);
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsHour::Prev(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
  if(nth == 0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  if(!dte.HasValue()) { return(dte); }
  if(nth<0) { return(Next(dte,-nth)); }
  BDate d = Predecessor(dte);
  d.IncDay(1-nth);
  assert(d<=dte);
  return(d);
}

//--------------------------------------------------------------------
BInt	BTmsHour::Difference(const BDate& dte1, const BDate& dte2) const
//--------------------------------------------------------------------
{
    return(DifInDays(dte1,dte2));
}



//--------------------------------------------------------------------
DeclareTimeSetEvaluator(BTmsMinute);
//--------------------------------------------------------------------
  DefExtOpr(1, BTmsMinute, "Mi", 1, 1, "Real",
  I2("(Real minute)",
     "(Real minuto)"),
  I2("Returns the time set of all the dates in the given minute.",
     "Devuelve el conjunto temporal de todas las fechas en el minuto dado."),
	    BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsMinute::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(!dte.HasValue()) { return(false); }
  return((dte.Minute()==minute_)&&(dte.Second()==0));
}

//--------------------------------------------------------------------
BDate BTmsMinute::Successor	(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
    BDate d = dte;
    d.PutSecond(0);
    if(d.Minute()>=minute_) { d.IncHour(1); }
    d.PutMinute(minute_);
  assert(d>=dte);
    return(d);
}

//--------------------------------------------------------------------
BDate BTmsMinute::Predecessor (const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
    BDate d = dte;
    d.PutSecond(0);
    if(d.Minute()<=minute_) { d.IncHour(-1); }
    d.PutMinute(minute_);
  assert(d<=dte);
    return(d);
}

//--------------------------------------------------------------------
BDate BTmsMinute::Next(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
  if(nth == 0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  if(!dte.HasValue()) { return(dte); }
  if(nth<0) { return(Prev(dte,-nth)); }
  BDate d = Successor(dte);
  d.IncHour(nth-1);
  assert(d>=dte);
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsMinute::Prev(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
  if(nth == 0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  if(!dte.HasValue()) { return(dte); }
  if(nth<0) { return(Next(dte,-nth)); }
  BDate d = Predecessor(dte);
  d.IncHour(1-nth);
  assert(d<=dte);
  return(d);
}

//--------------------------------------------------------------------
BInt	BTmsMinute::Difference(const BDate& dte1, const BDate& dte2) const
//--------------------------------------------------------------------
{
    return(DifInHours(dte1,dte2));
}



//--------------------------------------------------------------------
DeclareTimeSetEvaluator(BTmsSecond);
//--------------------------------------------------------------------
DefExtOpr(1, BTmsSecond, "S", 1, 1, "Real",
  I2("(Real second)",
     "(Real segundo)"),
  I2("Returns the time set of all the dates in the given second.",
     "Devuelve el conjunto temporal de todas las fechas en el segundo dado."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsSecond::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(!dte.HasValue()) { return(false); }
  return(Floor(dte.Second())==second_);
}

//--------------------------------------------------------------------
BDate BTmsSecond::Successor	(const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
    BDate d = dte;
    d.PutSecond(BInt(d.Second()));
    if(d.Second()>=second_) { d.IncMinute(1); }
    d.PutSecond(second_);
  assert(d>=dte);
    return(d);
}

//--------------------------------------------------------------------
BDate BTmsSecond::Predecessor (const BDate& dte) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
    BDate d = dte;
    d.PutSecond(BInt(d.Second()));
    if(d.Second()<=second_) { d.IncMinute(-1); }
    d.PutSecond(second_);
  assert(d<=dte);
    return(d);
}

//--------------------------------------------------------------------
BDate BTmsSecond::Next(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsSuccessor(dte);
  if(nth == 0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  if(!dte.HasValue()) { return(dte); }
  if(nth<0) { return(Prev(dte,-nth)); }
  BDate d = Successor(dte);
  d.IncMinute(nth-1);
  assert(d>=dte);
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsSecond::Prev(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  EnsureLimitsPredecessor(dte);
  if(nth == 0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  if(!dte.HasValue()) { return(dte); }
  if(nth<0) { return(Next(dte,-nth)); }
  BDate d = Predecessor(dte);
  d.IncMinute(1-nth);
  assert(d<=dte);
  return(d);
}

//--------------------------------------------------------------------
BInt	BTmsSecond::Difference(const BDate& dte1, const BDate& dte2) const
//--------------------------------------------------------------------
{
    return(DifInMinutes(dte1,dte2));
}

