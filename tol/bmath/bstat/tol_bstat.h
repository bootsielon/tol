/* tol_bstat.h: Definition of generic statistical functions.
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

#ifndef TOL_BSTAT_H 
#define TOL_BSTAT_H 1

#include <tol/tol_bsymmat.h>

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
typedef BDat (*BSimpleStatisticFunction) (const BArray<BDat>& vec);

TOL_API BDat SubSampleAverage(BSimpleStatisticFunction stat,
		      const BArray<BDat>& vec);

TOL_API BDat Known                (const BArray<BDat>& vec);
TOL_API BDat Sum	                (const BArray<BDat>& vec);
TOL_API BDat Product	            (const BArray<BDat>& vec);
TOL_API BDat Average	            (const BArray<BDat>& vec);
TOL_API BDat HarmonicAverage      (const BArray<BDat>& vec);
TOL_API BDat GeometricAverage     (const BArray<BDat>& vec);
TOL_API BDat Moment	              (const BArray<BDat>& vec, BInt m);
TOL_API BDat CenterMoment         (const BArray<BDat>& vec, BInt m);
TOL_API BDat Varianze	            (const BArray<BDat>& vec);
TOL_API BDat StandardDeviation    (const BArray<BDat>& vec);
TOL_API BDat AsymmetryCoefficient (const BArray<BDat>& vec);
TOL_API BDat Kurtosis	            (const BArray<BDat>& vec);
TOL_API BDat Maximum	            (const BArray<BDat>& vec);
TOL_API BDat Minimum	            (const BArray<BDat>& vec);
TOL_API BDat Quantile	            (const BArray<BDat>& vec, BDat q);
TOL_API void Quantile             (const BArray<BDat>& vec, 
                                   const BArray<BDat>& r, 
                                         BArray<BDat>& Qu);
TOL_API BDat Median	              (const BArray<BDat>& vec);
TOL_API BDat Covarianze           (const BArray<BDat>& v1,
		                               const BArray<BDat>& v2);
TOL_API BDat Correlation          (const BArray<BDat>& v1,
		                               const BArray<BDat>& v2);
TOL_API void Covarianze           (const BMatrix<BDat>&    data,
		                                     BSymMatrix<BDat>& cor);
TOL_API void Correlation          (const BMatrix<BDat>&    data,
		                                     BSymMatrix<BDat>& cor);
TOL_API void PartialCorrelation   (const BMatrix<BDat>&    data,
		                     	               BSymMatrix<BDat>& pcor);
TOL_API void CorrelationF         (const BSymMatrix<BDat>& cov,
	                                       BSymMatrix<BDat>& cor);
TOL_API void PartialCorrelationF  (const BSymMatrix<BDat>& cor,
			                                   BSymMatrix<BDat>& pcor);
TOL_API void RegressionCoef       (const BSymMatrix<BDat>& cov,
		                               const BSymMatrix<BDat>& pcor,
		                                     BMatrix<BDat>&    regr);
TOL_API BDat PearsonCoefficient   (const BArray<BDat>& v1,
		 	                             const BArray<BDat>& v2);
TOL_API BDat R2Coefficient        (const BArray<BDat>& v1, 
                                   const BArray<BDat>& v2);
TOL_API void AutoCov              (const BArray<BDat>& vec, 
                                         BArray<BDat>& cov, 
                                         BInt order, 
		                                     BInt demean=0);
TOL_API void AutoCor              (const BArray<BDat>& vec, 
                                         BArray<BDat>& cor, 
                                         BInt order, 
		                                     BInt demean=0);
TOL_API void DurbinAutoReg        (const BArray<BDat>&  r, 
                                         BMatrix<BDat>& FI);
TOL_API void PartAutoCor          (const BArray<BDat>& cor, 
                                         BArray<BDat>& p);
TOL_API void InverseAutoCor       (const BArray<BDat>& cor, 
                                         BArray<BDat>& p);
TOL_API void InvPartAutoCor       (const BArray<BDat>& cor, 
                                         BArray<BDat>& p);
TOL_API BDat BoxPierceLjung       (const BArray<BDat>& vec, BInt m);
TOL_API BDat BoxPierceLjungACF    (const BArray<BDat>& acf, BInt m, BInt s);
TOL_API BDat BoxPierceACF         (const BArray<BDat>& acf, BInt m, BInt s);
TOL_API BDat BoxPierceModACF      (const BArray<BDat>& acf, BInt m, BInt s);


//! To obtine the exponent of the best Box-Cox exponential transformation of a 
//! vector and the number needed add to the series to be positive 
TOL_API void BoxCoxTrans(BDat& exponent,
		                 BDat& addition,
		                 const BArray<BDat>& vec,
		                 BInt period,
		                 const BArray<BDat>& values);

//! To obtine the exponent of the best Box-Cox exponential transformation of a 
//! vector and the number needed add to the series to be positive 
TOL_API void BoxCoxTrans(BDat& exponent,
		                 BDat& addition,
		                 const BArray<BDat>& vec,
		                 BInt period);

TOL_API void Frequency(const BArray<BDat>& vec, BMatrix<BDat>& M ,
	                   BInt parts,
	                   BDat min=BDat::Unknown(),
	                   BDat max=BDat::Unknown());

#endif	// TOL_BSTAT_H
