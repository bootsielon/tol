/* tol_bpolmatgra.h: Classes and functions for polynomial matrix grammar.
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

#ifndef TOL_BPOLMATGRA_H
#define TOL_BPOLMATGRA_H 1

#include <tol/tol_bpolyn.h>
#include <tol/tol_bratio.h>
#include <tol/tol_bmatrixgen.h>
#include <tol/tol_matgen.hpp>
#include <tol/tol_bgencon.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BPolMat          BMatrixGen      < BPolyn<BDat> >
#define BUserPolMat      BGraContensBase < BPolMat >
#define BContensPolMat   BGraContens     < BPolMat >
#define BSystemPolMat    BGraConstant    < BPolMat >
#define BPolMatTemporary BTmpContens     < BPolMat >

template class TOL_API BArray          < BPolyn<BDat> >;
template class TOL_API BMatrixGen      < BPolyn<BDat> >;
template class TOL_API BGraContensBase < BPolMat >;
template class TOL_API BGraContens     < BPolMat >;
//template class TOL_API BMatrix         < BRational<BDat> >;

CastingsDeclaration(PolMat);

TOL_API int Degree(const BPolMat& P);
TOL_API bool IsStationary(const BPolMat& P);

#endif // TOL_BPOLMATGRA_H
