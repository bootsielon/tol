/* tol_btxtgra.h: Classes and functions for text grammar.
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

#ifndef TOL_BTXTGRA_H
#define TOL_BTXTGRA_H 1

#include <tol/tol_bgencon.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserText      BGraContensBase < BText >
#define BContensText   BGraContens     < BText >
#define BSystemText    BGraConstant    < BText >
#define BParamText     BGraParameter   < BText >
#define BTxtTemporary  BTmpContens     < BText >


template class TOL_API BGraContensBase < BText >;
template class TOL_API BGraContens     < BText >;

//--------------------------------------------------------------------
// inline functions
//--------------------------------------------------------------------
CastingsDeclaration(Text);

TOL_API BText & GCFFile();

#endif // TOL_BTXTGRA_H

