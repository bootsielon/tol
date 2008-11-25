/* tol_btmsgra.h: Classes and functions for time set grammar.
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

#ifndef TOL_BTMSGRA_H
#define TOL_BTMSGRA_H 1

#include <tol/tol_btmset.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bgenobj.h>
#include <tol/tol_btmsexc.h>


//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------


//--------------------------------------------------------------------
class BUserTimeSet: public BGraObject<BTimeSet>
//--------------------------------------------------------------------
{
public:
  BUserTimeSet(BList* arg=NULL)
  : BGraObject<BTimeSet>() {}

  BUserTimeSet(const BText& name)
  : BGraObject<BTimeSet>(name) {}

  BUserTimeSet(const BText& name, const BText& description)
  : BGraObject<BTimeSet>(name,description) {}

 ~BUserTimeSet() {}

  BText Dump       () const { return(Identify()); };
  BText GetIdentify() const { return(Identify()); };
  BSyntaxObject* CopyContens ();

  bool IsBounded () const 
  {
    return(Inf().HasValue() && Sup().HasValue());
  }
};


//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
inline BUserTimeSet* Tms(BSyntaxObject* arg)
{
  BUserTimeSet* uTms = NIL;
  if(arg && (arg->Grammar()==BUserTimeSet::OwnGrammar()))
  { uTms = BCast<BUserTimeSet,BCore>::c(arg); }
  return(uTms);
}

inline BUserTimeSet* Tms(BCore* arg)
{ return(Tms(BCast<BSyntaxObject,BCore>::c(arg))); }

inline BUserTimeSet* Tms(const BText& expression)
{ return(Tms(BUserTimeSet::OwnGrammar()->EvaluateExpr(expression))); }


//--------------------------------------------------------------------
template<class BRefTemplate>
class BTmsReference : public BRefTemplate
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTmsReference(const BText& name, BSyntaxObject* d)
  : BRefTemplate(name, d) {}

 ~BTmsReference() {}

  void ReCalc () {}


  const BDate& CurrentCalcFirstDate() const 
  { 
    return(this->GetResult()->CurrentCalcFirstDate()); 
  }
  const BDate& CurrentCalcLastDate () const 
  { 
    return(this->GetResult()->CurrentCalcLastDate()); 
  }
  void  ReadData()
  {
    return(this->GetResult()->ReadData()); 
  }
  BBool Includes(const BDate& dte) const
  {
    return(this->GetResult()->Includes(dte)); 
  }
  BDate CalcInf()const
  {
    return(this->GetResult()->CalcInf()); 
  }
  BDate CalcSup()const
  {
    return(this->GetResult()->CalcSup()); 
  }
  BInt  HashFindDate(BReal h, BBool& okHash) const
  {
    return(this->GetResult()->HashFindDate(h,okHash)); 
  }
  BBool HashIncludes(BReal h, BBool& okHash, BInt& pos) const
  {
    return(this->GetResult()->HashIncludes(h,okHash,pos)); 
  }
  BDate HashSuccessor(BReal h, BBool& okHash, BInt& pos) const
  {
    return(this->GetResult()->HashSuccessor(h,okHash,pos)); 
  }
  BDate HashPredecessor(BReal h, BBool& okHash, BInt& pos) const
  {
    return(this->GetResult()->HashPredecessor(h,okHash,pos)); 
  }
  BBool Contain(const BDate& dte)     const
  { 
    return(this->GetResult()->Contain(dte)); 
  }
  BDate Successor(const BDate& dte) const
  { 
    return(this->GetResult()->Successor(dte)); 
  }
  BDate Predecessor(const BDate& dte) const
  { 
    return(this->GetResult()->Predecessor(dte)); 
  }
  BDate Next(const BDate& dte, BInt nth=1)     const
  {
    return(this->GetResult()->Next(dte, nth)); 
  }
  BDate Prev(const BDate& dte, BInt nth=1)     const
  {
    return(this->GetResult()->Prev(dte, nth)); 
  }

  
  const BHash& Cache () const 
  { 
    return(this->GetResult()->Cache());  
  }
  void  SetCache           (const BHash& hash)
  {
    this->GetResult()->SetCache(hash); 
  }
  void  FreeCache()
  {
    this->GetResult()->FreeCache(); 
  }
  int CacheCallsNumber() const
  {
    return(this->GetResult()->CacheCallsNumber());  
  }
  void  AddCacheCallsNumber() 
  {
    this->GetResult()->AddCacheCallsNumber(); 
  }
  const BDate& BeginCache() const
  {
    return(this->GetResult()->BeginCache()); 
  }
  void PutBeginCache(const BDate& dte) 
  {
    this->GetResult()->PutBeginCache(dte); 
  }
  const BDate& EndCache  () const
  {
    return(this->GetResult()->EndCache()); 
  }
  void PutEndCache  (const BDate& dte) 
  {
    this->GetResult()->PutEndCache(dte); 
  }
  bool  BuildingCache() const
  {
    return(this->GetResult()->BuildingCache());  
  }
  void  PutBuildingCache(bool f)
  {
    this->GetResult()->PutBuildingCache(f); 
  }
  bool  ForcingCache() const
  {
    return(this->GetResult()->ForcingCache());  
  }
  void  PutForcingCache(bool f)
  {
    this->GetResult()->PutForcingCache(f); 
  }

  void  PutCache  (const BHash& hash, const BDate& begin, const BDate& end)
  {
    this->GetResult()->PutCache(hash, begin, end); 
  }
  void ForceCache(BDate first = BDate::DefaultFirst(),
                  BDate last  = BDate::DefaultLast ()) const
  {
    this->GetResult()->ForceCache(first,last); 
  }

  BInt  Difference(const BDate& dte1, const BDate& dte2) const
  { 
    return(this->GetResult()->Difference(dte1, dte2)); 
  }
  BInt  PeriodsBetween(const BDate& d1, const BDate& d2) const
  {
    return(this->GetResult()->PeriodsBetween(d1,d2)); 
  }
  void GetHashBetween (BHash& hash,
                       BDate first = BDate::DefaultFirst(),
                       BDate last  = BDate::DefaultLast ()) const
  {
    this->GetResult()->GetHashBetween(hash,first,last); 
  }
  void PutInf       (const BDate& dte) 
  {
    this->GetResult()->PutInf(dte); 
  }
  void PutSup       (const BDate& dte) 
  {
    this->GetResult()->PutSup(dte); 
  }
  const BDate& Inf() const 
  {
    return(this->GetResult()->Inf()); 
  }
  const BDate& Sup() const 
  {
    return(this->GetResult()->Sup()); 
  }
  BText TestCoherence(      int    verbose, 
                              const BDate& first, 
                              const BDate& last,
                                    double maxTimeSim, 
                                    int    numSim, 
                                    int    maxLag)
  {
    return(this->GetResult()->TestCoherence(verbose,first,last,
                                            maxTimeSim,numSim,maxLag)); 
  }
  BText ListOfDates(BDate d1, BDate d2, BText sep = ", ", BInt jump = 7)
  {
    return(this->GetResult()->ListOfDates(d1,d2,sep,jump)); 
  }
  void CalcHashBetween(BHash& hash, 
                       BDate first = BDate::DefaultFirst(),
                       BDate last  = BDate::DefaultLast ())
  {
    this->GetResult()->CalcHashBetween(hash,first,last); 
  }
  const BText& TestError(const BText& cod, 
                         const BText& msg, 
                         const BText& interval)
  {
    return(this->GetResult()->TestError(cod,msg,interval)); 
  }
  const BText& TestAborted(const BText& cod, 
                         const BText& msg, 
                         const BText& interval)
  {
    return(this->GetResult()->TestAborted(cod,msg,interval)); 
  }
  bool AbortedMessageSended() const 
  { 
    return(this->GetResult()->AbortedMessageSended()); 
  }

};



//--------------------------------------------------------------------
class BTmsRenamed : public BTmsReference< BRefObject< BUserTimeSet > >
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTmsRenamed(const BText& name, BSyntaxObject* d)
  :  BTmsReference< BRefObject< BUserTimeSet > >(name, d) {}
 ~BTmsRenamed() {}
  // Implementation: tmsgra.cpp
  static BSyntaxObject* New(const BText& name, BSyntaxObject* d)
  { return((BSyntaxObject*)(new BTmsRenamed(name, d))); }
  DeclareClassNewDelete(BTmsRenamed);
};

//--------------------------------------------------------------------
class BTmsFunArg : public BTmsReference< BFunArgObject<BUserTimeSet> >
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTmsFunArg(const BText& localName, BSyntaxObject* d)
  : BTmsReference< BFunArgObject<BUserTimeSet> >(localName, d) {}
 ~BTmsFunArg() {}

  // Implementation: tmsgra.cpp
  static BSyntaxObject* NewLocal(const BText& localName, BSyntaxObject* d)
  { return((BSyntaxObject*)(new BTmsFunArg(localName, d))); }
  DeclareClassNewDelete(BTmsFunArg);
};


//--------------------------------------------------------------------
  class BCacheInfo
//--------------------------------------------------------------------
{ 
public:
  BHash cache_;  
  int   cacheCallsNumber_;
  BDate beginCache_;
  BDate endCache_;
  bool  forzingCache_;
  bool  buildingCache_;
  BCacheInfo()
  : cache_               (0),
    cacheCallsNumber_    (0),
    beginCache_          (BDate::Unknown()),
    endCache_            (BDate::Unknown()),
    forzingCache_        (false),
    buildingCache_       (false)
  {}
};

//--------------------------------------------------------------------
enum BTmsSCBaseId
//--------------------------------------------------------------------
{
  BI_NONE     = -1,
  BI_VOID     = 0,
  BI_ALL      = 1,
  BI_EASTER   = 2,
  BI_WEEKDAY  = 3,
  BI_MONTHDAY = 4,
  BI_MONTH    = 5,
  BI_HOUR     = 6,
  BI_MINUTE   = 7,
  BI_SECOND   = 8
};


//--------------------------------------------------------------------
template<class BTmsBase, BTmsSCBaseId baseId_>
class BTmsStaticCached : public BTmsBase

/*! Time set that contains all days between iniDate_ and last_.
 */
