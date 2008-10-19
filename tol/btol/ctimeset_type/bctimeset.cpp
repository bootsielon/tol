/* bctimeset.cpp - TOL Language. BCTimeSet class implementation.
 
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

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_bctimeset.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
bool BCTimeSet::includes4Succ(const BCTime &tmi, BCTimeSet *uctms)
{ return includes(tmi); }

bool BCTimeSet::includes4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{ 
    bool belongs = false;

    BCTime aux(tmi, getGranul());
    BCTime pivot = this->pred(aux);
    BCTime iniIT(pivot, granul), endIT(iniIT);

    do
    {
	iniIT = pivot;
	iniIT.setGranul(granul);
	iniIT.nSuccessor(niter, granul);
	
	endIT = iniIT;
	endIT.nSuccessor( 1, getGranul());
	endIT.nSuccessor(-1, granul);
	
	if(iniIT.startsBeforeEQ(tmi) && endIT.endsAfterEQ(tmi)) 
	    belongs = true;

	pivot = this->succ(pivot);
    }
    while(!belongs && (tmi>endIT));

    return belongs;
}

bool BCTimeSet::includes4SelfSucc(const BCTime &tmi, int ndisp)
{ return includes(tmi); }


//--------------------------------------------------------------------
/*! Returns a BCTime representing the nth successor of BCTime given as
 * argument. Parameter \a nth can be positive or negative.
 * Parameter \a tmi and the BCTime returned has the same granularity.
 */
BCTime BCTimeSet::succ(const BCTime &tmi)
{
    BCTime nextTime(tmi);
    nextTime.successor(1);
    return(nextTime);
}

BCTime BCTimeSet::pred(const BCTime &tmi)
{
    BCTime prevTime(tmi);
    prevTime.successor(-1);
    return(prevTime);
}

BCTime BCTimeSet::succ4Succ(const BCTime &tmi, BCTimeSet *uctms)
{
    return tmi;
}

BCTime BCTimeSet::pred4Succ(const BCTime &tmi, BCTimeSet *uctms)
{
    return tmi;
}

BCTime BCTimeSet::succ4SuccInG(const BCTime &tmi, 
			       int niter, granularity granul)
{
    BCTime auxtmi(tmi, granul);
    auxtmi.successor(1);
    return auxtmi;
}

BCTime BCTimeSet::pred4SuccInG(const BCTime &tmi, 
			       int niter, granularity granul)
{
    BCTime auxtmi(tmi, granul);
    auxtmi.successor(-1);
    return auxtmi;
}

BCTime BCTimeSet::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime auxtmi(tmi);
    auxtmi.successor(1);
    return auxtmi;
}

BCTime BCTimeSet::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime auxtmi(tmi);
    auxtmi.successor(-1);
    return auxtmi;
}

void* BCTimeSet::getAttr()                                    { return 0;     }
bool  BCTimeSet::isVoidIntersectedWith(BCTimeSet *ctms) const { return false; }

bool  BCTimeSet::isSuccableInG(int niter,granularity granul) const 
{return true;}

int BCTimeSet::BinWrite(FILE* fil) const { return 0; }

//--------------------------------------------------------------------
/*! Returns the maximal successor of a given maximal IT.
 */
BCTime BCTimeSet::maximalSucc(const BCTime& tmi)
{
    BCTime ret = getMaximal(tmi);
    ret = this->succ(ret);
    return ret;
}

/*! Returns the maximal predecessor of a given maximal IT.
 */
BCTime BCTimeSet::maximalPred(const BCTime& tmi)
{
    BCTime ret = getMaximal(tmi);
    ret = this->pred(ret);
    return ret;
}

//--------------------------------------------------------------------
BCTime BCTimeSet::maxGranulAdjust(const BCTime& tmi, granularity granul)
{
    granularity granux = granul;
    BCTime aux(tmi, granux);

    while(!includes(aux))
    {
	granux = granularity (((int)granux) - 1);
	aux = tmi;

	if(granux==tmi.getGranul()) break;
	else aux.setGranul(granux);
    }
    return aux;
}

//--------------------------------------------------------------------
/*! Returns true if a given CTimeSet \a ts is compatible with this CTimeSet
 * between two given instants of time: \a firstTmi and \a lastTmi. Both
 * arguments, \a firstTmi and \a lastTmi, are optional.
 * If this BCTimeSet and \a ts produce same instants of time, returns true,
 * in other case returns false.
 */
