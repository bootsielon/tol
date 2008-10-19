/* ctmsgra.cpp - CTimeSet Tol Data Type native function implementation.
 *               TOL Language
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

#include <tol/tol_bctmsgra.h>
#include <tol/tol_bctmsfnit.h>
#include <tol/tol_bctsrgra.h>
#include <tol/tol_bctmsbas.h>

#if defined(__USE_TC__)

template<>
BGrammar* BUserCTimeSet::ownGrammar_ = NIL;

//- Required Data Type Behaviour -------------------------------------
template<>
BDat BUserCTimeSet::Compare(const BSyntaxObject* obj1,
			    const BSyntaxObject* obj2)
{
    BText name1 = obj1->Name();
    BText name2 = obj2->Name();
    return((name1<name2)?-1:(name1>name2)?1:0);    
}

template<>
void BUserCTimeSet::InitInstances()
{
    BCTmsVoid*	     void_	= new BCTmsVoid("CTEmpty");

    BCTmsWholeTime* ctYears_ = new BCTmsWholeTime("CTInYears", YEARS);
    BCTmsWholeTime* ctMonths_ = new BCTmsWholeTime("CTInMonths", MONTHS);
    BCTmsWholeTime* ctDays_ = new BCTmsWholeTime("CTInDays", DAYS);
    BCTmsWholeTime* ctHours_ = new BCTmsWholeTime("CTInHours", HOURS);
    BCTmsWholeTime* ctMinutes_ = new BCTmsWholeTime("CTInMinutes", MINUTES);
    BCTmsWholeTime* ctSeconds_ = new BCTmsWholeTime("CTInSeconds", SECONDS);

    BCTmsEaster*    ceaster_ = new BCTmsEaster("CTSEaster");
    OwnGrammar()->PutDefect(void_);
}

template<>
BSyntaxObject* BUserCTimeSet::FindConstant(const BText&)
{ return(NIL); }

template<>
BSyntaxObject* BUserCTimeSet::Casting(BSyntaxObject* obj)
{
    if(!obj)			        { return(NIL); }
    if(obj->Grammar()==OwnGrammar())    { return(obj); }

    if(obj->Grammar()->Name()=="Time") { 
	return(new BCTmsInside(NCons(obj))); 
    }

    return(NIL);
}

//--------------------------------------------------------------------
// BCTmsRenamed
/*
BSyntaxObject* BCTmsRenamed::New(FILE* fil)
{
    BUserCTimeSet* ret = 0;
    BCTmsTemporary* ctms;
    int intType, intGranul;
    tmsType type;
    granularity granul;
    BText name;

    fread(&intType, 1, sizeof(int), fil);
    type = static_cast<tmsType>(intType);
    fread(&intGranul, 1, sizeof(int), fil);
    granul = static_cast<granularity>(granul);
    name.BinRead(fil);
    if(name=="-") name = "";

    try 
    {
	switch(type)
	{
	    case CTmsWholeTime:
		ret = new BCTmsWholeTime(name, granul);		
		break;
	    case CTmsYear:
		ctms = new BCTmsYear(name, "", fil);
		ret = CTms(ctms);
		break;
	    case CTmsMonth:
		ctms = new BCTmsMonth(name, "", fil);
		ret = CTms(ctms);
		break;
	    case CTmsWeekDay:
		ctms = new BCTmsWeekDay(name, "", fil);
		ret = CTms(ctms);
		break;
	    case CTmsDay:
		ctms = new BCTmsDay(name, "", fil);
		ret = CTms(ctms);
		break;
	    case CTmsHour:
		ctms = new BCTmsHour(name, "", fil);
		ret = CTms(ctms);
		break;
	    case CTmsMinute:
		ctms = new BCTmsHour(name, "", fil);
		ret = CTms(ctms);
		break;
	    case CTmsSecond:
		ctms = new BCTmsHour(name, "", fil);
		ret = CTms(ctms);
		break;
	}
    } catch(BEmptyCTimeSetException &exc) {
        if(BGrammar::Level()==0)
	    Std(I2(exc.enReason_.c_str(), exc.esReason_.c_str()));
        ret = new BCTmsVoid("");
    } catch(BException& exc) {
	Std(I2(exc.enReason_.c_str(), exc.esReason_.c_str()));
	ret = 0;
    } catch(...) {
        ret = 0;
    }

    ret->PutName(name);
    OwnGrammar()->AddObject(ret);
    return ret;
}
*/
//--------------------------------------------------------------------
BCTime BCTmsWholeTime::succ(const BCTime& tmi)
{
    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) {
	if(tmi==BCTime::Begin()) 
	    ret = BCTime::fillNew(getGranul(), LV, 6,
				  BEG_YEAR, BEG_MONTH,  BEG_DAY+1,
				  BEG_HOUR, BEG_MINUTE, BEG_SECOND);
	
	return ret;
    }
    //--------------------------------------------------
    ret.successor(1);

    if(!includes(ret) && ret.isNormal()) 
	if(ret.getGranul()>getGranul()) 
	    ret.setGranul(getGranul());
    
    return ret;
}

