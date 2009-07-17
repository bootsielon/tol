/* tol_btsrgrav.h: Internal operators for time serie grammar.
                   GNU/TOL Language.

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

#ifndef TOL_BTSRGRAV_H
#define TOL_BTSRGRAV_H 1

#include <tol/tol_btsrgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bdtegra.h>
//#include <tol/tol_bprdist.h>


//--------------------------------------------------------------------
// extern functions
//--------------------------------------------------------------------
BUserTimeSet* DefaultDating(BSyntaxObject* dating);


//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------


//--------------------------------------------------------------------
class BTsrDummy : public BTsrTemporary
//
/*! Abstract class to deriving dummy series operators.
 */
//--------------------------------------------------------------------
{
protected:
  BDate center_;
  BDate postCenter_;

public:
  BTsrDummy(BList* arg) : BTsrTemporary(arg) 
  {
    BUserTimeSet* dating = NIL;
    if(Arg(2))   { dating = Tms(Arg(2)); }
    if(!dating)  { dating = Tms("C"); }
    PutDating(dating);
    center_ = Date(Arg(1));
    postCenter_ = Center() + Dating();
    if(Dating()) { Dating()->ForceCache(); }
  }
  void CalcObject() {}
  BDate Center	  () const { return(center_); }
  BDate PostCenter() const { return(postCenter_); }
  DeclareClassNewDelete(BTsrDummy);
};

#define DeclareDummyTsrClass(BTSRCLASS)			      \
class BTSRCLASS: public BTsrDummy		      \
{							      \
public:							      \
  BTSRCLASS(BList* arg)					      \
  : BTsrDummy(arg) { }					      \
 ~BTSRCLASS() {}					      \
  BDat GetDat(const BDate& dte);			      \
};							      \
DeclareEvaluator(BTSRCLASS)

DeclareDummyTsrClass(BTsrPulse);
DeclareDummyTsrClass(BTsrCompens);
DeclareDummyTsrClass(BTsrStep);
DeclareDummyTsrClass(BTsrTrend);


//--------------------------------------------------------------------
class BTsrLine: public BTsrTemporary
//--------------------------------------------------------------------
{
private:
  BDate d0;
  BReal y0;
  BDate d1;
  BReal y1;
public:
  BTsrLine(BList* arg) : BTsrTemporary(arg)
  {
    d0=Date(Arg(1));
    y0=Real(Arg(2));
    d1=Date(Arg(3));
    y1=Real(Arg(4));
    PutDating(DefaultDating(Arg(5)));
    if(Dating()) { Dating()->ForceCache(); }
  }
  void CalcObject() {}
  BDat operator[] (const BDate& dte) { return(GetDat(dte)); }
  BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrLine);
};
DeclareEvaluator(BTsrLine)


//--------------------------------------------------------------------
class BTsrCalendary : public BTsrTemporary
//--------------------------------------------------------------------
{
private:
  BUserTimeSet* center_;
public:
  BTsrCalendary(BList* arg) : BTsrTemporary(arg) 
  {
    center_ = Tms(Arg(1));
    PutDating(DefaultDating(Arg(2)));
    if(Dating()) { Dating()->ForceCache(); }
    if(center_ ) { center_ ->ForceCache(); }
  }
  void CalcObject() { }
  BDat operator[] (const BDate& dte) { return(GetDat(dte)); }
  BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrCalendary);
};
DeclareEvaluator(BTsrCalendary);


//--------------------------------------------------------------------
class BTsrIndicator : public BTsrTemporary
//--------------------------------------------------------------------
{
private:
  BUserTimeSet* center_;
public:
  BTsrIndicator(BList* arg) : BTsrTemporary(arg) 
  {
    center_ = Tms(Arg(1));
    PutDating(DefaultDating(Arg(2)));
    if(Dating()) { Dating()->ForceCache(); }
    if(center_ ) { center_ ->ForceCache(); }
  }
  void CalcObject() { }
  BDat operator[] (const BDate& dte) { return(GetDat(dte)); }
  BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrIndicator);
};
DeclareEvaluator(BTsrIndicator);