bool BCTimeSet::isCompatibleWith(BCTimeSet& ts,
				 BCTime firstTmi, BCTime lastTmi)
{
    if(!firstTmi.isNormal()) firstTmi = BCTime::Now();
    if( !lastTmi.isNormal()) 
	for(int i=0; i<10; i++) lastTmi = succ(firstTmi);
    
    BCTime tmi1 =    nonGreater(firstTmi);
    BCTime tmi2 = ts.nonGreater(firstTmi);
    
    while(tmi1 <= lastTmi) {
	tmi1 =    succ(tmi1);
	tmi2 = ts.succ(tmi2);
	if(tmi1!=tmi2) return false;
    }
    
    return true;
}

//--------------------------------------------------------------------
BCTime BCTimeSet::next(const BCTime& tmi, int nth)
{
    if(nth<0) return prev(tmi, -nth);
    else
    {
	BCTime ret(tmi);
	
	for(int i=0; i<nth; i++)
	{
	    ret = succ(ret);
	    if(ret.getGranul() < tmi.getGranul())
		ret = maxGranulAdjust(ret, tmi.getGranul());
	}

	return ret;
    }
}

BCTime BCTimeSet::prev(const BCTime& tmi, int nth)
{
    if(nth<0) return next(tmi, -nth);
    else
    {
	BCTime ret(tmi);

	for(int i=0; i<nth; i++)
	{
	    ret = pred(ret);
	    if(ret.getGranul() < tmi.getGranul())
		ret = maxGranulAdjust(ret, tmi.getGranul());
	}

	return ret;
    }
}

//--------------------------------------------------------------------
/*! returns the first BCTime object representing the next instant of time
 * non smaller than parameter BCTime \a tmi given.
 * If BCTime object given is included, then returns the same object. In
 * other case, returns the successor.
 */
BCTime BCTimeSet::nonSmaller(const BCTime& tmi)
{
    if(includes(tmi)) return tmi;
    else return succ(tmi);
}

//--------------------------------------------------------------------
/*! returns the first BCTime object representing the previous instant of 
 * time non greater than parameter BCTime \a tmi given.
 * If BCTime object given is included, then returns the same object. In
 * other case, returns the predecessor.
 */
BCTime BCTimeSet::nonGreater(const BCTime& tmi)
{
    if(includes(tmi)) return tmi;
    else return pred(tmi);
}

//--------------------------------------------------------------------
BCTime BCTimeSet::getMaximal(const BCTime& tmi)
{
    BCTime ret(tmi);

    if(tmi.getGranul() != YEARS)
    {
	int g = static_cast<int>(tmi.getGranul());
	BCTime growingIT(tmi, static_cast<granularity>(++g));

	while(includes(growingIT))
	{
	    ret = growingIT;
	    g++;
	    if(static_cast<granularity>(g)==UNK) break;
	    growingIT.setGranul(static_cast<granularity>(g)); 
	}
    }
    
    return ret;
}

//--------------------------------------------------------------------
/*! Set a new granularity for this CTimeSet
 */
void BCTimeSet::setGranul(granularity granul)
{
    if((granul>=SECONDS) && (granul<=YEARS)) {
	granul_ = granul;
    }
}

//--------------------------------------------------------------------
int BCTimeSet::distance(const BCTime& tmi1, const BCTime& tmi2)
{
    if(tmi1>tmi2) return -distance(tmi2, tmi1);

    // check whether tmi1 and tmi2 are in the same interval, out of (this)
    if(tmi2<succ(tmi1)) return 0;

    BCTime max1 = getMaximal(tmi1);
    BCTime max2 = getMaximal(tmi2);

    if((max1==max2)) return tmi1.distanceTo(tmi2);

    bool reached = false;
    int dist = 0;

    if(includes(tmi1)) dist = 1;

    if(tmi1!=max1) 
	dist += tmi1.distanceTo(max1.getEndPointIn(tmi1.getGranul()));

    BCTime ini, end, next(max1);
    
    while(!reached)
    {
	next = maximalSucc(next);
	ini = next.getStartPointIn(tmi1.getGranul());
	
	if(next>=max2)
	{
	    reached = true;
	    if(next==max2) dist = dist + ini.distanceTo(tmi2);
	    break;
	}
	
	end = next.getEndPointIn(tmi1.getGranul());
	dist = dist + ini.distanceTo(end) + 1;
    }

    return dist;
}

