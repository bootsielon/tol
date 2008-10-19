/* tol_btsrgrp.h: Definition of class BSerieGroup (srg).
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

#ifndef TOL_BTSRGRP_H
#define TOL_BTSRGRP_H 1

#define BDT  BText("(BDT) ")

#include <tol/tol_btsrgra.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BSerArr  BArray< BUserTimeSerie* >


//--------------------------------------------------------------------
class BSerieGroup
//--------------------------------------------------------------------
{
protected:
  BUserTimeSet* dating_;
  BHash		hash_;
  BDate		firstDate_;
  BDate		lastDate_;
  BDate		maxFirst_;
  BDate		minLast_;
  BInt		numDates_;
  BSerArr serArr_;
  BBool		checkDating_;
  bool    volatileDating_;

public:
  BSerieGroup();
 ~BSerieGroup() {}

  void Alloc  (int numSer) { serArr_.AllocBuffer  (numSer); }
  void Realloc(int numSer) { serArr_.ReallocBuffer(numSer); }
  void Delete ()           { serArr_.DeleteBuffer ();       }
  void AddSerie(BUserTimeSerie* ser) { serArr_.Add(ser); }
  void AddList (BList* lst);
  void AddSet  (const BSet&  set);
  void AddGroup(const BSerieGroup& group);

  void PrepareOperate();
  void PrepareFill(const BDate& f = BDate::Unknown(),
	                 const BDate& l = BDate::Unknown());
  BBool TestDates();

  BUserTimeSet*   Dating	   ()       const { return(dating_); }
  BDate		        FirstDate  ()       const { return(firstDate_); }
  BDate		        LastDate   ()       const { return(lastDate_); }
  BDate		        MaxFirst   ()       const { return(maxFirst_); }
  BDate		        MinLast	   ()       const { return(minLast_); }
  BBool		        CheckDating()       const { return(checkDating_); }
  BInt		        NumSeries  ()       const { return(serArr_.Size()); }
  BInt		        NumDates   ()       const { return(numDates_); }
  BUserTimeSerie* Serie	     (BInt n) const { return(Tsr(serArr_[n-1]));}
  BUserTimeSerie& SerieR     (BInt n) const { return(*(Serie(n))); }

  BUserTimeSerie* operator [](BInt n) const { return(Tsr(serArr_[n-1]));}

  BDat Dat(BInt n, const BDate& d) const { return(SerieR(n)[d]); }

  bool HasVolatileDating() { return volatileDating_; };
};



//--------------------------------------------------------------------
class BTableFormat
{
 private:
    static BDateFormat dateFormat_;
#ifdef __USE_TC__
    static std::string ctimeFormat_;
#endif
    static BText       realFormat_;
    static BAlignType  align_;
    static char        fill_;
    static const char*       sep_;
    static int	       firstLength_;
    static int	       restLength_;
    static BText       separator_;
    
 public:
    static const BDateFormat& DateFormat() { return(dateFormat_);  }
#ifdef __USE_TC__
    static const std::string CTimeFormat() { return(ctimeFormat_); }
#endif
    static const BText&	      RealFormat() { return(realFormat_);  }

    static BAlignType Align() { return(align_);	      }
    static char	      Fill()  { return(fill_);	      }
    static const char*      Sep()   { return(sep_);	      }
    static int FirstLength()  { return(firstLength_); }
    static int RestLength()   { return(restLength_);  }
    
    static void PutDateFormat(const BDateFormat& f) { dateFormat_  = f; }
#ifdef __USE_TC__
    static void PutCTimeFormat(std::string f)       { ctimeFormat_ = f; }
#endif
    static void PutAlign(BAlignType f)              { align_       = f; }
    static void PutFill(char f)                     { fill_        = f; }
    static void PutSep(char* f)                     { sep_         = f; }
    static void PutFirstLength(int f)               { firstLength_ = f; }
    static void PutRealFormat(const BText& f)       { realFormat_  = f; }

    static void  PutRealFormat (int len, int dec);
    static int   Length	       (int col);
    static BText Format	       (const BText& txt, int col);
    static BText Format	       (const BDat& dat, int col);
};


//--------------------------------------------------------------------
class BSerieTable : public BSerieGroup
//--------------------------------------------------------------------
{
private:
  BArray<BDate> date_;
  BMatrix<BDat> data_;
  BArray<BText> name;
  static BDat	formatBDT_;

public:
  BSerieTable() {}
 ~BSerieTable() {}
  void Fill  (const BDate& f = BDate::Unknown(),
	            const BDate& l = BDate::Unknown());  

  BHash&         Hash() { return(hash_); }
  BMatrix<BDat>& Data() { return(data_); }

  BText	 Name	   (BInt s)	       const { return(SerieR(s).Identify()); }
  BDat	 Dat	   (BInt s,BInt d) const { return(data_(s-1,d-1)); }
  BText	 TextData(BInt s,BInt d) const { return(Dat(s,d).Name()); }
  BDate	 Date	   (BInt d)	       const { return(date_[d-1]); }
  BText	 TextDate(BInt d)	       const { return(DateToText(date_[d])); }

  BText DateToText(const BDate& dte) const
  { return(BTableFormat::DateFormat().DateToText(dte)); }

  BDate TextToDate(const BText& txt) const
  { return(BTableFormat::DateFormat().TextToDate(txt)); }

  void WriteAsBDC(ostream& os);
  void WriteAsBDT(ostream& os, const BText& header="");
  BInt CreateTextTable(BArray< BArray<BText> >& txt) const;

  friend ostream& operator << (ostream& os, BSerieTable& dt);
  friend void Std (BSerieTable& dt);

  static BDat&	FormatBDT() { return(formatBDT_); }
  static void	PutFormatBDT(BDat f) { formatBDT_=f; }

};


//--------------------------------------------------------------------
class BDataLine
//--------------------------------------------------------------------
{
protected:
  static BDateFormat dateFormat_;
  static BData	     emptyData_;
	 BReal	     hash_;
	 BData*	     data_;

public:
  BDataLine()
  : hash_(0), data_(&emptyData_) { }
  BDataLine(BReal hash, BData& data = emptyData_)
  : hash_(hash), data_(&data) { }
  BDataLine(const BDate& date, BData& data = emptyData_)
  : hash_(date.Hash()), data_(&data) { }
 ~BDataLine() {}

  static const BDateFormat& DateFormat() { return(dateFormat_); }

  const BDataLine& operator= (const BDataLine& line)
  {
    hash_ = line.Hash();
    data_ = &(line.Data());
    return(*this);
  }

	BData& Data	 ()	  const { return(*data_); }
  const BDat&  operator[](BInt n) const { return((*data_)[n]); }
	BReal  Hash	 ()	  const { return(hash_); }
	void   PutData	 (BData& data)	{ data_ = &data; }
	void   PutLine	 (const BArray<BText>& line);
	void   PutLine	 (const BDate& date, BData& data)
	{ data_ = &data; hash_=date.Hash(); }
	void   PutLine	 (BReal h, BData& data)
	{ data_ = &data; hash_=h; }
};


//--------------------------------------------------------------------
inline BInt BDataLineOrderCriterium(const BAny any1, const BAny any2)

/*! For ordering alphabetically BInt.
 */