//--------------------------------------------------------------------
class BTsrDatingChange : public BTsrTemporary
//--------------------------------------------------------------------
{
private:
  BBool harmonical_;
public:
  BTsrDatingChange(BList* arg) : BTsrTemporary(arg) 
  {
    harmonical_ = BTRUE;
    if(//(Stat()->MinArg()!=1)			   ||
       (Stat()->MaxArg()!=3)			 ||
       !Stat()->Grammar() ||
       (Stat()->Grammar()->Name()!="Real")	 ||
       (Stat()->GrammarForArg(1)->Name()!="Serie") ||
       (Stat()->GrammarForArg(2)->Name()!="Date")	 ||
       (Stat()->GrammarForArg(2)->Name()!="Date")	   )
    {
      Error(Stat()->Grammar()->Name()+" "+Stat()->Name()+" "+Stat()->Arguments()+
	    I2(" is not a valid statistic for time series. Expected delaration was: \n",
	       " no es un estadístico válido para series temporales. LA declaración que se esperaba era:\n")+
       "  Real "+Stat()->Name()+"(Series ser, Date datIni, Date datEnd)");
    }
    PutDating(DefaultDating(Tms(Arg(2))));
    if(Ser() && Ser()->Dating()) { Ser()->Dating()->ForceCache(); }
    if(                Dating()) {        Dating()->ForceCache(); }
    Do();
  }
  void CalcObject() {}
  BUserTimeSerie*    Ser ()	 const { return(Tsr(Arg(1))); }
  BStandardOperator* Stat()	 const ;
  BDate		     FirstDate() const;
  BDate		     LastDate()	 const;
  BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrDatingChange);
};
DeclareEvaluator(BTsrDatingChange);


//--------------------------------------------------------------------
class BTsrInverseDatingChange : public BTsrTemporary
//--------------------------------------------------------------------
{
public:
  BTsrInverseDatingChange(BList* arg)
  : BTsrTemporary(arg) 
  { 
    PutDating(Ser(2)->Dating());
    if(Dating()) { Dating()->ForceCache(); }
    Do();
  }
  BUserTimeSerie* Ser(BInt n) const { return(Tsr(Arg(n))); }
  BDate FirstDate() const;
  BDate LastDate()  const;
  BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrInverseDatingChange);
};
DeclareEvaluator(BTsrInverseDatingChange);


//--------------------------------------------------------------------
class BTsrExpand : public BTsrTemporary
//--------------------------------------------------------------------
{
public:
  BTsrExpand (BList* arg) : BTsrTemporary(arg) 
  {
    PutDating(Ser()->Dating());
    if(Dating()) { Dating()->ForceCache(); }
    Do();
  }
  BUserTimeSerie* Ser()	      const { return(Tsr(Arg(1))); }
  BDate		  FirstDate() const { return(Ser()->FirstDate()); }
  BDate		  LastDate()  const { return(Ser()->LastDate()); }
  BDat operator[](const BDate& dte)
  {
    if(!Dating()) { return(BDat::Unknown()); }
    else	  { return(GetDat(dte)); }
  }
  BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrExpand);
};
DeclareEvaluator(BTsrExpand);


//--------------------------------------------------------------------
class BTsrSubSerie : public BTsrTemporary
//--------------------------------------------------------------------
{
 public:
    BTsrSubSerie(BList* arg);
    BUserTimeSerie* Ser()       const;
    BDate	    FirstDate() const;
    BDate	    LastDate()  const;

    BDat operator[](const BDate& dte)
	{
	    if(!Dating()) { return(BDat::Unknown()); }
	    return(GetDat(dte));
	}

    BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrSubSerie);
};
DeclareEvaluator(BTsrSubSerie);


//--------------------------------------------------------------------
class BTsrPolyn : public BTsrTemporary
//--------------------------------------------------------------------
{
private:
  BPol		  pol_;
  BUserTimeSerie* ser_;
  BInt		  backward_;
  BInt		  foreward_;
public:
  BTsrPolyn() : BTsrTemporary(NULL)	{}
  BTsrPolyn(BList* arg) : BTsrTemporary(arg)
  {
    if(Arg(1))
    {
      pol_ = Pol(Arg(1));
	    ser_ = Tsr(Arg(2));
      backward_ = pol_.Degree();
      foreward_ = pol_[0].Degree();
      PutDating(ser_->Dating());
      PutFirstDate(GetFirstDate());
      PutLastDate (GetLastDate());
      Do();
    }
  }

  BTsrPolyn(const BPol& pol, BUserTimeSerie* ser);
  void CalcObject() {}
  BDate GetFirstDate()
  {
    if(Dating()) { return(Dating()->Next(ser_->FirstDate(),backward_)); }
    else	 { return(BDate::Unknown()); }
  }
  BDate GetLastDate()
  {
    if(Dating()) { return(Dating()->Next(ser_->LastDate(), foreward_)); }
    else	 { return(BDate::Unknown()); }
  }
  BDat operator[](const BDate& dte)
  {
    if(Dating()) { return(GetDat(dte)); }
    else	 { return(BDat::Unknown()); }

  }
  BDat GetDat	 (const BDate& dte);
  DeclareClassNewDelete(BTsrPolyn);
};
DeclareEvaluator(BTsrPolyn);


