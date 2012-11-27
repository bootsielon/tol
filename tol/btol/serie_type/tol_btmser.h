/* tol_btmser.h: Class and functions header.
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

#ifndef TOL_BTMSER_H
#define TOL_BTMSER_H 1

#include <tol/tol_btmsgra.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------

enum dating_type { DATING_FIXED, DATING_VOLATILE };

//--------------------------------------------------------------------
class TOL_API BTimeSerie

/*! Base class for series that has dating.
 *  If also has dates its data are resident in memory after they are
 *  been consulted.
 */
//--------------------------------------------------------------------
{
 private:
  static BDat globalizeSeries_; // used as a Tol Global Variable

 protected:
  BData	        data_;
  BDate	        firstCache_;
  BDate	        firstDate_;
  BDate	        lastDate_;
  BInt	        length_;
  BUserTimeSet* dating_;
  dating_type   datingType_;
  bool          compacted_;
#if(__USE_POOL__==__POOL_BFSMEM__)
  BCore::MemAssignInfo datingMai_;
  void CheckRefDating();
#endif    
 public:
  // Constructors and destructors: tsr.cpp
  BTimeSerie();
  virtual ~BTimeSerie();
  // Access & Manipulation: inline
  virtual BUserTimeSet* Dating () const;
  virtual BDate	  FirstCache() const;
  virtual BDate	  FirstDate() const;
  virtual BDate	  LastDate () const;
  void PutDating(BUserTimeSet* dating, bool forceRef);
  virtual void PutDating(BUserTimeSet* dating) { PutDating(dating, false); }
  virtual void PutFirstCache(const BDate&  firstCache);
  virtual void PutFirstDate (const BDate&  firstDate);
  virtual void PutLastDate    (const BDate&  lastDate);
  virtual BDat operator[](const BDate& dte);
  virtual void PutDat(const BDate& dte, const BDat& dat);
  virtual const BData& Data() const { return(data_); }
  virtual BData& GetDataBuffer() { return(data_); }
  virtual void GetData(      BData& dta,
                       const BDate& firstDate,
                       const BDate& lastDate,
                             BInt	length=0);
  virtual void ReadData() {};
  virtual int GetLength();
  virtual void PutLength(int l) { length_ = l; }
  virtual enum dating_type getDatingType() { return datingType_; }
  virtual void setDatingType(enum dating_type dt) { datingType_ = dt;   }
  virtual BDat GetDat(const BDate& dte) = 0;
  virtual void CompactData();
  virtual int GetIndex (const BDate& dte) const;
  virtual BDate IndexToDate(int index) const;
  virtual int Length() const { return(length_); }
  virtual BBool IsStochastic() const ;
  virtual BBool IsEmpty() const ;

  BInt BinWrite(FILE*);
  BInt BinRead (FILE*);
  void Realloc(int length) { GetDataBuffer().ReallocBuffer(length); }
  void Alloc  (int length) { GetDataBuffer().AllocBuffer(length); }
  void Delete ()           { GetDataBuffer().DeleteBuffer(); }
  void PutBufDat(int k, const BDat& dat) { GetDataBuffer()[k] = dat; }
  void GetData(const BDate& firstDate, const BDate& lastDate, BInt	length=0)
  { GetData(data_,firstDate,lastDate,length);  }
  void GetData(BData& dta) { GetData(dta,FirstDate(),LastDate()); }
  void FreeDates() 
  {
    PutFirstDate(BDate::Begin());
    PutLastDate (BDate::End  ());
  }
  void FreeData() { GetDataBuffer().DeleteBuffer(); }
  BBool HasDating   () const { return(Dating()!=NIL); }
  static BDat& GlobalizeSeries();
};


//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#define ForAllDate(dte, obj)                       \
                                                   \
  for(BDate dte =  (obj).FirstDate();              \
	    dte <= (obj).LastDate();               \
	    dte =  (obj).Dating()->Successor(dte))


#define ForEachDate(dte, ini, fin, dating)         \
                                                   \
  for(BDate dte =  ini;                            \
	    dte <= fin;                            \
	    dte =  dating->Successor(dte))


#endif // TOL_BTMSER_H