BCTime BCTmsWholeTime::pred(const BCTime& tmi)
{
    BCTime ret(tmi);
    //--------------------------------------------------
    if(!tmi.isNormal()) {
	if(tmi==BCTime::End())
	    ret = BCTime::fillNew(getGranul(), HV, 6,
				  END_YEAR, END_MONTH,  END_DAY-1,
				  END_HOUR, END_MINUTE, END_SECOND);

	return ret;
    }
    //--------------------------------------------------
    ret.successor(-1);

    if(!includes(ret) && ret.isNormal()) 
	if(ret.getGranul()>getGranul())
	{
	    ret = tmi;
	    ret.setGranul(getGranul());
	    ret.successor(-1);
	}
    
    return ret;
}

int BCTmsWholeTime::BinWrite(FILE* fil) const
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

    return bytesWritten;
}

//--------------------------------------------------------------------
/*! Returns true if BCTime argument \a tmi is an Easter Day.
 * All instants of time before 1582 are based on the Julian Calendar.
 * Instants of time after 1582 are based on Gregorian Calendar.
 */
bool BCTmsEaster::includes(const BCTime &tmi)
{
    if((tmi==BCTime::Easter(tmi.year())) || 
       BCTime::Easter(tmi.year()).includes(tmi))
	return true;
    else return false;
}

bool BCTmsEaster::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    BCTime auxtmi = tmi;
    auxtmi.nSuccessor(-ndisp, DAYS);
    return includes(auxtmi);
}

bool BCTmsEaster::includes4SuccInG(const BCTime &tmi, 
				   int niter, granularity granul)
{
    if(granul>DAYS) return false;
    else if(granul==DAYS) return includes4SelfSucc(tmi, niter);
    else 
    {
	BCTime auxtmi = tmi;
	auxtmi.nSuccessor(-niter, granul);
	return includes(auxtmi);	
    }
}

//--------------------------------------------------
/*! Returns the next (or previous) Easter BCTime that 
 * represents the successor (or predecessor) of the 
 * BCTime \a tmi given as argument. 
 */
BCTime BCTmsEaster::succ(const BCTime &tmi)
{
    long int year = tmi.year();
    BCTime easter = BCTime::Easter(year);

    if(((tmi.getGranul()<=DAYS) && (tmi>=easter)) || 
       ((tmi.getGranul()==MONTHS) && (tmi.month()==easter.month())) ||
       (tmi.getGranul()==YEARS))
    { year += 1; }

    if(tmi.getGranul()<DAYS)
    {
	if(easter.includes(tmi)) 
	{
	    BCTime newTmi = tmi;
	    newTmi.successor(1);

	    if(easter.includes(newTmi)) 
		return newTmi;
	    else return succ(newTmi);
	} 
	else
	    return BCTime::fillNew(tmi.getGranul(),LV,BCTime::Easter(year));
    }
    else return BCTime::Easter(year);
}

BCTime BCTmsEaster::pred(const BCTime &tmi)
{
    long int year = tmi.year();
    BCTime easter = BCTime::Easter(year);

    if(((tmi.getGranul()<=DAYS) && (tmi<=easter)) || 
       ((tmi.getGranul()==MONTHS) && (tmi.month()==easter.month())) ||
       (tmi.getGranul()==YEARS))
    { year -= 1; }

    if(tmi.getGranul()<DAYS)
    {
	if(easter.includes(tmi)) 
	{
	    BCTime newTmi = tmi;
	    newTmi.successor(-1);

	    if(easter.includes(newTmi)) 
		return newTmi;
	    else return pred(newTmi);
	} 
	else
	    return BCTime::fillNew(tmi.getGranul(),HV,BCTime::Easter(year));
    }
    else return BCTime::Easter(year);
}

