/* tol_btest.h: Definition of a-temporal statistics functions.
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

#ifndef TOL_BTEST_H
#define TOL_BTEST_H 1

#include <tol/tol_bstat.h>


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
BDat HannShapiroW (const BArray<BDat>& vec, BDat& b2);
BDat DAgostinoD	(const BArray<BDat>& vec, BDat& T);
BBool HannShapiroWTest (BDat W, BInt n, BDat alfa);
BBool DAgostinoDTest (BDat W, BInt n, BDat alfa);
BBool NormalityTest (const BArray<BDat>& vec, BDat alfa);

#endif	//BTEST
