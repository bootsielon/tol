/* tol_bmoninfo.h: BMonthInformation and functions header.
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

#ifndef TOL_BMONINFO_H
#define TOL_BMONINFO_H 1

#include <tol/tol_bdayinfo.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
#define BTimeTable BArray< BArray< BArray< BArray<BDayInformation> > > >

//--------------------------------------------------------------------
class BMonthInformation

/*! Elemental class for visual representation of time sets in
 *  a month.
 */
//--------------------------------------------------------------------
{
private:
  static BTmsMonthDay* firstOfMonth_;
  static BTmsWeekDay*  monday_;
  const	 BUserTimeSet*	   tms_;
	 BTimeTable    info_;
	 BDate	       initial_;
	 BInt	       rows_;
	 BInt	       columns_;

public:
  // Constructors and destructors: mnf.cpp
  BMonthInformation();
  BMonthInformation(BDate i, const BUserTimeSet* t, BInt r=1, BInt c=1);
 ~BMonthInformation();

  // Virtual functions redefinition: inline: mnf.hpp
  BText Name() const;

  // Access and manipulation: inline: mnf.hpp
  BDate Initial() const { return(initial_); }

  BDayInformation& Info(BInt weekDay, BInt week,
			BInt row=1, BInt column=1) const
  { return(info_[weekDay-1][week-1][row-1][column-1]); }

  const BUserTimeSet* TimeSet() const { return(tms_); }

  void PutInitial(const BDate& d)
  {
    initial_ = d;
    Reset();
  }

  void PutTimeSet(const BUserTimeSet* t)
  {
    tms_ = t;
    Reset();
  }

  void PutInfo(const BDate& d, const BUserTimeSet* t)
  {
    initial_ = firstOfMonth_->FirstNoGreat(d);
    tms_ = t;
    Reset();
  }

  // Implementation: tmsmisc.cpp
  void	Reset();
  void	TextInfo(BText& txt, BInt week, BInt row, BInt column) const;
  void	TextInfo(BText& txt, const BChar* margin, BInt row, BInt column) const;
  void	TextInfo(BText& txt, const BChar* margin = NIL) const;

  static void PageMaker( BText& txt,
			 BUserTimeSet* tms,
			 BInt initialYear,
			 BInt years,
			 const BChar* upMargin	  = "\n\n",
			 const BChar* downMargin  = "\n",
			 const BChar* rightMargin = " ");
  static void PageMaker( BText& txt,
			 BUserTimeSet* tms,
			 const BDate& from	  = BDate::DefaultFirst(),
			 const BDate& until	  = BDate::DefaultLast(),
			 const BChar* upMargin	  = "\n\n",
			 const BChar* downMargin  = "\n",
			 const BChar* rightMargin = " ");

  BMonth Month(BInt row=1, BInt column=1)const;

  BInt Year(BInt row=1, BInt column=1) const;

  // Operators overloading: mnfopr.cpp
  BMonthInformation& operator+=(BInt n);

  BMonthInformation& operator-=(BInt n);

  BMonthInformation& operator++(int internal);

  BMonthInformation& operator--(int internal);
};


#endif // BMONINFO
