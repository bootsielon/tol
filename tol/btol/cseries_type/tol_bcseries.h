/* tol_bcseries.h: TOL Language. Class and functions header.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BCSERIES_H
#define TOL_BCSERIES_H 1

#include <tol/tol_cbtol.h>
#include <tol/tol_bctmsgra.h>
#include <tol/tol_bdat.h>
#include <tol/tol_barray.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
//#define BData BArray<BDat>

enum ctms_type { CDATING_FIXED, CDATING_VOLATILE };

//--------------------------------------------------------------------
/*! Base class for series that has dating.
 *  If also has dates its data are resident in memory after they are
 *  been consulted.
 *  If also has a non null file pointer, its datas are saved in this
 *  file.
 */
class TOL_API BCTimeSeries
{
 private:
    static BDat globalizeCSeries_; // used as a Tol Global Variable
    
 protected:
    BSyntaxObject*   file_;
    BData	     data_;
    
    BUserCTimeSet* dating_;
    BCTime	   firstCTime_;
    BCTime	   lastCTime_;
    int	           length_;
    enum ctms_type datingType_;
    
 public:
    // Constructors and destructors: tsr.cpp
    BCTimeSeries() : 
	length_(0),
	data_(),
	dating_(NIL),
	firstCTime_(BCTime::Begin()),
	lastCTime_(BCTime::End()),
	file_(NIL),
	datingType_(CDATING_FIXED)
	{}

    ~BCTimeSeries() {}

    int BinWrite(FILE*);
    int BinRead (FILE*);

    // Access & Manipulation: inline
    virtual BUserCTimeSet* Dating   () const;
    virtual BCTime	  FirstCTime();
    virtual BCTime	  LastCTime ();
    
    virtual void PutDating(BUserCTimeSet* dating) {
	if(dating_) dating_->DecNRefs();
	dating_ = dating;
	if(dating_) dating_->IncNRefs();
    }

    virtual void PutFirstCTime(const BCTime& firstCTime) {
	if(Dating() && firstCTime.isNormal()) 
	{ firstCTime_ = Dating()->nonSmaller(firstCTime); }
    }

    virtual void PutLastCTime    (const BCTime&  lastCTime) {
	if(Dating() && lastCTime.isNormal())
	{ lastCTime_ = Dating()->nonGreater(lastCTime); }
    }
    
    virtual BDat operator[](const BCTime& tmi);
    void PutDat(const BCTime& tmi, const BDat& dat);

    virtual const BData& Data() const { return(data_); }
    BData& GetDataBuffer() { return(data_); }

    virtual void GetData(	BData& dta,
			  const BCTime& firstCTime,
			  const BCTime& lastCTime,
			  int	length=0);

    void GetData(BData& dta) { GetData(dta,FirstCTime(),LastCTime()); }
    BSyntaxObject* File() const { return(file_); }
    void PutFile(BSyntaxObject* file) { file_ = file; }

    virtual void ReadData() {};

    void FreeDates() {
	firstCTime_ = BCTime::Begin();
	firstCTime_ = BCTime::End();
    }

    void FreeData() { data_.DeleteBuffer(); }

    bool HasDating   () const { return (Dating()!=NIL); }
    bool IsStochastic() { 
	return(HasDating() && 
	       FirstCTime().isNormal() && LastCTime().isNormal()); 
    }

    int GetLength();
    void PutLength(int l) { length_ = l; }

    virtual enum ctms_type getDatingType() { return datingType_; }
    void setDatingType(enum ctms_type dt) { datingType_ = dt;   }

    static BDat& GlobalizeCSeries();

 protected:
    virtual BDat GetDat(const BCTime& tmi) = 0;
    void CompactData();
    int GetIndex (const BCTime& tmi);
    BCTime IndexToDate(int index) const;
    int Length() const { return(length_); }
};


//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#define ForAllCTime(tmi, obj)                      \
                                                   \
  for(BCTime tmi =  (obj).FirstCTime();            \
	    tmi <= (obj).LastCTime();              \
	    tmi =  (obj).Dating()->Successor(tmi))


#define ForEachCTime(tmi, ini, fin, dating)        \
                                                   \
  for(BCTime tmi =  ini;                           \
	    tmi <= fin;                            \
	    tmi =  dating->Successor(tmi))

#endif // __USE_TC__
#endif // TOL_BCSERIES_H
