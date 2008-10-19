/* tol_bctmigra.h: Classes and functions for CDate Tol Data Type.
                  GNU/TOL Language.
 
   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])
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

#ifndef TOL_BCTMIGRA_H
#define TOL_BCTMIGRA_H 1

#include <tol/tol_cbtol.h>
#include <tol/tol_bctime.h>
#include <tol/tol_bgencon.h>
#include <tol/tol_bdatgra.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserCTime      BGraContens  < BCTime >
#define BSystemCTime    BGraConstant < BCTime >
#define BParamCTime     BGraParameter< BCTime >
#define BCTimeTemporary BTmpContens  < BCTime >

template class TOL_API BGraContensBase < BCTime >;
template class TOL_API BGraContens     < BCTime >;

//-------------------------------------------------------------------
// forward references
CastingsDeclaration(CTime);

//--------------------------------------------------------------
#define DeclareCTimeContentClass(BTMPCLASS, BCLASS)            \
class BCLASS: public BTMPCLASS			               \
{						               \
public:						               \
  BCLASS(BList* arg) : BTMPCLASS(arg) {}	               \
 ~BCLASS()  {}					               \
  void CalcContens();                                          \
};                                                             \
DeclareCTimeEvaluator(BCLASS);

//--------------------------------------------------------------
#define DeclareCTimeEvaluator(BCLASS)                          \
inline BSyntaxObject* BCLASS##Evaluator(BList* args) {         \
    try {                                                      \
	return((BSyntaxObject*)(new BCLASS(args)));            \
    } catch(BException &exc) {                                 \
        Std("\n");                                             \
	Std(I2(exc.enReason_.c_str(), exc.esReason_.c_str())); \
	return NIL;                                            \
    }                                                          \
};
    

//--------------------------------------------------------------------
class BCTimeToday: public BSystemCTime
{
 public:
    BCTimeToday() 
	: BSystemCTime("Today", BCTime::Now(), I2("Today","Hoy")) 
	{ }

    ~BCTimeToday() {}

    BCTime& Contens() {
	contens_ = BCTime::Now();
	contens_.setGranul(DAYS);
	return contens_; 
    }
};


//--------------------------------------------------------------------
class BCTimeNow: public BSystemCTime
{
 public:
    BCTimeNow() 
	: BSystemCTime("Now", BCTime::Now(), I2("Now","Ahora")) 
	{ }

    ~BCTimeNow() {}

    BCTime& Contens() { 
	contens_ = BCTime::Now();
	return contens_; 
    }
};

//--------------------------------------------------------------------
class BCTimeNewCTime: public BCTimeTemporary
{
 public:
    BCTimeNewCTime(BList *args) : BCTimeTemporary(args) {
	long int year = static_cast<long int>(Real(Arg(1)));
	int month, day, hour, minute, second;

	month  = (Arg(2)) ? static_cast<int>(Real(Arg(2))) :  0;
	day    = (Arg(3)) ? static_cast<int>(Real(Arg(3))) :  0;
	hour   = (Arg(4)) ? static_cast<int>(Real(Arg(4))) : -1;
	minute = (Arg(5)) ? static_cast<int>(Real(Arg(5))) : -1;
	second = (Arg(6)) ? static_cast<int>(Real(Arg(6))) : -1;
	BCTime newCTime(year, month, day, hour, minute, second);
	contens_ = newCTime;
    }
    ~BCTimeNewCTime() {};
    void CalcContens() {};
};

DeclareCTimeEvaluator(BCTimeNewCTime);

DefExtOpr(1, BCTimeNewCTime, "NewCTime", 1, 6, "Real Real Real Real Real Real",
	  I2("(Real year, [Real month=0, Real day=0," 
	     "Real hour=-1,  Real minute=-1, Real second=-1])",
	     "(Real año, [Real mes=0, Real dia=0," 
	     "Real hora=-1,  Real minuto=-1, Real segundo=-1])"),
	  I2("Returns a new Time based on function given arguments. "
	     "The granularity of the created Time will be set with the "
	     "last valid parameter given in the list.\n\nExamples:\n"
	     "CTime tmi1 = NewTime(2005,1,2);  -> y2005m1d2 (Daily)\n"
	     "CTime tmi2 = NewTime(2005,0);   -> y2005 (Yearly)\n"
	     "CTime tmi3 = NewTime(2005,3,33); -> Error\n",
	     "Devuelve un nuevo Time basado en los argumentos dados."
	      "La base temporal del Time creado será establecida con "
	      "el último parámetro válido de la lista de argumentos.\n"
	      "\nEjemplos:\n"
	     "CTime tmi1 = NewTime(2005,1,2);  -> y2005m1d2 (Daily)\n"
	     "CTime tmi2 = NewTime(2005,0);   -> y2005 (Yearly)\n"
	     "CTime tmi3 = NewTime(2005,3,33); -> Error\n"),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
class BCTimeEasterIn: public BCTimeTemporary
{
 public:
    BCTimeEasterIn(BList* arg) : BCTimeTemporary(arg) {
	BCTime arg1 = CTime(Arg(1));
	contens_ = BCTime::Easter(arg1.year());
    }
    ~BCTimeEasterIn() {};
    void CalcContens() {};
};

DeclareCTimeEvaluator(BCTimeEasterIn);

DefExtOpr(1, BCTimeEasterIn, "EasterIn", 1, 1, "CTime",
	  I2("(CTime tmi)","(CTime tmi)"),
	  I2("Returns the day representing Easter in a given year. "
	     "Any Time can be used as argument.",
	     "Devuelve el día que representa la Pascua en un año dado. "
	     " Cualquier Time puede ser usado como argumento."),
	  BOperClassify::TimeAlgebra_);

//--------------------------------------------------------------------
class BCTimeEasterInYear: public BCTimeTemporary
{
 public:
    BCTimeEasterInYear(BList* arg) : BCTimeTemporary(arg) {
	double year = Real(Arg(1));
	contens_ = BCTime::Easter(static_cast<long int>(year));
    }
    ~BCTimeEasterInYear() {};
    void CalcContens() {};
};

DeclareCTimeEvaluator(BCTimeEasterInYear);

DefExtOpr(1, BCTimeEasterInYear, "EasterInYear", 1, 1, "Real",
	  I2("(Real year)","(Real año)"),
	  I2("Returns the day representing Easter in a given year.",
	     "Devuelve el día que representa la Pascua en un año dado."),
	  BOperClassify::TimeAlgebra_);


#endif /* __USE_TC__ */
#endif /* TOL_BTMIGRA_H */