//--------------------------------------------------------------------
{
private:
  static BArray<BCacheInfo> cacheInfo_;
  virtual int CachePosition() const =0;

public:
  BTmsStaticCached(BList* arg=NULL)
  : BTmsBase(arg)
  {}

  BTmsStaticCached(const BText& name)
  : BTmsBase(name) {}

  BTmsStaticCached(const BText& name, const BText& description)
  : BTmsBase(name,description) {}

  const BHash& Cache       () const           { return(cacheInfo_(CachePosition()).cache_);  }
  void  SetCache           (const BHash& hash){ cacheInfo_(CachePosition()).cache_ = hash; }
  void  FreeCache          ()                 { }
  int   CacheCallsNumber   () const           { return(cacheInfo_(CachePosition()).cacheCallsNumber_); }
  void  AddCacheCallsNumber()                 { cacheInfo_(CachePosition()).cacheCallsNumber_++; }
  const BDate& BeginCache  () const           { return(cacheInfo_(CachePosition()).beginCache_);  }
  void  PutBeginCache      (const BDate& dte) { cacheInfo_(CachePosition()).beginCache_ = dte; }
  const BDate& EndCache    () const           { return(cacheInfo_(CachePosition()).endCache_);  }
  void  PutEndCache        (const BDate& dte) { cacheInfo_(CachePosition()).endCache_= dte; }
  bool  BuildingCache      () const           { return(cacheInfo_(CachePosition()).buildingCache_);  }
  void  PutBuildingCache   (bool f)           { cacheInfo_(CachePosition()).buildingCache_ = f; }
  bool  ForcingCache       () const           { return(cacheInfo_(CachePosition()).forzingCache_);  }
  void  PutForcingCache    (bool f)           { cacheInfo_(CachePosition()).forzingCache_ = f; }
};