//--------------------------------------------------------------------
/*! Returns the number of instants of time between two given BCTime.
 */
int BCTimeSet::difference(const BCTime& startTmi, const BCTime& stopTmi)
{
    if(!(startTmi.isNormal() && stopTmi.isNormal())) return 0;
    if(startTmi == stopTmi)                          return 0;
    if(startTmi>stopTmi) return -difference(stopTmi, startTmi);

//    BCTime validStart = nonSmaller(startTmi);
//    BCTime validStop  = nonGreater(stopTmi);
    BCTime validStart = startTmi;
    BCTime validStop  = stopTmi;

    int diff = 0;
    bool ok = false;

    if(validStart >= validStop) ok = true;
    else if(hasInstants()) {
	struct tai startTai = validStart.getTai();
	struct tai  stopTai = validStop.getTai();
	std::vector<struct tai>::const_iterator cur_tai;	
	int idx = 0;
	int startIdx = instants_.size();
	int  stopIdx = instants_.size();

	for(cur_tai = instants_.begin(); 
	    cur_tai != instants_.end(); 
	    cur_tai++, idx++) 
	{
	    if(!(tai_less(cur_tai,&startTai)) && 
	       !(tai_less(&startTai,cur_tai))) startIdx = idx;
	    if(!(tai_less(cur_tai,&stopTai)) && 
	       !(tai_less(&stopTai,cur_tai)))  stopIdx = idx;
	    if((startIdx<instants_.size()) && ( stopIdx<instants_.size())) 
	    {
		diff = stopIdx - startIdx;
		ok = true;
		break;
	    }
	}
    }

    if(!ok) {
	setInstantsBetween(startTmi.getGranul(),
			   startTmi.getTai(), stopTmi.getTai());
	diff = instants_.size() - 1;
    }

    return diff;
}

//--------------------------------------------------------------------
std::string BCTimeSet::listOfTimes(const BCTime& startTmi,
				   const BCTime& stopTmi, 
				   std::string separator, int tmsInRow)
{
    if(!startTmi.isNormal() || !stopTmi.isNormal() || startTmi>stopTmi)
	return std::string("");

    BCTime tmi = nonSmaller(startTmi);
    std::string str = "(";

    for(int i=0; tmi<stopTmi; i++) {
	if(i>0) str += separator;      // put a separator between values
	str << tmi;
	if((tmsInRow>0) && !(i%tmsInRow)) str += "\n";
	tmi = succ(tmi);
    }
    str += ")";

    return str;
}

//--------------------------------------------------------------------
/*! Calculates attribute \a instants_ and returns it as a vector<struct tai>,
 *  a C++ Standard Template Library Vector with TAI64 values from
 * BCTime \a firstInstant and BCTime \a lastInstant.
 * \sa settInstantsBetween
 */
std::vector<struct tai>& BCTimeSet::getInstantsBetween(BCTime firstInstant, 
						       BCTime lastInstant)
{
    if(firstInstant > lastInstant) {
	instants_.clear(); // empty vector
	return instants_;
    }

    // Adjust to a normal situation
    if(!firstInstant.isNormal()) firstInstant = BCTime::Now();
    if (!lastInstant.isNormal())
	for(int i=0; i<20; i++) lastInstant = succ(firstInstant);

    firstInstant = nonSmaller(firstInstant);
    lastInstant  = nonGreater(lastInstant);

    if(firstInstant>lastInstant) 
	instants_.clear();
    else 
	setInstantsBetween(firstInstant.getGranul(),
			   firstInstant.getTai(), 
			   lastInstant.getTai());

    return instants_;
}

//--------------------------------------------------------------------
/*! Calculates attribute \a instants_,
 *  a C++ STL Vector with struct tai values calculated from \a firstTai
 *  parameter and \a lastTai parameter, two struct tai values representing
 *  valid times. This method is called after getInstantsBetween
 * \sa getInstantsBetween
 */
void BCTimeSet::setInstantsBetween(granularity granul,
				   const struct tai &firstTai, 
				   const struct tai  &lastTai)
{
    struct tai tai;
    BCTime tmi(firstTai, granul);
    
    instants_.clear();                 // erase all elements
    tai = firstTai;
    while(!tai_less(&lastTai, &tai))
    {
	instants_.push_back(tai);
	tmi = succ(tmi);
	tai = tmi.getTai();
    }
}

#endif /* __USE_TC__ */