BCTime BCTmsEaster::succ4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    //--------------------------------------------------
    if(granul==DAYS) return succ4SelfSucc(tmi, niter);
    //--------------------------------------------------
    long int year = tmi.year();
    BCTime easterIterIni = BCTime(BCTime::Easter(year), granul);
    easterIterIni.nSuccessor(niter, granul);
    BCTime easterIterEnd = BCTime(easterIterIni);
    easterIterEnd.nSuccessor(1, DAYS);
    easterIterEnd.nSuccessor(-1, granul);

    if(tmi.getGranul()>DAYS)
    {
	if((tmi.getGranul()==YEARS) ||
	   ((tmi.getGranul()==MONTHS) && (tmi.month()>=easterIterEnd.month())))
	{ year++; }

	BCTime newTmi(BCTime::Easter(year), granul);
	newTmi.nSuccessor(niter, granul);
	return newTmi;
    }
    else if(tmi.getGranul()<=DAYS)
    {
	granularity g = (granul<tmi.getGranul()) ? granul : tmi.getGranul();
	BCTime ret(tmi, g);
	ret.nSuccessor(1, tmi.getGranul());

	if((easterIterIni.startsBeforeEQ(ret)) && 
	   (easterIterEnd.endsAfterEQ(ret))) 
	{ return ret; }
	else
	{
	    if(ret.endsAfter(easterIterEnd)) year++;

	    BCTime newTmi(BCTime::Easter(year), g);
	    newTmi.nSuccessor(niter, granul);
	    return newTmi;
	}
    }
}

BCTime BCTmsEaster::pred4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    //--------------------------------------------------
    if(granul==DAYS) return pred4SelfSucc(tmi, niter);
    //--------------------------------------------------
    long int year = tmi.year();
    BCTime easterIterIni = BCTime(BCTime::Easter(year), granul);
    easterIterIni.nSuccessor(niter, granul);
    BCTime easterIterEnd = BCTime(easterIterIni);
    easterIterEnd.nSuccessor(1, DAYS);
    easterIterEnd.nSuccessor(-1, granul);

    if(tmi.getGranul()>DAYS)
    {
	if((tmi.getGranul()==YEARS) ||
	   ((tmi.getGranul()==MONTHS) && (tmi.month()<=easterIterIni.month())))
	{ year--; }

	BCTime auxTmi(BCTime::Easter(year), granul);
	auxTmi.nSuccessor(niter, granul);
	BCTime newTmi = BCTime(auxTmi);
	newTmi.nSuccessor(1, DAYS);
	newTmi.nSuccessor(-1, granul);
	return newTmi;
    }
    else if(tmi.getGranul()<=DAYS)
    {
	granularity g = (granul<tmi.getGranul()) ? granul : tmi.getGranul();
	BCTime ret(tmi, g);

	if(tmi.getGranul()==DAYS)
	    ret.nSuccessor(-1, granul);
	else ret.nSuccessor(-1, tmi.getGranul());

	if((easterIterIni.startsBeforeEQ(ret)) && 
	   (easterIterEnd.endsAfterEQ(ret))) 
	{ return ret; }
	else
	{
	    if(ret.startsBefore(easterIterIni)) year--;

	    BCTime auxTmi(BCTime::Easter(year), g);
	    auxTmi.nSuccessor(niter, granul);
	    BCTime newTmi = BCTime(auxTmi);
	    newTmi.nSuccessor(1, DAYS);
	    newTmi.nSuccessor(-1, granul);
	    return newTmi;
	}
    }
}

BCTime BCTmsEaster::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    long int year = tmi.year();
    BCTime easterSuc = BCTime::Easter(year);
    easterSuc.successor(ndisp);

    if(((tmi.getGranul()<=DAYS) && (tmi>=easterSuc)) || 
       ((tmi.getGranul()==MONTHS) && (tmi.month()==easterSuc.month())) ||
       (tmi.getGranul()==YEARS))
    { year += 1; }

    if(tmi.getGranul()<DAYS)
    {
	if(easterSuc.includes(tmi)) 
	{
	    BCTime newTmi = tmi;
	    newTmi.successor(1);

	    if(easterSuc.includes(newTmi)) 
		return newTmi;
	    else return succ4SelfSucc(newTmi, ndisp);
	} 
	else
	{
	    BCTime newTmi(BCTime::Easter(year));
	    newTmi.successor(ndisp);
	    return BCTime::fillNew(tmi.getGranul(), LV, newTmi);
	}
    }
    else 
	{
	    BCTime newTmi(BCTime::Easter(year));
	    newTmi.successor(ndisp);
	    return newTmi;
	}
}

BCTime BCTmsEaster::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    long int year = tmi.year();
    BCTime easterSuc = BCTime::Easter(year);
    easterSuc.successor(ndisp);

    if(((tmi.getGranul()<=DAYS) && (tmi<=easterSuc)) || 
       ((tmi.getGranul()==MONTHS) && (tmi.month()==easterSuc.month())) ||
       (tmi.getGranul()==YEARS))
    { year -= 1; }

    if(tmi.getGranul()<DAYS)
    {
	if(easterSuc.includes(tmi)) 
	{
	    BCTime newTmi = tmi;
	    newTmi.successor(-1);

	    if(easterSuc.includes(newTmi)) 
		return newTmi;
	    else return pred4SelfSucc(newTmi, ndisp);
	} 
	else
	{
	    BCTime newTmi(BCTime::Easter(year));
	    newTmi.successor(ndisp);
	    return BCTime::fillNew(tmi.getGranul(), HV, newTmi);
	}
    }
    else 
	{
	    BCTime newTmi(BCTime::Easter(year));
	    newTmi.successor(ndisp);
	    return newTmi;
	}
}

