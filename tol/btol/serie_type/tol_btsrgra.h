/* tol_btsrgra.h: Classes and functions for time serie grammar.
                  GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BTSRGRA_H
#define TOL_BTSRGRA_H 1

#include <tol/tol_btmser.h>
#include <tol/tol_blanguag.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserTimeSerie BGraObject<BTimeSerie>

//--------------------------------------------------------------------
class TOL_API BUserTimeSerieDo: public BUserTimeSerie
//--------------------------------------------------------------------
{
public:
  BUserTimeSerieDo()
  : BUserTimeSerie() {}

  BUserTimeSerieDo(const BText& name)
  : BUserTimeSerie(name) {}

  BUserTimeSerieDo(const BText& name, const BText& description)
  : BUserTimeSerie(name,description) {}
 ~BUserTimeSerieDo() {}

  BText Info() const;
  BText Dump() const { return(Identify()); };
  void Do ();
  BSyntaxObject* CopyContens ();
  virtual void Clean() {}
  void PutContens(BUserTimeSerieDo* ser);
  bool IsBounded () const 
  {
    return(!Dating() || (FirstDate().HasValue() && LastDate().HasValue()));
  }
};


//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
inline BUserTimeSerie* Tsr (BSyntaxObject* arg)
{
  BUserTimeSerie* uTsr = NIL;
  if(arg && (arg->Grammar()==BUserTimeSerie::OwnGrammar()))
  { uTsr = BCast<BUserTimeSerie,BCore>::c(arg); }
  return(uTsr);
}

inline BUserTimeSerie* Tsr (BCore* arg)
{ return(Tsr(BCast<BSyntaxObject,BCore>::c(arg))); }

inline BUserTimeSerie* Tsr(const BText& expression)
{ return(Tsr(BUserTimeSerie::OwnGrammar()->EvaluateExpr(expression))); }

inline BUserTimeSerie& TsrR(BSyntaxObject* arg)
{ return(*Tsr(arg)); }


//--------------------------------------------------------------------
class TOL_API BTsrPrimary: public BUserTimeSerieDo

/*! Base class for each operator which result is a time serie.
 */
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors
  BTsrPrimary(const BText&  name   = "",
	      const BText&  title  = "",
	      BUserTimeSet* dating = NIL,
	      const BDate&  first  = BDate::Unknown(),
	      const BDate&  last   = BDate::Unknown());

  BTsrPrimary(const BText&  name,
	      const BText&  title,
	      BUserTimeSet* dating,
	      const BDate&  first,
	      const BDate&  last,
	      const BData&  data);

  BTsrPrimary(const BText&  name,
	      const BText&  title,
	      BUserTimeSet* dating,
	      const BDate&  first,
	      const BData&  data);
 ~BTsrPrimary() {}

  // Access
  void ReadData();
  BDat GetDat(const BDate& dte);
  DeclareClassNewDelete(BTsrPrimary);
};


//--------------------------------------------------------------------
class BTsrBSI : public BTsrPrimary

/*! Serie that has been readed from a Bayes Serie Index file.
 */
//--------------------------------------------------------------------
{
private:
  BText bsdFileName_;
  BInt	offset_;
public:
  // Constructors and destructors
  BTsrBSI(const BText&	name,
	  const BText&	bsdFileName,
	  const BInt	offset)
  : BTsrPrimary(name),
    bsdFileName_(bsdFileName),
    offset_(offset)
  {
  }
 ~BTsrBSI() 
  {
    //When dating is given as a TimeSet expression withou name
    //a new object will be created and must be incremented it 
    //references to protect it and destroyed with the BTsrBSI
    /*
      not needed as base class take care of it!
     */
  }
  const BText& Description () const;
  void ReadData();
  static void	WriteFile (const BText& bsiFileName, BList* serList);
  static BList* ReadFile  (const BText& bsiFileName);
  RedeclareClassNewDelete(BTsrBSI);
};


