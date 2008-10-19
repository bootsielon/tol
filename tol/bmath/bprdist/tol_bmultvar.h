/* tol_bmultvar.h: Multivariant distributions.
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

#ifndef TOL_BMULTVAR_H
#define TOL_BMULTVAR_H 1

#include <tol/tol_bprdist.h>
#include <tol/tol_bsymmat.h>
#include <tol/tol_bltrimat.h>


//--------------------------------------------------------------------
class BMultivarDist

/*! Generic Nth-Dimensional Distribution Class
 */
//--------------------------------------------------------------------
{
private:
  BInt				 N_;
  BArray	     <BProbDist*> component_;
  BMatrix	     <BDat>	nu_;
  BMatrix	     <BDat>	avr_;
  BSymMatrix	 <BDat>	cov_;
  BMatrix      <BDat>	choleski_;

  void Build();

public:
  // Constructors and destructors:
  BMultivarDist(      BProbDist*	      dist,
		const BMatrix	 <BDat>	      nu,
		const BSymMatrix <BDat>&      cov);

  BMultivarDist(const BArray	 <BProbDist*> component,
		const BMatrix	 <BDat>	      nu,
		const BSymMatrix <BDat>&      cov);

 ~BMultivarDist() { }

  // Implementation:
	BMatrix	   <BDat>  Random     () const;
  const BMatrix	   <BDat>& Average    () const { return(avr_); }
  const BMatrix	   <BDat>& Nu	      () const { return(nu_ ); }
  const BSymMatrix <BDat>& Covarianze () const { return(cov_); }

// BDat ProbF (const BMatrix<BDat>& ) const;
// BDat Densf (const BMatrix<BDat>& ) const;
};



//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
void RandPermutation(BMatrix<BDat>& m, BInt numPer, BInt numElem);
void RandCombination(BMatrix<BDat>& M, BInt numCom, BInt m, BInt n);
void RandMultinomial(int N, const BArray<double>& p,
                     BArray<unsigned int>& n);

//--------------------------------------------------------------------

bool RandTruncatedMultNormal(      BMatrix<BDat>& tn, 
                             const BMatrix<BDat>& A,
                             const BMatrix<BDat>& B,
                             const BMatrix<BDat>& nu,
                                   BDat           s2,
                                   BInt           num);

BDat LogDensTruncatedMultNormal(const BMatrix<BDat>& y,
                                const BMatrix<BDat>& A,
                                const BMatrix<BDat>& B,
                                const BMatrix<BDat>& nu,
                                      BDat           s2);

bool RandTruncatedMultNormal(      BMatrix<BDat>&      tn, 
                             const BMatrix<BDat>&      A,
                             const BMatrix<BDat>&      B,
                             const BMatrix<BDat>&      nu,
                             const BLowTrMatrix<BDat>& LCov,
                                   BDat                s2,
                                   BInt                num);

BDat LogDensTruncatedMultNormal(const BMatrix<BDat>&      y,
                                const BMatrix<BDat>&      A,
                                const BMatrix<BDat>&      B,
                                const BMatrix<BDat>&      nu,
                                const BLowTrMatrix<BDat>& LCov,
                                const BUpTrMatrix <BDat>& LCovi,
                                      BDat                s2);

#endif	// TOL_BMULTVAR_H
