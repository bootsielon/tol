/* tol_bdtegra.h: Classes and functions for dates grammar.
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

#ifndef TOL_BDTEGRA_H
#define TOL_BDTEGRA_H 1

#include <tol/tol_bdate.h>
#include <tol/tol_bgencon.h>


//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserDate     BGraContensBase  < BDate >
#define BContensDate  BGraContens      < BDate >
#define BSystemDate   BGraConstant     < BDate >
#define BParamDate    BGraParameter    < BDate >
#define BDteTemporary BTmpContens      < BDate >


template class TOL_API BGraContensBase < BDate >;
template class TOL_API BGraContens     < BDate >;

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
CastingsDeclaration(Date);


//--------------------------------------------------------------------
class BDteToday: public BSystemDate
//--------------------------------------------------------------------
{
 public:
    BDteToday() : BSystemDate
	(
	    "Today",
	    BDate::Unknown(),
	    I2("Today date",
	       "La fecha de hoy")
	) { }
    ~BDteToday() {}
    BDate& Contens() { return(contens_ = DteToday()); }
};


//--------------------------------------------------------------------
class BDteNow: public BSystemDate
//--------------------------------------------------------------------
{
public:
  BDteNow() : BSystemDate
  (
    "Now",
    BDate::Unknown(),
    I2("Today date and time",
       "La fecha y la hora de hoy")
  ) { }
 ~BDteNow() {}
  BDate& Contens() { return(contens_ = DteNow()); }
};


#endif // TOL_BDTEGRA_H
