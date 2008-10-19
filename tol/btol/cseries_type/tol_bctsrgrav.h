/* tol_bctsrgrav.h: TOL Language. Internal operators for time serie grammar.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BCTSRGRAV_H
#define TOL_BCTSRGRAV_H 1

#include <tol/tol_cbtol.h>
#include <tol/tol_bctsrgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bprdist.h>
#include <map>

#ifdef __USE_TC__
//--------------------------------------------------------------------
// extern functions
//--------------------------------------------------------------------
BUserCTimeSet* DefaultCDating(BSyntaxObject* dating);


//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! Abstract class to deriving dummy series operators.
 */
class BCTsrDummy : public BCTsrTemporary
{
 protected:
    BCTime center_;
    BCTime postCenter_;
    
 public:
    BCTsrDummy(BList* arg) : BCTsrTemporary(arg) {}
    void CalcObject();
    BCTime Center    () const { return(center_);     }
    BCTime PostCenter() const { return(postCenter_); }
};

#define DeclareDummyTsrClass(BCTSRCLASS) \
class BCTSRCLASS: public BCTsrDummy      \
{					 \
public:					 \
  BCTSRCLASS(BList* arg)		 \
  : BCTsrDummy(arg) { }			 \
 ~BCTSRCLASS() {}			 \
  BDat GetDat(const BCTime& tmi);	 \
};					 \
DeclareEvaluator(BCTSRCLASS)

DeclareDummyTsrClass(BCTsrPulse);
DeclareDummyTsrClass(BCTsrCompens);
DeclareDummyTsrClass(BCTsrStep);
DeclareDummyTsrClass(BCTsrTrend);

//--------------------------------------------------------------------
class BCTsrLine: public BCTsrTemporary
{
 private:
    BCTime t0;
    BReal  y0;
    BCTime t1;
    BReal  y1;
 public:
    BCTsrLine(BList* arg) : BCTsrTemporary(arg) {}
    void CalcObject()
	{
	    t0 = CTime(Arg(1));
	    y0 = Real(Arg(2));
	    t1 = CTime(Arg(3));
	    y1 = Real(Arg(4));
	}
    BUserCTimeSet* Dating() const { return(DefaultCDating(Arg(5))); }
    BDat operator[] (const BCTime& tmi) { return(GetDat(tmi)); }
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrLine)

//--------------------------------------------------------------------
class BCTsrCalendary : public BCTsrTemporary
{
 private:
    BUserCTimeSet* center_;
 public:
    BCTsrCalendary(BList* arg) : BCTsrTemporary(arg) {}
    void CalcObject()
	{
	    center_ = CTms(Arg(1));
	    PutDating(DefaultCDating(CTms(Arg(2))));
	}
    BDat operator[] (const BCTime& tmi) { return(GetDat(tmi)); }
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrCalendary);

//--------------------------------------------------------------------
class BCTsrIndicator : public BCTsrTemporary
{
 private:
    BUserCTimeSet* center_;
 public:
    BCTsrIndicator(BList* arg) : BCTsrTemporary(arg) {}

    void CalcObject()
	{
	    center_ = CTms(Arg(1));
	    PutDating(DefaultCDating(CTms(Arg(2))));
	}
    BDat operator[] (const BCTime& tmi) { return(GetDat(tmi)); }
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrIndicator);

//--------------------------------------------------------------------
class BCTsrDatingChange : public BCTsrTemporary
{
 private:
    bool harmonical_;
 public:
    BCTsrDatingChange(BList* arg) : BCTsrTemporary(arg) {}

    void CalcObject();
    BUserCTimeSeries* Ser() const { return(CTsr(Arg(1))); }
    BStandardOperator* Stat() const;
    BUserCTimeSet* Dating() const { return(CTms(Arg(2))); }
    BCTime FirstCTime();
    BCTime LastCTime();
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrDatingChange);

//--------------------------------------------------------------------
class BCTsrInverseDatingChange : public BCTsrTemporary
{
 public:
    BCTsrInverseDatingChange(BList* arg)
	: BCTsrTemporary(arg) { }

    BUserCTimeSeries* Ser(int n) const { return(CTsr(Arg(n))); }
    BUserCTimeSet* Dating() const { return(Ser(2)->Dating()); }
    BCTime FirstCTime();
    BCTime LastCTime();
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrInverseDatingChange);

//--------------------------------------------------------------------
class BCTsrExpand : public BCTsrTemporary
{
 public:
    BDat val_;
    BCTsrExpand (BList* arg) : BCTsrTemporary(arg) 
	{ 
	    if(Arg(2)) 
		val_ = Dat(Arg(2));
	    else val_ = 0;

	    if((!FirstCTime().isNormal()) || (!LastCTime().isNormal()))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "The Series has been created, but will be 0 for "
		      << "every time," << endl << "due to Expand function "
		      << "requires an stochastic Series as parameter." << endl
		      << "Parameter Series must be temporarily limited."
		      << endl;
		ES_ss << "La Serie se ha creado, pero será 0 para todos los "
		      << "instantes de tiempo," << endl << "debido a que la "
		      << "función Expand requiere una serie estocástica como "
		      << "parámetro." << endl
		      << "La Serie pasada debe estar limitada temporalmente."
		      << endl;
		Std(I2(EN_ss.str().c_str(), ES_ss.str().c_str()));
	    }
	}

    BUserCTimeSeries* Ser() const { return(CTsr(Arg(1))); }
    BUserCTimeSet* Dating() const { return(Ser()->Dating()); }
    BCTime FirstCTime() { return(Ser()->FirstCTime()); }
    BCTime LastCTime()  { return(Ser()->LastCTime()); }
    BDat operator[](const BCTime& tmi)
	{
	    if(!Dating()) { return(BDat::Unknown()); }
	    else	  { return(GetDat(tmi)); }
	}
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrExpand);