//--------------------------------------------------------------------
template<class BRefTemplate>
class BTsrReference : public BRefTemplate
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTsrReference(const BText& name, BSyntaxObject* d)
  : BRefTemplate(name, d) {}

 ~BTsrReference() {}

  BUserTimeSerieDo* GetResult() const
  {
    return(BRefObject<BUserTimeSerieDo>::GetResult());
  }
  void CalcObject() 
  {
    assert(GetResult());
    GetResult()->CalcObject(); 
  }
  void Clean() 
  {
    assert(GetResult());
    GetResult()->Clean();
  }

  BUserTimeSet* Dating	 () const
  {
    assert(GetResult());
    return(GetResult()->Dating());
  }
  BDate		FirstDate() const
  {
    assert(GetResult());
    return(GetResult()->FirstDate());
  }
  BDate		LastDate () const
  {
    assert(GetResult());
    return(GetResult()->LastDate()); 
  }
  void PutDating(BUserTimeSet* dating) 
  {
    assert(GetResult());
    GetResult()->PutDating(dating);
  }
  void PutFirstDate(const BDate&  firstDate) 
  {
    assert(GetResult());
    GetResult()->PutFirstDate(firstDate);
	}
  void PutLastDate    (const BDate&  lastDate) 
  {
    assert(GetResult());
    GetResult()->PutLastDate(lastDate);
  }
  BDat operator[] (const BDate& dte)
  { 
    assert(GetResult());
    return((*GetResult())[dte]);
  }
  void PutDat(const BDate& dte, const BDat& dat)
  {
    assert(GetResult());
    GetResult()->PutDat(dte,dat);
  }
  const BData& Data() const
  {
    assert(GetResult());
    return(GetResult()->Data());
  }
  BData& GetDataBuffer() 
  {
    assert(GetResult());
    return(GetResult()->GetDataBuffer());
  }
  void GetData(	     BData& dta,
	       const BDate& first,
	       const BDate& last,
		     BInt   len=0)
  {
    assert(GetResult());
    GetResult()->GetData(dta,first,last,len);
  }
  void ReadData() 
  {
    assert(GetResult());
    GetResult()->ReadData();
  }
  int GetLength()
  {
    assert(GetResult());
    return(GetResult()->GetLength());
  }
  void PutLength(int l)
  {
    assert(GetResult());
    GetResult()->PutLength(l);
  }
  enum dating_type getDatingType() 
  { 
    assert(GetResult());
    return(GetResult()->getDatingType());
  }
  void setDatingType(enum dating_type dt)
  { 
    assert(GetResult());
    GetResult()->setDatingType(dt);
  }
  BDat GetDat(const BDate& dte)
  {
    assert(GetResult());
    return((*GetResult())[dte]);
  }
  void CompactData()
  {
    assert(GetResult());
    GetResult()->CompactData();
  }
  int GetIndex (const BDate& dte) const
  {
    assert(GetResult());
    return(GetResult()->GetIndex(dte));
  }
  BDate IndexToDate(int index) const
  {
    assert(GetResult());
    return(GetResult()->IndexToDate(index));
  }
  int Length() const
  {
    assert(GetResult());
    return(GetResult()->Length());
  }

};

//--------------------------------------------------------------------
class BTsrRenamed : public BTsrReference< BRefObject<BUserTimeSerieDo> >
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTsrRenamed(const BText& name, BSyntaxObject* d)
  : BTsrReference< BRefObject<BUserTimeSerieDo> >(name, d) 
  {}
 ~BTsrRenamed() {}

  static BSyntaxObject* New(const BText& name, BSyntaxObject* d);
  DeclareClassNewDelete(BTsrRenamed);
};

//--------------------------------------------------------------------
class BTsrFunArg : public BTsrReference< BFunArgObject<BUserTimeSerieDo> >
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTsrFunArg(const BText& localName, BSyntaxObject* d)
  : BTsrReference< BFunArgObject<BUserTimeSerieDo> >(localName, d) 
  {}
 ~BTsrFunArg() {}

  static BSyntaxObject* NewLocal(const BText& localName, BSyntaxObject* d)
  { return((BSyntaxObject*)(new BTsrFunArg(localName, d))); }
  DeclareClassNewDelete(BTsrFunArg);
};


//--------------------------------------------------------------------
class BTsrTemporary: public BTmpObject<BUserTimeSerieDo>

/*! Base class for each operator which result is a time serie.
 */
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTsrTemporary(BList* arg=NULL)
  : BTmpObject<BUserTimeSerieDo>(arg) {}

 ~BTsrTemporary() {}

  BDat LogicShort(BBool(*comp)(const BDat&,const BDat&),
		  const BDate& dte) const;
  void ReCalc ();
  void Clean() 
  {
    if(data_.Size()) { data_.DeleteBuffer(); }
    for(int i=0; i<card_; i++)
    {
      if(array_[i]->Grammar()==GraSerie()) 
      {
        BUserTimeSerieDo* tsr = (BUserTimeSerieDo*)array_[i];
        tsr->Clean();
      }
    }
  }
};


//--------------------------------------------------------------------
class BTsrFromReal : public BTsrTemporary

/*! Casting from real.Really, a real number is not a serie by self
 *  because it hasn't any dating, but it can operate with every serie
 *  in its own dating returning the same value for each date of this
 *  dating.
 */
//--------------------------------------------------------------------
{
public:
  BTsrFromReal(BList* arg);
  BTsrFromReal(const BDat& dat);
 ~BTsrFromReal() {}

  BDat operator[] (const BDate& dte);
  BDat GetDat	  (const BDate& dte);
  DeclareClassNewDelete(BTsrFromReal);
};

#endif // BTIMESERIEGRAMMAR



