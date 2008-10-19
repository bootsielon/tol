/* tol_bar.h: Auto-Regressive identification and estimation.
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

#ifndef TOL_BAR_H
#define TOL_BAR_H 1

#include <tol/tol_bpolyn.h>
class BDat;

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
void ApplyPolyn(const BPolyn<BDat>& pol,
		const BArray<BDat>& x,
		BArray<BDat>& y);

BPolyn<BDat> IdentifyAutoReg(const BArray <BDat>& z,
			     BArray <BDat>& e,
			     BDat&  	   sigma,
			     BDat&	   swartz,
			     BInt	   minOrder,
			     BInt	   maxnOrder);

BPolyn<BDat> IdentifyAutoReg(const BArray <BDat>& z,
			     BArray <BDat>& err,
			     BDat&	   s,
			     BDat&	   sw,
			     BInt	   period,
			     BInt	   minOrder,
			     BInt	   maxOrder);

BPolyn<BDat> IdentifyAutoReg(const BArray <BDat>& z,
			     BInt	   period=1,
			     BInt	   minOr=1,
			     BInt	   maxOr=-1);

void IdentifyAutoRegIntegrated(const BArray <BDat>& z,
			       BArray<BDat>&  err,
			       BDat&	     s,
			       BDat&	     sw,
			       BArray<BDat>&  w,
			       BPolyn<BDat>&  dif,
			       BPolyn<BDat>&  ar,
			       BInt	     period=1);

void IdentifyAutoRegIntegrated(const BArray <BDat>& z,
			       BPolyn<BDat>&  dif,
			       BPolyn<BDat>&  ar,
			       BInt	     period=1);


#endif	// TOL_BAR_H
