/* tol_bctimeset.h - TOL Language. BCTimeSet class declaration.
 
   Copyright (C) 2003-2005 - Bayes Decision, SL
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

#ifndef TOL_BCTIMESET_H
#define TOL_BCTIMESET_H 1

#include <vector>
#include <tol/tol_cbtol.h>
#include <tol/tol_bctime.h>
#include <tol/tol_bexc.h>

#if defined(__USE_TC__)

enum tmsType { 
    Abstract=1,
    // Constants:
    CTmsWholeTime, CTmsVoid, CTmsEaster,

    // Basic Datings
    CTmsYear, CTmsMonth, CTmsWeekDay, CTmsDay, CTmsHour, CTmsMinute, 
    CTmsSecond,

    // Finite Datings
    CTmsInside, CTmsOfSerie, CTmsInterval, CTmsCTimesOfSet, CTmsOfSer,

    // Boolean Operators
    CTmsBinary,  CTmsUnion, CTmsIntersection, CTmsDifference,

    // Special Operations: 
    // El antiguo CTmsSuccessor es CTmsIterator
    // CTmsSuccessor es el Sucesor de un conjunto en otro
    CTmsSucc,     CTmsSuccInG,  CTmsSelfSucc,
    CTmsRange,    CTmsRangeInG, CTmsSelfRange,
    CTmsPeriodic,               CTmsSelfPeriodic
};
typedef enum tmsType tmsType;

//--------------------------------------------------------------------
class TOL_API BCTimeSet 
//--------------------------------------------------------------------
{
 protected:  // Accesible only in BCTimeSet and its derived classes
    std::vector<struct tai> instants_;
    granularity granul_;
    tmsType     type_;

 public:
    BCTimeSet() : granul_(SECONDS), type_(Abstract) {}
    ~BCTimeSet() {}

    //! Necessary only to be compatible with BGraObject<Any> template
    virtual void readData() {}

    virtual void* getAttr();
    virtual int getHLimit() const { return -1; };
    virtual bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    virtual bool isSuccableInG(int niter, granularity granul) const;

    //------------------------------
    // (must be Pure) virtual method to serialize objects.
    // Method for read objects is BCTmsRenamed::New(FILE*) (tol_bctmsgra.h)
    virtual int BinWrite(FILE* fil) const;
    //------------------------------

    //------------------------------
    // Includes:
    virtual bool includes(const BCTime&)=0;
    virtual bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    virtual bool includes4SuccInG(const BCTime &tmi, 
				  int niter, granularity granul);
    virtual bool includes4SelfSucc(const BCTime &tmi,int ndisp);

    //------------------------------
    // Successors:
    virtual BCTime succ(const BCTime& tmi);
    virtual BCTime succ4Succ(const BCTime& tmi, BCTimeSet *uctms);
    virtual BCTime succ4SuccInG(const BCTime& tmi, 
				int niter, granularity granul);
    virtual BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    // maximal successor
    BCTime maximalSucc(const BCTime& tmi);

    //------------------------------
    // Predecesors:
    virtual BCTime pred(const BCTime& tmi);
    virtual BCTime pred4Succ(const BCTime& tmi, BCTimeSet *uctms);
    virtual BCTime pred4SuccInG(const BCTime& tmi, 
				int niter, granularity granul);
    virtual BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);

    // maximal predecessor
    BCTime maximalPred(const BCTime& tmi);

    //------------------------------
    bool isCompatibleWith(BCTimeSet& ts, 
			  BCTime firstTmi = BCTime::Unknown(), 
			  BCTime lastTmi  = BCTime::Unknown());

    BCTime next(const BCTime& tmi, int nth=1);
    BCTime prev(const BCTime& tmi, int nth=1);
    
    BCTime nonSmaller(const BCTime& tmi);
    BCTime nonGreater(const BCTime& tmi);
    BCTime getMaximal(const BCTime& tmi);

    tmsType     getType  ()             const { return type_;   };
    void        setType  (tmsType type)       { type_ = type;   };
    granularity getGranul()             const { return granul_; };
    void        setGranul(granularity granul);

    virtual BCTime maxGranulAdjust(const BCTime& tmi, granularity granul);

    //! Method "difference" is obsolete. This new one improves its performance.
    int distance(const BCTime& tmi1, const BCTime& tmi2);

    //! Returns the number of instants of times between two given BCTimes
    virtual int difference(const BCTime& startTmi, const BCTime& stopTmi);

    std::string listOfTimes(const BCTime& startTmi,
			    const BCTime& stopTmi, 
			    std::string separator=", ", int tmsInRow=7);

    //! Returns true when attribute \a instants_ has values
    inline bool hasInstants() const { return !instants_.empty(); }

    //! Calculate and returns attribute \a instants_ between two given BCDates
    std::vector<struct tai>& getInstantsBetween(
	BCTime firstInstant=BCTime::Unknown(),
	BCTime  lastInstant=BCTime::Unknown());

    friend BCTime& operator+=(BCTime& tmi, BCTimeSet* dating)
	{ return(tmi = tmi + dating); }
    friend BCTime& operator-=(BCTime& tmi, BCTimeSet* dating)
	{ return(tmi = tmi - dating); }

    friend BCTime operator+ (const BCTime& tmi, BCTimeSet* dating);
    friend BCTime operator- (const BCTime& tmi, BCTimeSet* dating);

 private:
    //! Method used to calculate \a instants_ between two given julian values
    void setInstantsBetween(granularity granul,
			    const struct tai &firstInstant, 
			    const struct tai &lastInstant);
};

//--------------------------------------------------
// definition of friend functions

inline BCTime operator+ (const BCTime& tmi, BCTimeSet* dating)
{
    BCTime ret = dating->succ(tmi);
    if(ret.getGranul()<tmi.getGranul())
    { ret = dating->maxGranulAdjust(ret, tmi.getGranul()); }
    return ret;
}

inline BCTime operator- (const BCTime& tmi, BCTimeSet* dating)
{ 
    BCTime ret = dating->pred(tmi);
    if(ret.getGranul()<tmi.getGranul())
    { ret = dating->maxGranulAdjust(ret, tmi.getGranul()); }
    return ret;
}

#endif /* __USE_TC__ */
#endif /* TOL_BCTIMESET_H */
