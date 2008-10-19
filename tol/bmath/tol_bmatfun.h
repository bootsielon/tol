/* bmatfun.h: Simple arithmetic functions.
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

#ifndef TOL_BMATFUN_H 
#define TOL_BMATFUN_H 1

#include <tol/tol_bdat.h>

#define TOL	 BDat::Tolerance()
#define EULER	 0.577215664901532860606512
#define SQRT2PI	 2.506628274631000502415765


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
BDat LogFactorial   (BInt n);
BDat Factorial	    (BInt n);
BDat LogBiFactorial (BInt n);
BDat BiFactorial    (BInt n);
BDat LogCombinat    (BInt m, BInt n);
BDat Combinat	    (BInt m, BInt n);

BDat RiemmanZeta_1	  (BDat s, BDat tol = TOL);
BDat RiemmanZeta	  (BDat s, BDat tol = TOL);
BDat ReciprocalPowerSum_1 (BInt m, BDat tol = TOL);
BDat ReciprocalPowerSum	  (BInt m, BDat tol = TOL);
BDat Bernouilli0	  (BInt s, BDat tol = TOL);
BDat LogAbsBernouilli0	  (BInt s, BDat tol = TOL);

BDat LogRecGamma    (BDat n, BDat tol = TOL);
BDat LogAbsRecGamma (BDat n, BDat tol = TOL);
BDat SignRecGamma   (BDat z, BDat tol = TOL);
BDat RecGamma	    (BDat n, BDat tol = TOL);
BDat LogGamma	    (BDat n, BDat tol = TOL);
BDat Gamma	    (BDat n, BDat tol = TOL);
BDat LogBeta	    (BDat m, BDat n, BDat tol = TOL);
BDat Beta	    (BDat m, BDat n, BDat tol = TOL);

BDat FHyp  (BDat a, BDat b, BDat c, BDat z, BDat tol = TOL);
BDat MCHyp (BDat a, BDat b, BDat z, BDat tol = TOL);
BDat UCHyp (BDat a, BDat b, BDat z, BDat tol = TOL);

BDat UParCyl (BDat a, BDat z, BDat tol = TOL);
BDat DParCyl (BDat a, BDat z, BDat tol = TOL);

BDat Erfc  (BInt n, BDat z, BDat tol = TOL);
BDat Hh	   (BInt n, BDat z, BDat tol = TOL);

BDat LogStirling (const BDat& x);
BDat InvFactorial(const BDat& x);

#endif	// TOL_BMATFUN_H

