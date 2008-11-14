/* tol_btmsgrav.h: Various Classes and functions for time set grammar.
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

#ifndef TOL_BTMSGRAV_H
#define TOL_BTMSGRAV_H 1

#include <tol/tol_btmsgra.h>
//#include <exception>


//--------------------------------------------------------------------
class BTmsCached : public BTmsTemporary

/*! Time set that contains all days between iniDate_ and last_.
 */
//--------------------------------------------------------------------
{
protected:
  BCacheInfo cacheInfo_;
public:
  BTmsCached(BList* arg=NULL)
  : BTmsTemporary(arg),
    cacheInfo_()
  {}
 ~BTmsCached() { FreeCache(); }
	const BHash& Cache       () const           { return(cacheInfo_.cache_);  }
  void  SetCache           (const BHash& hash){ cacheInfo_.cache_ = hash; }
  void  FreeCache          ();
  int   CacheCallsNumber   () const           { return(cacheInfo_.cacheCallsNumber_); }
  void  AddCacheCallsNumber()                 { cacheInfo_.cacheCallsNumber_++; }
  const BDate& BeginCache  () const           { return(cacheInfo_.beginCache_); }
  void  PutBeginCache      (const BDate& dte) { cacheInfo_.beginCache_ = dte; }
  const BDate& EndCache    () const           { return(cacheInfo_.endCache_); }
  void  PutEndCache        (const BDate& dte) { cacheInfo_.endCache_   = dte; }
  bool  BuildingCache      () const           { return(cacheInfo_.buildingCache_); }
  void  PutBuildingCache   (bool b)           { cacheInfo_.buildingCache_=b; }
  bool  ForcingCache       () const           { return(cacheInfo_.forzingCache_); }
  void  PutForcingCache    (bool f)           { cacheInfo_.forzingCache_ = f; }
  virtual BDate	SafeSuccessor  (const BDate& dte_) const = 0;
  virtual BDate	SafePredecessor(const BDate& dte_) const = 0;
  virtual BDate	SafeNext	     (const BDate& dte, BInt nth) const 
  {
    return(BTmsTemporary::Next(dte,nth)); 
  };
  virtual BDate	SafePrev	     (const BDate& dte, BInt nth) const
  {
    return(BTmsTemporary::Prev(dte,nth)); 
  };
  BDate	Successor(const BDate& dte) const
  {
    BDate d = SafeSuccessor(dte);
    assert(d>=dte);
    return(d);
  }
  BDate	Predecessor(const BDate& dte) const
  {
    BDate d = SafePredecessor(dte);
    assert(d<=dte);
    return(d);
  }
  BDate	Next(const BDate& dte, BInt nth) const
  {
    BDate d = SafeNext(dte,nth);
    assert((nth>0&&d>=dte)||(nth<0&&d<=dte)||(nth==0));
    return(d);
  }
  BDate	Prev(const BDate& dte, BInt nth) const
  {
    BDate d = SafePrev(dte,nth);
    assert((nth<0&&d>=dte)||(nth>0&&d<=dte)||(nth==0));
    return(d);
  }
};

//--------------------------------------------------------------------
class BTmsInterval : public BTmsCached

/*! Time set that contains all days between iniDate_ and last_.
 */
//--------------------------------------------------------------------
{
private:
  BDate first_;
  BDate last_;
  BUserTimeSet* units_;
public:
  BTmsInterval(BList* arg);
 ~BTmsInterval() {}
  void  FreeCache() 
  { 
    BTmsCached::FreeCache();
    units_->FreeCache();
  }
  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const { return(first_); }
  BDate CalcSup() const { return(last_); }
  DeclareClassNewDelete(BTmsInterval);
};
DeclareTimeSetEvaluator(BTmsInterval);


//--------------------------------------------------------------------
class BTmsOneDay : public BTmsCached

/*! Time set that contains only the one day date_.
 */
//--------------------------------------------------------------------
{
private:
  BDate date_;
  BReal base_;
public:
  BTmsOneDay(BList* arg);
  BTmsOneDay(const BDate& d)
  : BTmsCached(NIL), date_(d) {}
 ~BTmsOneDay() {}

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const { return(date_); }
  BDate CalcSup() const { return(date_); }
  DeclareClassNewDelete(BTmsOneDay);
};
DeclareTimeSetEvaluator(BTmsOneDay);


