/* bcseries.cpp: TOL Language. Messages that support algebra.

   Copyright (C) 2003-2005 - Bayes Decision, SL (Spain [EU])

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

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_bcseries.h>
#include <tol/tol_bout.h>
#include <tol/tol_butil.h>
#include <tol/tol_bsetgra.h>

#if defined(__USE_TC__)

BDat  BCTimeSeries::globalizeCSeries_ = 1;
BDat& BCTimeSeries::GlobalizeCSeries() { return(globalizeCSeries_); }

//--------------------------------------------------------------------
void BCTimeSeries::CompactData()
//--------------------------------------------------------------------
{
    if(Dating() && data_.Size())
    {
	int n, n0=0, n1=data_.Size()-1, m1=0, d=0;
	while((n1 >= 0 ) && !data_[n1].IsKnown()) { n1--; m1++; d++; }
	while((n0 <	 n1) && !data_[n0].IsKnown()) { n0++; d++; }
	if((d>0)&&(d<data_.Size()))
	{
	    length_ = n1-n0+1;
	    for(n=0; n<length_; n++) { data_[n] = data_[n+n0]; }
	    data_.ReallocBuffer(length_);
	    firstCTime_ = Dating()->next(firstCTime_, n0);
	    lastCTime_	= Dating()->prev(lastCTime_,  m1);
	}
    }
}

//--------------------------------------------------------------------
BUserCTimeSet* BCTimeSeries::Dating() const
{
    const_cast<BCTimeSeries*>(this)->ReadData();
    return dating_;
}


//--------------------------------------------------------------------
BCTime BCTimeSeries::FirstCTime()
{
    const_cast<BCTimeSeries*>(this)->ReadData();
    const_cast<BCTimeSeries*>(this)->CompactData();
    return firstCTime_;
}


//--------------------------------------------------------------------
BCTime BCTimeSeries::LastCTime()
{
    const_cast<BCTimeSeries*>(this)->ReadData();
    const_cast<BCTimeSeries*>(this)->CompactData();
    return(lastCTime_);
}


//--------------------------------------------------------------------
/*! Appends this BCTimeSeries to a binary file.
 */
int BCTimeSeries::BinWrite(FILE* fil)
{
    length_ = GetLength();
    (*this)[FirstCTime()];

    int	items = 
	FirstCTime().BinWrite(fil) +
	LastCTime().BinWrite(fil) +
	fwrite(&length_,        sizeof(int),  1,       fil) +
	fwrite(Data().Buffer(), sizeof(BDat), length_, fil);

    return(items);
}


//--------------------------------------------------------------------
/*! Reads this BCTimeSeries from a binary file.
 */
int BCTimeSeries::BinRead(FILE* fil)
{
    int	items = 
	firstCTime_.BinRead(fil) +
	lastCTime_. BinRead(fil) +
	fread(&length_, sizeof(int), 1, fil);

    data_.ReallocBuffer(length_);
    items+=  fread(data_.GetBuffer(), sizeof(BDat), length_, fil);
    return(items);
}

//--------------------------------------------------------------------
/*! Tranforms a date in the corresponding index from the first known
 *  date of the CSeries.
 */
int BCTimeSeries::GetIndex(const BCTime& tmi)
{
    this->ReadData();
    if(!Dating()) { return(-1); }
    int index = -1;

    if(FirstCTime().isNormal()) 
	index = Dating()->distance(FirstCTime(),tmi);

    return index;
}


//--------------------------------------------------------------------
/*! tranforms an index in the corresponding date from the first known
 *  CTime of the CSeries.
 */
BCTime BCTimeSeries::IndexToDate(int index) const
{
    BCTime tmi;

    if(dating_ && firstCTime_.isNormal())
    { tmi = dating_->next(firstCTime_, index); }

    return tmi;
}


//--------------------------------------------------------------------
/*! If this CSeries is stochastic, returns the number of its datas.
 *  Else, it returns -1.
 */
int BCTimeSeries::GetLength()
{
    if(!Dating()) { return(length_ = -1); }

    if(!length_)
    {
	if(IsStochastic())
	{ length_= 1 + Dating()->distance(FirstCTime(), LastCTime()); }
	else
	{ length_=0; }
    }

    if(length_<0) { length_=0; }

    return length_; 
}



//--------------------------------------------------------------------
BDat BCTimeSeries::operator[](const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    if(!Dating()->hasInstants() && GetLength())
    {
	Dating()->getInstantsBetween(FirstCTime(),LastCTime());
    }

    BDat dat;

    if(!GetLength())
    {
	dat = GetDat(tmi);
    }
    else if(Data().HasValue()) { dat = data_[GetIndex(tmi)]; }
    else
    {
	data_.ReallocBuffer(length_);
	BCTime auxtmi(firstCTime_);

	for(int n=0; n<length_; n++)
	{
	    data_[n] = GetDat(auxtmi);
	    if(auxtmi==tmi) { dat = data_[n]; }
	    auxtmi = Dating()->succ(auxtmi);
	}

	CompactData();
    }

    return dat;
}


//--------------------------------------------------------------------
void BCTimeSeries::PutDat(const BCTime& tmi, const BDat& dat)
{
    if(!GetLength() || !Dating()) { return; }

    if(!Data().HasValue())
    {
	data_.ReallocBuffer(length_);
	BCTime auxtmi(firstCTime_);
	
	for(int n=0; n<length_; n++)
	{
	    data_[n] = GetDat(auxtmi);
	    auxtmi = dating_->succ(auxtmi);
	}
	CompactData();
    }

    data_[GetIndex(tmi)] = dat;
}



//--------------------------------------------------------------------
/*! Returns all data of the CSeries between firstCTime and lastCTime.
 */
void BCTimeSeries::GetData(      BData& dta,
			 const BCTime& firstD,
			 const BCTime& lastD,
			       int   length)
{
    ReadData();
    CompactData();
    
    if(!Dating())
    {
	if(length)
	{
	    dta.ReallocBuffer(length);
	    BDat dat = GetDat(firstD);
	    for(int k=0; k<length; k++) { dta[k]=dat; }
	}
	return;
    }

    int n=0, m=0;
    BCTime first = firstD;
    BCTime last  = lastD;

    if(!first.isNormal())
	first = BCTime::fillNew(Dating()->getGranul(), LV, 1,
				BCTime::Now().year()); 
    
    if(!last.isNormal())
	last = BCTime::fillNew(Dating()->getGranul(), HV, 1,
			       BCTime::Now().year());

    if(first==last) last.successor(1);
    if(length<=0) length = 1 + Dating()->distance(first, last);

    if(length>0)
    {
	dta.ReallocBuffer(length);
	if(Data().HasValue())
	{
	    int ini = Dating()->distance(FirstCTime(), first);

	    for((n=ini),(m=0); m<length; n++,m++)
	    {
		if((n>=0)&&(n<Data().Size()))
		    dta[m] = Data()[n];
		else dta[m] = BDat::Unknown();
	    }
	}
	else
	    for(n=0; (first<=last); n++)
	    {
		dta[n] = (*this)[first];
		first = Dating()->succ(first);
	    }
    }
}

#endif // __USE_TC__
