/* tol_bctmsfnit.h - CTimeSet Tol Data Type creation.
   Finite CTimeSets (cInside, cBetween, cCTimesOfSet)
 
   Copyright (C) 2005 - Bayes Decision, SL
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

#ifndef TOL_BCTMSNOPS_H
#define TOL_BCTMSNOPS_H 1

#include <tol/tol_bctime.h>
#include <tol/tol_cbtol.h>
#include <tol/tol_bctimeset.h>
#include <tol/tol_bctmsgra.h>
#include <tol/tol_bctmigra.h>
#include <tol/tol_bsetgra.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
/*! CTimeSet that represents an specific CDate
 */
class BCTmsInside: public BCTmsTemporary
{
 private:
    BCTime tmi_;

 public:
    BCTmsInside(BList* arg)
	: BCTmsTemporary(arg)
	{
	    tmi_ = CTime(Arg(1));
	    setGranul(tmi_.getGranul());
	    setType(CTmsInside);
	}
    
    BCTmsInside(const BCTime& tmi)
	: BCTmsTemporary(NIL), tmi_(tmi)
	{
	    setGranul(tmi_.getGranul());
	    setType(CTmsInside);
	}

    ~BCTmsInside() {}

    void* getAttr() { return &tmi_; }

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);
    
    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
};
DeclareCTimeSetEvaluator(BCTmsInside);

//--------------------------------------------------------------------
/*! CTimeSet that represents an Interval from a BCTime to another
 */
class BCTmsInterval: public BCTmsTemporary
{
 private:
    BCTime iniTmi_, endTmi_;

 public:
    BCTmsInterval(BList* arg)
	: BCTmsTemporary(arg)
	{
	    iniTmi_ = CTime(Arg(1));
	    endTmi_ = CTime(Arg(2));

	    if(!iniTmi_.startsBeforeEQ(endTmi_))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong parameter values: " << endl 
		      << "from: " << iniTmi_  << ", to: " << endTmi_ 
		      << endl 
		      << "\"from\" must go before \"to\".";
		ES_ss << "Parámetros erróneos: " << endl 
		      << "from: " << iniTmi_  << ", to: " << endTmi_ 
		      << endl 
		      << "\"from\" debe preceder a \"to\".";
		throw BException(EN_ss.str(), ES_ss.str());
	    }

	    if(iniTmi_.getGranul() < endTmi_.getGranul())
		setGranul(iniTmi_.getGranul());
	    else setGranul(endTmi_.getGranul());

	    iniTmi_ = BCTime::fillNew(getGranul(), LV, iniTmi_);
	    endTmi_ = BCTime::fillNew(getGranul(), HV, endTmi_);

	    setType(CTmsInterval);
	}
    
    ~BCTmsInterval() {}

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
};
DeclareCTimeSetEvaluator(BCTmsInterval);

//--------------------------------------------------------------------
/*! CTimeSet that represents an specific CDate
 */
class BCTmsCTimesOfSet: public BCTmsTemporary
{
 private:
    BSet& set_;
    BCTime lLim_, hLim_;
    // Attributes for use only with SuccInG
    std::vector<BCTime> iniITs_;
    std::vector<BCTime> endITs_;

 public:
    BCTmsCTimesOfSet(BList* arg)
	: BCTmsTemporary(arg), set_(Set(Car(arg)))
	{
	    setGranul(YEARS);

	    BCTime setTmi;
	    lLim_ = BCTime::End();
	    hLim_ = BCTime::Begin();

	    for(int i=1; i<=set_.Card(); i++)
	    {
		setTmi = CTime(set_[i]);

		if(setTmi.startsBefore(lLim_)) { lLim_ = setTmi; }
		if(setTmi.endsAfter(hLim_))    { hLim_ = setTmi; }
	    
		if(getGranul() > setTmi.getGranul())
		    setGranul(setTmi.getGranul());
	    }
	    
	    setType(CTmsCTimesOfSet);
	}
    
    ~BCTmsCTimesOfSet() {}

    void loadVectors(int niter, granularity granul);

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
};
DeclareCTimeSetEvaluator(BCTmsCTimesOfSet);

#endif /* __USE_TC__ */
#endif /* TOL_BCTMSNOPS_H */
