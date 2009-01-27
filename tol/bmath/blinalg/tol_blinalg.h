/* tol_blinalg.h: Selected Linear Algebra Functions.
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

#ifndef TOL_BLINALG_H
#define TOL_BLINALG_H 1

#include <tol/tol_bmatimp.h>
//#include <tol/tol_bmfstpro.h>
//#include <tol/tol_bsvdsep.h>

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------

BDat MatTraze   (const BMat& A);

BMatrix<BDat> gsl_MinimumResidualsSolve
(
  const BMatrix<BDat>& A,
  const BMatrix<BDat>& b
);

BMatrix<BDat> gsl_linalgHHSolve
(
  const BMatrix<BDat>& A,
  const BMatrix<BDat>& b
);

BMat MinimumResidualsSolve
(
  const BMat& A,
  const BMat& b,
  const BMat& x,
  double chop = -1,
  int maxIter = -1
);

BMat MinimumResidualsSolve
(
  const BMat& A,
  const BMat& b,
  double chop = -1,
  int maxIter = -1
);

BDat LinearLeastSquareSolve
(
  const BMatrix<BDat>&	  A,
  const BMatrix<BDat>&	  Y,
  const BArray <BText>&	  name,
	BMatrix<BDat>&	  X,
	BMatrix<BDat>&	  R,
	BMatrix<BDat>&	  P,
	BSymMatrix<BDat>& information,
	BSymMatrix<BDat>& covarianze,
	BMatrix<BDat>&	  kernel,
	BArray <BBool>&	  varElim,
	BBool		  onlySolve
);

BDat LinearLeastSquareSolve
(
  const BMatrix<BDat>&	  A,
  const BMatrix<BDat>&	  Y,
	BMatrix<BDat>&	  X,
	BMatrix<BDat>&	  R
);

BMat Kernel(const BMat& A);

void OrthonormalCompletion(BMat& U, BInt m);

void WriteRowNullEquations(const BMatrix<BDat >& A,
			   const BArray <BText>& name);

void EliminateLinearRelations(const BMatrix<BDat>&  kernel,
			      const BArray <BText>& name,
			      BArray <BBool>& varElim);


#ifdef __USE_DEPRECATED_LINALG_METHOD__
//Old deprecated methods that has been abandoned
//are saved here for a few months
BBool MatDiagDom (const BMat& A);
BBool FirstComponent(BMatrix<BDat>& S,
			       BMat& b,
			       BDat& lambda,
			       BDat tol=BDat::Tolerance());
BBool JordanIterative(BMatrix<BDat> S,
		      BMatrix<BDat>& B,
		      BDiagMatrix<BDat>& lambda,
		      BDat t=BDat::Tolerance());
BMat GaussSeidelSolve
(
  const BMat& A,
  const BMat& b,
  const BMat& x0
);
BMat ConjugateGradientSolve
(
  const BMat& A,
  const BMat& b,
  const BMat& x0
);
BMat BiconjugateGradientSolve
(
  const BMat& A,
  const BMat& b,
  const BMat& x0
);
#endif //__USE_DEPRECATED_LINALG_METHOD__

#endif	// TOL_BLINALG_H
