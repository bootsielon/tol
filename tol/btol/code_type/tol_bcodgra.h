/* tol_bcodgra.h: related classes with Code Grammar 
                  of GNU/TOL language.

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

#ifndef TOL_BCODGRA_H 
#define TOL_BCODGRA_H 1

#include <tol/tol_bgencon.h>
class BCode;

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------

#define BUserCode	      BGraContensBase  < BCode >
#define BContensCode    BGraContens      < BCode >
#define BCodeTemporary	BTmpContens      < BCode >

template class TOL_API BGraContensBase< BCode >;
template class TOL_API BGraContens    < BCode >;

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
CastingsDeclaration(Code);

//--------------------------------------------------------------------
class BUserFunCode : public BContensCode
//--------------------------------------------------------------------
{
public:
  BUserFunCode(const BText& name, const BCode& aCode, const BText& desc);
  BUserFunCode(const BText& name, BStandardOperator* opr, const BText& desc);
 ~BUserFunCode() {}
  RedeclareClassNewDelete(BUserFunCode);
};


BOperator* GetOperator(BUserCode* uCode);

#endif // TOL_BCODGRA_H