//--------------------------------------------------------------------
class BTmsYear : public BTmsCached

/*! Time set of all days that are in the year_-th year.
 */
//--------------------------------------------------------------------
{
private:
  BInt  year_;
  BDate first_;
  BDate last_;
public:
  BTmsYear(BList* arg)
  : BTmsCached(arg) 
  { 
    year_  = (BInt)Real(Arg(1)); 
    first_ = BDate(year_, JANUARY,   1);
    last_  = BDate(year_, DECEMBER, 31);
  }
  BTmsYear(BInt y)
  : BTmsCached(), year_(y) 
  {
    first_ = BDate(year_, JANUARY,   1);
    last_  = BDate(year_, DECEMBER, 31);
  }
 ~BTmsYear() {}

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const { return(first_); }
  BDate CalcSup() const { return(last_); }
  DeclareClassNewDelete(BTmsYear);
};
DeclareTimeSetEvaluator(BTmsYear);



//--------------------------------------------------------------------
class BTmsAbortable: public BTmsCached
/*! Time set that could be abortable during Successor or Predecessor
 *  methods to avoid infinite cycles in empty timeset's
 */
//--------------------------------------------------------------------
{
protected:
  static BDat  maxIter_;
  static BDat  outOfRange_;
  bool  abortedMessageSended_;
  BDate abortSuccFirst_;
  BDate abortSuccLast_;
  BDate abortPredFirst_;
  BDate abortPredLast_;
public:
  BTmsAbortable(BList* arg=NULL) 
  : BTmsCached(arg),
    abortedMessageSended_(false),
    abortSuccFirst_(BDate::Unknown()),
    abortSuccLast_ (BDate::Unknown()),
    abortPredFirst_(BDate::Unknown()),
    abortPredLast_ (BDate::Unknown())
  {}
 ~BTmsAbortable() {}

  static BDat GetMaxIter   () { return(maxIter_); }
  static BDat GetOutOfRange() { return(outOfRange_); }
  bool AbortedMessageSended() const { return(abortedMessageSended_); }

protected:
  static BDat& MaxIter   () { return(maxIter_); }
  static BDat& OutOfRange() { return(outOfRange_); }

  const BDate& EnsureNotAbortedSuccessor  (const BDate& dte) const;
  const BDate& EnsureNotAbortedPredecessor(const BDate& dte) const;

  void AbortMessageMaxIter(const BText& function, 
                           const BDate& d1, 
                           const BDate& d2,
                           short sign) const;
  void AbortMessageOutOfRange(const BText& function, 
                              const BDate& d1, 
                              const BDate& d2,
                           short sign) const;

  void AbortSaveInfo(const BDate& d1, 
                     const BDate& d2,
                     short sign) const;

};


//--------------------------------------------------------------------
class BTmsAllUnion: public BTmsCached

/*! Time set that contains all days that are in a_ or b_.
 */
//--------------------------------------------------------------------
{
public:
  BTmsAllUnion(BList* arg) : BTmsCached(arg) {}
 ~BTmsAllUnion() {}

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
  void  FreeCache() 
  { 
    BTmsCached::FreeCache();
    for(int m=0; m<NumArgs(); m++)
    {
      Tms(Arg(m+1))->FreeCache();
    }
  }
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const;
  void CalcHashBetween (BHash& hash,
                        BDate first = BDate::DefaultFirst(),
			                  BDate last  = BDate::DefaultLast ());
  DeclareClassNewDelete(BTmsAllUnion);
};
DeclareTimeSetEvaluator(BTmsAllUnion);


//--------------------------------------------------------------------
class BTmsAllIntersection: public BTmsAbortable

/*! Time set that contains all days that are in a_ and b_.
 */
//--------------------------------------------------------------------
{
public:
  BTmsAllIntersection(BList* arg) : BTmsAbortable(arg) {}
 ~BTmsAllIntersection() {}
  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
  void  FreeCache() 
  { 
    BTmsCached::FreeCache();
    for(int m=0; m<NumArgs(); m++)
    {
      Tms(Arg(m+1))->FreeCache();
    }
  }
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const;
  void CalcHashBetween (BHash& hash,
                        BDate first = BDate::DefaultFirst(),
			                  BDate last  = BDate::DefaultLast ());
  DeclareClassNewDelete(BTmsAllIntersection);
};
DeclareTimeSetEvaluator(BTmsAllIntersection);


