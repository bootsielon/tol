/* dnf.cpp: Constructors and destructors for BDayInformation class.
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

#include <tol/tol_bdayinfo.h>

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("dnf.cpp");
BChar BDayInformation::beforeIncluded_ = '[';
BChar BDayInformation::afterIncluded_ = ']';
BBool BDayInformation::printNotIncluded_ = BTRUE;


//--------------------------------------------------------------------
BDayInformation::BDayInformation()

/*! Default constructor for BDayInformation Class.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BDayInformation::BDayInformation(BInt d, BDayFlag f): day_(d),
						      flag_(f)

/*! Constructor for BDayInformation Class.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BDayInformation::BDayInformation(BDate dte,
				 BMonth m,
				 const BUserTimeSet* tms)

/*! Constructor for BDayInformation Class.
 */
//--------------------------------------------------------------------
{ PutInfo(dte, m, tms); }


//--------------------------------------------------------------------
BDayInformation::~BDayInformation()

/*! Destructor for BDayInformation Class.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BText BDayInformation::Name() const

/*! Returns a BText that contains information about a day.
 */
//--------------------------------------------------------------------
{
    BText txt;
    if(flag_ != NOTEXIST)
    {
	BText nameDay;
	nameDay.Copy(day_, "%2ld");
	if(flag_==INCLUDED) {
          txt << beforeIncluded_ << nameDay.String() << afterIncluded_ << " "; 
	}
	else if(printNotIncluded_) {
	    txt << " " << nameDay.String() << "  ";
	} else {
	    txt << " __  "; 
	}
    }
    else
    { txt << "	 "; }
    return(txt);
}


//--------------------------------------------------------------------
void BDayInformation::PutInfo(BDate dte, 
			      BMonth m, 
			      const BUserTimeSet* tms)
//--------------------------------------------------------------------
{
    day_ = dte.Day();
    if(BTmsMonth(m).Contain(dte))
    {
	if(tms->Contain(dte)) { flag_ = INCLUDED; }
	else		      { flag_ = NOTINCLUDED; }
    }
    else
    { flag_ = NOTEXIST; }
}

