/* tol_bctmsbool.h - CTimeSet Tol Data Type creation.
 *    Boolean Operations: Union (+), Intersection(*) and Difference(-)
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

#ifndef TOL_BCTMSBOOL_H
#define TOL_BCTMSBOOL_H 1

#include <tol/tol_bctime.h>
#include <tol/tol_cbtol.h>
#include <tol/tol_bctimeset.h>
#include <tol/tol_bctmsgra.h>
#include <tol/tol_bctmigra.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
class BCTmsBinary : public BCTmsTemporary
{
 protected:
    BUserCTimeSet* ctmsA_;
    BUserCTimeSet* ctmsB_;

 public:
    BCTmsBinary(BList *arg) : BCTmsTemporary(arg) 
	{
	    ctmsA_ = CTms(Arg(1));
	    ctmsB_ = CTms(Arg(2));
	    
	    if(ctmsA_->getGranul() < ctmsB_->getGranul())
		granul_ = ctmsA_->getGranul();
	    else granul_ = ctmsB_->getGranul();
	    
	    type_ = CTmsBinary;
	}

    ~BCTmsBinary() {}

    BUserCTimeSet* getCTmsA() const { return ctmsA_; }
    BUserCTimeSet* getCTmsB() const { return ctmsB_; }
};

//--------------------------------------------------
class BCTmsUnion : public BCTmsBinary
{
 public:
    BCTmsUnion(BList *arg) : BCTmsBinary(arg) 
	{
	    type_ = CTmsUnion;
	}
    ~BCTmsUnion() {}

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, int niter, granularity granul);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4Succ(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
};

DeclareCTimeSetEvaluator(BCTmsUnion);

//--------------------------------------------------
class BCTmsIntersection : public BCTmsBinary
{
 public:
    BCTmsIntersection(BList *arg) : BCTmsBinary(arg) 
	{    
	    if(ctmsA_->isVoidIntersectedWith(ctmsB_) ||
	       ctmsB_->isVoidIntersectedWith(ctmsA_))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Intersection returns an empty CTimeSet" << endl;
		ES_ss << "La intersección ha devuelto un CTimeSet vacío" 
		      << endl;
		throw BEmptyCTimeSetException(EN_ss.str(), ES_ss.str());
	    } 
	    
	    type_ = CTmsIntersection;
	}

    ~BCTmsIntersection() {}

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4Succ(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
//    bool isSuccable(int niters, granularity granul) const;
};

DeclareCTimeSetEvaluator(BCTmsIntersection);

//--------------------------------------------------
class BCTmsDifference : public BCTmsBinary
{
 public:
    BCTmsDifference(BList *arg) : BCTmsBinary(arg)  
	{
	    type_ = CTmsDifference;
	}
    ~BCTmsDifference() {}

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4Succ(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
//    bool isSuccable(int niters, granularity granul) const;
};

DeclareCTimeSetEvaluator(BCTmsDifference);

#endif /* __USE_TC__ */
#endif /* TOL_BCTMSBOOL_H */