//--------------------------------------------------------------------
template<class BTmsBase>
class BTmsBinary: public BTmsBase

/*! Abstract time set class to deriving binary time set operators.
 */
//--------------------------------------------------------------------
{
protected:
  BUserTimeSet* a_;
  BUserTimeSet* b_;
public:
  BTmsBinary(BList* arg)
  : BTmsBase(arg)
  {
    a_=Tms(this->Arg(1));
    b_=Tms(this->Arg(2));
  }
  BTmsBinary(BUserTimeSet* a, BUserTimeSet* b)
  : BTmsBase(),
    a_(a), 
    b_(b) 
  { 
  }
 ~BTmsBinary() 
  {
  }
  void FreeCache() 
  { 
    BTmsCached::FreeCache();
    a_->FreeCache();
    b_->FreeCache();
  }
};



//--------------------------------------------------------------------
class BTmsUnion: public BTmsBinary<BTmsCached>

/*! Time set that contains all days that are in a_ or b_.
 */
//--------------------------------------------------------------------
{
public:
  BTmsUnion(BList* arg) : BTmsBinary<BTmsCached>(arg) {}
  BTmsUnion(BUserTimeSet* a, BUserTimeSet* b) : BTmsBinary<BTmsCached>(a,b) {}
 ~BTmsUnion() {}

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const;
  void CalcHashBetween (BHash& hash,
                        BDate first = BDate::DefaultFirst(),
			                  BDate last  = BDate::DefaultLast ());
  DeclareClassNewDelete(BTmsUnion);
};
DeclareTimeSetEvaluator(BTmsUnion);


//--------------------------------------------------------------------
class BTmsIntersection: public BTmsBinary<BTmsAbortable>

/*! Time set that contains all days that are in a_ and b_.
 */
//--------------------------------------------------------------------
{
public:
  BTmsIntersection(BList* arg) 
  : BTmsBinary<BTmsAbortable>(arg)
  {}
  BTmsIntersection(BUserTimeSet* a, BUserTimeSet* b) 
  : BTmsBinary<BTmsAbortable>(a,b)
  {}
 ~BTmsIntersection() {}

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const;
  void CalcHashBetween (BHash& hash,
                        BDate first = BDate::DefaultFirst(),
			                  BDate last  = BDate::DefaultLast ());
  DeclareClassNewDelete(BTmsIntersection);
};
DeclareTimeSetEvaluator(BTmsIntersection);


//--------------------------------------------------------------------
class BTmsDifference: public BTmsBinary<BTmsAbortable>

/*! Time set that contains all days that are in a_ but not in b_.
 */
//--------------------------------------------------------------------
{
public:
  BTmsDifference(BList* arg) 
  : BTmsBinary<BTmsAbortable>(arg)
  {}
  BTmsDifference(BUserTimeSet* a, BUserTimeSet* b) 
  : BTmsBinary<BTmsAbortable>(a,b)
  {}
 ~BTmsDifference() {}

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const;
  void CalcHashBetween (BHash& hash,
                        BDate first = BDate::DefaultFirst(),
			                  BDate last  = BDate::DefaultLast ());
  DeclareClassNewDelete(BTmsDifference);
};
DeclareTimeSetEvaluator(BTmsDifference);


//--------------------------------------------------------------------
class BTmsPeriodical : public BTmsCached

/*! Time set that contains all days which difference with center_ is
 *  zero module period_, it's to say all days in the form
 *
 *   center_ + period_ * n; where n = ... -3, -2, 1, 0, 1, 2, 3, ...
 */
//--------------------------------------------------------------------
{
private:
  BDate center_;
  BInt	period_;
  BUserTimeSet* units_;
  BBool  centerIsInUnits_;
public:
  BTmsPeriodical(BList* arg);
  BTmsPeriodical(const BDate& c, BInt p, BUserTimeSet* tms)
  : BTmsCached(), center_(c), period_(p), units_(tms) {}
 ~BTmsPeriodical() {}

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
//BInt	Difference  (const BDate& dte1, const BDate& dte2) const
//{ return((dte2.Index()-dte1.Index())/period_); }
  void FreeCache() 
  { 
    BTmsCached::FreeCache();
    units_->FreeCache();
  }
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const;
  void CalcHashBetween (BHash& hash,
                        BDate first = BDate::DefaultFirst(),
			                  BDate last  = BDate::DefaultLast ());
  DeclareClassNewDelete(BTmsPeriodical);
};
DeclareTimeSetEvaluator(BTmsPeriodical);