//--------------------------------------------------------------------
class BTmsVoid: public BTmsStaticCached<BUserTimeSet, BI_VOID>
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTmsVoid() 
  : BTmsStaticCached<BUserTimeSet,BI_VOID>("W", "The empty time set")
  {
  }
 ~BTmsVoid() { }

  void ReCalc () {}
  // Implementation: inline
  BDate Successor   (const BDate& dte) const { return(BDate::End  ()); }
  BDate Predecessor (const BDate& dte) const { return(BDate::Begin()); }
  BDate Next        (const BDate& dte, BInt nth=1) const { return(BDate::End  ()); }
  BDate Prev        (const BDate& dte, BInt nth=1) const { return(BDate::Begin()); }
  BInt  Difference  (const BDate& dte1, const BDate& dte2) const {  return(0); }
  void GetHashBetween (BHash& hash,
                       BDate first = BDate::DefaultFirst(),
                       BDate last  = BDate::DefaultLast ())
  { 
    hash.DeleteBuffer();
  }
protected:
  BBool Includes    (const BDate& dte) const { return(BFALSE); }
  BDate CalcInf() const { return(BDate::End  ()); }
  BDate CalcSup() const { return(BDate::Begin()); }
private:
  int CachePosition() const { return(0); }
  DeclareClassNewDelete(BTmsVoid);
};


