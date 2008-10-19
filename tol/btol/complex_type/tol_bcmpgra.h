/* tol_bcmpgra.h: Classes and functions for complex numbers grammar
                  GNU/TOL Language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BCMPGRA_H
#define TOL_BCMPGRA_H 1

#include <tol/tol_bcomplex.h>
class BComplex;
#define BCmp BComplex

//--------------------------------------------------------------------
// TYPES
//--------------------------------------------------------------------
#define BUserCmp      BGraContensBase  < BComplex >
#define BContensCmp   BGraContens      < BComplex >
#define BSystemCmp    BGraConstant     < BComplex >
#define BParamCmp     BGraParameter    < BComplex >
#define BCmpTemporary BTmpContens      < BComplex >


template class TOL_API BGraContensBase< BComplex >;
template class TOL_API BGraContens    < BComplex >;

//--------------------------------------------------------------------
// FORWARD REFERENCES
//--------------------------------------------------------------------
CastingsDeclaration(Cmp);



#endif // TOL_BCMPGRA_H
