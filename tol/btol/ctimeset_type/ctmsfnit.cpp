/* ctmsnops.cpp - CTimeSet Tol Data Type native function implementation.
                   Normal Operations (not Boolean, Special or Basic Datings)
  
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

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_bctmsfnit.h>
#include <tol/tol_bctmsbool.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsInside, "Inside", 1, 1, "CTime", "(CTime tmi)",
	  I2("Returns the CTimeSet composed by the only one given instant "
	     "of time.",
	     "Devuelve el CTimeSet formado por el único instante de tiempo "
	     "dado."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsInside::includes(const BCTime& tmi)
{ 
    if(tmi.getGranul()>getGranul()) return false;
    else return tmi_.includes(tmi); 
}

bool BCTmsInside::includes4SuccInG(const BCTime &tmi, 
				   int niter, granularity granul)
{
    bool belongs = false;

    tmi_.nSuccessor(niter, granul);
    if(tmi_.startsBeforeEQ(tmi) && tmi_.endsAfterEQ(tmi)) belongs = true;
    tmi_.nSuccessor(-niter, granul);

    return belongs;
}

bool BCTmsInside::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldTmi(tmi_);
    tmi_.nSuccessor(ndisp, getGranul());
    bool ret = includes(tmi);
    tmi_ = oldTmi;
    
    return ret;
}

BCTime BCTmsInside::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    { 
	if(tmi==BCTime::Begin()) return BCTime(tmi_);
	if(tmi==BCTime::End()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret) && ret.isNormal()) 
    {
	if(tmi_ < ret) ret = BCTime::End();

	if(tmi_ > tmi) 
	    if(tmi_.getGranul() > tmi.getGranul())
		return BCTime::fillNew(tmi.getGranul(), LV, tmi_);
	    else return BCTime(tmi_);
    }

    return ret;
}

BCTime BCTmsInside::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    { 
	if(tmi==BCTime::End()) return BCTime(tmi_);
	if(tmi==BCTime::Begin()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);

    if(!includes(ret) && ret.isNormal()) 
    {
	if(tmi_ > ret) ret = BCTime::Begin();

	if(tmi_ < tmi) 
	    if(tmi_.getGranul() > tmi.getGranul())
		return BCTime::fillNew(tmi.getGranul(), HV, tmi_);
	    else return BCTime(tmi_);
    }

    return ret;
}

BCTime BCTmsInside::succ4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    if(granul==getGranul()) return succ4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    ret.successor(1);

    if(!includes4SuccInG(ret, niter, granul)) 
    {
	BCTime oldTmi(tmi_);

	tmi_.nSuccessor(niter, granul);
	tmi_.setGranul(granul);
	ret.setGranul(granul);

	granularity g = granul;
	if(tmi.isNormal() && (tmi.getGranul() < granul)) g = tmi.getGranul();

	BCTime startTmi = BCTime::fillNew(g, LV, tmi_);
	BCTime endTmi(tmi_);
	endTmi.nSuccessor(1, getGranul());
	endTmi.nSuccessor(-1, granul);
	endTmi = BCTime::fillNew(g, HV, endTmi);

	if(ret.endsAfter(endTmi))           ret = BCTime::End();
	else if(ret.startsBefore(startTmi)) ret = startTmi;

	tmi_ = oldTmi;
    }

    return ret;
}

BCTime BCTmsInside::pred4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    if(granul==getGranul()) return pred4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes4SuccInG(ret, niter, granul)) 
    {
	BCTime oldTmi(tmi_);

	tmi_.nSuccessor(niter, granul);
	tmi_.setGranul(granul);

	ret = tmi;
	ret.setGranul(granul);
	ret.successor(-1);

	granularity g = granul;
	if(tmi.isNormal() && (tmi.getGranul() < granul)) g = tmi.getGranul();

	BCTime startTmi = BCTime::fillNew(g, LV, tmi_);
	BCTime endTmi(tmi_);
	endTmi.nSuccessor(1, getGranul());
	endTmi.nSuccessor(-1, granul);
	endTmi = BCTime::fillNew(g, HV, endTmi);

	if(ret.startsBefore(startTmi)) ret = BCTime::Begin();
	else if(ret.endsAfter(endTmi)) ret = endTmi;
	
	tmi_ = oldTmi;
    }

    return ret;
}

BCTime BCTmsInside::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldTmi(tmi_);
    tmi_.nSuccessor(ndisp, getGranul());
    BCTime ret = succ(tmi);
    tmi_ = oldTmi;
    
    return ret;
}

BCTime BCTmsInside::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldTmi(tmi_);
    tmi_.nSuccessor(ndisp, getGranul());
    BCTime ret = pred(tmi);
    tmi_ = oldTmi;
    
    return ret;
}

bool BCTmsInside::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    if(ctms->getType() == CTmsInside)
    {
	BCTime* tmi = (BCTime*) ctms->getAttr();
	if((*tmi) != tmi_) ret = true;
    }
    else if(ctms->getType() == CTmsYear)
    {
	BCTime* year = (BCTime*) ctms->getAttr();
	if(((*year).year()) != tmi_.year()) ret = true;	
    }
    else if(ctms->getType() == CTmsIntersection)
    {
	BCTmsBinary* ctmsBinary = static_cast<BCTmsBinary*>(ctms);
	if(isVoidIntersectedWith(ctmsBinary->getCTmsA()) ||
	   isVoidIntersectedWith(ctmsBinary->getCTmsB())) ret = true;
    }

    return ret;
}

bool BCTmsInside::isSuccableInG(int niter, granularity granul) const
{
    switch(granul) 
    {
	case MONTHS:
	    if(niter>11) return false;
	    else return true;

	case DAYS:
	    if(BCTime::isLeapYear(tmi_.year())) {
		if(niter>365) return false; 
		else return true;
	    } else {
		if(niter>364) return false;
		else return true;
	    }

	case HOURS:
	    if(BCTime::isLeapYear(tmi_.year())) {
		if(niter>8783) return false; 
		else return true;
	    } else {
		if(niter>8759) return false; 
		else return true;
	    }

	case MINUTES:
	    if(BCTime::isLeapYear(tmi_.year())) {
		if(niter>527039) return false; 
		else return true;
	    } else {
		if(niter>525599) return false; 
		else return true;
	    }

	case SECONDS:
	    if(BCTime::isLeapYear(tmi_.year())) {
		if(niter>31622399) return false;
		else return true;
	    } else {
		if(niter>31535999) return false;
		else return true;
	    }

	default: return true;
    }
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsInterval, "Interval", 2, 2, "CTime CTime", 
	  "(CTime from, CTime to)",
	  I2("Returns the CTimeSet of all instants of time between \"from\" "
	     "and \"to\". The base granularity for all instants is taken "
	     "from the granularity of the second parameter.\n\n"
	     "Example:\n"
	     "CTimeSet daysOfYear2005 = In(y2005, y2005m12d31);\n",
	     "Devuelve el CTimeSet formado por todos los instantes de tiempo "
	     "entre \"from\" y \"to\". La granularidad básica de todos los "
	     "instantes de tiempo se toma de la granularidad del segundo "
	     "parámetro\n\n. Ejemplo:\n"
	     "CTimeSet diasDelAnyo2005 = In(y2005, y2005m12d31);\n"),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsInterval::includes(const BCTime& tmi)
{ 
    if(iniTmi_.startsBeforeEQ(tmi) && endTmi_.endsAfterEQ(tmi)) 
	return true;
    return false;
}

bool BCTmsInterval::includes4SuccInG(const BCTime &tmi, 
				  int niter, granularity granul)
{
    if(granul==getGranul()) return includes4SelfSucc(tmi, niter);
 
    BCTime iniTmi = BCTime::fillNew(granul, LV, iniTmi_);
    BCTime endTmi = BCTime::fillNew(granul, HV, endTmi_);
    iniTmi.nSuccessor(niter, granul);
    endTmi.nSuccessor(niter, granul);
    
    if(iniTmi.startsBeforeEQ(tmi) && endTmi.endsAfterEQ(tmi)) 
	return true;
    return false;
}

bool BCTmsInterval::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime iniTmi = BCTime::fillNew(getGranul(), LV, iniTmi_);
    BCTime endTmi = BCTime::fillNew(getGranul(), HV, endTmi_);
    iniTmi.nSuccessor(ndisp, getGranul());
    endTmi.nSuccessor(ndisp, getGranul());

    if(iniTmi.startsBeforeEQ(tmi) && endTmi.endsAfterEQ(tmi)) 
	return true;
    return false;
}

BCTime BCTmsInterval::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) 
	return BCTime(iniTmi_, getGranul());
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);
    
    if(ret.isNormal())
	if(!includes(ret)) 
	{
	    ret.setGranul(getGranul());

	    if(ret.endsAfter(endTmi_)) ret = BCTime::End();
	    
	    if(ret.startsBefore(iniTmi_)) 
		if(getGranul() > tmi.getGranul())
		    ret = BCTime::fillNew(tmi.getGranul(), LV, iniTmi_);
		else ret = BCTime(iniTmi_, getGranul());
	}
	else if(getGranul()<ret.getGranul()) 
	    ret = BCTime::fillNew(getGranul(), LV, ret);
    
    return ret;
}

BCTime BCTmsInterval::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) 
	return BCTime(endTmi_, getGranul());
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(ret.isNormal())
	if(!includes(ret)) 
	{
	    ret = tmi;
	    ret.setGranul(getGranul());
	    ret.successor(-1);

	    if(ret.startsBefore(iniTmi_)) ret = BCTime::Begin();
	    
	    if(ret.endsAfter(endTmi_)) 
		if(getGranul() > tmi.getGranul())
		    ret = BCTime::fillNew(tmi.getGranul(), HV, endTmi_);
		else ret = BCTime(endTmi_, getGranul());
	}
	else if(getGranul()<ret.getGranul()) 
	    ret = BCTime::fillNew(getGranul(), HV, ret);
    
    return ret;
}

BCTime BCTmsInterval::succ4SuccInG(const BCTime &tmi, 
				int niter, granularity granul)
{
    BCTime oldIniTmi(iniTmi_);
    BCTime oldEndTmi(endTmi_);
    granularity g = getGranul();

    iniTmi_ = BCTime::fillNew(granul, LV, iniTmi_);
    endTmi_ = BCTime::fillNew(granul, HV, endTmi_);
    iniTmi_.nSuccessor(niter, granul);
    endTmi_.nSuccessor(niter, granul);
    setGranul(granul);

    BCTime ret = succ(tmi);

    setGranul(g);
    iniTmi_ = oldIniTmi;
    endTmi_ = oldEndTmi;
    
    return ret;
}

BCTime BCTmsInterval::pred4SuccInG(const BCTime &tmi, 
				int niter, granularity granul)
{
    BCTime oldIniTmi(iniTmi_);
    BCTime oldEndTmi(endTmi_);
    granularity g = getGranul();

    iniTmi_ = BCTime::fillNew(granul, LV, iniTmi_);
    endTmi_ = BCTime::fillNew(granul, HV, endTmi_);
    iniTmi_.nSuccessor(niter, granul);
    endTmi_.nSuccessor(niter, granul);
    setGranul(granul);

    BCTime ret = pred(tmi);

    setGranul(g);
    iniTmi_ = oldIniTmi;
    endTmi_ = oldEndTmi;
    
    return ret;
}

BCTime BCTmsInterval::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldIniTmi(iniTmi_);
    BCTime oldEndTmi(endTmi_);
    iniTmi_.nSuccessor(ndisp, getGranul());
    endTmi_.nSuccessor(ndisp, getGranul());

    BCTime ret = succ(tmi);

    iniTmi_ = oldIniTmi;
    endTmi_ = oldEndTmi;
    
    return ret;
}

BCTime BCTmsInterval::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldIniTmi(iniTmi_);
    BCTime oldEndTmi(endTmi_);
    iniTmi_.nSuccessor(ndisp, getGranul());
    endTmi_.nSuccessor(ndisp, getGranul());

    BCTime ret = pred(tmi);

    iniTmi_ = oldIniTmi;
    endTmi_ = oldEndTmi;
    
    return ret;
}

bool BCTmsInterval::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    BCTime tmi(iniTmi_, getGranul());
    tmi.successor(-1);
    if(ctms->succ(tmi) > endTmi_) ret = true;
    return ret;
}

bool BCTmsInterval::isSuccableInG(int niter, granularity granul) const
{
    if(granul<=getGranul())
	return true;
    else return false;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsCTimesOfSet, "CTimesOfSet", 1, 1, "Set", 
	  "(Set setOfCTimes)",
	  I2("Returns the CTimeSet of all instants of time contained in "
	     "parameter setOfCTimes.",
	     "Devuelve el CTimeSet formado por todos los instantes de "
	     "tiempo contenidos en el parámetro setOfCTimes."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsCTimesOfSet::includes(const BCTime& tmi)
{ 
    bool included = false;

    for(int i=1; i<=set_.Card() && !included; i++)
	if(CTime(set_[i]).includes(tmi)) 
	    included = true;

    return included;
}

void BCTmsCTimesOfSet::loadVectors(int niter, granularity granul)
{
    BCTime iniIT, endIT;
    for(int i=1; i<=set_.Card(); i++) 
    {
	iniIT = BCTime::fillNew(granul, LV, CTime(set_[i]));
	iniIT.nSuccessor(niter, granul);
	endIT = BCTime::fillNew(granul, HV, CTime(set_[i]));
	endIT.nSuccessor(niter, granul);
	iniITs_.push_back(iniIT);
	endITs_.push_back(endIT);
    }
}

bool BCTmsCTimesOfSet::includes4SuccInG(const BCTime &tmi, 
				     int niter, granularity granul)
{
    bool belongs = false;

    loadVectors(niter, granul);
    std::vector<BCTime>::iterator vit1, vit2;
    BCTime lLim(BCTime::End()), hLim(BCTime::Begin());

    //------------------------------
    // 1st. Step -> apply niter to each BCTime in vectors
    for(vit1=iniITs_.begin(), vit2=endITs_.begin(); 
	vit1!=iniITs_.end(); 
	vit1++, vit2++)
    {
	if((*vit1).startsBefore(lLim)) { lLim = (*vit1); }
	if((*vit2).endsAfter(hLim))    { hLim = (*vit2); }
    }

    for(vit1=iniITs_.begin(), vit2=endITs_.begin(); 
	vit1!=iniITs_.end(); 
	vit1++, vit2++)
    {
	if((*vit1).startsBeforeEQ(tmi) && (*vit2).endsAfterEQ(tmi)) 
	{ 
	    belongs = true; 
	    break; 
	}
    }

    return belongs;
}

bool BCTmsCTimesOfSet::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    bool included = false;
    BCTime auxTmi;
    
    for(int i=1; i<=set_.Card() && !included; i++) {
	auxTmi = CTime(set_[i]);
	auxTmi.nSuccessor(ndisp, auxTmi.getGranul());
	if(auxTmi.includes(tmi)) included = true;
    }
    
    return included;
}

BCTime BCTmsCTimesOfSet::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return lLim_;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);
    
    if(!includes(ret)) 
    {
	if     (ret>hLim_) ret = BCTime::End();
	else if(tmi<lLim_) ret = lLim_;
	else 
	{
	    BCTime setTmi;
	    BCTime aux = BCTime::End();

	    for(int i=1; i<=set_.Card(); i++)
	    {
		setTmi = CTime(set_[i]);
		if((tmi<setTmi) && setTmi.startsBefore(aux)) aux = setTmi;
	    }

	    ret = aux;
	}

	if(ret.isNormal() && (ret.getGranul() > tmi.getGranul()))
	    ret = BCTime::fillNew(tmi.getGranul(), LV, ret);
    }

    return ret;
}

BCTime BCTmsCTimesOfSet::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return hLim_;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes(ret)) 
    {
	if     (tmi>hLim_) ret = hLim_;
	else if(tmi<lLim_) ret = BCTime::Begin();
	else 
	{
	    BCTime setTmi;
	    BCTime aux = BCTime::Begin();

	    for(int i=1; i<=set_.Card(); i++)
	    {
		setTmi = CTime(set_[i]);
		if((tmi>setTmi) && setTmi.endsAfter(aux)) aux = setTmi;
	    }

	    ret = aux;
	}

	if(ret.isNormal() && (ret.getGranul() > tmi.getGranul()))
	    ret = BCTime::fillNew(tmi.getGranul(), HV, ret);
    }

    return ret;
}

BCTime BCTmsCTimesOfSet::succ4SuccInG(const BCTime &tmi, 
				   int niter, granularity granul)
{
    loadVectors(niter, granul);
    std::vector<BCTime>::iterator vit1, vit2;
    BCTime lLim(BCTime::End()), hLim(BCTime::Begin());

    for(vit1=iniITs_.begin(), vit2=endITs_.begin(); 
	vit1!=iniITs_.end(); 
	vit1++, vit2++)
    {
	if((*vit1).startsBefore(lLim)) { lLim = (*vit1); }
	if((*vit2).endsAfter(hLim))    { hLim = (*vit2); }
    }

    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return lLim;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);
    
    if(!includes4SuccInG(ret, niter, granul)) 
    {
	if(tmi.getGranul()>granul)
	{
	    ret.setGranul(granul);
	    if(includes4SuccInG(ret, niter, granul)) return ret;
	} 

	if     (ret>hLim) ret = BCTime::End();
	else if(tmi<lLim) ret = lLim;
	else 
	{
	    BCTime aux = BCTime::End();

	    for(vit1=iniITs_.begin(), vit2=endITs_.begin(); 
		vit1!=iniITs_.end(); 
		vit1++, vit2++)
	    { if((tmi<(*vit1)) && (*vit1).startsBefore(aux)) aux = (*vit1); }
	    
	    ret = aux;
	}

	if(ret.isNormal() && (ret.getGranul() > tmi.getGranul()))
	    ret = BCTime::fillNew(tmi.getGranul(), LV, ret);
    }

    return ret;
}

BCTime BCTmsCTimesOfSet::pred4SuccInG(const BCTime &tmi, 
				   int niter, granularity granul)
{
    loadVectors(niter, granul);
    std::vector<BCTime>::iterator vit1, vit2;
    BCTime lLim(BCTime::End()), hLim(BCTime::Begin());

    for(vit1=iniITs_.begin(), vit2=endITs_.begin(); 
	vit1!=iniITs_.end(); 
	vit1++, vit2++)
    {
	if((*vit1).startsBefore(lLim)) { lLim = (*vit1); }
	if((*vit2).endsAfter(hLim))    { hLim = (*vit2); }
    }

    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return hLim;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes4SuccInG(ret, niter, granul) && ret.isNormal()) 
    {
	if(tmi.getGranul()>granul)
	{
	    ret = tmi;
	    ret.setGranul(granul);
	    ret.successor(-1);
	    if(includes4SuccInG(ret, niter, granul)) return ret;
	} 

	if     (ret>hLim) ret = hLim;
	else if(ret<lLim) ret = BCTime::Begin();
	else 
	{
	    BCTime aux = BCTime::Begin();
	    
	    for(vit1=iniITs_.begin(), vit2=endITs_.begin(); 
		vit1!=iniITs_.end(); 
		vit1++, vit2++)
	    { if((tmi>(*vit2)) && (*vit2).endsAfter(aux)) aux = (*vit2); }
	    
	    ret = aux;
	}
	
	if(ret.isNormal() && (ret.getGranul() > tmi.getGranul()))
	    ret = BCTime::fillNew(tmi.getGranul(), HV, ret);
    }

    return ret;
}

BCTime BCTmsCTimesOfSet::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldLLim(lLim_), oldHLim(hLim_);
    lLim_ = BCTime::End();
    hLim_ = BCTime::Begin();
    int i;

    for(i=1; i<=set_.Card(); i++)
    {	    
	CTime(set_[i]).nSuccessor(ndisp);

	if(CTime(set_[i]).startsBefore(lLim_)) { lLim_ = CTime(set_[i]); }
	if(CTime(set_[i]).endsAfter(hLim_))    { hLim_ = CTime(set_[i]); }
    }

    BCTime ret = succ(tmi);

    for(i=1; i<=set_.Card(); i++) CTime(set_[i]).nSuccessor(-ndisp);

    lLim_ = oldLLim;
    hLim_ = oldHLim;

    return ret;
}

BCTime BCTmsCTimesOfSet::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldLLim(lLim_), oldHLim(hLim_);
    lLim_ = BCTime::End();
    hLim_ = BCTime::Begin();
    int i;

    for(i=1; i<=set_.Card(); i++)
    {
	CTime(set_[i]).nSuccessor(ndisp);

	if(CTime(set_[i]).startsBefore(lLim_)) { lLim_ = CTime(set_[i]); }
	if(CTime(set_[i]).endsAfter(hLim_))    { hLim_ = CTime(set_[i]); }
    }

    BCTime ret = pred(tmi);

    for(i=1; i<=set_.Card(); i++) CTime(set_[i]).nSuccessor(-ndisp);

    lLim_ = oldLLim;
    hLim_ = oldHLim;

    return ret;
}

bool BCTmsCTimesOfSet::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    
    for(int i=1; i<set_.Card() && !ret; i++)
	if(ctms->includes(CTime(set_[i]))) 
	    ret = true;

    return ret;
}

bool BCTmsCTimesOfSet::isSuccableInG(int niter, granularity granul) const
{
    if(granul<=getGranul())
	return true;
    else return false;
}

//--------------------------------------------------------------------

#endif /* __USE_TC__ */
