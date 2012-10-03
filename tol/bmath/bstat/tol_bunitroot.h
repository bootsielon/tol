/* tol_bunitroot.h: Unit troots identification.
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

#ifndef TOL_BUNITROOT_H
#define TOL_BUNITROOT_H 1

#include <tol/tol_bar.h>


//--------------------------------------------------------------------
struct BProbPol
//--------------------------------------------------------------------
{
    BDat		prob_;
    BPolyn<BDat>	U_;
    BArray <BDat> difZ_;
    BPolyn <BDat> ar_;
    BArray <BDat> a_;
    BDat		sigma_;
    BDat		swartz_;
};


BInt
BProbPolOrderCriterium (const BAny txt1, 
			const BAny txt2);

BDat
ProbUnitRoot( const BArray <BDat>& z,
	      const BPolyn <BDat>& ari,
	      BProbPol&            UnitRootPolyn);

void
IdentifyUnitRoot (const BArray <BDat>& z,
		  const BPolyn<BDat>&  ari,
		  BInt		       period,
		  BArray< BProbPol >&  UnitRootTable);

#endif	// TOL_BUNITROOT_H
