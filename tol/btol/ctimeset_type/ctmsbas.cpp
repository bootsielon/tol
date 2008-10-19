/* ctmsbas.cpp - CTimeSet Tol Data Type creation.
 *               Basic Datings implementation: Y, M, D, H, Mi, S, W, WD
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

#include <tol/tol_bctmsbas.h>
#include <tol/tol_bctmsbool.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsYear, "Year", 1, 1, "Real", "(Real year)",
	  I2("Returns the CTimeSet composed from all instants of time of the "
	     "year given as a parameter.",
	     "Devuelve el CTimeSet compuesto por todos los instante de tiempo "
	     "del año indicado como parámetro."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsYear::includes(const BCTime &tmi)
{ 
    return year_.includes(tmi); 
}

bool BCTmsYear::includes4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    bool belongs = false;

    year_.nSuccessor(niter, granul);
    if(year_.startsBeforeEQ(tmi) && year_.endsAfterEQ(tmi)) belongs = true;
    year_.nSuccessor(-niter, granul);

    return belongs;
}

bool BCTmsYear::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldYear(year_);
    year_.nSuccessor(ndisp, YEARS);
    bool ret = includes(tmi);
    year_ = oldYear;
    
    return ret;
}

BCTime BCTmsYear::succ(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) { 
	if(tmi==BCTime::Begin()) return year_; 
	if(tmi==BCTime::End())   return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret)) 
    {
	if(year_ < ret) ret = BCTime::End();
	else            ret = BCTime::fillNew(tmi.getGranul(), LV, year_);
    }

    return ret;
}

BCTime BCTmsYear::pred(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) { 
	if(tmi==BCTime::End())   return year_; 
	if(tmi==BCTime::Begin()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);

    if(!includes(ret)) 
    {
	if(year_ > ret) ret = BCTime::Begin();
	else            ret = BCTime::fillNew(tmi.getGranul(), HV, year_);
    }

    return ret;
}

BCTime BCTmsYear::succ4SuccInG(const BCTime &tmi, 
			       int niter, granularity granul)
{
    if(granul==YEARS) return succ4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    ret.successor(1);

    if(!includes4SuccInG(ret, niter, granul)) 
    {
	year_.nSuccessor(niter, granul);
	year_.setGranul(granul);
	ret.setGranul(granul);

	granularity g = granul;
	if(tmi.isNormal() && (tmi.getGranul() < granul)) g = tmi.getGranul();

	BCTime startYear = BCTime::fillNew(g, LV, year_);
	BCTime endYear(year_);
	endYear.nSuccessor(1, YEARS);
	endYear.nSuccessor(-1, granul);
	endYear = BCTime::fillNew(g, HV, endYear);

	if(ret.endsAfter(endYear))           ret = BCTime::End();
	else if(ret.startsBefore(startYear)) ret = startYear;

	year_.setGranul(YEARS);
    }

    return ret;
}

BCTime BCTmsYear::pred4SuccInG(const BCTime &tmi, 
			       int niter, granularity granul)
{
    if(granul==YEARS) return pred4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    ret.successor(-1);
    
    if(!includes4SuccInG(ret, niter, granul)) 
    {
	year_.nSuccessor(niter, granul);
	year_.setGranul(granul);

	ret = tmi;
	ret.setGranul(granul);
	ret.successor(-1);

	granularity g = granul;
	if(tmi.isNormal() && (tmi.getGranul() < granul)) g = tmi.getGranul();

	BCTime startYear = BCTime::fillNew(g, LV, year_);
	BCTime endYear(year_);
	endYear.nSuccessor(1, YEARS);
	endYear.nSuccessor(-1, granul);
	endYear = BCTime::fillNew(g, HV, endYear);	

	if(ret.startsBefore(startYear)) ret = BCTime::Begin();
	else if(ret.endsAfter(endYear)) ret = endYear;
	
	year_.setGranul(YEARS);
    }

    return ret;
}

BCTime BCTmsYear::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldYear(year_);
    year_.nSuccessor(ndisp, YEARS);
    BCTime ret = succ(tmi);
    year_ = oldYear;
    
    return ret;
}

BCTime BCTmsYear::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime oldYear(year_);
    year_.nSuccessor(ndisp, YEARS);
    BCTime ret = pred(tmi);
    year_ = oldYear;
    
    return ret;
}

bool BCTmsYear::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    if(ctms->getType() == CTmsYear)
    {
	BCTime* year = (BCTime*) ctms->getAttr();
	if((*year) != year_) ret = true;
    }
    else if(ctms->getType() == CTmsInside)
    {
	BCTime* tmi = (BCTime*) ctms->getAttr();
	if(((*tmi).year()) != year_.year()) ret = true;	
    }
    else if(ctms->getType() == CTmsIntersection)
    {
	BCTmsBinary* ctmsBinary = static_cast<BCTmsBinary*>(ctms);
	if(isVoidIntersectedWith(ctmsBinary->getCTmsA()) ||
	   isVoidIntersectedWith(ctmsBinary->getCTmsB())) ret = true;
    }

    return ret;
}

bool BCTmsYear::isSuccableInG(int niter, granularity granul) const
{
    switch(granul) 
    {
	case MONTHS:
	    if(niter>11) return false;
	    else return true;

	case DAYS:
	    if(BCTime::isLeapYear(year_.year())) {
		if(niter>365) return false; 
		else return true;
	    } else {
		if(niter>364) return false;
		else return true;
	    }

	case HOURS:
	    if(BCTime::isLeapYear(year_.year())) {
		if(niter>8783) return false; 
		else return true;
	    } else {
		if(niter>8759) return false; 
		else return true;
	    }

	case MINUTES:
	    if(BCTime::isLeapYear(year_.year())) {
		if(niter>527039) return false; 
		else return true;
	    } else {
		if(niter>525599) return false; 
		else return true;
	    }

	case SECONDS:
	    if(BCTime::isLeapYear(year_.year())) {
		if(niter>31622399) return false;
		else return true;
	    } else {
		if(niter>31535999) return false;
		else return true;
	    }

	default: 
	    return false;
    }
}

int BCTmsYear::BinWrite(FILE* fil) const
{
    int bytesWritten = 0;
    int intType = static_cast<int>(type_);
    int intGranul = static_cast<int>(granul_);
    fwrite(&intType, 1, sizeof(int), fil);
    fwrite(&intGranul, 1, sizeof(int), fil);
    bytesWritten = (sizeof(int) * 2);
    if(!HasName()) {
	BText guion = "-";
	bytesWritten = guion.BinWrite(fil);
    } else
	bytesWritten = Name().BinWrite(fil);

    // specific attributes
    long int year = year_.year();
    fwrite(&year, 1, sizeof(long int), fil);
    bytesWritten += sizeof(long int);

    return bytesWritten;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsMonth, "Month", 1, 1, "Real", "(Real month)",
	  I2("Returns the CTimeSet composed from all instants of time of the "
	     "month given as a parameter.",
	     "Devuelve el CTimeSet compuesto por todos los instante de tiempo "
	     "del mes indicado como parámetro."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsMonth::includes(const BCTime &tmi)
{
    if((tmi.getGranul()>MONTHS) || (!tmi.isNormal()))
	return false;
    else return (month_ == tmi.month());
}

bool BCTmsMonth::includes4SuccInG(const BCTime &tmi, 
				  int niter, granularity granul)
{
    bool belongs = false;

    BCTime aux(tmi, getGranul());
    BCTime pivot = this->pred(aux);
    BCTime iniIT(pivot, granul), endIT(iniIT);

    do
    {
	iniIT = pivot;
	endIT = pivot;
	
	iniIT.setGranul(granul);
	iniIT.nSuccessor(niter, granul);
	
	endIT.successor(1);
	endIT.setGranul(granul);
	endIT.nSuccessor(niter, granul);
	endIT.nSuccessor(-1, granul);
	
	if(iniIT.startsBeforeEQ(tmi) && endIT.endsAfterEQ(tmi)) 
	    belongs = true;

	pivot = this->succ(pivot);
    }
    while(!belongs && (tmi>endIT));

    return belongs;
}

bool BCTmsMonth::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    bool belongs = false;
    int curMonth = month_;
    int newMonth = ((curMonth - 1 + ndisp) % 12 ) + 1;
    if(newMonth<0) newMonth += 12;

    month_ = newMonth;
    belongs = includes(tmi);
    month_ = curMonth;

    return belongs;
}

BCTime BCTmsMonth::succ(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	    if(month_<3) return BCTime(BEG_YEAR+1, month_);
	    else         return BCTime(BEG_YEAR,   month_);

	if(tmi==BCTime::End()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret) && ret.isNormal())
    {
	granularity g = (tmi.getGranul()>MONTHS) ? MONTHS : tmi.getGranul();
	if((ret.year()==tmi.year()) && (tmi.month()>=month_)) ret.nextYear();
	if(ret.isNormal()) ret = BCTime::fillNew(g, LV, 2, ret.year(), month_);
    }
    
    return ret;
}

BCTime BCTmsMonth::pred(const BCTime &tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End()) return BCTime(END_YEAR, month_);
	if(tmi==BCTime::Begin()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    if(!ret.isNormal()) {ret = tmi; ret.setGranul(MONTHS); ret.successor(-1);}

    if(!includes(ret) && ret.isNormal())
    {
	granularity g = (tmi.getGranul()>MONTHS) ? MONTHS : tmi.getGranul();
	if((ret.year()==tmi.year()) && (tmi.month()<=month_)) ret.prevYear();
	if(ret.isNormal()) ret = BCTime::fillNew(g, HV, 2, ret.year(), month_);
    }
    
    return ret;
}

BCTime BCTmsMonth::succ4SuccInG(const BCTime &tmi, 
				int niter, granularity granul)
{
    if(granul==MONTHS) return succ4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	{
	    if(month_<3)
		ret = BCTime::fillNew(granul,LV,2,BEG_YEAR+1, month_);
	    else ret = BCTime::fillNew(granul,LV,2,BEG_YEAR, month_);

	    ret.nSuccessor(niter, granul);

	    if(!includes4SuccInG(ret, niter, granul))
		ret = succ4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	    endIT = iniIT;

	    iniIT.setGranul(granul);
	    iniIT.nSuccessor(niter, granul);

	    endIT.successor(1);
	    endIT.setGranul(granul);
	    endIT.nSuccessor(niter, granul);
	    endIT.nSuccessor(-1, granul);

	    pivot = this->succ(pivot);
	    if(ret<iniIT) ret = iniIT;
	}
	while((ret>endIT) && (pivot.isNormal()));
	      
	if(!pivot.isNormal()) ret = BCTime::End();
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, LV, ret);
    }
    
    return ret;
}

BCTime BCTmsMonth::pred4SuccInG(const BCTime &tmi, 
				int niter, granularity granul)
{
    if(granul==MONTHS) return pred4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End()) 
	{
	    ret = BCTime::fillNew(granul, HV, 2, END_YEAR, month_);

	    if(!includes4SuccInG(ret, niter, granul))
		ret = pred4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	    endIT = iniIT;

	    iniIT.setGranul(granul);
	    iniIT.nSuccessor(niter, granul);

	    endIT.successor(1);
	    endIT.setGranul(granul);
	    endIT.nSuccessor(niter, granul);
	    endIT.nSuccessor(-1, granul);
	    
	    pivot = this->pred(pivot);
	    if(ret>endIT) ret = endIT;
	}
	while((ret<iniIT) && (pivot.isNormal()));

	if(!pivot.isNormal()) ret = BCTime::Begin();
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, HV, ret);
    }
    
    return ret;
}

BCTime BCTmsMonth::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curMonth = month_;
    int newMonth = ((curMonth - 1 + ndisp) % 12 ) + 1;
    if(newMonth<0) newMonth += 12;

    month_ = newMonth;
    ret = succ(tmi);
    month_ = curMonth;

    return ret;
}

BCTime BCTmsMonth::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curMonth = month_;
    int newMonth = ((curMonth - 1 + ndisp) % 12 ) + 1;
    if(newMonth<0) newMonth += 12;

    month_ = newMonth;
    ret = pred(tmi);
    month_ = curMonth;

    return ret;
}

bool BCTmsMonth::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    if(ctms->getType() == CTmsMonth)
    {
	int* month = (int*) ctms->getAttr();
	if((*month) != month_) ret = true;
    }
    else if(ctms->getType() == CTmsIntersection)
    {
	BCTmsBinary* ctmsBinary = static_cast<BCTmsBinary*>(ctms);
	if(isVoidIntersectedWith(ctmsBinary->getCTmsA()) ||
	   isVoidIntersectedWith(ctmsBinary->getCTmsB())) ret = true;
    }

    return ret;
}

bool BCTmsMonth::isSuccableInG(int niter, granularity granul) const
{
    switch(granul) 
    {
	case MONTHS:
	    return true;

	case DAYS:
	    if(niter>366) return false; 
	    else return true;

	case HOURS:
	    if(niter>8783) return false; 
	    else return true;
	    

	case MINUTES:
	    if(niter>527039) return false; 
	    else return true;
	    
	case SECONDS:
	    if(niter>31622399) return false;
	    else return true;

	default: 
	    return false;
    }
}

int BCTmsMonth::BinWrite(FILE* fil) const
{
    int bytesWritten = 0;
    int intType = static_cast<int>(type_);
    int intGranul = static_cast<int>(granul_);
    fwrite(&intType, 1, sizeof(int), fil);
    fwrite(&intGranul, 1, sizeof(int), fil);
    bytesWritten = (sizeof(int) * 2);
    if(!HasName()) {
	BText guion = "-";
	bytesWritten = guion.BinWrite(fil);
    } else
	bytesWritten = Name().BinWrite(fil);

    // specific attributes
    fwrite(&month_, 1, sizeof(int), fil);
    bytesWritten += sizeof(int);

    return bytesWritten;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsDay, "Day", 1, 1, "Real", "(Real day)",
	  I2("Returns the CTimeSet composed from all instants of time of the "
	     "day given as a parameter. If the number day given is 31, only "
	     "the 31 day of months with 31 days will be included.\n"
	     "To create the CTimeSet with the last day of each month you "
	     "must use a negative value.\n"
	     "Examples:\n"
	     "CTimeSet stDay   = Day( 1);\n"
	     "CTimeSet lastDay = Day(-1);"
	     ,
	     "Devuelve el CTimeSet compuesto por todos los instante de tiempo "
	     "del día indicado como parámetro. Si el día indicado es el 31, "
	     "solo el día 31 de los meses con 31 días estará incluído.\n"
	     "Es posible crear el CTimeSet de el último día de cada mes "
	     "usando el valor negativo -1:\n"
	     "Ejemplo:\n"
	     "CTimeSet stDay   = Day( 1);\n"
	     "CTimeSet lastDay = Day(-1);"),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsDay::includes(const BCTime& tmi)
{
    if((tmi.getGranul()>DAYS) || (!tmi.isNormal())) 
	return false;
    else return (day_ == tmi.day());
}

bool BCTmsDay::includes4SuccInG(const BCTime &tmi, 
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

bool BCTmsDay::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    bool belongs = false;
    int curDay = day_;
    int newDay = ((curDay - 1 + ndisp) % 31 ) + 1;
    if(newDay<0) newDay += 31;

    day_ = newDay;
    belongs = includes(tmi);
    day_ = curDay;

    return belongs;
}

BCTime BCTmsDay::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	    if(day_==1)
		return BCTime::fillNew(DAYS,LV,3, BEG_YEAR, BEG_MONTH+1, day_);
	    else return BCTime::fillNew(DAYS,LV,3, BEG_YEAR, BEG_MONTH, day_);

	if(tmi==BCTime::End()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret) && ret.isNormal())
    {
	int day = (day_>0) ? day_ : ret.whichDayIs(day_);
	
	if((tmi.getGranul() <= DAYS) && 
	   (ret.month()     == tmi.month()) && 
	   (ret.day()       >  day)) 
	{ 
	    ret.setGranul(MONTHS);
	    ret.successor(1); 
	}

	long int year = ret.year();
	int month = ret.month();

	if((tmi.getGranul() >= MONTHS) && (ret.year()!=tmi.year()))
	{ month = 0; }

	granularity granul = (tmi.getGranul()>DAYS) ? DAYS : tmi.getGranul();
	if(ret.isNormal()) 
	    ret = BCTime::fillNew(granul, LV, 3, year, month, day_);
    }
    
    return ret;
}

BCTime BCTmsDay::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End())
	    if(day_==31)
		return BCTime::fillNew(DAYS,HV,3, END_YEAR, END_MONTH-2, day_);
	    else return BCTime::fillNew(DAYS,HV,3, END_YEAR, END_MONTH, day_);
	
	if(tmi==BCTime::Begin()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    if(!ret.isNormal()) { ret = tmi; ret.setGranul(DAYS); ret.successor(-1); }

    if(!includes(ret) && ret.isNormal())
    {
	int day = (day_>0) ? day_ : ret.whichDayIs(day_);

	if((tmi.getGranul() <= DAYS) && 
	   (ret.month()     == tmi.month()) &&
	   (ret.day()       <  day)) 
	{ 
	    ret.setGranul(MONTHS);
	    ret.successor(-1); 
	}

	long int year = ret.year();
	int month = ret.month();
	
	if((tmi.getGranul() >= MONTHS) && (ret.year()!=tmi.year()))
	{ month = 0; }

	granularity granul = (tmi.getGranul()>DAYS) ? DAYS : tmi.getGranul();
	if(ret.isNormal())
	   ret = BCTime::fillNew(granul, HV, 3, year, month, day_);
    }
    
    return ret;
}

BCTime BCTmsDay::succ4SuccInG(const BCTime &tmi, int niter, granularity granul)
{
    if(granul==DAYS) return succ4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	{
	    if(day_==1)
		ret = BCTime(BEG_YEAR,BEG_MONTH,31,1);
	    else {
		ret = BCTime::fillNew(granul,LV,3,BEG_YEAR,BEG_MONTH,day_);
		ret.nSuccessor(niter, granul);
	    }

	    if(!includes4SuccInG(ret, niter, granul))
		ret = succ4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, LV, ret);
    }
    
    return ret;
}

BCTime BCTmsDay::pred4SuccInG(const BCTime &tmi, int niter, granularity granul)
{
    if(granul==MONTHS) return pred4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End()) 
	{
	    if(day_==31)
		ret = BCTime::fillNew(granul,HV,3,END_YEAR,END_MONTH-2, day_);
	    else ret = BCTime::fillNew(granul,HV,3,END_YEAR,END_MONTH, day_);

	    ret.nSuccessor(niter, granul);

	    if(!includes4SuccInG(ret, niter, granul))
		ret = pred4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, HV, ret);
    }
    
    return ret;
}

BCTime BCTmsDay::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curDay = day_;
    int newDay = ((curDay - 1 + ndisp) % 31 ) + 1;
    if(newDay<=0) newDay += 31;

    day_ = newDay;
    ret = succ(tmi);
    day_ = curDay;

    return ret;
}

BCTime BCTmsDay::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curDay = day_;
    int newDay = ((curDay - 1 + ndisp) % 31 ) + 1;
    if(newDay<=0) newDay += 31;

    day_ = newDay;
    ret = pred(tmi);
    day_ = curDay;

    return ret;
}

bool BCTmsDay::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    if(ctms->getType() == CTmsDay)
    {
	int* day = (int*) ctms->getAttr();
	if((*day) != day_) ret = true;
    }
    else if(ctms->getType() == CTmsIntersection)
    {
	BCTmsBinary* ctmsBinary = static_cast<BCTmsBinary*>(ctms);
	if(isVoidIntersectedWith(ctmsBinary->getCTmsA()) ||
	   isVoidIntersectedWith(ctmsBinary->getCTmsB())) ret = true;
    }

    return ret;
}

bool BCTmsDay::isSuccableInG(int niter, granularity granul) const
{
    switch(granul) 
    {
	case DAYS:
	    return true;

	case HOURS:
	    if(abs(niter)>743) return false; 
	    else return true;

	case MINUTES:
	    if(abs(niter)>44639) return false; 
	    else return true;
	    
	case SECONDS:
	    if(abs(niter)>2678399) return false;
	    else return true;

	default: 
	    return false;
    }
}

int BCTmsDay::BinWrite(FILE* fil) const
{
    int bytesWritten = 0;
    int intType = static_cast<int>(type_);
    int intGranul = static_cast<int>(granul_);
    fwrite(&intType, 1, sizeof(int), fil);
    fwrite(&intGranul, 1, sizeof(int), fil);
    bytesWritten = (sizeof(int) * 2);
    if(!HasName()) {
	BText guion = "-";
	bytesWritten = guion.BinWrite(fil);
    } else
	bytesWritten = Name().BinWrite(fil);

    // specific attributes
    fwrite(&day_, 1, sizeof(int), fil);
    bytesWritten += sizeof(int);

    return bytesWritten;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsHour, "Hour", 1, 1, "Real", "(Real hour)",
	  I2("Returns the CTimeSet composed from all instants of time of the "
	     "hour given as a parameter.",
	     "Devuelve el CTimeSet compuesto por todos los instante de tiempo "
	     "de la hora dada como parámetro."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsHour::includes(const BCTime& tmi)
{
    if((tmi.getGranul()>HOURS) || (!tmi.isNormal()))
	return false;
    else return (hour_ == tmi.hour());
}

bool BCTmsHour::includes4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    bool belongs = false;

    BCTime aux(tmi, getGranul());
    BCTime pivot = this->pred(aux);
    if(!pivot.isNormal()) return belongs;

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

bool BCTmsHour::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    bool ret = false;
    int curHour = hour_;
    int newHour = (curHour + ndisp) % 24;
    if(newHour<0) newHour += 24;

    hour_ = newHour;
    ret = includes(tmi);
    hour_ = curHour;

    return ret;
}

BCTime BCTmsHour::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	    return BCTime(BEG_YEAR, BEG_MONTH, BEG_DAY+1, hour_);

	if(tmi==BCTime::End()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret) && ret.isNormal())
    {
	if((tmi.getGranul() <= HOURS) && 
	   (ret.day()       == tmi.day()) && 
	   (ret.hour()      >  hour_)) { ret.nSuccessor(1, DAYS); }

	long int year = ret.year();
	int month = ret.month();
	int day = ret.day();

	if(tmi.getGranul() >= DAYS)
	{
	    if     (ret.year() !=tmi.year())  { month = 0; day = 0; }
	    else if(ret.month()!=tmi.month()) {            day = 0; }
	}
	
	granularity granul = (tmi.getGranul()>HOURS) ? HOURS : tmi.getGranul();
	ret=BCTime::fillNew(granul,LV,4,year,month,day, hour_);
    }
    
    return ret;
}

BCTime BCTmsHour::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End())
	    return BCTime(END_YEAR, END_MONTH, END_DAY-1, hour_);

	if(tmi==BCTime::Begin()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    if(!ret.isNormal()) {ret = tmi; ret.setGranul(MONTHS); ret.successor(-1);}

    if(!includes(ret) && ret.isNormal())
    {
	if((tmi.getGranul() <= HOURS) && 
	   (ret.day()       == tmi.day()) &&
	   (ret.hour()      <  hour_)) { ret.nSuccessor(-1, DAYS); }
	
	long int year = ret.year();
	int month = ret.month();
	int day = ret.day();
	
	if(tmi.getGranul() >= DAYS)
	{
	    if     (ret.year() !=tmi.year())  { month = 0; day = 0; }
	    else if(ret.month()!=tmi.month()) {            day = 0; }
	}
	
	granularity granul = (tmi.getGranul()>HOURS) ? HOURS : tmi.getGranul();
	ret=BCTime::fillNew(granul,HV,4,year,month,day, hour_);
    }
    
    return ret;
}

BCTime BCTmsHour::succ4SuccInG(const BCTime &tmi, 
			       int niter, granularity granul)
{
    if(granul==DAYS) return succ4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	{
	    if(niter<0)
		ret = BCTime(BEG_YEAR, BEG_MONTH, BEG_DAY+1, 23, 59);
	    else ret = BCTime(BEG_YEAR, BEG_MONTH, BEG_DAY+1, hour_, 00);
	    ret.nSuccessor(niter, granul);

	    if(!includes4SuccInG(ret, niter, granul))
		ret = succ4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, LV, ret);
    }
    
    return ret;
}

BCTime BCTmsHour::pred4SuccInG(const BCTime &tmi, 
			       int niter, granularity granul)
{
    if(granul==MONTHS) return pred4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End()) 
	{
	    ret = BCTime(END_YEAR, END_MONTH, END_DAY-1, hour_, 59);

	    if(!includes4SuccInG(ret, niter, granul))
		ret = pred4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, HV, ret);
    }
    
    return ret;
}

BCTime BCTmsHour::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curHour = hour_;
    int newHour = (curHour + ndisp) % 24;
    if(newHour<0) newHour += 24;

    hour_ = newHour;
    ret = succ(tmi);
    hour_ = curHour;

    return ret;
}

BCTime BCTmsHour::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curHour = hour_;
    int newHour = (curHour + ndisp) % 24;
    if(newHour<0) newHour += 24;

    hour_ = newHour;
    ret = pred(tmi);
    hour_ = curHour;

    return ret;
}

bool BCTmsHour::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    if(ctms->getType() == CTmsHour)
    {
	int* hour = (int*) ctms->getAttr();
	if((*hour) != hour_) ret = true;
    }
    else if(ctms->getType() == CTmsIntersection)
    {
	BCTmsBinary* ctmsBinary = static_cast<BCTmsBinary*>(ctms);
	if(isVoidIntersectedWith(ctmsBinary->getCTmsA()) ||
	   isVoidIntersectedWith(ctmsBinary->getCTmsB())) ret = true;
    }

    return ret;
}

bool BCTmsHour::isSuccableInG(int niter, granularity granul) const
{
    switch(granul) 
    {
	case HOURS:
	    return true;

	case MINUTES:
	    if(abs(niter)>1439) return false; 
	    else return true;
	    
	case SECONDS:
	    if(abs(niter)>86399) return false;
	    else return true;

	default: 
	    return false;
    }
}

int BCTmsHour::BinWrite(FILE* fil) const
{
    int bytesWritten = 0;
    int intType = static_cast<int>(type_);
    int intGranul = static_cast<int>(granul_);
    fwrite(&intType, 1, sizeof(int), fil);
    fwrite(&intGranul, 1, sizeof(int), fil);
    bytesWritten = (sizeof(int) * 2);
    if(!HasName()) {
	BText guion = "-";
	bytesWritten = guion.BinWrite(fil);
    } else
	bytesWritten = Name().BinWrite(fil);

    // specific attributes
    fwrite(&hour_, 1, sizeof(int), fil);
    bytesWritten += sizeof(int);

    return bytesWritten;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsMinute, "Minute", 1, 1, "Real", "(Real minute)",
	  I2("Returns the CTimeSet composed from all instants of time of the "
	     "minute given as a parameter.",
	     "Devuelve el CTimeSet compuesto por todos los instante de tiempo "
	     "del minuto dado como parámetro."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsMinute::includes(const BCTime& tmi)
{
    if((tmi.getGranul()>MINUTES) || (!tmi.isNormal())) 
	return false;
    else return (minute_ == tmi.minute());
}

bool BCTmsMinute::includes4SuccInG(const BCTime &tmi,
				   int niter,granularity granul)
{
    bool belongs = false;

    BCTime aux(tmi, getGranul());
    BCTime pivot = this->pred(aux);
    if(!pivot.isNormal()) return belongs;

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

bool BCTmsMinute::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    bool ret = false;
    int curMinute = minute_;
    int newMinute = (curMinute + ndisp) % 60;
    if(newMinute<0) newMinute += 60;

    minute_ = newMinute;
    ret = includes(tmi);
    minute_ = curMinute;

    return ret;
}

BCTime BCTmsMinute::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	    return BCTime(BEG_YEAR, BEG_MONTH, BEG_DAY+1, BEG_HOUR, minute_);

	if(tmi==BCTime::End()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret) && ret.isNormal())
    {
	if((tmi.getGranul() <= MINUTES) && 
	   (ret.hour()      == tmi.hour()) &&
	   (ret.minute()    >  minute_)) { ret.nSuccessor(1, HOURS); }
	
	long int year = ret.year();
	int month = ret.month();
	int day = ret.day();
	int hour = ret.hour();
	
	if(tmi.getGranul() >= HOURS)
	{
	    if(ret.year() !=tmi.year())  { month = 0; day = 0; hour = -1; }
	    else if(ret.month()!=tmi.month()) {       day = 0; hour = -1; }
	    else if(ret.day()!=tmi.day()) {                    hour = -1; }
	}

	granularity granul = 
	    (tmi.getGranul()>MINUTES) ? MINUTES : tmi.getGranul();
	ret = BCTime::fillNew(granul, LV, 5, year, month, day, hour, minute_);
    }
    
    return ret;
}

BCTime BCTmsMinute::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End())
	    return BCTime(END_YEAR, END_MONTH, END_DAY-1, END_HOUR, minute_);

	if(tmi==BCTime::Begin()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    if(!ret.isNormal()) {ret = tmi; ret.setGranul(MONTHS); ret.successor(-1);}

    if(!includes(ret) && ret.isNormal())
    {
	if((tmi.getGranul() <= MINUTES) && 
	   (ret.hour()      == tmi.hour()) &&
	   (ret.minute()    <  minute_)) { ret.nSuccessor(-1, HOURS); }
	
	long int year = ret.year();
	int month = ret.month();
	int day = ret.day();
	int hour = ret.hour();
	
	if(tmi.getGranul() >= HOURS)
	{
	    if(ret.year() !=tmi.year())  { month = 0; day = 0; hour = -1; }
	    else if(ret.month()!=tmi.month()) {       day = 0; hour = -1; }
	    else if(ret.day()!=tmi.day()) {                    hour = -1; }
	}
	
	granularity granul = 
	    (tmi.getGranul()>MINUTES) ? MINUTES : tmi.getGranul();
	ret = BCTime::fillNew(granul, HV, 5, year, month, day, hour, minute_);
    }
    
    return ret;
}

BCTime BCTmsMinute::succ4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    if(granul==MINUTES) return succ4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	{
	    if(niter<0)
		ret=BCTime(BEG_YEAR,BEG_MONTH,BEG_DAY+1,BEG_HOUR,      59,59);
	    else ret=BCTime(BEG_YEAR,BEG_MONTH,BEG_DAY+1,BEG_HOUR,minute_,00);
	    ret.nSuccessor(niter, granul);

	    if(!includes4SuccInG(ret, niter, granul))
		ret = succ4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, LV, ret);
    }
    
    return ret;
}

BCTime BCTmsMinute::pred4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    if(granul==MINUTES) return pred4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End()) 
	{
	    if(niter<0) {
		ret=BCTime(END_YEAR,END_MONTH,END_DAY-1,END_HOUR,59,59);
		ret.nSuccessor(niter, granul);
	    } else 
		ret=BCTime(END_YEAR,END_MONTH,END_DAY-1,END_HOUR,minute_,59);

	    if(!includes4SuccInG(ret, niter, granul))
		ret = pred4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, HV, ret);
    }
    
    return ret;
}

BCTime BCTmsMinute::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curMinute = minute_;
    int newMinute = (curMinute + ndisp) % 60;
    if(newMinute<0) newMinute += 60;

    minute_ = newMinute;
    ret = succ(tmi);
    minute_ = curMinute;

    return ret;
}

BCTime BCTmsMinute::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curMinute = minute_;
    int newMinute = (curMinute + ndisp) % 60;
    if(newMinute<0) newMinute += 60;

    minute_ = newMinute;
    ret = pred(tmi);
    minute_ = curMinute;

    return ret;
}

bool BCTmsMinute::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    if(ctms->getType() == CTmsMinute)
    {
	int* minute = (int*) ctms->getAttr();
	if((*minute) != minute_) ret = true;
    }
    else if(ctms->getType() == CTmsIntersection)
    {
	BCTmsBinary* ctmsBinary = static_cast<BCTmsBinary*>(ctms);
	if(isVoidIntersectedWith(ctmsBinary->getCTmsA()) ||
	   isVoidIntersectedWith(ctmsBinary->getCTmsB())) ret = true;
    }

    return ret;
}

bool BCTmsMinute::isSuccableInG(int niter, granularity granul) const
{
    switch(granul) 
    {
	case MINUTES:
	    return true;
	    
	case SECONDS:
	    if(abs(niter)>3599) return false;
	    else return true;

	default: 
	    return false;
    }
}

int BCTmsMinute::BinWrite(FILE* fil) const
{
    int bytesWritten = 0;
    int intType = static_cast<int>(type_);
    int intGranul = static_cast<int>(granul_);
    fwrite(&intType, 1, sizeof(int), fil);
    fwrite(&intGranul, 1, sizeof(int), fil);
    bytesWritten = (sizeof(int) * 2);
    if(!HasName()) {
	BText guion = "-";
	bytesWritten = guion.BinWrite(fil);
    } else
	bytesWritten = Name().BinWrite(fil);

    // specific attributes
    fwrite(&minute_, 1, sizeof(int), fil);
    bytesWritten += sizeof(int);

    return bytesWritten;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsSecond, "Second", 1, 1, "Real", "(Real second)",
	  I2("Returns the CTimeSet composed with the second given.",
	     "Devuelve el CTimeSet compuesto por el segundo dado."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsSecond::includes(const BCTime& tmi)
{
    if((tmi.getGranul()>SECONDS) || (!tmi.isNormal())) 
	return false;
    else return (second_ == tmi.second());
}

bool BCTmsSecond::includes4SuccInG(const BCTime &tmi, 
				   int niter, granularity granul)
{ 
    return includes4SelfSucc(tmi, niter); 
}

bool BCTmsSecond::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    bool ret = false;
    int curSecond = second_;
    int newSecond = (curSecond + ndisp) % 60;
    if(newSecond<0) newSecond += 60;

    second_ = newSecond;
    ret = includes(tmi);
    second_ = curSecond;

    return ret;
}

BCTime BCTmsSecond::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	    return BCTime(BEG_YEAR, BEG_MONTH, BEG_DAY+1, 
			  BEG_HOUR, BEG_MINUTE, second_);
	
	if(tmi==BCTime::End()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret) && ret.isNormal())
    {
	if((tmi.getGranul() == SECONDS) && 
	   (ret.minute()    == tmi.minute()) &&
	   (ret.second()    >  second_)) { ret.nSuccessor(1, MINUTES); }

	long int yy = ret.year();
	int mm = ret.month();
	int dd = ret.day();
	int hh = ret.hour();
	int mi = ret.minute();

	if(tmi.getGranul() >= MINUTES)
	{
	    if(ret.year() !=tmi.year()) { mm = 0; dd = 0; hh = -1; mi = -1; }
	    else if(ret.month()!=tmi.month()) {   dd = 0; hh = -1; mi = -1; }
	    else if(ret.day()!=tmi.day()) {               hh = -1; mi = -1; }
	    else if(ret.hour()!=tmi.hour()) {                      mi = -1; }
	}
	
	granularity granul = 
	    (tmi.getGranul()>SECONDS) ? SECONDS : tmi.getGranul();
	ret = BCTime::fillNew(granul, LV, 6, yy, mm, dd, hh, mi, second_);
    }
    
    return ret;
}

BCTime BCTmsSecond::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End()) 
	    return BCTime(END_YEAR, END_MONTH, END_DAY-1, 
			  END_HOUR, END_MINUTE, second_);

	if(tmi==BCTime::Begin()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    if(!ret.isNormal()) {ret = tmi; ret.setGranul(MONTHS); ret.successor(-1);}

    if(!includes(ret) && ret.isNormal())
    {
	if((tmi.getGranul() == SECONDS) && 
	   (ret.minute()==tmi.minute()) &&
	   (ret.second()<second_)) { ret.nSuccessor(-1, MINUTES); }

	long int yy = ret.year();
	int mm = ret.month();
	int dd = ret.day();
	int hh = ret.hour();
	int mi = ret.minute();

	if(tmi.getGranul() >= MINUTES)
	{
	    if(ret.year() !=tmi.year()) { mm = 0; dd = 0; hh = -1; mi = -1; }
	    else if(ret.month()!=tmi.month()) {   dd = 0; hh = -1; mi = -1; }
	    else if(ret.day()!=tmi.day()) {               hh = -1; mi = -1; }
	    else if(ret.hour()!=tmi.hour()) {                      mi = -1; }
	}

	granularity granul = 
	    (tmi.getGranul()>SECONDS) ? SECONDS : tmi.getGranul();
	ret = BCTime::fillNew(granul, HV, 6, yy, mm, dd, hh, mi, second_);
    }
    
    return ret;
}

BCTime BCTmsSecond::succ4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{ 
    return succ4SelfSucc(tmi, niter); 
}

BCTime BCTmsSecond::pred4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{ 
    return pred4SelfSucc(tmi, niter); 
}

BCTime BCTmsSecond::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curSecond = second_;
    int newSecond = (curSecond + ndisp) % 60;
    if(newSecond<0) newSecond += 60;

    second_ = newSecond;
    ret = succ(tmi);
    second_ = curSecond;

    return ret;
}

BCTime BCTmsSecond::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curSecond = second_;
    int newSecond = (curSecond + ndisp) % 60;
    if(newSecond<0) newSecond += 60;

    second_ = newSecond;
    ret = pred(tmi);
    second_ = curSecond;

    return ret;
}

bool BCTmsSecond::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    if(ctms->getType() == CTmsSecond)
    {
	int* second = (int*) ctms->getAttr();
	if((*second) != second_) ret = true;
    }
    else if(ctms->getType() == CTmsIntersection)
    {
	BCTmsBinary* ctmsBinary = static_cast<BCTmsBinary*>(ctms);
	if(isVoidIntersectedWith(ctmsBinary->getCTmsA()) ||
	   isVoidIntersectedWith(ctmsBinary->getCTmsB())) ret = true;
    }

    return ret;
}

bool BCTmsSecond::isSuccableInG(int niter, granularity granul) const
{
    if(granul==SECONDS)
	return true;
    else return false;
}

int BCTmsSecond::BinWrite(FILE* fil) const
{
    int bytesWritten = 0;
    int intType = static_cast<int>(type_);
    int intGranul = static_cast<int>(granul_);
    fwrite(&intType, 1, sizeof(int), fil);
    fwrite(&intGranul, 1, sizeof(int), fil);
    bytesWritten = (sizeof(int) * 2);
    if(!HasName()) {
	BText guion = "-";
	bytesWritten = guion.BinWrite(fil);
    } else
	bytesWritten = Name().BinWrite(fil);

    // specific attributes
    fwrite(&second_, 1, sizeof(int), fil);
    bytesWritten += sizeof(int);

    return bytesWritten;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsWeekDay, "WD", 1, 1, "Real", "(Real weekday)",
	  I2("Returns the CTimeSet composed of all instants of time "
	     "that are the teenth day of the week. Parameter n should "
	     "be in a range between 1 (Monday) and 7 (Sunday).",
	     "Devuelve el CTimeSet compuesto por todos los instantes "
	     "de tiempo que son el enésimo día de la semana. El "
	     "parámetro n tiene que estar entre los valores 1 (Lunes) "
	     "y 7 (Domingo)."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsWeekDay::includes(const BCTime& tmi)
{
    if((tmi.getGranul()>DAYS) || (!tmi.isNormal())) 
	return false;
    else return (wd_ == tmi.dayOfWeek());
}

bool BCTmsWeekDay::includes4SuccInG(const BCTime &tmi, int niter,
				 granularity granul)
{
    bool belongs = false;

    BCTime aux(tmi, getGranul());
    BCTime pivot = this->pred(aux);
    if(!pivot.isNormal()) return belongs;

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

bool BCTmsWeekDay::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    bool ret = false;
    int curWD = wd_;
    int newWD = ((curWD - 1 + ndisp) % 7 ) + 1;
    if(newWD<0) newWD += 7;

    wd_ = newWD;
    ret = includes(tmi);
    wd_ = curWD;

    return ret;
}

BCTime BCTmsWeekDay::succ(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	    return BCTime(BEG_YEAR, BEG_MONTH, ((wd_==1) ? wd_+7 : wd_));
	
	if(tmi==BCTime::End()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(1);

    if(!includes(ret) && ret.isNormal())
    {
	long int year = ret.year();
	int month     = ret.month();
	int day       = ret.day();

	if(tmi.getGranul() >= MONTHS)
	{
	    day = 0;
	    if(ret.year()!=tmi.year())  month = 0;
	}

	granularity granul = (tmi.getGranul()>DAYS) ? DAYS : tmi.getGranul();
	
	ret = BCTime::fillNew(granul, LV, 3, year, month, day);
	int wd = ret.dayOfWeek();
	
	if     (wd < wd_) ret.add(0, 0, (wd_ - wd    ), 0, 0, 0); 
	else if(wd > wd_) ret.add(0, 0, (wd_ - wd + 7), 0, 0, 0); 
    }
    
    return ret;
}

BCTime BCTmsWeekDay::pred(const BCTime& tmi)
{
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End())
	    return BCTime(END_YEAR, END_MONTH, ((wd_>5) ? 19+wd_ : 26+wd_));

	if(tmi==BCTime::Begin()) return tmi; 
    }
    //--------------------------------------------------
    BCTime ret(tmi);
    ret.successor(-1);
    if(!ret.isNormal()) {ret = tmi; ret.setGranul(DAYS); ret.successor(-1);}

    if(!includes(ret) && ret.isNormal())
    {
	long int year = ret.year();
	int month     = ret.month();
	int day       = ret.day();

	if(tmi.getGranul() >= MONTHS)
	{
	    day = 0;
	    if(ret.year()!=tmi.year())  month = 0;
	}

	granularity granul = (tmi.getGranul()>DAYS) ? DAYS : tmi.getGranul();
	
	ret = BCTime::fillNew(granul, HV, 3, year, month, day);
	int wd = ret.dayOfWeek();

	if     (wd < wd_) ret.add(0, 0, (wd_ - wd - 7), 0, 0, 0);
	else if(wd > wd_) ret.add(0, 0, (wd_ - wd    ), 0, 0, 0);   
    }
    
    return ret;
}

BCTime BCTmsWeekDay::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curWD = wd_;
    int newWD = ((curWD - 1 + ndisp) % 7 ) + 1;
    if(newWD<0) newWD += 7;

    wd_ = newWD;
    ret = succ(tmi);
    wd_ = curWD;

    return ret;
}

BCTime BCTmsWeekDay::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime ret = tmi;
    int curWD = wd_;
    int newWD = ((curWD - 1 + ndisp) % 7 ) + 1;
    if(newWD<0) newWD += 7;

    wd_ = newWD;
    ret = pred(tmi);
    wd_ = curWD;

    return ret;
}

BCTime BCTmsWeekDay::succ4SuccInG(const BCTime &tmi, 
				  int niter, granularity granul)
{
    if(granul==DAYS) return succ4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::Begin())
	{
	    if(niter>0)
	    {
		ret = BCTime::fillNew(granul, LV, 3, BEG_YEAR, BEG_MONTH, 
				      ((wd_==1) ? wd_+7 : wd_));
		ret.nSuccessor(niter, granul);
	    } else ret = BCTime(BEG_YEAR, BEG_MONTH, 2);

	    if(!includes4SuccInG(ret, niter, granul))
		ret = succ4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, LV, ret);
    }
    
    return ret;
}

BCTime BCTmsWeekDay::pred4SuccInG(const BCTime &tmi, 
				  int niter, granularity granul)
{
    if(granul==MONTHS) return pred4SelfSucc(tmi, niter);

    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) 
    {
	if(tmi==BCTime::End()) 
	{
	    if(niter<0)
	    {
		ret = BCTime::fillNew(granul, HV, 3, END_YEAR, END_MONTH, 
				      ((wd_>5) ? 19+wd_ : 26+wd_));
		ret.nSuccessor(niter, granul);
	    }
	    else ret = BCTime(END_YEAR, END_MONTH, 30);

	    if(!includes4SuccInG(ret, niter, granul))
		ret = pred4SuccInG(ret, niter, granul);
	}

	return ret;
    }
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
	else if(ret.getGranul()>g) ret = BCTime::fillNew(g, HV, ret);
    }
    
    return ret;
}

bool BCTmsWeekDay::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    bool ret = false;
    if(ctms->getType() == CTmsWeekDay)
    {
	int* wd = (int*) ctms->getAttr();
	if((*wd) != wd_) ret = true;
    }
    else if(ctms->getType() == CTmsIntersection)
    {
	BCTmsBinary* ctmsBinary = static_cast<BCTmsBinary*>(ctms);
	if(isVoidIntersectedWith(ctmsBinary->getCTmsA()) ||
	   isVoidIntersectedWith(ctmsBinary->getCTmsB())) ret = true;
    }

    return ret;
}

bool BCTmsWeekDay::isSuccableInG(int niter, granularity granul) const
{
    switch(granul) 
    {
	case DAYS:
	    return true;

	case HOURS:
	    if(abs(niter)>167) return false; 
	    else return true;

	case MINUTES:
	    if(abs(niter)>10019) return false; 
	    else return true;
	    
	case SECONDS:
	    if(abs(niter)>601199) return false;
	    else return true;

	default: 
	    return false;
    }
}

int BCTmsWeekDay::BinWrite(FILE* fil) const
{
    int bytesWritten = 0;
    int intType = static_cast<int>(type_);
    int intGranul = static_cast<int>(granul_);
    fwrite(&intType, 1, sizeof(int), fil);
    fwrite(&intGranul, 1, sizeof(int), fil);
    bytesWritten = (sizeof(int) * 2);
    if(!HasName()) {
	BText guion = "-";
	bytesWritten = guion.BinWrite(fil);
    } else
	bytesWritten = Name().BinWrite(fil);

    // specific attributes
    fwrite(&wd_, 1, sizeof(int), fil);
    bytesWritten += sizeof(int);

    return bytesWritten;
}

#endif /* __USE_TC__ */

