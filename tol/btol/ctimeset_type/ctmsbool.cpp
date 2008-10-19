/* ctmsbool.cpp - CTimeSet Tol Data Type creation.
 *    Boolean Operations: Union (+), Intersection(*) and Difference(-)
 *
 * Copyright (C) 2005 - Bayes Decision, SL
 * Author: Daniel Rus <danirus@tol-project.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_bctmsbool.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
DefIntOpr(1, BCTmsUnion, "+", 2, 2, 
	  "ctms1 + ctms2  {CTimeSet ctms1, CTimeSet ctms2}",
	  I2("Returns the union of both time sets.",
	     "Devuelve la unión de ambos conjuntos temporales."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsUnion::includes(const BCTime &tmi)
{ 
    return (ctmsA_->includes(tmi) || ctmsB_->includes(tmi));
}

bool BCTmsUnion::includes4SuccInG(const BCTime &tmi, 
				  int niter, granularity granul)
{
    return (ctmsA_->includes4SuccInG(tmi, niter, granul) || 
	    ctmsB_->includes4SuccInG(tmi, niter, granul));
}

bool BCTmsUnion::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    return (ctmsA_->includes4SelfSucc(tmi, ndisp) || 
	    ctmsB_->includes4SelfSucc(tmi, ndisp));
}

BCTime BCTmsUnion::succ(const BCTime& tmi)
{
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret)) 
    {
	BCTime succA = ctmsA_->succ(tmi);
	BCTime succB = ctmsB_->succ(tmi);

	if(succA.startsBefore(succB))
	    ret = succA;
	else ret = succB;
    }

    return ret;
}

BCTime BCTmsUnion::pred(const BCTime& tmi)
{
    BCTime ret(tmi);
    ret.successor(-1);

    if(!includes(ret)) 
    {
	BCTime predA = ctmsA_->pred(tmi);
	BCTime predB = ctmsB_->pred(tmi);

	if(predA.endsAfter(predB))
	    ret = predA;
	else ret = predB;
    }

    return ret;
}

BCTime BCTmsUnion::succ4SuccInG(const BCTime &tmi, 
				int niter, granularity granul)
{
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes4SuccInG(ret, niter, granul)) 
    {
	BCTime succA = ctmsA_->succ4SuccInG(tmi, niter, granul);
	BCTime succB = ctmsB_->succ4SuccInG(tmi, niter, granul);

	if(succA.startsBefore(succB))
	    ret = succA;
	else ret = succB;
    }

    return ret;
}

BCTime BCTmsUnion::pred4SuccInG(const BCTime &tmi, 
				int niter, granularity granul)
{
    BCTime ret(tmi);
    ret.successor(-1);

    if(!includes4SuccInG(ret, niter, granul)) 
    {
	BCTime predA = ctmsA_->pred4SuccInG(tmi, niter, granul);
	BCTime predB = ctmsB_->pred4SuccInG(tmi, niter, granul);

	if(predA.endsAfter(predB))
	    ret = predA;
	else ret = predB;
    }

    return ret;
}

BCTime BCTmsUnion::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes4SelfSucc(ret, ndisp)) 
    {
	BCTime succA = ctmsA_->succ4SelfSucc(tmi, ndisp);
	BCTime succB = ctmsB_->succ4SelfSucc(tmi, ndisp);
	
	if(succA.startsBefore(succB))
	    ret = succA;
	else ret = succB;
    }

    return ret;
}

BCTime BCTmsUnion::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret(tmi);
    ret.successor(-1);

    if(!includes4SelfSucc(ret, ndisp)) 
    {
	BCTime predA = ctmsA_->pred4SelfSucc(tmi, ndisp);
	BCTime predB = ctmsB_->pred4SelfSucc(tmi, ndisp);
	
	if(predA.endsAfter(predB))
	    ret = predA;
	else ret = predB;
    }

    return ret;
}

bool BCTmsUnion::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;

    if(ctmsA_->isVoidIntersectedWith(ctms) &&
       ctmsB_->isVoidIntersectedWith(ctms))
    { ret = true; }

    return ret;
}

bool BCTmsUnion::isSuccableInG(int niter, granularity granul) const
{
    if(ctmsA_->isSuccableInG(niter, granul) &&
       ctmsB_->isSuccableInG(niter, granul)) 
	return true;
    else return false;
}


//--------------------------------------------------------------------
DefIntOpr(1, BCTmsIntersection, "*", 2, 2, 
	  "ctms1 * ctms2  {CTimeSet ctms1, CTimeSet ctms2}",
	  I2("Returns the intersection of both time sets.",
	     "Devuelve la intersección de ambos conjuntos temporales."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsIntersection::includes(const BCTime &tmi)
{ 
    return (ctmsA_->includes(tmi) && ctmsB_->includes(tmi));
}

bool BCTmsIntersection::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    return (ctmsA_->includes4SelfSucc(tmi, ndisp) && 
	    ctmsB_->includes4SelfSucc(tmi, ndisp));
}

BCTime BCTmsIntersection::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret))
    {
	if((tmi.getGranul() > getGranul()) || (!tmi.isNormal())) 
	    ret = BCTime::fillNew(getGranul(), LV, ret);
	
	BCTime succA = ctmsA_->succ(ret);
	BCTime succB = ctmsB_->succ(ret);

	if(succA==succB) ret = succA;

	if     ((succA<succB) && ctmsA_->includes(succB)) ret = succB;
	else if((succA>succB) && ctmsB_->includes(succA)) ret = succA;
	else 
	    if     (succA<succB) ret = succ(succB);
	    else if(succA>succB) ret = succ(succA);
    }
    
    return ret;
}

BCTime BCTmsIntersection::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);

    if(!includes(ret))
    {
	if((tmi.getGranul() > getGranul()) || (!tmi.isNormal())) 
	    ret = BCTime::fillNew(getGranul(), HV, ret);

	BCTime predA = ctmsA_->pred(ret);
	BCTime predB = ctmsB_->pred(ret);

	if(predA==predB) ret = predA;

	if     ((predA>predB) && ctmsA_->includes(predB)) ret = predB;
	else if((predA<predB) && ctmsB_->includes(predA)) ret = predA;
	else 
	    if     (predA>predB) ret = pred(predB);
	    else if(predA<predB) ret = pred(predA);
    }
    
    return ret;
}

BCTime BCTmsIntersection::succ4SelfSucc(const BCTime& tmi, int ndisp)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes4SelfSucc(ret, ndisp)) 
    {
	granularity granul = tmi.getGranul();
	if((granul > getGranul()) || (!tmi.isNormal())) granul = getGranul();
	BCTime auxtmi = BCTime::fillNew(granul, LV, ret);

	BCTime succA = ctmsA_->succ4SelfSucc(auxtmi, ndisp);
	BCTime succB = ctmsB_->succ4SelfSucc(auxtmi, ndisp);
	
	if(succA==succB) ret = succA;

	if((succA<succB) && ctmsA_->includes4SelfSucc(succB, ndisp)) 
	{ ret = succB; }
	else if((succA>succB) && ctmsB_->includes4SelfSucc(succA, ndisp)) 
	{ ret = succA; }
	else 
	    if(succA<succB) 
		ret = succ4SelfSucc(succB, ndisp);
	    else ret = succ4SelfSucc(succA, ndisp);
    }
    
    return ret;
}

BCTime BCTmsIntersection::pred4SelfSucc(const BCTime& tmi, int ndisp)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);

    if(!includes4SelfSucc(ret, ndisp)) 
    {
	granularity granul = tmi.getGranul();
	if((granul > getGranul()) || (!tmi.isNormal())) granul = getGranul();
	BCTime auxtmi = BCTime::fillNew(granul, HV, ret);

	BCTime predA = ctmsA_->pred4SelfSucc(auxtmi, ndisp);
	BCTime predB = ctmsB_->pred4SelfSucc(auxtmi, ndisp);
	
	if(predA==predB) ret = predA;
	
	if     ((predA>predB) && ctmsA_->includes4SelfSucc(predB, ndisp)) 
	{ ret = predB; }
	else if((predA<predB) && ctmsB_->includes4SelfSucc(predA, ndisp)) 
	{ ret = predA; }
	else 
	    if(predA>predB) 
		ret = pred4SelfSucc(predB, ndisp);
	    else ret = pred4SelfSucc(predA, ndisp);
    }
    
    return ret;
}

bool BCTmsIntersection::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;

    if(ctmsA_->isVoidIntersectedWith(ctms) ||
       ctmsB_->isVoidIntersectedWith(ctms))
    { ret = true; }

    return ret;
}

//--------------------------------------------------------------------
DefIntOpr(1, BCTmsDifference, "-", 2, 2, 
	  "ctms1 - ctms2  {CTimeSet ctms1, CTimeSet ctms2}",
	  I2("Returns the difference between both time sets.",
	     "Devuelve la diferencia entre ambos conjuntos temporales."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsDifference::includes(const BCTime &tmi)
{
    if(getGranul()>=tmi.getGranul())
	return (ctmsA_->includes(tmi) && !ctmsB_->includes(tmi));
    else
    {
	bool included = true;
	BCTime auxtmi(tmi, getGranul());
	while(tmi.includes(auxtmi) && included)
	{
	    included = (ctmsA_->includes(auxtmi) && !ctmsB_->includes(auxtmi));
	    auxtmi.successor(1);
	}
	return included;
    }
}

bool BCTmsDifference::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    if(getGranul()>=tmi.getGranul())
	return (ctmsA_->includes4SelfSucc(tmi, ndisp) && 
		!ctmsB_->includes4SelfSucc(tmi, ndisp));
    else
    {
	bool included = true;
	BCTime auxtmi(tmi, getGranul());
	while(tmi.includes(auxtmi) && included)
	{
	    included = (ctmsA_->includes4SelfSucc(auxtmi, ndisp) && 
			!ctmsB_->includes4SelfSucc(auxtmi, ndisp));
	    auxtmi.successor(1);
	}
	return included;
    }
}

BCTime BCTmsDifference::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret)) 
    {
	granularity granul = tmi.getGranul();
	if((granul > getGranul()) || (!tmi.isNormal())) granul = getGranul();
	ret = BCTime::fillNew(granul, LV, ret);
	
	if(!includes(ret)) 
	{
	    BCTime succ = ctmsA_->succ(ret);

	    if(succ.isNormal())
	    {
		if(succ.getGranul() > getGranul()) 
		    succ = BCTime::fillNew(getGranul(), LV, succ);

		if(ctmsB_->includes(succ)) succ = (this)->succ(succ);
	    }
	    ret = succ;
	}
    }
    
    return ret;
}

BCTime BCTmsDifference::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes(ret)) 
    {
	granularity granul = tmi.getGranul();
	if((granul > getGranul()) || (!tmi.isNormal())) granul = getGranul();
	ret = BCTime::fillNew(granul, HV, ret);

	if(!includes(ret)) 
	{
	    BCTime pred = ctmsA_->pred(ret);

	    if(pred.isNormal())
	    {
		if(pred.getGranul() > getGranul()) 
		    pred = BCTime::fillNew(getGranul(), HV, pred);

		if(ctmsB_->includes(pred)) pred = (this)->pred(pred);
	    }
	    ret = pred;
	}
    }
    
    return ret;
}

BCTime BCTmsDifference::succ4SelfSucc(const BCTime& tmi, int ndisp)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);
    
    if(!includes4SelfSucc(ret, ndisp)) 
    {
	granularity granul = tmi.getGranul();
	if((granul > getGranul()) || (!tmi.isNormal())) granul = getGranul();
	ret = BCTime::fillNew(granul, LV, ret);

	if(!includes4SelfSucc(ret, ndisp))
	{
	    BCTime succ = ctmsA_->succ4SelfSucc(ret, ndisp);

	    if(succ.isNormal())
	    {
		if(succ.getGranul() > getGranul()) 
		    succ = BCTime::fillNew(getGranul(), LV, succ);
		
		if(ctmsB_->includes4SelfSucc(succ, ndisp))
		    succ = succ4SelfSucc(succ, ndisp);
	    }
	    ret = succ;
	}
    }
    
    return ret;
}

BCTime BCTmsDifference::pred4SelfSucc(const BCTime& tmi, int ndisp)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes4SelfSucc(ret, ndisp)) 
    {
	granularity granul = tmi.getGranul();
	if((granul > getGranul()) || (!tmi.isNormal())) granul = getGranul();
	ret = BCTime::fillNew(granul, HV, ret);

	if(!includes4SelfSucc(ret, ndisp))
	{
	    BCTime pred = ctmsA_->pred4SelfSucc(ret, ndisp);

	    if(pred.isNormal())
	    {
		if(pred.getGranul() > getGranul()) 
		    pred = BCTime::fillNew(getGranul(), HV, pred);
		
		if(ctmsB_->includes4SelfSucc(pred, ndisp))
		    pred = pred4SelfSucc(pred, ndisp);
	    }
	    ret = pred;
	}
    }
    
    return ret;
}

bool BCTmsDifference::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    if(ctmsA_->isVoidIntersectedWith(ctms)) ret = true;
    return ret;
}

#endif /* __USE_TC__ */
