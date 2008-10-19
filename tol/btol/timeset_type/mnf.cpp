/* mnf.cpp: Constructors and destructors for BMonthInformation class.
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bmoninfo.h>

//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("mnf.cpp");

BTmsMonthDay* BMonthInformation::firstOfMonth_ = new BTmsMonthDay(1);
BTmsWeekDay*  BMonthInformation::monday_       = new BTmsWeekDay(MONDAY);


//--------------------------------------------------------------------
BMonthInformation::BMonthInformation(): tms_(NIL)
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BMonthInformation::BMonthInformation(BDate i,
				     const BUserTimeSet* t,
				     BInt rows,
				     BInt columns)
//--------------------------------------------------------------------
    : tms_(t), rows_(rows), columns_(columns)
{
    BInt d, w, r;
    info_.AllocBuffer(7);
    for(d=0; d<7; d++)
    {
	info_[d].AllocBuffer(6);
	for(w=0; w<6; w++)
	{
	    info_[d][w].AllocBuffer(rows_);
	    for(r=0; r<rows_; r++)
	    {
		info_[d][w][r].AllocBuffer(columns_);
	    }
	}
    }
    PutInitial(i);
}


//--------------------------------------------------------------------
BMonthInformation::~BMonthInformation()
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
void BMonthInformation::Reset()
//--------------------------------------------------------------------
{
    BInt r, c, w , d, m;
    BDate dte;
    for(r=1; r<=rows_; r++)
    {
	for(c=1; c<=columns_; c++)
	{
	    dte = firstOfMonth_->Next(initial_, columns_*(r-1) + (c-1));
	    m = dte.Month();
	    dte = monday_->FirstNoGreat(dte);
	    for(w=1; w<=6; w++)
	    {
		for(d=1; d<=7; d++)
		{
		    Info(d , w, r, c).PutInfo(dte, (BMonth)m, tms_);
		    dte.IncDay(1);
		}
	    }
	}
    }
}


//--------------------------------------------------------------------
BText BMonthInformation::Name() const
//--------------------------------------------------------------------
{
    BText txt;
    TextInfo(txt);
    return(txt);
}


//--------------------------------------------------------------------
void BMonthInformation::TextInfo(BText& txt,
				 BInt week, 
				 BInt row, 
				 BInt column) const
//--------------------------------------------------------------------
{
  BInt d;

  if(week==-2)
  {
    BText aux = BDateFormat::LongMonth(Month(row, column));
    aux << " " << Year(row, column);
    BText space(36-aux.Length(),' ');
    txt += aux + space;
  }
  else if(week==-1)
  {
    for(d=1; d<=7; d++)
    { txt << " " <<BDateFormat::ShortWeekDay((BWeekDay)d) << " "; }
  }
  else if(week==0)
  {
    txt <<"---------------------------------- ";
  }
  else if(week<=6)
  {
    for(d=1; d<=7; d++)
    {
      BText info = Info(d, week, row, column).Name();
      txt << info.String();
    }
  }
}


//--------------------------------------------------------------------
void BMonthInformation::TextInfo( BText& txt, const BChar* margin,
				  BInt row, BInt column) const
//--------------------------------------------------------------------
{
    for(BInt w = -2; w <= 6; w++)
    {
	TextInfo(txt, w, row, column);
	txt << "\n";
    }
}


//--------------------------------------------------------------------
void BMonthInformation::TextInfo(BText& txt, const BChar* margin) const
//--------------------------------------------------------------------
{
    BInt r, c, w;
    BText expr = tms_->Name();
    if(!expr.HasName()) { expr = tms_->Identify(); }
    txt += margin;
    txt += expr + "\n" + margin + "+--";
    for(c=1; c<=columns_; c++)
    {
	txt << "-----------------------------------";
	if(c<columns_) { txt << "-+--"; }
    }
    txt << "-+\n";
    for(r=1; r<=rows_; r++)
    {
	for(w=-2; w<=6; w++)
	{
	    txt << margin << "|  ";
	    for(c=1; c<=columns_; c++)
	    {
		TextInfo(txt, w, r, c);
		if(c<columns_) { txt << " |  "; }
	    }
	    txt << " |\n";
	}
	txt << margin << "+--";
	for(c=1; c<=columns_; c++)
	{
	    txt << "-----------------------------------";
	    if(c<columns_) { txt << "-+--"; }
	}
	txt << "-+\n";
    }
}


//--------------------------------------------------------------------
void BMonthInformation::PageMaker
(BText& txt, BUserTimeSet* tms, BInt initialYear, BInt years ,
 const BChar* upMargin,
 const BChar* downMargin,
 const BChar* rightMargin)
//--------------------------------------------------------------------
{
    BMonthInformation info(BDate(initialYear, BMonth(1), 1), tms, 6, 2);
    BInt n;
    for(n=0; n<years; n++)
    {
	info.TextInfo(txt, rightMargin);
	if(n+1<years)
	{
	    info++;
	    txt += BChar(12);
	}
    }
}


//--------------------------------------------------------------------
void BMonthInformation::PageMaker(BText& txt,
				  BUserTimeSet* tms,
				  const BDate& from,
				  const BDate& until,
				  const BChar* upMargin,
				  const BChar* downMargin,
				  const BChar* rightMargin)
//--------------------------------------------------------------------
{
    PageMaker(txt, tms, from.Year(), until.Year()-from.Year()+1,
	      upMargin, downMargin, rightMargin);
}


//--------------------------------------------------------------------
BMonth BMonthInformation::Month(BInt row, BInt column)const
//--------------------------------------------------------------------
{
    BDate dte = firstOfMonth_->Next(initial_, columns_*(row-1) + (column-1));
    return(dte.Month());
}


//--------------------------------------------------------------------
BInt BMonthInformation::Year(BInt row, BInt column) const
//--------------------------------------------------------------------
{
    BDate dte = firstOfMonth_->Next(initial_, columns_*(row-1) + (column-1));
    return(dte.Year());
}


//--------------------------------------------------------------------
BMonthInformation& BMonthInformation::operator+=(BInt n)
//--------------------------------------------------------------------
{
    PutInitial(firstOfMonth_->Next(initial_, n*rows_));
    return(*this);
}


//--------------------------------------------------------------------
BMonthInformation& BMonthInformation::operator-=(BInt n)
//--------------------------------------------------------------------
{
    PutInitial(firstOfMonth_->Prev(initial_, n*rows_));
    return(*this);
}


//--------------------------------------------------------------------
BMonthInformation& BMonthInformation::operator++(int internal)
//--------------------------------------------------------------------
{
    PutInitial(firstOfMonth_->Next(initial_, rows_*columns_));
    return(*this);
}


//--------------------------------------------------------------------
BMonthInformation& BMonthInformation::operator--(int internal)
//--------------------------------------------------------------------
{
    PutInitial(firstOfMonth_->Prev(initial_, rows_*columns_));
    return(*this);
}
