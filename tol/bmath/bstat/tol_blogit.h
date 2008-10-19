/* tol_blogit.h: Definition leinear regression related functions.
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

#ifndef TOL_BLOGIT_H
#define TOL_BLOGIT_H 1

#include <tol/tol_bprdist.h>

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
BBool Logit(const BMatrix<BDat>& y,
	    const BMatrix<BDat>& X,
	    BMatrix<BDat>&       B,
	    BMatrix<BDat>&       e,
	    BMatrix<BDat>&       p,
	    BMatrix<BDat>&       G,
	    BSymMatrix<BDat>&    H,
	    BMatrix<BDat>&       lnL,
	    BDat&		  lnLikelyhood,
	    BDat&		  likelyhood);

BBool Probit(const BMatrix<BDat>& y,
	     const BMatrix<BDat>& X,
	     BMatrix<BDat>&       B,
	     BMatrix<BDat>&       e,
	     BMatrix<BDat>&       p,
	     BMatrix<BDat>&       G,
	     BSymMatrix<BDat>&    H,
	     BMatrix<BDat>&       lnL,
	     BDat&		   lnLikelyhood,
	     BDat&		   likelyhood);

BBool BooleanModel(const BArray<BDat>&  y,
		   const BMatrix<BDat>& X,
		   BArray<BDat>&        B,
		   BArray<BDat>&        e,
		   BSymMatrix<BDat>&    cov,
		   BArray<BDat>&        p,
		   BArray<BDat>&        yPrev,
		   BDat&                s,
		   BDat&                R2,
		   BDat&                F,
		   BInt&                v1,
		   BInt&                v2,
		   BDat&                alfa,
		   BDat&                resSqSum,
		   BDat&                totSqSum,
		   BDat                 tolerance,
		   BProbDist*           dist);
/*
  BBool Logit(const BArray	    <BDat>& y,
  const BMatrix    <BDat>& X,
  BArray	    <BDat>& B,
  BArray	    <BDat>& e,
  BSymMatrix <BDat>& cov,
  BArray	    <BDat>& p,
  BArray	    <BDat>& yPrev,
  BDat & s,
  BDat & R2,
  BDat & F,
  BInt & v1,
  BInt & v2,
  BDat & alfa,
  BDat & resSqSum,
  BDat & totSqSum,
  BDat   tolerance);
  
BBool Probit(const BArray     <BDat>& y,
const BMatrix    <BDat>& X,
BArray     <BDat>& B,
BArray     <BDat>& e,
BSymMatrix <BDat>& cov,
BArray     <BDat>& p,
BArray     <BDat>& yPrev,
BDat & s,
BDat & R2,
BDat & F,
BInt & v1,
BInt & v2,
BDat & alfa,
BDat & resSqSum,
BDat & totSqSum,
BDat   tolerance);
*/

#endif	// TOL_BLOGIT_H