//--------------------------------------------------------------------
class BTsrDifEq : public BTsrTemporary
//--------------------------------------------------------------------
{
private:
  BPol		  pol_;
  BTsrPolyn*  ser_;
  BUserTimeSerie* ini_;
  bool            destroyIni_;
public:
  BTsrDifEq(BList* arg) 
  : BTsrTemporary(arg), pol_(), ser_(NULL), ini_(NULL), destroyIni_(false)
  { 
    Initialize(Rat(Arg(1)),Tsr(Arg(2)),Arg(3)?Tsr(Arg(3)):NULL);
  }
  BTsrDifEq(const BRat& rat, BUserTimeSerie* ser, BUserTimeSerie* ini = NULL)
  : BTsrTemporary(NULL), pol_(), ser_(NULL), ini_(NULL), destroyIni_(false)
  {
    Initialize(rat,ser,ini);
    if(destroyIni_ && ini_)  { DESTROY(ini_); }
  }
 ~BTsrDifEq()
  {
    if(destroyIni_ && ini_)  { DESTROY(ini_); }
	if(ser_) { delete ser_; }
  }
  void Initialize(const BRat& rat, BUserTimeSerie* ser, BUserTimeSerie* ini)
  {
    pol_ = 1-rat.Denominator();
    ser_ = new BTsrPolyn(rat.Numerator(), ser);
    ini_ = ini;
    if(!ini_) 
    { 
      ini_ = new BTsrFromReal(BDat(0.0)); 
      destroyIni_  = true;
    }
    PutDating   (ser_->Dating());
    if(Dating())
    {
      PutFirstDate(Dating()->Prev(ser_->FirstDate(), pol_.Degree()));
      PutLastDate (ser_->LastDate());
      GetLength();
    }
    Do();
  }

  void CalcObject() {}
  BDat operator[](const BDate& dte)
  {
    if(!Dating()) { return(BDat::Unknown()); }
    return(GetDat(dte));
  }
  BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrDifEq);
};
DeclareEvaluator(BTsrDifEq);



//--------------------------------------------------------------------
class BTsrRandom : public BTsrTemporary
//--------------------------------------------------------------------
{
public:
  BTsrRandom(BList* arg) : BTsrTemporary(arg) {}
  BUserTimeSet* Dating() const { return(DefaultDating(Arg(3))); }
  BDat operator[](const BDate& dte) { return(GetDat(dte)); }
  BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrRandom);
};
DeclareEvaluator(BTsrRandom);




//--------------------------------------------------------------------
class BTsrConcat : public BTsrTemporary

/*! Abstract class to derive concat operators classes
 */
//--------------------------------------------------------------------
{
public:
  BTsrConcat(BList* arg) : BTsrTemporary(arg) 
  {
    CalcDating();
  }
  BUserTimeSerie* LeftSer () const { return(Tsr(Arg(1))); }
  BUserTimeSerie* RightSer() const { return(Tsr(Arg(2))); }
private:
  void  CalcDating  ();
  DeclareClassNewDelete(BTsrConcat);
};


//--------------------------------------------------------------------
class BTsrCenterConcat : public BTsrConcat
//--------------------------------------------------------------------
{
public:
  BTsrCenterConcat(BList* arg) : BTsrConcat(arg) { Do(); }
  BDat GetDat(const BDate& dte);
  BDate Center() const { return(Date(Arg(3))); }
  BDate	 FirstDate() const;
  BDate	 LastDate()  const;
  DeclareClassNewDelete(BTsrCenterConcat);
};
DeclareEvaluator(BTsrCenterConcat);


//--------------------------------------------------------------------
class BTsrLeftConcat : public BTsrConcat
//--------------------------------------------------------------------
{
public:
  BTsrLeftConcat(BList* arg) : BTsrConcat(arg) { Do(); }
  BDat GetDat(const BDate& dte);
  BDate	 FirstDate() const;
  BDate	 LastDate()  const;
  DeclareClassNewDelete(BTsrLeftConcat);
};
DeclareEvaluator(BTsrLeftConcat);


//--------------------------------------------------------------------
class BTsrRightConcat : public BTsrConcat
//--------------------------------------------------------------------
{
public:
  BTsrRightConcat(BList* arg): BTsrConcat(arg) { Do(); }
  BDat GetDat(const BDate& dte);
  BDate	 FirstDate() const;
  BDate	 LastDate()  const;
  DeclareClassNewDelete(BTsrRightConcat);
};
DeclareEvaluator(BTsrRightConcat);


#endif // TOL_BTSRGRAV_H
