/* ctmsspec.cpp - CTimeSet Tol Data Type creation.
 *               Special Operations: Succ, Range and Periodic
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

#include <tol/tol_bctmsspec.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsSucc, "Succ", 3, 3, "CTimeSet Real CTimeSet", 
	  "(CTimeSet ctmsSrc, Real niter, CTimeSet ctmsDest)",
	  I2("beta","beta"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
bool BCTmsSucc::includes(const BCTime &tmi)
{ 
    if(!f_->includes(tmi)) return false;

    BCTime it1 = f_->getMaximal(tmi);
           it1 = f_->maximalPred(it1);
    BCTime it2 = ct_->getMaximal(it1);
    BCTime it3 = f_->pred(it1);

    if(it2>=it3) 
	return true;
    else return false;
}

BCTime BCTmsSucc::succ(const BCTime &tmi)
{
    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) { 
	if(tmi==BCTime::Begin()) {
	    ret = BCTime(BEG_YEAR, BEG_MONTH,  BEG_DAY+1);
	    ret.nSuccessor(1, ct_->getGranul());
	}
	if(tmi==BCTime::End()) return tmi; 
    }
    //--------------------------------------------------
    else ret.successor(1);

    if(!includes(ret))
    {
	BCTime it1 = f_->pred(ret);
	BCTime it2 = ct_->maximalPred(it1);

	ret = BCTime::fillNew(f_->getGranul(), LV, it2);
	ret.successor(-1);
	for(int i=0; i<abs(ndisp_); i++) ret = f_->succ(ret);
	if(ret<=tmi) ret = succ(ct_->maximalSucc(it2));

	// returned granularity adjust
	if(ret.getGranul()>tmi.getGranul())
	    ret = BCTime::fillNew(tmi.getGranul(), LV, ret);
    }

    return ret;
}

BCTime BCTmsSucc::pred(const BCTime &tmi)
{
    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) { 
	if(tmi==BCTime::End()) {
	    ret = BCTime(END_YEAR, END_MONTH,  END_DAY-1);
	    ret.nSuccessor(-1, ct_->getGranul());
	}
	if(tmi==BCTime::Begin()) return tmi; 
    }
    //--------------------------------------------------
    else ret.successor(-1);

    if(!includes(ret))
    {
	BCTime it1 = f_->pred(ret);
	BCTime it2 = ct_->maximalPred(it1);

	ret = BCTime::fillNew(f_->getGranul(), LV, it2);
	ret.successor(-1);
	for(int i=0; i<abs(ndisp_); i++) ret = f_->succ(ret);
	if(ret>=tmi) ret = pred(ct_->maximalPred(it2));

	// returned granularity adjust
	if(ret.getGranul()>tmi.getGranul())
	    ret = BCTime::fillNew(tmi.getGranul(), HV, ret);
    }

    return ret;
}


//--------------------------------------------------------------------
DefExtOpr(1, BCTmsSelfSucc, "SelfSucc", 2, 2, "CTimeSet Real", 
	  "(CTimeSet ctms, Real niter)",
	  I2("Returns a CTimeSet as a result of iterates the given CTimeSet "
	     "over its range of acceptable values. A Month(x) CTimeSet can be "
	     "iterated from 1 to 12, a Day(x) over 1 to 31, a Year(x) accepts "
	     "all years from TheBegin yo TheEnd, and so on.\n",
	     "Devuele un Conjunto Temporal como resultado de iterar el "
	     "CTimeSet dado en el rango de valores que acepta. Un CTimeSet "
	     "Month(x) puede ser iterado entr 1 y 12, un Day(x) entre 1 y "
	     "31, un Year(x) acepta años entre TheBegin y TheEnd..."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsSelfSucc::includes(const BCTime &tmi)
{ 
    return ctms_->includes4SelfSucc(tmi, ndisp_);
}

bool BCTmsSelfSucc::includes4SuccInG(const BCTime &tmi, 
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

bool BCTmsSelfSucc::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    ndisp_ += ndisp;
    bool ret = includes(tmi);
    ndisp_ -= ndisp;
    return ret;
}

BCTime BCTmsSelfSucc::succ(const BCTime &tmi)
{
    return ctms_->succ4SelfSucc(tmi, ndisp_);
}

BCTime BCTmsSelfSucc::pred(const BCTime &tmi)
{
    return ctms_->pred4SelfSucc(tmi, ndisp_);
}

BCTime BCTmsSelfSucc::succ4SuccInG(const BCTime &tmi, 
				   int niter, granularity granul)
{
    if(granul==getGranul()) return succ4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) ret = succ(tmi);
    //--------------------------------------------------
    ret.successor(1);

    if(!includes4SuccInG(ret, niter, granul) && ret.isNormal())
    {
	granularity g = (tmi.getGranul()>granul) ? granul : tmi.getGranul();
	ret.setGranul(g);

	BCTime aux(ret, getGranul());
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

	    pivot = this->succ(pivot);
	    if(ret<iniIT) ret = iniIT;
	}
	while((ret>endIT) && (pivot.isNormal()));
	      
	if(!pivot.isNormal()) ret = BCTime::End();
    }
    
    return ret;
}

BCTime BCTmsSelfSucc::pred4SuccInG(const BCTime &tmi, 
				   int niter, granularity granul)
{
    if(granul==getGranul()) return pred4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) ret = pred(tmi);
    //--------------------------------------------------
    ret.successor(-1);

    if(!includes4SuccInG(ret, niter, granul) && ret.isNormal())
    {
	ret = tmi;
	granularity g = (tmi.getGranul()>granul) ? granul : tmi.getGranul();
	ret.setGranul(g);
	ret.successor(-1);

	BCTime aux(ret, getGranul());
	BCTime pivot = this->succ(aux);
	BCTime iniIT(pivot, granul), endIT(iniIT);
	
	do
	{
	    iniIT = pivot;
	    iniIT.setGranul(granul);
	    iniIT.nSuccessor(niter, granul);

	    endIT = iniIT;
	    endIT.nSuccessor( 1, getGranul());
	    endIT.nSuccessor(-1, granul);
	    
	    pivot = this->pred(pivot);
	    if(ret>endIT) ret = endIT;
	}
	while((ret<iniIT) && (pivot.isNormal()));

	if(!pivot.isNormal()) ret = BCTime::Begin();
    }
    
    return ret;
}

BCTime BCTmsSelfSucc::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    ndisp_ += ndisp;
    BCTime ret = succ(tmi);
    ndisp_ -= ndisp;
    return ret;
}

BCTime BCTmsSelfSucc::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    ndisp_ += ndisp;
    BCTime ret = pred(tmi);
    ndisp_ -= ndisp;
    return ret;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsSuccInG, "SuccInG", 2, 3, "CTimeSet Real Text", 
	  "(CTimeSet ctms, Real niter, Text granularity)",
	  I2("Returns a CTimeSet as a result of iterates a CTimeSet "
	     "over its range of acceptable values. A Month(x) CTimeSet can be "
	     "iterated from 1 to 12, a Day(x) over 1 to 31, a Year(x) accepts "
	     "all years from TheBegin yo TheEnd, and so on.\n"
	     "Parameter granularity must be one of the values listed below:"
	     "\"YEARS\", \"MONTHS\", \"DAYS\", \"HOURS\", \"MINUTES\","
	     " \"SECONDS\"",
	     "Devuele un Conjunto Temporal como resultado de iterar un "
	     "CTimeSet en el rango de valores que acepta. Un CTimeSet "
	     "Month(x) puede ser iterado entr 1 y 12, un Day(x) entre 1 y "
	     "31, un Year(x) acepta años entre TheBegin y TheEnd...\n"
	     "El parámetro granularidad debe tomar uno de los siguientes"
	     "valores:\n\"YEARS\", \"MONTHS\", \"DAYS\", \"HOURS\", "
	     "\"MINUTES\", \"SECONDS\""),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
bool BCTmsSuccInG::includes(const BCTime &tmi)
{
    return ctms_->includes4SuccInG(tmi, niter_, getGranul());
}

bool BCTmsSuccInG::includes4SuccInG(const BCTime &tmi,
				    int niter, granularity granul)
{
    granularity granulaux = getGranul();
    setGranul(granul);
    niter_ += niter;
    bool ret = includes(tmi);
    niter_ -= niter;
    setGranul(granulaux);
    return ret;
}

bool BCTmsSuccInG::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    if(tmi.getGranul()>getGranul()) return false;
    else 
    {
	BCTime tmiaux(tmi);
	tmiaux.nSuccessor(-niter_, getGranul());
	return ctms_->includes4SelfSucc(tmiaux, ndisp);
    }
}

BCTime BCTmsSuccInG::succ(const BCTime &tmi)
{
    return ctms_->succ4SuccInG(tmi, niter_, getGranul());
}

BCTime BCTmsSuccInG::pred(const BCTime &tmi)
{
    return ctms_->pred4SuccInG(tmi, niter_, getGranul());
}

BCTime BCTmsSuccInG::succ4SuccInG(const BCTime &tmi, 
				  int niter, granularity granul)
{
    granularity granulaux = getGranul();
    setGranul(granul);
    niter_ += niter;
    BCTime ret = succ(tmi);
    niter_ -= niter;
    setGranul(granulaux);
    return ret;
}

BCTime BCTmsSuccInG::pred4SuccInG(const BCTime &tmi, 
				  int niter, granularity granul)
{
    granularity granulaux = getGranul();
    setGranul(granul);
    niter_ += niter;
    BCTime ret = pred(tmi);
    niter_ -= niter;
    setGranul(granulaux);
    return ret;
}

BCTime BCTmsSuccInG::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);
    
    if(!includes4SelfSucc(ret, ndisp))
    {
	BCTime succ = ctms_->succ4SelfSucc(tmi, ndisp);
	succ.nSuccessor(niter_, getGranul());
	ret = succ;
    }
    return ret;
}

BCTime BCTmsSuccInG::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes4SelfSucc(ret, ndisp))
    {
	BCTime pred = ctms_->pred4SelfSucc(tmi, ndisp);
	pred.nSuccessor(niter_, getGranul());
	ret = pred;
    }
    return ret;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsSelfRange, "SelfRange", 3, 3, "CTimeSet Real Real", 
	  "(CTimeSet ctms, Real initIter, Real endIter)",
	  I2("Iterates \a ctms from \a initIter to \a endIter and adds "
	     "each iteration to the resulted CTimeSet. Is like multiple "
	     "union of Succ(ctms, n) operations, varying n from \a initIter "
	     "to \a endITer.\n",
	     "Itera \a ctms desde \a initIter hasta \a endIter y añade "
	     "cada iteración al Conjunto Temporal resultado. "
	     "Es como múltiples operaciones Union de Succ(ctms, n), en las "
	     "cuales se varia n desde \a initIter hasta \a endIter.\n"),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsSelfRange::includes(const BCTime &tmi)
{
    for(int i=ini_; i<=end_; i++)
	if(ctms_->includes4SelfSucc(tmi, i)) 
	    return true;

    return false;
}

bool BCTmsSelfRange::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    ini_ += ndisp;
    end_ += ndisp;
    bool ret = includes(tmi);
    ini_ -= ndisp;
    end_ -= ndisp;

    return ret;
}

BCTime BCTmsSelfRange::succ(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);
    
    if(!includes(ret))
    {
	BCTime succ;
	ret = BCTime::End();

	for(int i=ini_; i<=end_; i++) 
	{
	    succ = ctms_->succ4SelfSucc(tmi, i);
	    if(succ.startsBefore(ret) && succ.endsAfter(tmi)) ret = succ;
	}
    }
    return ret;
}

BCTime BCTmsSelfRange::pred(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes(ret))
    {
	BCTime pred;
	ret = BCTime::Begin();
	
	for(int i=ini_; i<=end_; i++) 
	{
	    pred = ctms_->pred4SelfSucc(tmi, i);
	    if(pred.endsAfter(ret) && pred.startsBefore(tmi)) ret = pred;
	}
    }
    return ret;
}

BCTime BCTmsSelfRange::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    ini_ += ndisp;
    end_ += ndisp;
    BCTime ret = succ(tmi);
    ini_ -= ndisp;
    end_ -= ndisp;

    return ret;
}

BCTime BCTmsSelfRange::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    ini_ += ndisp;
    end_ += ndisp;
    BCTime ret = pred(tmi);
    ini_ -= ndisp;
    end_ -= ndisp;

    return ret;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsPeriodic, "Periodic", 2, 3, "CTime Real CTimeSet",
  I2("(CTime D, Real P [, CTimeSet units = CTSAll])",
     "(CTime D, Real P [, CTimeSet units = CTSAll])"),
  I2("Returns the periodic CTimeSet, composed by a CTime and all its "
     "iterations to a number of given multiple P within a given unit's "
     "dating.",
     "Devuelve el conjunto temporal periódico, compuesto por un CTime y "
     "todas sus translaciones a un número de ITs múltiplo de P dentro del "
     "fechado de unidades dado."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
bool BCTmsPeriodic::includes(const BCTime& tmi)
{
    if(tmi.getGranul() > getGranul())
	return false;
    else if(ctms_->includes(tmi))
    {
	BCTime auxtmi(tmi);
	if(tmi.getGranul() < getGranul()) auxtmi.setGranul(getGranul());
	int dif       = ctms_->distance(startTmi_, auxtmi);
	bool included = !(dif % period_);
	return included;
    } else
	return false;
}

bool BCTmsPeriodic::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    return true;
}

BCTime BCTmsPeriodic::succ(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);
    
    if(!includes(ret))
    {
	ret = BCTime::fillNew(getGranul(), HV, tmi);
	int dif = ctms_->distance(ret, startTmi_);
	dif = (dif % period_ + period_) % period_;
	int limit = dif ? dif : period_;

	for(int i=0; i<limit; i++) ret = ctms_->succ(ret);
	
	if(ret.getGranul() < tmi.getGranul())
	    ret = ctms_->maxGranulAdjust(ret, tmi.getGranul());
	else if(ret.getGranul() > tmi.getGranul())
	    ret = BCTime::fillNew(tmi.getGranul(), LV, ret);
    }

    return ret;
}

BCTime BCTmsPeriodic::pred(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes(ret))
    {
	ret = BCTime::fillNew(getGranul(), LV, tmi);
	int dif = ctms_->distance(ret, startTmi_);
	dif = (dif % period_ + period_) % period_;
	int limit = period_ - dif;
	if(!ctms_->includes(ret)) limit++;

	for(int i=0; i<limit; i++) ret = ctms_->pred(ret);

	if(ret.getGranul() < tmi.getGranul())
	    ret = ctms_->maxGranulAdjust(ret, tmi.getGranul());
	else if(ret.getGranul() > tmi.getGranul())
	    ret = BCTime::fillNew(tmi.getGranul(), HV, ret);
    }

    return ret;
}

BCTime BCTmsPeriodic::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    return tmi;
}

BCTime BCTmsPeriodic::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    return tmi;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsSelfPeriodic, "SelfPeriodic", 2, 2, "CTimeSet Real",
	  "(CTimeSet ctms, Real p)",
	  I2("Returns something","Returns something"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
bool BCTmsSelfPeriodic::includes(const BCTime &tmi)
{
    int *att = (int*) ctms_->getAttr();

    for(int i=0, disp=0; 
	ctms_->getHLimit() >= ((*att) + disp); 
	i++, disp = period_ * i
	)
	if(ctms_->includes4SelfSucc(tmi, disp)) return true;
    
    return false;
}

bool BCTmsSelfPeriodic::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    int *att = (int*) ctms_->getAttr();

    for(int i=0, disp=0; 
	ctms_->getHLimit() >= ((*att) + disp); 
	i++, disp = period_ * i
	)
	if(ctms_->includes4SelfSucc(tmi, disp+ndisp)) return true;
    
    return false;
}

BCTime BCTmsSelfPeriodic::succ(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);
    
    if(!includes(ret))
    {
	if(tmi.getGranul() > ctms_->getGranul()) 
	    ret = ctms_->succ(tmi);
	else
	{
	    int *att = (int*) ctms_->getAttr();

	    BCTime auxtmi;
	    ret = ctms_->succ(tmi);

	    for(int i=1, disp=period_;
		ctms_->getHLimit() >= ((*att) + disp);
		i++, disp = period_ * i)
	    {
		auxtmi = ctms_->succ4SelfSucc(tmi, disp);
		if(auxtmi<ret) ret = auxtmi;
	    }
	}
    }

    return ret;
}

BCTime BCTmsSelfPeriodic::pred(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes(ret))
    {
	if(tmi.getGranul() > ctms_->getGranul()) 
	    ret = ctms_->pred(tmi);
	else
	{
	    int *att = (int*) ctms_->getAttr();

	    BCTime auxtmi;
	    ret = ctms_->pred(tmi);

	    for(int i=1, disp=period_;
		ctms_->getHLimit() >= ((*att) + disp);
		i++, disp = period_ * i)
	    {
		auxtmi = ctms_->pred4SelfSucc(tmi, disp);
		if(auxtmi>ret) ret = auxtmi;
	    }
	}
    }

    return ret;
}

BCTime BCTmsSelfPeriodic::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::End())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);
    
    if(!includes4SelfSucc(ret, ndisp))
    {
	if(tmi.getGranul() > ctms_->getGranul()) 
	    ret = ctms_->succ4SelfSucc(tmi, ndisp);
	else
	{
	    int *att = (int*) ctms_->getAttr();
	    
	    BCTime auxtmi;
	    ret = ctms_->succ4SelfSucc(tmi, ndisp);
	    
	    for(int i=1, disp=period_;
		ctms_->getHLimit() >= ((*att) + disp);
		i++, disp = period_ * i)
	    {
		auxtmi = ctms_->succ4SelfSucc(tmi, disp+ndisp);
		if(auxtmi<ret) ret = auxtmi;
	    }
	}
    }

    return ret;
}

BCTime BCTmsSelfPeriodic::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    //--------------------------------------------------
    if(!tmi.isNormal() && (tmi==BCTime::Begin())) return tmi;
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes4SelfSucc(ret, ndisp))
    {
	if(tmi.getGranul() > ctms_->getGranul()) 
	    ret = ctms_->pred4SelfSucc(tmi, ndisp);
	else
	{
	    int *att = (int*) ctms_->getAttr();

	    BCTime auxtmi;
	    ret = ctms_->pred4SelfSucc(tmi, ndisp);

	    for(int i=1, disp=period_;
		ctms_->getHLimit() >= ((*att) + disp);
		i++, disp = period_ * i)
	    {
		auxtmi = ctms_->pred4SelfSucc(tmi, disp+ndisp);
		if(auxtmi>ret) ret = auxtmi;
	    }
	}
    }

    return ret;
}

#endif /* __USE_TC__ */
