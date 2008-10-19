/* tol_blinreg.h: Definition leinear regression related functions.
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

#ifndef TOL_BLINREG_H
#define TOL_BLINREG_H 1

#include <tol/tol_bsymmat.h>

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
BDat LinearRegression(const BArray<BDat>&  y,
		      const BMatrix<BDat>& X,
		      BArray<BDat>&        A,
		      BArray<BDat>&        e,
		      BMatrix<BDat>&       L,
		      BMatrix<BDat>&       Li,
		      BMatrix<BDat>&       cov,
		      BMatrix<BDat>&       cor,
		      BDat&                R2,
		      BDat&                F,
		      BInt&                v1,
		      BInt&                v2,
		      BDat&                alfa,
		      BDat&                resSqSum,
		      BDat&                totSqSum);


#endif	// TOL_BLINREG_H