//--------------------------------------------------------------------
class BCTsrSubSerie : public BCTsrTemporary
{
 public:
    BCTsrSubSerie(BList* arg);
    BUserCTimeSeries* Ser() const;
    BUserCTimeSet* Dating() const;
    BCTime FirstCTime();
    BCTime LastCTime();
    
    BDat operator[](const BCTime& tmi)
	{
	    if(!Dating()) { return(BDat::Unknown()); }
	    return(GetDat(tmi));
	}
    
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrSubSerie);

//--------------------------------------------------------------------
class BCTsrPolyn : public BCTsrTemporary
{
 private:
    BPol              pol_;
    BUserCTimeSeries* ser_;
    int	              backward_;
    int		      foreward_;
 public:
    BCTsrPolyn() : BCTsrTemporary() {}
    BCTsrPolyn(BList* arg) : BCTsrTemporary(arg) {}
    BCTsrPolyn(const BPol& pol, BUserCTimeSeries* ser);
    void CalcObject();

    BCTime GetFirstCTime() {
	if(Dating()) { return(Dating()->next(ser_->FirstCTime(),backward_)); }
	else	     { return(BCTime::Unknown()); }
    }

    BCTime GetLastCTime() {
	if(Dating()) { return(Dating()->next(ser_->LastCTime(), foreward_)); }
	else	     { return(BCTime::Unknown()); }
    }

    BDat operator[](const BCTime& tmi) {
	if(Dating()) { return(GetDat(tmi));     }
	else	     { return(BDat::Unknown()); }
    }

    BDat GetDat	 (const BCTime& tmi);
};
DeclareEvaluator(BCTsrPolyn);


//--------------------------------------------------------------------
class BCTsrDifEq : public BCTsrTemporary
{
 private:
    BPol	      pol_;
    BCTsrPolyn	      ser_;
    BUserCTimeSeries* ini_;
    bool              destroyIni_;
 public:
    BCTsrDifEq(BList* arg) : BCTsrTemporary(arg), destroyIni_(false) {}

    BCTsrDifEq(const BRat& rat, 
	       BUserCTimeSeries* ser, 
	       BUserCTimeSeries* ini = NIL);

    ~BCTsrDifEq();

    void CalcObject();

    BDat operator[](const BCTime& tmi)
	{
	    if(!Dating()) { return(BDat::Unknown()); }
	    return(GetDat(tmi));
	}
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrDifEq);

//--------------------------------------------------------------------
class BCTsrRandom : public BCTsrTemporary
{
 public:
    BCTsrRandom(BList* arg) : BCTsrTemporary(arg) {}
    BUserCTimeSet* Dating() const { return(DefaultCDating(Arg(3))); }
    BDat operator[](const BCTime& tmi) { return(GetDat(tmi)); }
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrRandom);

//--------------------------------------------------------------------
class BCTsrGaussian : public BCTsrTemporary
{
 private:
    BNormalDist dist_;
    std::map<BCTime, BDat> valuesMap_;
 public:
    BCTsrGaussian(BList* arg);
    BUserCTimeSet* Dating() const { return(DefaultCDating(Arg(3))); }
    BDat operator[](const BCTime& tmi) { return(GetDat(tmi)); }
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrGaussian);

//--------------------------------------------------------------------
/*! Abstract class to derive concat operators classes
 */
class BCTsrConcat : public BCTsrTemporary
{
 public:
    BCTsrConcat(BList* arg) : BCTsrTemporary(arg) {}
    BUserCTimeSeries* LeftSer() const { return(CTsr(Arg(1))); }
    BUserCTimeSeries* RightSer() const { return(CTsr(Arg(2))); }
    BUserCTimeSet* Dating() const;
    BCTime FirstCTime();
    BCTime LastCTime();
};

//--------------------------------------------------------------------
class BCTsrCenterConcat : public BCTsrConcat
{
 public:
    BCTsrCenterConcat(BList* arg) : BCTsrConcat(arg) {}
    BDat GetDat(const BCTime& tmi);
    BCTime Center() const { return(CTime(Arg(3))); }
};
DeclareEvaluator(BCTsrCenterConcat);

//--------------------------------------------------------------------
class BCTsrLeftConcat : public BCTsrConcat
{
 public:
    BCTsrLeftConcat(BList* arg) : BCTsrConcat(arg) {}
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrLeftConcat);

//--------------------------------------------------------------------
class BCTsrRightConcat : public BCTsrConcat
{
 public:
    BCTsrRightConcat(BList* arg): BCTsrConcat(arg) {}
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrRightConcat);

#endif // __USE_TC__
#endif // TOL_BCTSRGRAV_H
