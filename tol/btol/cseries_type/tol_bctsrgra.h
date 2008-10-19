/* tol_btsrgra.h: TOL Language. Classes and functions for time serie grammar.

   Copyright (C) 2003,2005 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BCTSRGRA_H
#define TOL_BCTSRGRA_H 1

#include <tol/tol_cbtol.h>
#include <tol/tol_bctmsgra.h>
#include <tol/tol_bcseries.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bdb.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserCTimeSeries BGraObject<BCTimeSeries>

//--------------------------------------------------------------------
class BUserCTimeSeriesDo: public BUserCTimeSeries
{
 public:
    BUserCTimeSeriesDo(const BText& name="", const BText& description="")
	: BGraObject<BCTimeSeries>(name,description) {}
    
    BText Info() const;
    BText Dump() const { return(Identify()); };
    void Do ();
    BSyntaxObject* CopyContens ();
    
    void PutContens(BUserCTimeSeriesDo* ser);
};

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
inline BUserCTimeSeries* CTsr (BSyntaxObject* arg)
{
    BUserCTimeSeries* uCTsr = NIL;
    if(arg && (arg->Grammar()==BUserCTimeSeries::OwnGrammar()))
    { uCTsr = (BUserCTimeSeries*)arg; }
    return(uCTsr);
}

inline BUserCTimeSeries* CTsr (BCore* arg)
{ return(CTsr((BSyntaxObject*) arg)); }

inline BUserCTimeSeries* CTsr(const BText& expression)
{ return(CTsr(BUserCTimeSeries::OwnGrammar()->EvaluateExpr(expression))); }

inline BUserCTimeSeries& CTsrR(BSyntaxObject* arg)
{ return(*CTsr(arg)); }

//--------------------------------------------------------------------
/*! Base class for each operator which result is a time CSeries.
 */
class BCTsrPrimary: public BUserCTimeSeriesDo
{
 public:
    // Constructors and destructors
    BCTsrPrimary(const BText&  name    = "",
		 const BText&  title   = "",
		 BUserCTimeSet* dating = NIL,
		 const BCTime&  first  = BCTime::Unknown(),
		 const BCTime&  last   = BCTime::Unknown());

    BCTsrPrimary(const BText&   name,
		 const BText&   title,
		 BUserCTimeSet* dating,
		 const BCTime&  first,
		 const BCTime&  last,
		 const BData&   data);
    
    BCTsrPrimary(const BText&   name,
		 const BText&   title,
		 BUserCTimeSet* dating,
		 const BCTime&  first,
		 const BData&   data);
    
    // Access
    void ReadData();
  BDat GetDat(const BCTime& tmi);
};


//--------------------------------------------------------------------
/*! CSeries that has been readed from a Bayes CSeries Index file.
 */
class BCTsrBSI : public BCTsrPrimary
{
 private:
    BText bsdFileName_;
    int	offset_;

 public:
    BCTsrBSI(const BText& name, const BText& bsdFileName, const int offset)
	: BCTsrPrimary(name), bsdFileName_(bsdFileName), offset_(offset)
	{}

    const BText& Description() const;
    void ReadData();
    static void	WriteFile(const BText& bsiFileName, BList* serList);
    static BList* ReadFile(const BText& bsiFileName);
    
};

//--------------------------------------------------------------------
class BCTsrRenamed : public BRefObject<BUserCTimeSeriesDo>
{
 public:
    BCTsrRenamed(const BText& name, BSyntaxObject* d)
	: BRefObject<BUserCTimeSeriesDo>(name, d) 
	{}

    // Implementation: ctsrgra.cpp
    BUserCTimeSeriesDo* GetResult() const
	{
	    if(Released()) { return(NIL); }
	    else { return(BRefObject<BUserCTimeSeriesDo>::GetResult()); }
	}

    static BSyntaxObject* New(const BText& name, BSyntaxObject* d)
	{ return((BSyntaxObject*)(new BCTsrRenamed(name, d))); }

    BDat operator[] (const BCTime& tmi)
	{ return(GetDat(tmi)); }

    BDat GetDat(const BCTime& tmi)
	{
	    if(GetResult()) { return((*GetResult())[tmi]); }
	    else	    { return(BUserCTimeSeriesDo::operator [](tmi)); }
	}

    BUserCTimeSet* Dating() const
	{
	    if(GetResult()) { return(GetResult()->Dating()); }
	    else	    { return(BUserCTimeSeriesDo::Dating()); }
	}

    BCTime FirstCTime()
	{
	    if(GetResult()) { return(GetResult()->FirstCTime()); }
	    else	    { return(BUserCTimeSeriesDo::FirstCTime()); }
	}

    BCTime LastCTime()
	{
	    if(GetResult()) { return(GetResult()->LastCTime()); }
	    else	    { return(BUserCTimeSeriesDo::LastCTime()); }
	}
    
    const BData& Data() const
	{
	    if(GetResult()) { return(GetResult()->Data()); }
	    else	    { return(BUserCTimeSeriesDo::Data()); }
	}
    
    void GetData(BData& dta, const BCTime& first,const BCTime& last, int len=0)
	{
	    if(GetResult()) { GetResult()->GetData(dta,first,last,len); }
	    else	    { BUserCTimeSeries::GetData(dta,first,last,len); }
	}
    
    enum ctms_type getDatingType() { 
	if(GetResult()) return(GetResult()->getDatingType());
	else return CDATING_FIXED;
    }
};

//--------------------------------------------------------------------
/*! Base class for each operator which result is a time CSeries.
 */
class BCTsrTemporary: public BTmpObject<BUserCTimeSeriesDo>
{
 public:
    BCTsrTemporary(BList* arg=NULL)
	: BTmpObject<BUserCTimeSeriesDo>(arg) {}

   ~BCTsrTemporary() {}
    
    BDat LogicShort(bool(*comp)(const BDat&,const BDat&),
		    const BCTime& tmi) const;
    void ReCalc ();
};


//--------------------------------------------------------------------
/*! Casting from real.Really, a real number is not a CSeries by self
 *  because it hasn't any dating, but it can operate with every CSeries
 *  in its own dating returning the same value for each date of this
 *  dating.
 */
class BCTsrFromReal : public BCTsrTemporary
{
 public:
    BCTsrFromReal(BList* arg);
    BCTsrFromReal(const BDat& dat);
    
    BDat operator[] (const BCTime& tmi);
    BDat GetDat	  (const BCTime& tmi);
};

#endif // __USE_TC__
#endif // BTIMESERIEGRAMMAR


