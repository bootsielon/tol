/* tol_bctmgra.h - CTimeSet Tol Data Type creation.
 *                 TOL Language.
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

#ifndef TOL_BCTMSGRA_H
#define TOL_BCTMSGRA_H 1

#include <tol/tol_cbtol.h>
#include <tol/tol_bexc.h>
#include <tol/tol_bctmsexc.h>
#include <tol/tol_bctime.h>
#include <tol/tol_bctimeset.h>
#include <tol/tol_bgenobj.h>
#include <tol/tol_bctmigra.h>
#include <tol/tol_bdatgra.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserCTimeSet BGraObject<BCTimeSet>

//-----------------------------------------------------------------
#define DeclareCTimeSetEvaluator(CLASS)		                  \
  inline BSyntaxObject* CLASS##Evaluator(BList* args) {           \
    try {                                                         \
        return((BSyntaxObject*)(new CLASS(args)));                \
    } catch(BEmptyCTimeSetException &exc) {                       \
        if(BGrammar::Level()==0)                                  \
          Std(I2(exc.enReason_.c_str(), exc.esReason_.c_str()));  \
        return new BCTmsVoid("");                                 \
    } catch(BException& exc) {                                    \
	Std(I2(exc.enReason_.c_str(), exc.esReason_.c_str()));    \
	return NIL;                                               \
    } catch(...) {                                                \
        return NIL;                                               \
    }                                                             \
}


//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
inline BUserCTimeSet* CTms(BSyntaxObject* arg)
{
    BUserCTimeSet* uCTms = NIL;
    if(arg && (arg->Grammar()==BUserCTimeSet::OwnGrammar()))
    { uCTms = (BUserCTimeSet*)arg; }
    return(uCTms);
}

inline BUserCTimeSet* CTms(BCore* arg)
{ return(CTms((BSyntaxObject*)arg)); }

inline BUserCTimeSet* CTms(const BText& expression)
{ return(CTms(BUserCTimeSet::OwnGrammar()->EvaluateExpr(expression))); }

//--------------------------------------------------------------------
template<class T>
class BCTmsReference : public T
{
 public:
  BCTmsReference(const BText& name, BSyntaxObject* d) :T(name, d) {}
  ~BCTmsReference() {}
  
  //------------------------------
  // Includes:
  bool includes(const BCTime& tmi)
  { return(this->GetResult()->includes(tmi)); }
  
  bool includes4Succ(const BCTime& tmi, BCTimeSet *uctms)
  { return(this->GetResult()->includes4Succ(tmi, uctms)); }
  
  bool includes4SuccInG(const BCTime& tmi, int niter, granularity granul)
  { return(this->GetResult()->includes4SuccInG(tmi, niter, granul)); }
  
  bool includes4SelfSucc(const BCTime& tmi, int ndisp)
  { return(this->GetResult()->includes4SelfSucc(tmi, ndisp)); }
  
  //------------------------------
  // Successors:
  BCTime succ(const BCTime& tmi)
  { return(this->GetResult()->succ(tmi)); }
  
  BCTime succ4Succ(const BCTime& tmi, BCTimeSet *uctms)
  { return(this->GetResult()->succ4Succ(tmi, uctms)); }
  
  BCTime succ4SuccInG(const BCTime& tmi, int niter, granularity granul)
  { return(this->GetResult()->succ4SuccInG(tmi, niter, granul)); }
  
  BCTime succ4SelfSucc(const BCTime& tmi, int ndisp)
  { return(this->GetResult()->succ4SelfSucc(tmi, ndisp)); }
  
  //------------------------------
  // Predecessors:
  BCTime pred(const BCTime& tmi)
  { return(this->GetResult()->pred(tmi)); }
  
  BCTime pred4Succ(const BCTime& tmi, BCTimeSet *uctms)
  { return(this->GetResult()->pred4Succ(tmi, uctms)); }
  
  BCTime pred4SuccInG(const BCTime& tmi, int niter, granularity granul)
  { return(this->GetResult()->pred4SuccInG(tmi, niter, granul)); }
  
  BCTime pred4SelfSucc(const BCTime& tmi, int ndisp)
  { return(this->GetResult()->pred4SelfSucc(tmi, ndisp)); }
  
  //------------------------------
  int difference(const BCTime& tmi1, const BCTime& tmi2) const
  { return(this->GetResult()->difference(tmi1, tmi2)); }
  
  granularity getGranul()
  { return(this->GetResult()->getGranul()); }
  
  tmsType getType()
  { return(this->GetResult()->getType()); }
  
  BCTime maxGranulAdjust(const BCTime& tmi, granularity granul)
  { return(this->GetResult()->maxGranulAdjust(tmi, granul)); }
  
  void* getAttr() { return(this->GetResult()->getAttr()); }
  
  int getHLimit() const { return(this->GetResult()->getHLimit()); }
  
  bool isVoidIntersectedWith(BCTimeSet *ctms) const
  { return(this->GetResult()->isVoidIntersectedWith(ctms)); }
  
  bool isSuccableInG(int niter, granularity granul) const
  { return(this->GetResult()->isSuccableInG(niter, granul)); }
  
  //------------------------------
  // writes a BCTimeSet to FILE
  int BinWrite(FILE* fil) const { return (this->GetResult()->BinWrite(fil)); }
  
  void ReCalc () {}
};

//--------------------------------------------------------------------
class BCTmsRenamed : public BCTmsReference< BRefObject< BUserCTimeSet > >
{
 public:
  // Constructors and destructors: inline
 BCTmsRenamed(const BText& name, BSyntaxObject* d)
   :  BCTmsReference< BRefObject< BUserCTimeSet > >(name, d) 
  {
    granul_ = GetResult()->getGranul();
    type_   = GetResult()->getType();
  }

  ~BCTmsRenamed() {}

  static BSyntaxObject* New(const BText& name, BSyntaxObject* d)
  { return((BSyntaxObject*)(new BCTmsRenamed(name, d))); }
};

//--------------------------------------------------------------------
class BCTmsFunArg : public BCTmsReference< BFunArgObject<BUserCTimeSet> >
{
 public:
  // Constructors and destructors: inline
 BCTmsFunArg(const BText& localName, BSyntaxObject* d)
   : BCTmsReference< BFunArgObject<BUserCTimeSet> >(localName, d) {}
  ~BCTmsFunArg() 
  {
    granul_ = GetResult()->getGranul();
    type_   = GetResult()->getType();
  }
  
  static BSyntaxObject* NewLocal(const BText& localName, BSyntaxObject* d)
  { return((BSyntaxObject*)(new BCTmsFunArg(localName, d))); }
};

//--------------------------------------------------------------------
/*! Represents the CTimeSet of All Days
 */
