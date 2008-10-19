/* tol_bestim.h: Estimation of ARIMA model.
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

#ifndef TOL_BESTIM_H
#define TOL_BESTIM_H 1

#include <tol/tol_bmodel.h>


//--------------------------------------------------------------------
class BEstimation : public BRnRmFunction

/*! Class to estimate a model.
 */
//--------------------------------------------------------------------
{
public:
  BModel* M_;
  BBool	  ok_;
  BDate	  first_;
  BDate	  last_;
public:
  BEstimation(	    BModel* model,
	      const BDate&  first = BDate::Begin(),
              const BDate&  last  = BDate::End()   );
  
  void Evaluate(      BArray<BDat>& a,
		const BArray<BDat>& p);
  void Jacobian(const BArray<BDat>&  x,
		const BArray<BDat>&  y,
		      BMatrix<BDat>& J);

  BBool Marquardt();

};

#endif // TOL_BESTIM_H