bool BCTmsEaster::isSuccableInG(int niter, granularity granul) const
{
    switch(granul) 
    {
	case YEARS:
	case MONTHS:
	case DAYS:
	    return false;

	case HOURS:
	    if(niter>8759) return false; 
	    else return true;

	case MINUTES:
	    if(niter>525599) return false; 
	    else return true;

	case SECONDS:
	    if(niter>31535999) return false; 
	    else return true;
    }
}

//--------------------------------------------------------------------
/*! CTimeSet that represents an specific CDate
 */
class BCTmsOfSer: public BCTmsTemporary
{
 private:
    BUserCTimeSeries *uctsr_;

 public:
    BCTmsOfSer(BList* arg)
	: BCTmsTemporary(arg)
	{
	    uctsr_ = CTsr(Arg(1));
	    setGranul(uctsr_->Dating()->getGranul());
	    setType(CTmsOfSer);
	}
    
    ~BCTmsOfSer() {}

    void* getAttr() { return &uctsr_; }

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
DeclareCTimeSetEvaluator(BCTmsOfSer);

//--------------------------------------------------------------------
DefExtOpr(1, BCTmsOfSer, "TmsOfSer", 1, 1, "CSeries", "(CSeries ser)",
	  I2("Returns the time set of all the dates of the dating of a "
	     "series in those which this is not annulled.",
	     "Devuelve el conjunto temporal de todas las fechas del fechado "
	     "de una serie en las que ésta no se anula."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
bool BCTmsOfSer::includes(const BCTime& tmi)
{ 
    if(uctsr_->Dating() && 
       uctsr_->Dating()->includes(tmi) &&
       uctsr_->FirstCTime().startsBeforeEQ(tmi) && 
       uctsr_->LastCTime().endsAfterEQ(tmi))
	return true; 
    else return false; 
}

bool BCTmsOfSer::includes4SuccInG(const BCTime &tmi, 
				   int niter, granularity granul)
{
    if(uctsr_->Dating() && 
       uctsr_->Dating()->includes4SuccInG(tmi, niter, granul) &&
       uctsr_->FirstCTime().startsBeforeEQ(tmi) && 
       uctsr_->LastCTime().endsAfterEQ(tmi))
	return true; 
    else return false; 
}

bool BCTmsOfSer::includes4SelfSucc(const BCTime &tmi, int ndisp)
{
    if(uctsr_->Dating() && 
       uctsr_->Dating()->includes4SelfSucc(tmi, ndisp) &&
       uctsr_->FirstCTime().startsBeforeEQ(tmi) && 
       uctsr_->LastCTime().endsAfterEQ(tmi))
	return true; 
    else return false; 
}

BCTime BCTmsOfSer::succ(const BCTime& tmi)
{
    if(uctsr_->Dating()) return uctsr_->Dating()->succ(tmi); 
}

BCTime BCTmsOfSer::pred(const BCTime& tmi)
{
    if(uctsr_->Dating()) return uctsr_->Dating()->pred(tmi); 
}

BCTime BCTmsOfSer::succ4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    if(uctsr_->Dating()) 
	return uctsr_->Dating()->succ4SuccInG(tmi, niter, granul); 
}

BCTime BCTmsOfSer::pred4SuccInG(const BCTime &tmi, 
				 int niter, granularity granul)
{
    if(uctsr_->Dating()) 
	return uctsr_->Dating()->pred4SuccInG(tmi, niter, granul); 
}

BCTime BCTmsOfSer::succ4SelfSucc(const BCTime &tmi, int ndisp)
{
    if(uctsr_->Dating()) return uctsr_->Dating()->succ4SelfSucc(tmi, ndisp); 
}

BCTime BCTmsOfSer::pred4SelfSucc(const BCTime &tmi, int ndisp)
{
    if(uctsr_->Dating()) return uctsr_->Dating()->pred4SelfSucc(tmi, ndisp); 
}

bool BCTmsOfSer::isVoidIntersectedWith(BCTimeSet *ctms) const
{
    if(uctsr_->Dating()) 
	return uctsr_->Dating()->isVoidIntersectedWith(ctms); 
}

bool BCTmsOfSer::isSuccableInG(int niter, granularity granul) const
{
    if(uctsr_->Dating()) 
	return uctsr_->Dating()->isSuccableInG(niter, granul); 
}

#endif /* __USE_TC__ */