//--------------------------------------------------------------------
class BTmsAllDays: public BTmsStaticCached<BUserTimeSet, BI_ALL>
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTmsAllDays(const BText& name="")
  : BTmsStaticCached<BUserTimeSet,BI_ALL>(name, "The time set of all days in history")
  {
  }
 ~BTmsAllDays() { }

  void ReCalc () {}
  // Implementation: inline
  BDate Successor   (const BDate& dte)         const;
  BDate Predecessor (const BDate& dte)         const;
  BDate Next        (const BDate& dte, BInt nth=1)     const;
  BDate Prev        (const BDate& dte, BInt nth=1)     const;
  BInt  Difference  (const BDate& dte1, const BDate& dte2) const;

protected:
  BBool Includes    (const BDate& dte) const;
private:
  int CachePosition() const { return(0); }
  DeclareClassNewDelete(BTmsAllDays);
};

//--------------------------------------------------------------------
class BTmsEaster : public BTmsStaticCached<BUserTimeSet, BI_EASTER>
//--------------------------------------------------------------------
{
public:
  BTmsEaster(const BText& name="");
 ~BTmsEaster() {}

  void ReCalc () {}
  BDate Successor   (const BDate& dte) const;
  BDate Predecessor (const BDate& dte) const;
  BDate Next        (const BDate& dte, BInt nth=1) const;
  BDate Prev        (const BDate& dte, BInt nth=1) const;
  BInt  Difference  (const BDate& dte1, const BDate& dte2) const;
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const { return(BDate::End  ()); }
private:
  int CachePosition() const { return(0); }
  DeclareClassNewDelete(BTmsEaster);
};


//--------------------------------------------------------------------
class BTmsTemporary: public BTmpObject< BUserTimeSet >

/*! Base class for operator which arguments are time sets.
 */
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTmsTemporary(BList* arg=NULL)
  : BTmpObject< BUserTimeSet >(arg) { }

  BTmsTemporary(const BText& name)
  : BTmpObject< BUserTimeSet >(NULL) {}

  BTmsTemporary(const BText& name, const BText& description)
  : BTmpObject< BUserTimeSet >(NULL) {}

 ~BTmsTemporary() { }
//BBool ReCalcFrom (BSyntaxObject* syn) { return(this==syn); }
  void ReCalc () {}
};


//--------------------------------------------------------------------
class BTmsWeekDay : public BTmsStaticCached<BTmsTemporary, BI_WEEKDAY>

/*! Time set of all days that are the weekDay_-th day in any week.
 */
//--------------------------------------------------------------------
{
 private:
  int weekDay_;
  int CachePosition() const { return(weekDay_-1); }
    
 public:
  BTmsWeekDay(BList* arg) //throw(TimeSetException) 
  : BTmsStaticCached<BTmsTemporary,BI_WEEKDAY>(arg)
  {
      weekDay_ = (BInt)Real(Arg(1));
      if((weekDay_<1) || (weekDay_>7)) throwBadCreationExc();
  }
  BTmsWeekDay(BInt wd) //throw(TimeSetException) 
  : BTmsStaticCached<BTmsTemporary,BI_WEEKDAY>(NIL) 
  {
      weekDay_ = wd;
      if((weekDay_<1) || (weekDay_>7)) throwBadCreationExc();
  }
 ~BTmsWeekDay() {}

    BDate Successor   (const BDate& dte) const;
    BDate Predecessor (const BDate& dte) const;
    BDate Next        (const BDate& dte, BInt nth=1) const;
    BDate Prev        (const BDate& dte, BInt nth=1) const;
    BInt  Difference  (const BDate& dte1, const BDate& dte2) const;

protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const { return(BDate::End  ()); }
  void throwBadCreationExc() throw(TimeSetException) 
  {
    std::string enMsg, esMsg;
    std::stringstream ss;
    ss << weekDay_;
    enMsg = "Bad argument used to create a TimeSet variable: WD(";
    enMsg += ss.str() + ").";
    esMsg = "Argumento errï¿½eo al crear una variable TimeSet: WD(";
    esMsg += ss.str() + ").";
    throw TimeSetException(enMsg, esMsg);  
  }
  DeclareClassNewDelete(BTmsWeekDay);
};
DeclareTimeSetEvaluator(BTmsWeekDay);