class BCTmsWholeTime: public BUserCTimeSet {
 public:
    BCTmsWholeTime(const BText& name, granularity granul) 
	: BUserCTimeSet(name, "The CTimeSet of the whole Time")
	{
	    PutSystem(true);
	    BSyntaxObject::IncNRefs();
	    BSyntaxObject::IncNRefs();
	    setGranul(granul);
	    setType(CTmsWholeTime);
	};
    
    ~BCTmsWholeTime() {};

    bool includes(const BCTime& tmi)
	{ if(tmi.getGranul()<=granul_) return true; else return false; }
    BCTime succ(const BCTime& tmi);
    BCTime pred(const BCTime& tmi);
    bool isSuccableInG(int niter, granularity granul) const { return false; }
    int BinWrite(FILE* fil) const;
};

//--------------------------------------------------------------------
/*! Represents the empty CTimeSet. A CTimeSet without instants of times.
 */
class BCTmsVoid: public BUserCTimeSet {
 public:
    BCTmsVoid(const BText& name)
	: BUserCTimeSet(name, "The empty CTimeSet. No instants of times.")
	{
	    PutSystem(true);
	    BSyntaxObject::IncNRefs();
	    BSyntaxObject::IncNRefs();
	    setType(CTmsVoid);
	}

    ~BCTmsVoid() {};

    bool includes(const BCTime& tmi) { return false;           }
    BCTime succ(const BCTime& tmi)   { return BCTime::End();   }
    BCTime pred(const BCTime& tmi)   { return BCTime::Begin(); }

    int difference(const BCTime& tmi1, const BCTime& tmi2) const 
	{ return -1; }

    bool isVoidIntersectedWith(BCTimeSet *ctms) const       { return true;  }
    bool isSuccableInG(int niter, granularity granul) const { return false; }
};

//--------------------------------------------------------------------
/*! Represents a Time Set of all Easter days.
 */
class BCTmsEaster: public BUserCTimeSet {
 public:
    BCTmsEaster(const BText& name)
	: BUserCTimeSet(name, 
			I2("The time set of all on Sundays of Easter. Dates "
			   "between 1250 BC.and 1582 AC. belongs to Judish "
			   "Easter, called Pesach, and are calculated using "
			   "the Julian Calendar. Since 1583 AC. all dates "
			   "belongs to Easter calculated using the "
			   "Gregorian Calendar.",
			   "El conjunto temporal de todos los domingos "
			   "de Pascua. Las fechas includidas entre el 1250 "
			   "AC. y el 1582 DC. corresponden a la Pascua "
			   "Judía o Pésaj calculada en el calendario Juliano. "
			   "A partir del 1583 DC. corresponden con la Pascua "
			   "del calendario Gregoriano"))
	{
	    PutSystem(true);;
	    IncNRefs();
	    IncNRefs();
	    setGranul(DAYS);
	    setType(CTmsEaster);
	}

    ~BCTmsEaster() {};

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime pred4Succ(const BCTime &tmi, BUserCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);

    bool isSuccableInG(int niter, granularity granul) const;
};

//--------------------------------------------------------------------
/*! Base class for operators which arguments are CTimeSets
 */
class BCTmsTemporary: public BTmpObject< BUserCTimeSet >
{
 public:
    BCTmsTemporary(BList* arg=NULL)
	: BTmpObject< BUserCTimeSet >(arg) {}

   ~BCTmsTemporary() {}

    void ReCalc () {}
};

#endif /* __USE_TC__ */
#endif /* TOL_BCTMSGRA_H */

