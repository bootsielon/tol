/* tol_bratgra.h: Classes and functions for rational polynomial grammar
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

#ifndef TOL_BRATGRA_H
#define TOL_BRATGRA_H 1

#include <tol/tol_bratio.h>
#include <tol/tol_bpolgra.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BRat	        BRational	       < BDat >
#define BUserRat      BGraContensBase  < BRat >
#define BContensRat   BGraContens      < BRat >
#define BSystemRat    BGraConstant     < BRat >
#define BRatTemporary BTmpContens      < BRat >


template class TOL_API BRational       < BDat >;
template class TOL_API BGraContensBase < BRat >;
template class TOL_API BGraContens     < BRat >;

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
CastingsDeclaration(Rat);

#endif // TOL_BRATGRA_H