//--------------------------------------------------------------------
class BTmsMonthDay: public BTmsStaticCached<BTmsTemporary, BI_MONTHDAY>

/*! Time set of all days that are the monthDay_-th day in any month.
 */
//--------------------------------------------------------------------
{
private:
  BInt monthDay_;
  int CachePosition() const { return(monthDay_-1); }

public:
  BTmsMonthDay(BList* arg)
  : BTmsStaticCached<BTmsTemporary,BI_MONTHDAY>(arg) 
  { 
    monthDay_=(BInt)Real(Arg(1)); 
    if((monthDay_<1) || (monthDay_>31)) throwBadCreationExc();
  }
  BTmsMonthDay(BInt md)
  : BTmsStaticCached<BTmsTemporary,BI_MONTHDAY>(NIL), monthDay_(md) 
  {
    if((monthDay_<1) || (monthDay_>31)) throwBadCreationExc();
  }
 ~BTmsMonthDay() {}
  BDate Successor   (const BDate& dte) const;
  BDate Predecessor (const BDate& dte) const;


protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const { return(BDate::End  ()); }

  void throwBadCreationExc() throw(TimeSetException) 
  {
    std::string enMsg, esMsg;
    std::stringstream ss;
    ss << monthDay_;
    enMsg = "Bad argument used to create a TimeSet variable: D(";
    enMsg += ss.str() + ").";
    esMsg = "Argumento erróneo al crear una variable TimeSet: D(";
    esMsg += ss.str() + ").";
    throw TimeSetException(enMsg, esMsg);  
  }
  DeclareClassNewDelete(BTmsMonthDay);
};
DeclareTimeSetEvaluator(BTmsMonthDay);



//--------------------------------------------------------------------
class BTmsMonth : public BTmsStaticCached<BTmsTemporary, BI_MONTH>

/*! Time set of all days that are in the month_-th month in any year
 */
//--------------------------------------------------------------------
{
private:
  BInt month_;
  int CachePosition() const { return(month_-1); }

public:
  BTmsMonth(BList* arg)
  : BTmsStaticCached<BTmsTemporary,BI_MONTH>(arg) 
  { 
    month_=(BInt)Real(Arg(1)); 
    if((month_<1) || (month_>12)) throwBadCreationExc();
  }
  BTmsMonth(BInt m)
  : BTmsStaticCached<BTmsTemporary,BI_MONTH>(NIL), month_(m) 
  {
    if((month_<1) || (month_>12)) throwBadCreationExc();
  }
 ~BTmsMonth() {}
  BDate Successor   (const BDate& dte) const;
  BDate Predecessor (const BDate& dte) const;
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const { return(BDate::End  ()); }

  void throwBadCreationExc() throw(TimeSetException) 
  {
    std::string enMsg, esMsg;
    std::stringstream ss;
    ss << month_;
    enMsg = "Bad argument used to create a TimeSet variable: M(";
    enMsg += ss.str() + ").";
    esMsg = "Argumento erróneo al crear una variable TimeSet: M(";
    esMsg += ss.str() + ").";
    throw TimeSetException(enMsg, esMsg);  
  }
  DeclareClassNewDelete(BTmsMonth);
};
DeclareTimeSetEvaluator(BTmsMonth);