//--------------------------------------------------------------------
class BTmsSuccessor : public BTmsAbortable

/*! Time set that contains all days whose displacement_-th next day
 *  is in center_.
 */
//--------------------------------------------------------------------
{
private:
  BUserTimeSet* center_;
  BInt		      displacement_;
  BUserTimeSet* units_;
  BTmsIntersection* icu_;
public:
  BTmsSuccessor(BList* arg);
 ~BTmsSuccessor() 
  { 
    if(icu_) { delete icu_; } 
  }

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
  BDate SafeNext	      (const BDate& dte, BInt nth) const;
  BDate SafePrev	      (const BDate& dte, BInt nth) const;
  void FreeCache() 
  { 
    BTmsCached::FreeCache();
    units_->FreeCache();
    center_->FreeCache();
  }
protected:
  BDate CalcInf() const;
  BDate CalcSup() const;
  BBool Includes    (const BDate& dte) const;
  void CalcHashBetween (BHash& hash,
                        BDate first = BDate::DefaultFirst(),
			                  BDate last  = BDate::DefaultLast ());
  DeclareClassNewDelete(BTmsSuccessor);
};
DeclareTimeSetEvaluator(BTmsSuccessor);


//--------------------------------------------------------------------
class BTmsRangeSuc : public BTmsAbortable

/*! Time set union of each BTmsSuccessor(center_, n) time set where
 *
 *      from_ <= n <= until_
 */
//--------------------------------------------------------------------
{
private:
  BUserTimeSet* center_;
  BInt		      from_;
  BInt		      until_;
  BUserTimeSet* units_;
  BTmsIntersection* icu_;
public:
  BTmsRangeSuc(BList* arg);
 ~BTmsRangeSuc() 
  { 
    if(icu_) { delete icu_; } 
  }

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
  BDate SafeNext	      (const BDate& dte, BInt nth) const;
  BDate SafePrev	      (const BDate& dte, BInt nth) const;
  void FreeCache() 
  { 
    BTmsCached::FreeCache();
    units_->FreeCache();
    center_->FreeCache();
  }
protected:
  BBool Includes    (const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const;
  void CalcHashBetween (BHash& hash,
                        BDate first = BDate::DefaultFirst(),
			                  BDate last  = BDate::DefaultLast ());

private:
  void GetChildHashes(const BDate& first, 
                      const BDate& last,
                            BDate& fU,
                            BDate& lU,
                            BDate& fU_,
                            BDate& lU_,
                            BHash& C,
                            BHash& U,
                            int&   US);
  DeclareClassNewDelete(BTmsRangeSuc);
};
DeclareTimeSetEvaluator(BTmsRangeSuc);


//--------------------------------------------------------------------
class BTmsOfSerie : public BTmsAbortable

/*! Time set that contains all days where a time serie is not null
 */
//--------------------------------------------------------------------
{
public:
  BTmsOfSerie(BList* arg) : BTmsAbortable(arg){}
 ~BTmsOfSerie() {}

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
  void GetHashBetween (BHash& hash,
                        BDate first = BDate::Unknown(),
			                  BDate last  = BDate::Unknown()) const;
protected:
  BBool Includes(const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const;
  DeclareClassNewDelete(BTmsOfSerie);
};

DeclareTimeSetEvaluator(BTmsOfSerie);


//--------------------------------------------------------------------
class BTmsDatesOfSet : public BTmsCached

/*! Time set that contains all dates included in a set
 */
//--------------------------------------------------------------------
{
public:
  BTmsDatesOfSet(BList* arg);
  BTmsDatesOfSet(BHash& h);
 ~BTmsDatesOfSet() {}

  BDate SafeSuccessor   (const BDate& dte) const;
  BDate SafePredecessor (const BDate& dte) const;
  void  GetHashBetween (BHash& hash,
                        BDate first = BDate::DefaultFirst(),
			                  BDate last  = BDate::DefaultLast ()) const;
protected:
  BBool Includes(const BDate& dte) const;
  BDate CalcInf() const;
  BDate CalcSup() const;
  DeclareClassNewDelete(BTmsDatesOfSet);
};
DeclareTimeSetEvaluator(BTmsDatesOfSet);


#endif // TOL_BTMSGRAV_H