//--------------------------------------------------------------------
{
    BDataLine& d1 = *(BDataLine*)(any1);
    BDataLine& d2 = *(BDataLine*)(any2);
    BReal h1 = d1.Hash();
    BReal h2 = d2.Hash();
    return((h1<h2) ? -1 : ((h1>h2) ? 1 : 0));
}


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
BBool ReadBDT (const BText& file,
	       BDat& fillValue,
	       BArray<BDataLine>& data,
	       BArray<BTsrPrimary*>& serie,
	       BBool enableWarnings);

BBool ReadBDTText (BDat& fillValue,
		   BText& txtData,
		   BArray<BTsrPrimary*>& serie,
		   BBool enableWarnings);

BBool ReadBDTFile (const BText& fileName,
		   BDat& fillValue,
		   istream& file,
		   BArray<BTsrPrimary*>& serie,
		   BBool enableWarning,
		   BDate first,
		   BDate last,
		   BChar sep,
		   BBool hasHeader);

void WriteAsBDC (BUserTimeSerie* serie, ostream& os);

void BDTWriteFile1 (BUserTimeSerie* ser, const BText& fileName);
void BDTWriteFileL (BList* lst, const BText& fileName);

void PeriodicTable (BUserTimeSerie* ser,
		    const BDate& first,
		    const BDate& last,
		    BInt period,
		    const BText& header,
		    const BDateFormat& fmt,
		    const BText& fileName);

void PeriodicTable (BUserTimeSerie* ser,
		    BUserTimeSet* tms,
		    BInt period,
		    const BText& header,
		    const BDateFormat &fmt,
		    const BText& fileName);

void PeriodicTable (BUserTimeSerie* ser,
		    const BText& fileName);

#endif // TOL_BTSRGRP_H
