/* tol_btmset.h: Definition of class BTimeSet (tms).
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

#ifndef TOL_BTMSET_H
#define TOL_BTMSET_H 1

#define BHash BArray<BReal>

#include <tol/tol_btimer.h>


//--------------------------------------------------------------------
class TOL_API BTimeSet

/*! Representation of set of dates (year, month, day).
 *  It is a Foundation class to derive more specific timeset.
 *  Each time set must define these methods 
 *   -Includes 
 *   -one of pairs
 *     -(Successor,Predecessor) or 
 *     -(Next,Prev), 
 *     -or both them.
 *   -Cache and FreeCache
 *   -CalcInf and CalcSup
 */
//--------------------------------------------------------------------
{
private:
  BTimer hashStatTimer_;
  BDate  inf_;
  BDate  sup_;
  BDate  currentCalcFirstDate_;
  BDate  currentCalcLastDate_;

protected:
  static double hashUsedKBytes_;

public:
  // Constructors and destructors: tms.cpp
  BTimeSet();
  virtual ~BTimeSet() {}
  virtual BText GetIdentify   () const = 0;
  virtual BBool Contain	      (const BDate&)		 const;
  virtual BDate Successor     (const BDate& dte) const { return(Next(dte)); }
  virtual BDate Predecessor   (const BDate& dte) const { return(Prev(dte)); }
  virtual BDate Next	        (const BDate&, BInt nth=1) const;
  virtual BDate Prev	        (const BDate&, BInt nth=1) const;
  virtual BInt	Difference    (const BDate& d1, const BDate& d2) const;
  virtual BInt	PeriodsBetween(const BDate& d1, const BDate& d2) const;
  virtual void  GetHashBetween(BHash& hash, 
                               BDate first = BDate::DefaultFirst(),
			                         BDate last  = BDate::DefaultLast ()) const;

  virtual const BHash& Cache       () const = 0;
  virtual void  SetCache           (const BHash&) = 0;
  virtual void  FreeCache          ()       = 0;
  virtual int   CacheCallsNumber   () const = 0;
  virtual void  AddCacheCallsNumber() = 0;
  virtual const BDate& BeginCache  () const =0;
  virtual void  PutBeginCache      (const BDate& dte) =0;
  virtual const BDate& EndCache    () const =0;
  virtual void  PutEndCache        (const BDate& dte) =0;
  virtual bool  BuildingCache      () const =0;
  virtual void  PutBuildingCache   (bool b) =0;
  virtual bool  ForcingCache       () const = 0;
  virtual void  PutForcingCache    (bool f) = 0;

  virtual void  PutCache  (const BHash& hash, const BDate& begin, const BDate& end);
  virtual void  ForceCache(BDate first = BDate::DefaultFirst(),
	                         BDate last  = BDate::DefaultLast ()) const;

  virtual const BDate& CurrentCalcFirstDate() const { return(currentCalcFirstDate_); }
  virtual const BDate& CurrentCalcLastDate () const { return(currentCalcLastDate_ ); }
  virtual void	ReadData () {}
  virtual BBool Includes (const BDate&) const=0;
  virtual BDate CalcInf  ()             const { return(BDate::Begin()); }
  virtual BDate CalcSup  ()             const { return(BDate::End  ()); }
  virtual BInt	HashFindDate	   (BReal h, BBool& okHash) const;
  virtual BBool HashIncludes	   (BReal h, BBool& okHash, BInt& pos) const;
  virtual BDate HashSuccessor  	 (BReal h, BBool& okHash, BInt& pos) const;
  virtual BDate HashPredecessor  (BReal h, BBool& okHash, BInt& pos) const;
  virtual void  PutInf (const BDate& dte) { inf_        = dte; }
  virtual void  PutSup (const BDate& dte) { sup_        = dte; }
  virtual const BDate& Inf() const;
  virtual const BDate& Sup() const;

  virtual BText TestCoherence(      int    verbose, 
                              const BDate& first, 
                              const BDate& last,
                                    double maxTimeSim, 
                                    int    numSim, 
                                    int    maxLag);
  virtual BText ListOfDates(BDate d1, BDate d2, BText sep = ", ", BInt jump = 7);
  virtual void CalcHashBetween(BHash& hash, 
                               BDate first = BDate::DefaultFirst(),
			                         BDate last  = BDate::DefaultLast ());
  virtual const BText& TestError(const BText& cod, 
                                 const BText& msg, 
                                 const BText& interval);
  virtual const BText& TestAborted(const BText& cod, 
                                   const BText& msg, 
                                   const BText& interval);
  virtual bool AbortMessageSent(short sign) { return(false); }

  BBool IsCompatibleWith   (const BTimeSet& tms,
	                          BDate first = BDate::DefaultFirst(),
	                          BDate last  = BDate::DefaultLast ()) const;
  BDate FirstNoLess (const BDate& dte) const;
  BDate FirstNoGreat(const BDate& dte) const;
  BBool HasCache() const { return(!BuildingCache() && !BeginCache().IsUnknown()); }

  static int HashSucc(const BHash& hash, BReal h);
  static int HashPred(const BHash& hash, BReal h);
  static double HashUsedKBytes() { return(hashUsedKBytes_); }

  // Miscellaneous:
  friend BDate& operator+=(BDate& date,	      const BTimeSet* dating)
  {
    return(date = date + dating);
  }
  friend BDate& operator-=(BDate& date,	      const BTimeSet* dating)
  {
    return(date = date - dating);
  }

  friend BDate  operator+ (const BDate& date, const BTimeSet* dating)
  {
    return(dating->Successor(date));
  }

  friend BDate  operator- (const BDate& date, const BTimeSet* dating)
  {
    return(dating->Predecessor(date));
  }
};

#endif // BTIMESET
