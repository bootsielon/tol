/* tol_bctmsspec.h - CTimeSet Tol Data Type creation.
 *                   Special Operations: Succ, Range and Periodic
 *
 * Copyright (C) 2005 - Bayes Decision, SL
 * author: Daniel Rus <danirus@tol-project.org>
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

#ifndef TOL_BCTMSSPEC_H
#define TOL_BCTMSSPEC_H 1

#include <tol/tol_cbtol.h>

#include <tol/tol_bctime.h>
#include <tol/tol_bctmigra.h>

#include <tol/tol_bctimeset.h>
#include <tol/tol_bctmsgra.h>
#include <tol/tol_bctmsbas.h>
#include <tol/tol_bctmsbool.h>
#include <tol/tol_btxtgra.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
class BCTmsSucc : public BCTmsTemporary
{
 protected:
    BUserCTimeSet *ct_, *f_;
    int ndisp_;

 public:
    BCTmsSucc(BList *arg) : BCTmsTemporary(arg) 
	{
	    ct_    = CTms(Arg(1));
	    ndisp_ = static_cast<int>(Real(Arg(2)));
	    f_     = CTms(Arg(3));

	    if(ct_->getGranul() < f_->getGranul())
		granul_ = ct_->getGranul();
	    else granul_ = f_->getGranul();

	    setType(CTmsSucc);
	}
    
    ~BCTmsSucc() {}

    bool includes(const BCTime &tmi);
    BCTime succ(const BCTime &tmi);
    BCTime pred(const BCTime &tmi);
};

DeclareCTimeSetEvaluator(BCTmsSucc);

//--------------------------------------------------------------------
class BCTmsSelfSucc : public BCTmsTemporary
{
 protected:
    BUserCTimeSet* ctms_;
    int ndisp_;
    
 public:
    BCTmsSelfSucc(BList *arg) : BCTmsTemporary(arg) 
	{
	    ctms_ = CTms(Arg(1));
	    ndisp_ = static_cast<int>(Real(Arg(2)));
	    setGranul(ctms_->getGranul());
	    setType(CTmsSelfSucc);
	}
    
    ~BCTmsSelfSucc() {}

    bool includes(const BCTime &tmi);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);
};

DeclareCTimeSetEvaluator(BCTmsSelfSucc);

//--------------------------------------------------------------------
class BCTmsSuccInG : public BCTmsTemporary
{
 protected:
    BUserCTimeSet* ctms_;
    int niter_;
    
 public:
    BCTmsSuccInG(BList *arg) : BCTmsTemporary(arg) 
	{
	    ctms_ = CTms(Arg(1));
	    niter_ = static_cast<int>(Real(Arg(2)));
	    char *granul = Text(Arg(3)).Buffer();

	    if(!granul) 
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong values for 3th parameter. " << endl 
		      << "granularity must be on the following values:" << endl
		      << "\"YEARS\", \"MONTHS\", \"DAYS\", "
		      << "\"HOURS\", \"MINUTES\", \"SECONDS\"" << endl;
		ES_ss << "Valor erróneo en el 3er parámetro. " << endl 
		      << "La granularidad debe tomar alguno de los "
		      << "siguientes valores:" << endl
		      << "\"YEARS\", \"MONTHS\", \"DAYS\", "
		      << "\"HOURS\", \"MINUTES\", \"SECONDS\"" << endl;
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    if(!strcmp(granul, "YEARS")) setGranul(YEARS);
	    else if(!strcmp(granul, "MONTHS")) setGranul(MONTHS);
	    else if(!strcmp(granul, "DAYS")) setGranul(DAYS);
	    else if(!strcmp(granul, "HOURS")) setGranul(HOURS);
	    else if(!strcmp(granul, "MINUTES")) setGranul(MINUTES);
	    else if(!strcmp(granul, "SECONDS")) setGranul(SECONDS);

	    if(getGranul() > ctms_->getGranul())
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong values for 3th parameter. " << endl 
		      << "granularity must be less or equal to "
		      << "granularity of 1st. parameter ctms." << endl
		      << "1st. parameter (" << ctms_->Name().String() 
		      << ") granularity is "
		      << BCTime::editedGranul(ctms_->getGranul()) << endl;
		ES_ss << "Valor erróneo en el 3er parámetro. " << endl 
		      << "La granularidad debe ser menor o igual que la "
		      << "granularidad del primer parámetro. " << endl
		      << "El 1er. parámetro, " << ctms_->Name().String() 
		      << " tiene granularidad "
		      << BCTime::editedGranul(ctms_->getGranul()) << endl;
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    if(!ctms_->isSuccableInG(niter_, getGranul()))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for 2nd or 3th parameter. " << endl 
		      << "Both niter and granularity values together "
		      << "overheads the given CTimeSet." << endl
		      << "It is possible to get the same result using "
		      << "the same CTimeSet with different parameters, and "
		      << "reducing the value for niter." << endl;
		ES_ss << "Valor erróneo en el 2º o 3er parámetro. " << endl 
		      << "La combinación de valores para niter y granularity "
		      << "sobrepasan el CTimeSet original." << endl
		      << "Es posible obtener el mismo resultado modificando "
		      << "la definición del CTimeSet y reduciendo el valor "
		      << "del parámetro niter." << endl;
		throw BException(EN_ss.str(), ES_ss.str());
	    }

	    setType(CTmsSuccInG);
	}
    
    ~BCTmsSuccInG() {}

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    bool succ4Succ(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    bool pred4Succ(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);
};

DeclareCTimeSetEvaluator(BCTmsSuccInG);


//--------------------------------------------------------------------
class BCTmsSelfRange : public BCTmsTemporary
{
 protected:
    BUserCTimeSet* ctms_;
    int ini_, end_;
    
 public:
    BCTmsSelfRange(BList *arg) : BCTmsTemporary(arg) 
	{
	    ctms_ = CTms(Arg(1));
	    ini_ = static_cast<int>(Real(Arg(2)));
	    end_ = static_cast<int>(Real(Arg(3)));

	    if(ini_>end_)
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong values for 2nd and 3th parameters. " << endl 
		      << "iniIter: " << ini_  << ", endIter: " << end_ 
		      << endl 
		      << "endIter must be greater or equal than iniIter.";
		ES_ss << "Valores erróneos para el 2o. y 3er. parámetro." 
		      << endl 
		      << "iniIter: " << ini_  << ", endIter: " << end_ 
		      << endl 
		      << "endIter debe ser mayor o igual que iniIter.";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(ctms_->getGranul());
	    setType(CTmsRange);
	}
    
    ~BCTmsSelfRange() {}

    bool includes(const BCTime &tmi);
//    bool includes4SuccInG(const BCTime &tmi, int ndisp);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);
};

DeclareCTimeSetEvaluator(BCTmsSelfRange);

//--------------------------------------------------------------------
class BCTmsPeriodic : public BCTmsTemporary
{
 protected:
    BCTime startTmi_;
    BUserCTimeSet* ctms_;
    int period_;
    
 public:
    BCTmsPeriodic(BList *arg) : BCTmsTemporary(arg) 
	{
	    startTmi_ = CTime(Arg(1));
	    period_ = static_cast<int>(Real(Arg(2)));

	    if(Arg(3)) 
	    {
		ctms_ = CTms(Arg(3));
		setGranul(ctms_->getGranul());
	    }
	    else 
	    {
		ctms_ = CTms(GraCTimeSet()->FindVariable("CTSAll"));
		setGranul(startTmi_.getGranul());
	    }
	    
	    if(!ctms_->includes(startTmi_)) 
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "1st. parameter wrong; must be part of parameter "
		      << " CTimeSet units.";
		ES_ss << "Primer parámetro erróneo; debe ser parte del "
		      << "parámetro CTimeSet units.";
		throw BException(EN_ss.str(), ES_ss.str());
	    }

	    setType(CTmsPeriodic);
	}
    
    ~BCTmsPeriodic() {}

    bool hasGrowableGranul() const { return true; }

    bool includes(const BCTime &tmi);
//    bool includes4SuccInG(const BCTime &tmi, int ndisp);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);
};

DeclareCTimeSetEvaluator(BCTmsPeriodic);

//--------------------------------------------------------------------
class BCTmsSelfPeriodic : public BCTmsTemporary
{
 protected:
    BUserCTimeSet* ctms_;
    int period_;
    
 public:
    BCTmsSelfPeriodic(BList *arg) : BCTmsTemporary(arg) 
	{
	    ctms_ = CTms(Arg(1));

	    if((ctms_->getType()<CTmsMonth) || (ctms_->getType()>CTmsSecond))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong values for 1st. parameter." << endl 
		      << "CTimeSet ctms must be one of the following:" << endl
		      << "cMonth(m), cDay(m), cHour(m), "
		      << "cMinute(m), cSecond(m), cWD(m) " << endl;
		ES_ss << "Valor erróneo para el 1er. parámetro." 
		      << "CTimeSet ctms tiene que ser uno de los "
		      << "siguientes:" << endl
		      << "cMonth(m), cDay(m), cHour(m), "
		      << "cMinute(m), cSecond(m), cWD(m) " << endl;
		throw BException(EN_ss.str(), ES_ss.str());
	    }

	    setGranul(ctms_->getGranul());
	    period_ = static_cast<int>(Real(Arg(2)));
	    setType(CTmsSelfPeriodic);
	}
    
    ~BCTmsSelfPeriodic() {}

    bool includes(const BCTime &tmi);
//    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);
};

DeclareCTimeSetEvaluator(BCTmsSelfPeriodic);

#endif /* __USE_TC__ */
#endif /* TOL_BCTMSSPEC_H */
