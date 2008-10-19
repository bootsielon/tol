/* tol_bsvdsep.h: Singular Value Decomposition and Symmetric Eigen Problem.
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

#ifndef TOL_BSVDSEP_H
#define TOL_BSVDSEP_H 1

#include <tol/tol_bmatimp.h>

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------

void LanczosBidiagonalization(const BMat&   A_,
			      BMat&   U_,
			      BMat&   B_,
			      BMat&   V_,
			      BDat&   anorm_);

void LanczosBidiagonalization(const BMat&   A,
			      const BMat&   b,
			      BMat&   U,
			      BMat&   B,
			      BMat&   V,
			      BDat&   anorm);

void TridiagSEPDivideAndConquer
(
    const BSymMatrix  <BDat>& T,
    BMatrix     <BDat>& U,
    BDiagMatrix <BDat>& D
);

BInt TridiagSEPNegCount
(
    const BSymMatrix<BDat>& A,
    BDat z
);

BInt TridiagSEPBisection
(
    const BSymMatrix<BDat>& A,
    BDat a, BDat b,
    BArray<BDat>& d,
    BDat tolerance
);

void SingularValueDecomposition(const BMat& A,
				BMat& U,
				BDiagMatrix<BDat>& D,
				BMat& V);


void gsl_SingularValueDecomposition(const BMatrix<BDat>& A,
				    BMatrix<BDat>& U,
				    BDiagMatrix<BDat>& D,
				    BMatrix<BDat>& V,
				    BText method);


void SymmetricEigenvalueProblem(const BSymMatrix<BDat>& A,
				BMat& U,
				BDiagMatrix<BDat>& D);


#endif	// TOL_BSVDSEP_H
