/* tol_bdayinfo.h: BDayInformation (dnf): Class and functions header.
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

#ifndef TOL_BDAYINFO_H
#define TOL_BDAYINFO_H 1

#include <tol/tol_btmsgra.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
enum BDayFlag
{
    NOTEXIST=0,
    INCLUDED=1,
    NOTINCLUDED=-1
};


//--------------------------------------------------------------------
class BDayInformation

/*! Elemental class for visual representation of time sets in a day.
 *  This class is util only for BMothInformation class.
 */
//--------------------------------------------------------------------
{
private:
  static BChar	       beforeIncluded_;
  static BChar	       afterIncluded_;
  static BBool	       printNotIncluded_;
	 BInt	       day_;
	 BDayFlag      flag_;
public:
  // Constructors and destructors:
  BDayInformation();
  BDayInformation(BInt d, BDayFlag f);
  BDayInformation(BDate dte, BMonth m, const BUserTimeSet* tms);
 ~BDayInformation();

  // Virtual functions redefinition
  BText Name() const;

  // Access and manipulation: tmsmisc.cpp
  static BChar	  BeforeIncluded  ()  { return(beforeIncluded_	); }
  static BChar	  AfterIncluded	  ()  { return(afterIncluded_	); }
  static BBool	  PrintNotIncluded()  { return(printNotIncluded_); }
	 BInt	  Day()	 const	      { return(day_ ); }
	 BDayFlag Flag() const	      { return(flag_); }
	 void	  PutDay (BInt d)     { day_  = d; }
	 void	  PutFlag(BDayFlag f) { flag_ = f; }
	 void	  PutInfo(BDate, BMonth, const BUserTimeSet*);
  static void	  PutFormat(BChar b = '[', BChar a = ']', BBool p = BTRUE)
  {
    beforeIncluded_   = b;
    afterIncluded_    = a;
    printNotIncluded_ = p;
  }
};

#endif // TOL_BDAYINFO_H