//--------------------------------------------------------------------
class BTmsHour : public BTmsStaticCached<BTmsTemporary, BI_HOUR>
//--------------------------------------------------------------------
{
private:
  BInt  hour_;
  int CachePosition() const { return(hour_); }

public:
  BTmsHour(BList* arg) : BTmsStaticCached<BTmsTemporary,BI_HOUR>(arg)
  {
    hour_=(BInt)Real(Arg(1));
    if((hour_<0) || (hour_>23)) throwBadCreationExc();
  }
  BTmsHour(int h) : BTmsStaticCached<BTmsTemporary,BI_HOUR>()
  {
    hour_= h;
    if((hour_<0) || (hour_>23)) throwBadCreationExc();
  }
 ~BTmsHour() {}
  BDate Successor   (const BDate& dte) const;
  BDate Predecessor (const BDate& dte) const;
  BDate Next        (const BDate& dte, BInt nth=1) const;
  BDate Prev        (const BDate& dte, BInt nth=1) const;
  BInt  Difference  (const BDate& dte1, const BDate& dte2) const;
protected:
  BBool Includes    (const BDate& dte) const;
  void throwBadCreationExc() throw(TimeSetException) 
  {
    std::string enMsg, esMsg;
    std::stringstream ss;
    ss << hour_;
    enMsg = "Bad argument used to create a TimeSet variable: H(";
    enMsg += ss.str() + ").";
    esMsg = "Argumento erróneo al crear una variable TimeSet: H(";
    esMsg += ss.str() + ").";
    throw TimeSetException(enMsg, esMsg);  
  }
  DeclareClassNewDelete(BTmsHour);
};


//--------------------------------------------------------------------
class BTmsMinute : public BTmsStaticCached<BTmsTemporary, BI_MINUTE>
//--------------------------------------------------------------------
{
private:
  BInt  minute_;
  int CachePosition() const { return(minute_); }

public:
  BTmsMinute(BList* arg) : BTmsStaticCached<BTmsTemporary,BI_MINUTE>(arg)
  {
    minute_=(BInt)Real(Arg(1));
    if((minute_<0) || (minute_>59)) throwBadCreationExc();
  }
  BTmsMinute(int m) : BTmsStaticCached<BTmsTemporary,BI_MINUTE>()
  {
    minute_= m;
    if((minute_<0) || (minute_>59)) throwBadCreationExc();
  }
 ~BTmsMinute() {}
  BDate Successor   (const BDate& dte) const;
  BDate Predecessor (const BDate& dte) const;
  BDate Next        (const BDate& dte, BInt nth=1) const;
  BDate Prev        (const BDate& dte, BInt nth=1) const;
  BInt  Difference  (const BDate& dte1, const BDate& dte2) const;
protected:
  BBool Includes    (const BDate& dte) const;
  void throwBadCreationExc() throw(TimeSetException) 
  {
    std::string enMsg, esMsg;
    std::stringstream ss;
    ss << minute_;
    enMsg = "Bad argument used to create a TimeSet variable: Mi(";
    enMsg += ss.str() + ").";
    esMsg = "Argumento erróneo al crear una variable TimeSet: Mi(";
    esMsg += ss.str() + ").";
    throw TimeSetException(enMsg, esMsg);  
  }
  DeclareClassNewDelete(BTmsMinute);
};

//--------------------------------------------------------------------
class BTmsSecond : public BTmsStaticCached<BTmsTemporary, BI_SECOND>
//--------------------------------------------------------------------
{
private:
  BInt  second_;
  int CachePosition() const { return(second_); }

public:
  BTmsSecond(BList* arg) : BTmsStaticCached<BTmsTemporary,BI_SECOND>(arg)
  {
    second_=(BInt)Real(Arg(1));
    if((second_<0) || (second_>59)) throwBadCreationExc();
  }
  BTmsSecond(int s) : BTmsStaticCached<BTmsTemporary,BI_SECOND>()
  {
    second_= s;
    if((second_<0) || (second_>59)) throwBadCreationExc();
  }
 ~BTmsSecond() {} 
  BDate Successor   (const BDate& dte) const;
  BDate Predecessor (const BDate& dte) const;
  BDate Next        (const BDate& dte, BInt nth=1) const;
  BDate Prev        (const BDate& dte, BInt nth=1) const;
  BInt  Difference  (const BDate& dte1, const BDate& dte2) const;
protected:
  BBool Includes    (const BDate& dte) const;
  void throwBadCreationExc() throw(TimeSetException) 
  {
    std::string enMsg, esMsg;
    std::stringstream ss;
    ss << second_;
    enMsg = "Bad argument used to create a TimeSet variable: S(";
    enMsg += ss.str() + ").";
    esMsg = "Argumento erróneo al crear una variable TimeSet: S(";
    esMsg += ss.str() + ").";
    throw TimeSetException(enMsg, esMsg);  
  }
  DeclareClassNewDelete(BTmsSecond);
};

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
extern BInt SelectAxisDates(BUserTimeSet* axisTms,
                            BUserTimeSet* serTms,
                            const BDate& first,
                            BInt max,
                            BChar* select);

#endif // TOL_BTMSGRA_H

